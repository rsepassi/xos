const std = @import("std");
const builtin = @import("builtin");

const log = std.log.scoped(.app);

const wgpu = struct {
    const c = @cImport({
        @cInclude("webgpu.h");
        @cInclude("wgpu.h");
    });
};

extern fn initGlfwWgpuSurface(wgpu.c.WGPUInstance, *glfw.c.GLFWwindow, *wgpu.c.WGPUSurface) c_int;

const glfw = struct {
    const c = @cImport({
        @cDefine("GLFW_INCLUDE_NONE", "1");
        @cInclude("GLFW/glfw3.h");
        switch (builtin.os.tag) {
            .macos => @cDefine("GLFW_EXPOSE_NATIVE_COCOA", "1"),
            .linux => @cDefine("GLFW_EXPOSE_NATIVE_X11", "1"),
            .windows => @cDefine("GLFW_EXPOSE_NATIVE_WIN32", "1"),
            else => @compileError("unsupported"),
        }
        @cInclude("GLFW/glfw3native.h");
    });
};

fn errorCallback(error_code: c_int, description: [*c]const u8) callconv(.C) void {
    log.err("glfw error: [{d}] {s}\n", .{ error_code, description });
}

fn handleRequestAdapter(status: wgpu.c.WGPURequestAdapterStatus, adapter: wgpu.c.WGPUAdapter, message: [*c]const u8, userdata: ?*anyopaque) callconv(.C) void {
    if (status == wgpu.c.WGPURequestAdapterStatus_Success) {
        var ctx: *Ctx = @ptrCast(@alignCast(userdata));
        ctx.adapter = adapter;
    } else {
        log.err("wgpu request adapter error: [{any}] {s}", .{ status, message });
    }
}

fn handleRequestDevice(
    status: wgpu.c.WGPURequestDeviceStatus,
    device: wgpu.c.WGPUDevice,
    message: [*c]const u8,
    userdata: ?*anyopaque,
) callconv(.C) void {
    if (status == wgpu.c.WGPURequestDeviceStatus_Success) {
        var ctx: *Ctx = @ptrCast(@alignCast(userdata));
        ctx.device = device;
    } else {
        log.err("wgpu request device error: [{any}] {s}", .{ status, message });
    }
}

fn handleGlfwFramebufferSize(window: ?*glfw.c.GLFWwindow, width: c_int, height: c_int) callconv(.C) void {
    if (width == 0 and height == 0) return;
    const userdata = glfw.c.glfwGetWindowUserPointer(window);
    if (userdata == null) return;
    const ctx: *Ctx = @ptrCast(@alignCast(userdata.?));

    updateWindowSize(window, ctx);
}

fn updateWindowSize(window: ?*glfw.c.GLFWwindow, ctx: *Ctx) void {
    var width: c_int = 0;
    var height: c_int = 0;
    glfw.c.glfwGetWindowSize(window, &width, &height);
    ctx.config.width = @intCast(width);
    ctx.config.height = @intCast(height);
    wgpu.c.wgpuSurfaceConfigure(ctx.surface, &ctx.config);
}

fn loadShader(device: wgpu.c.WGPUDevice, name: [:0]const u8, buf: [:0]const u8) !wgpu.c.WGPUShaderModule {
    const chain = wgpu.c.WGPUShaderModuleWGSLDescriptor{
        .chain = .{
            .sType = wgpu.c.WGPUSType_ShaderModuleWGSLDescriptor,
        },
        .code = buf,
    };
    return wgpu.c.wgpuDeviceCreateShaderModule(device, &.{
        .label = name,
        .nextInChain = @ptrCast(&chain),
    }) orelse return error.WgpuShader;
}

const AppGlfw = struct {
    const Self = @This();

    window: *glfw.c.GLFWwindow,

    fn init() !Self {
        _ = glfw.c.glfwSetErrorCallback(errorCallback);
        if (glfw.c.glfwInit() != glfw.c.GLFW_TRUE) return error.GlfwInit;
        glfw.c.glfwWindowHint(glfw.c.GLFW_CLIENT_API, glfw.c.GLFW_NO_API);
        const window = glfw.c.glfwCreateWindow(640, 480, "Hello", null, null) orelse return error.GlfwWindow;
        return .{ .window = window };
    }

    fn deinit(self: Self) void {
        glfw.c.glfwDestroyWindow(self.window);
        glfw.c.glfwTerminate();
    }

    fn getSurface(self: Self, gpu: wgpu.c.WGPUInstance) !wgpu.c.WGPUSurface {
        var surface: wgpu.c.WGPUSurface = undefined;
        if (initGlfwWgpuSurface(gpu, self.window, &surface) != 0) return error.Glue;
        return surface;
    }
};

const Ctx = struct {
    adapter: wgpu.c.WGPUAdapter = null,
    device: wgpu.c.WGPUDevice = null,
    config: wgpu.c.WGPUSurfaceConfiguration = .{},
    surface: wgpu.c.WGPUSurface = null,
};

pub fn main() !void {
    var ctx: Ctx = .{};

    const gpu = wgpu.c.wgpuCreateInstance(null);
    defer wgpu.c.wgpuInstanceRelease(gpu);

    const app = try AppGlfw.init();
    defer app.deinit();

    ctx.surface = try app.getSurface(gpu);
    defer wgpu.c.wgpuSurfaceRelease(ctx.surface);

    const adapter_options = wgpu.c.WGPURequestAdapterOptions{
        .compatibleSurface = ctx.surface,
    };
    wgpu.c.wgpuInstanceRequestAdapter(gpu, &adapter_options, handleRequestAdapter, &ctx);
    if (ctx.adapter == null) return error.WgpuAdapter;
    defer wgpu.c.wgpuAdapterRelease(ctx.adapter);

    wgpu.c.wgpuAdapterRequestDevice(ctx.adapter, null, handleRequestDevice, &ctx);
    if (ctx.device == null) return error.WgpuDevice;
    defer wgpu.c.wgpuDeviceRelease(ctx.device);

    const queue = wgpu.c.wgpuDeviceGetQueue(ctx.device) orelse return error.WgpuQueue;
    defer wgpu.c.wgpuQueueRelease(queue);

    const wgsl = @embedFile("demo.wgsl");
    const shader = try loadShader(ctx.device, "demo", wgsl);
    defer wgpu.c.wgpuShaderModuleRelease(shader);

    const pipeline_layout = wgpu.c.wgpuDeviceCreatePipelineLayout(
        ctx.device,
        &.{ .label = "pipeline_layout" },
    ) orelse return error.WgpuPipelineLayout;
    defer wgpu.c.wgpuPipelineLayoutRelease(pipeline_layout);

    var surface_capabilities = wgpu.c.WGPUSurfaceCapabilities{};
    wgpu.c.wgpuSurfaceGetCapabilities(ctx.surface, ctx.adapter, &surface_capabilities);
    defer wgpu.c.wgpuSurfaceCapabilitiesFreeMembers(surface_capabilities);

    const render_pipeline = wgpu.c.wgpuDeviceCreateRenderPipeline(ctx.device, &.{
        .label = "render_pipeline",
        .layout = pipeline_layout,
        .vertex = .{
            .module = shader,
            .entryPoint = "vs_main",
        },
        .fragment = &.{
            .module = shader,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = &.{
                .format = surface_capabilities.formats[0],
                .writeMask = wgpu.c.WGPUColorWriteMask_All,
            },
        },
        .primitive = .{
            .topology = wgpu.c.WGPUPrimitiveTopology_TriangleList,
        },
        .multisample = .{
            .count = 1,
            .mask = 0xFFFFFFFF,
        },
    }) orelse return error.WgpuRenderPipeline;
    defer wgpu.c.wgpuRenderPipelineRelease(render_pipeline);

    ctx.config = .{
        .device = ctx.device,
        .usage = wgpu.c.WGPUTextureUsage_RenderAttachment,
        .format = surface_capabilities.formats[0],
        .presentMode = wgpu.c.WGPUPresentMode_Fifo,
        .alphaMode = surface_capabilities.alphaModes[0],
    };

    updateWindowSize(app.window, &ctx);

    glfw.c.glfwSetWindowUserPointer(app.window, &ctx);
    _ = glfw.c.glfwSetFramebufferSizeCallback(app.window, handleGlfwFramebufferSize);
    while (glfw.c.glfwWindowShouldClose(app.window) == glfw.c.GLFW_FALSE) {
        glfw.c.glfwPollEvents();

        var maybe_texture: wgpu.c.WGPUSurfaceTexture = undefined;
        wgpu.c.wgpuSurfaceGetCurrentTexture(ctx.surface, &maybe_texture);
        defer if (maybe_texture.texture) |tex| wgpu.c.wgpuTextureRelease(tex);

        switch (maybe_texture.status) {
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_Success => {},
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_Timeout,
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_Outdated,
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_Lost,
            => {
                // Skip this frame, and re-configure surface.
                updateWindowSize(app.window, &ctx);
                continue;
            },
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_OutOfMemory,
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_DeviceLost,
            wgpu.c.WGPUSurfaceGetCurrentTextureStatus_Force32,
            => {
                log.err("wgpu get current texture [{any}]", .{maybe_texture.status});
                return error.WgpuTexture;
            },
            else => unreachable,
        }
        const texture = maybe_texture.texture orelse return error.WgpuTexture;

        const frame = wgpu.c.wgpuTextureCreateView(texture, null) orelse return error.WgpuFrame;
        defer wgpu.c.wgpuTextureViewRelease(frame);

        const command_encoder = wgpu.c.wgpuDeviceCreateCommandEncoder(ctx.device, &.{
            .label = "command_encoder",
        }) orelse return error.WgpuCommandEncoder;
        defer wgpu.c.wgpuCommandEncoderRelease(command_encoder);

        const render_pass_encoder = wgpu.c.wgpuCommandEncoderBeginRenderPass(
            command_encoder,
            &.{
                .label = "render_pass_encoder",
                .colorAttachmentCount = 1,
                .colorAttachments = &.{
                    .view = frame,
                    .loadOp = wgpu.c.WGPULoadOp_Clear,
                    .storeOp = wgpu.c.WGPUStoreOp_Store,
                    .depthSlice = wgpu.c.WGPU_DEPTH_SLICE_UNDEFINED,
                    .clearValue = .{
                        .r = 0.0,
                        .g = 1.0,
                        .b = 0.0,
                        .a = 1.0,
                    },
                },
            },
        ) orelse return error.WgpuRenderPassEncoder;
        defer wgpu.c.wgpuRenderPassEncoderRelease(render_pass_encoder);

        wgpu.c.wgpuRenderPassEncoderSetPipeline(render_pass_encoder, render_pipeline);
        wgpu.c.wgpuRenderPassEncoderDraw(render_pass_encoder, 3, 1, 0, 0);
        wgpu.c.wgpuRenderPassEncoderEnd(render_pass_encoder);

        const command_buffer = wgpu.c.wgpuCommandEncoderFinish(command_encoder, &.{
            .label = "command_buffer",
        }) orelse return error.WgpuCommandBuffer;
        defer wgpu.c.wgpuCommandBufferRelease(command_buffer);

        wgpu.c.wgpuQueueSubmit(queue, 1, &command_buffer);
        wgpu.c.wgpuSurfacePresent(ctx.surface);
    }
}
