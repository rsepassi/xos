const std = @import("std");
const builtin = @import("builtin");

const log = std.log.scoped(.app);

pub const std_options = .{
    .log_level = .debug,
    .logFn = androidLogFn,
};

extern fn doAndroidLog(msg: [*:0]const u8) void;
var android_log_fn_buf: [2048]u8 = undefined;
pub fn androidLogFn(
    comptime level: std.log.Level,
    comptime scope: @TypeOf(.EnumLiteral),
    comptime format: []const u8,
    args: anytype,
) void {
    _ = level;
    _ = scope;
    const msg = std.fmt.bufPrintZ(&android_log_fn_buf, format, args) catch "<log message too long>";
    doAndroidLog(msg);
}

const wgpu = struct {
    const c = @cImport({
        @cInclude("webgpu.h");
        @cInclude("wgpu.h");
    });

    const LogLevel = enum(u32) {
        off = 0,
        err,
        warn,
        info,
        debug,
        trace,

        fn wrap(i: u32) LogLevel {
            return @enumFromInt(i);
        }
    };
};

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

fn updateWindowSize(ctx: *Ctx) void {
    _ = ctx;
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

const Ctx = struct {
    gpu: wgpu.c.WGPUInstance = null,
    adapter: wgpu.c.WGPUAdapter = null,
    surface: wgpu.c.WGPUSurface = null,
    device: wgpu.c.WGPUDevice = null,
    queue: wgpu.c.WGPUQueue = null,
    shader: wgpu.c.WGPUShaderModule = null,
    pipeline_layout: wgpu.c.WGPUPipelineLayout = null,
    surface_capabilities: wgpu.c.WGPUSurfaceCapabilities = .{},
    render_pipeline: wgpu.c.WGPURenderPipeline = null,
    config: wgpu.c.WGPUSurfaceConfiguration = .{},

    fn deinit(ctx: @This()) void {
        defer wgpu.c.wgpuInstanceRelease(ctx.gpu);
        defer wgpu.c.wgpuAdapterRelease(ctx.adapter);
        defer wgpu.c.wgpuSurfaceRelease(ctx.surface);
        defer wgpu.c.wgpuDeviceRelease(ctx.device);
        defer wgpu.c.wgpuQueueRelease(ctx.queue);
        defer wgpu.c.wgpuShaderModuleRelease(ctx.shader);
        defer wgpu.c.wgpuPipelineLayoutRelease(ctx.pipeline_layout);
        defer wgpu.c.wgpuSurfaceCapabilitiesFreeMembers(ctx.surface_capabilities);
        defer wgpu.c.wgpuRenderPipelineRelease(ctx.render_pipeline);
    }
};

var ctxglobal: *CtxGlobal = undefined;
const CtxGlobal = struct {
    ctx: Ctx,
};

fn wgpuLogCallback(level: wgpu.c.WGPULogLevel, message: [*c]const u8, userdata: ?*anyopaque) callconv(.C) void {
    _ = userdata;
    log.info("[wgpu {s}]: {s}", .{ @tagName(wgpu.LogLevel.wrap(level)), message });
}

const android = struct {
    fn provideNativeWindow(window: *anyopaque, width: i32, height: i32) callconv(.C) c_int {
        provideNativeWindowSafe(window, width, height) catch |err| {
            log.err("provideNativeWindow failed: {any}", .{err});
            return 1;
        };
        return 0;
    }

    fn provideNativeWindowSafe(window: *anyopaque, width: i32, height: i32) !void {
        wgpu.c.wgpuSetLogLevel(wgpu.c.WGPULogLevel_Trace);
        wgpu.c.wgpuSetLogCallback(wgpuLogCallback, null);

        ctxglobal = try std.heap.c_allocator.create(CtxGlobal);
        const ctx = &ctxglobal.ctx;

        try setupCtx(ctx, window, width, height);
        try xonFrame();
    }

    fn onFrame() callconv(.C) void {
        xonFrame() catch @panic("bad frame");
    }
};

comptime {
    @export(android.provideNativeWindow, .{ .name = "_xos_android_provide_native_window" });
    @export(android.onFrame, .{ .name = "_xos_android_frame" });
}

// TODO: errdefers?
fn setupCtx(ctx: *Ctx, window: *anyopaque, width: i32, height: i32) !void {
    var instance_desc = wgpu.c.WGPUInstanceExtras{
        .chain = .{
            .sType = wgpu.c.WGPUSType_InstanceExtras,
        },
        //.backends = wgpu.c.WGPUInstanceBackend_GL,
        .flags = wgpu.c.WGPUInstanceFlag_Debug | wgpu.c.WGPUInstanceFlag_Validation,
    };
    ctx.gpu = wgpu.c.wgpuCreateInstance(&.{
        .nextInChain = @ptrCast(&instance_desc),
    }) orelse return error.WgpuInit;
    log.debug("wgpu created", .{});

    var surface_desc = wgpu.c.WGPUSurfaceDescriptorFromAndroidNativeWindow{
        .chain = .{
            .sType = wgpu.c.WGPUSType_SurfaceDescriptorFromAndroidNativeWindow,
        },
        .window = window,
    };
    ctx.surface = wgpu.c.wgpuInstanceCreateSurface(ctx.gpu, &.{
        .nextInChain = @ptrCast(&surface_desc),
    }) orelse return error.WgpuSurface;
    log.debug("surface created", .{});

    const adapter_options = wgpu.c.WGPURequestAdapterOptions{
        .compatibleSurface = ctx.surface,
    };
    wgpu.c.wgpuInstanceRequestAdapter(ctx.gpu, &adapter_options, handleRequestAdapter, ctx);
    if (ctx.adapter == null) return error.WgpuAdapter;

    wgpu.c.wgpuAdapterRequestDevice(ctx.adapter, &.{
        .requiredLimits = &.{
            .limits = defaultLimits(),
        },
    }, handleRequestDevice, ctx);
    if (ctx.device == null) return error.WgpuDevice;

    ctx.queue = wgpu.c.wgpuDeviceGetQueue(ctx.device) orelse return error.WgpuQueue;

    const wgsl = @embedFile("demo.wgsl");
    ctx.shader = try loadShader(ctx.device, "demo", wgsl);

    ctx.pipeline_layout = wgpu.c.wgpuDeviceCreatePipelineLayout(
        ctx.device,
        &.{ .label = "pipeline_layout" },
    ) orelse return error.WgpuPipelineLayout;

    wgpu.c.wgpuSurfaceGetCapabilities(ctx.surface, ctx.adapter, &ctx.surface_capabilities);

    ctx.render_pipeline = wgpu.c.wgpuDeviceCreateRenderPipeline(ctx.device, &.{
        .label = "render_pipeline",
        .layout = ctx.pipeline_layout,
        .vertex = .{
            .module = ctx.shader,
            .entryPoint = "vs_main",
        },
        .fragment = &.{
            .module = ctx.shader,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = &.{
                .format = ctx.surface_capabilities.formats[0],
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

    ctx.config = .{
        .device = ctx.device,
        .usage = wgpu.c.WGPUTextureUsage_RenderAttachment,
        .format = ctx.surface_capabilities.formats[0],
        .presentMode = wgpu.c.WGPUPresentMode_Fifo,
        .alphaMode = ctx.surface_capabilities.alphaModes[0],
    };

    ctx.config.width = @intCast(width);
    ctx.config.height = @intCast(height);
    wgpu.c.wgpuSurfaceConfigure(ctx.surface, &ctx.config);
    log.debug("surface configured", .{});
}

fn xonFrame() !void {
    const ctx = &ctxglobal.ctx;

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
            updateWindowSize(ctx);
            return;
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

    wgpu.c.wgpuRenderPassEncoderSetPipeline(render_pass_encoder, ctx.render_pipeline);
    wgpu.c.wgpuRenderPassEncoderDraw(render_pass_encoder, 3, 1, 0, 0);
    wgpu.c.wgpuRenderPassEncoderEnd(render_pass_encoder);

    const command_buffer = wgpu.c.wgpuCommandEncoderFinish(command_encoder, &.{
        .label = "command_buffer",
    }) orelse return error.WgpuCommandBuffer;
    defer wgpu.c.wgpuCommandBufferRelease(command_buffer);

    wgpu.c.wgpuQueueSubmit(ctx.queue, 1, &command_buffer);
    wgpu.c.wgpuSurfacePresent(ctx.surface);
}

fn defaultLimits() wgpu.c.WGPULimits {
    return .{
        .maxTextureDimension1D = 8192,
        .maxTextureDimension2D = 8192,
        .maxTextureDimension3D = 2048,
        .maxTextureArrayLayers = 256,
        .maxBindGroups = 4,
        .maxBindingsPerBindGroup = 1000,
        .maxDynamicUniformBuffersPerPipelineLayout = 8,
        .maxDynamicStorageBuffersPerPipelineLayout = 4,
        .maxSampledTexturesPerShaderStage = 16,
        .maxSamplersPerShaderStage = 16,
        .maxStorageBuffersPerShaderStage = 8,
        .maxStorageTexturesPerShaderStage = 4,
        .maxUniformBuffersPerShaderStage = 12,
        .maxUniformBufferBindingSize = 64 << 10, // (64 KiB)
        .maxStorageBufferBindingSize = 128 << 20, // (128 MiB)
        .minUniformBufferOffsetAlignment = 256,
        .minStorageBufferOffsetAlignment = 256,
        .maxVertexBuffers = 8,
        .maxBufferSize = 256 << 20, // (256 MiB)
        .maxVertexAttributes = 16,
        .maxVertexBufferArrayStride = 2048,
        .maxInterStageShaderComponents = 60,
        .maxColorAttachments = 8,
        .maxColorAttachmentBytesPerSample = 32,
        .maxComputeWorkgroupStorageSize = 16384,
        .maxComputeInvocationsPerWorkgroup = 256,
        .maxComputeWorkgroupSizeX = 256,
        .maxComputeWorkgroupSizeY = 256,
        .maxComputeWorkgroupSizeZ = 64,
        .maxComputeWorkgroupsPerDimension = 65535,

        // not specified in the rust code
        // .maxBindGroupsPlusVertexBuffers
        // .maxInterStageShaderVariables

        // not listed in the c code
        // max_push_constant_size: 0,
        // max_non_sampler_bindings: 1_000_000,
    };
}
