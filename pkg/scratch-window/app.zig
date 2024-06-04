const std = @import("std");

const app = @import("app");
const gpu = @import("gpu");
const appgpu = @import("appgpu");

pub const std_options = .{
    .log_level = .debug,
};
pub const App = @This();

const log = std.log.scoped(.app);

const AppAllocator = std.heap.GeneralPurposeAllocator(.{});

ctx: *app.Ctx,
allocator: AppAllocator,
gpu_instance: gpu.Instance,
surface: gpu.Surface,
surface_config: gpu.Surface.Config,
device: gpu.Device,
queue: gpu.Queue,
shader: gpu.ShaderModule,
pipeline: gpu.RenderPipeline,

pub fn appConfig() app.Config {
    return .{
        .window_title = "Hello",
        .window_size = .{ 640, 480 },
    };
}

pub fn init(self: *App, ctx: *app.Ctx) !void {
    const gpu_instance = try gpu.Instance.init();
    errdefer gpu_instance.deinit();
    const surface = try appgpu.getSurface(gpu_instance, ctx);
    errdefer surface.deinit();

    const adapter = try gpu_instance.requestAdapter(&.{
        .compatibleSurface = surface.ptr,
    });
    defer adapter.deinit();

    const surface_capabilities = surface.getCapabilities(adapter);
    defer surface_capabilities.deinit();

    const device = try adapter.requestDevice(null);
    errdefer device.deinit();
    const queue = try device.getQueue();
    errdefer queue.deinit();

    const surface_config = gpu.Surface.Config{
        .device = device.ptr,
        .usage = @intFromEnum(gpu.TextureUsage.RenderAttachment),
        .format = surface_capabilities.formats[0],
        .presentMode = @intFromEnum(gpu.PresentMode.Fifo),
        .alphaMode = surface_capabilities.alphaModes[0],
    };

    const wgsl = @embedFile("demo.wgsl");
    const shader = try device.createShaderModule("demo", .{ .wgsl = wgsl });
    errdefer shader.deinit();

    const pipeline_layout = try device.createPipelineLayout(&.{});
    defer pipeline_layout.deinit();

    const pipeline = try device.createRenderPipeline(&.{
        .label = "render_pipeline",
        .layout = pipeline_layout.ptr,
        .vertex = .{
            .module = shader.ptr,
            .entryPoint = "vs_main",
        },
        .fragment = &.{
            .module = shader.ptr,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = &.{
                .format = surface_capabilities.formats[0],
                .writeMask = @intFromEnum(gpu.ColorWriteMask.All),
            },
        },
        .primitive = .{
            .topology = @intFromEnum(gpu.PrimitiveTopology.TriangleList),
        },
        .multisample = .{
            .count = 1,
            .mask = 0xFFFFFFFF,
        },
    });
    errdefer pipeline.deinit();

    self.* = .{
        .ctx = ctx,
        .allocator = .{},
        .gpu_instance = gpu_instance,
        .surface = surface,
        .surface_config = surface_config,
        .device = device,
        .queue = queue,
        .shader = shader,
        .pipeline = pipeline,
    };

    self.updateWindowSize();
    try self.render();
}

pub fn deinit(self: *App) void {
    defer if (self.allocator.deinit() == .leak) log.err("leak!", .{});
    defer self.gpu_instance.deinit();
    defer self.surface.deinit();
    defer self.device.deinit();
    defer self.queue.deinit();
    defer self.shader.deinit();
    defer self.pipeline.deinit();
}

pub fn onEvent(self: *App, event: app.Event) !void {
    _ = self;
    _ = event;
}

fn updateWindowSize(self: *App) void {
    const window_size = self.ctx.getWindowSize();
    self.surface_config.width = window_size.width;
    self.surface_config.height = window_size.height;
    self.surface.configure(&self.surface_config);
}

fn render(self: *App) !void {
    const texture = try self.surface.getCurrentTexture();
    defer texture.deinit();

    const view = try texture.createView(null);
    defer view.deinit();

    const command_encoder = try self.device.createCommandEncoder(null);
    defer command_encoder.deinit();

    const pass = try command_encoder.beginRenderPass(
        &.{
            .label = "render_pass_encoder",
            .colorAttachmentCount = 1,
            .colorAttachments = &.{
                .view = view.ptr,
                .loadOp = @intFromEnum(gpu.LoadOp.Clear),
                .storeOp = @intFromEnum(gpu.StoreOp.Store),
                .depthSlice = gpu.DepthSliceUndefined,
                .clearValue = .{
                    .r = 0.0,
                    .g = 1.0,
                    .b = 0.0,
                    .a = 1.0,
                },
            },
        },
    );
    defer pass.deinit();

    pass.setPipeline(self.pipeline);
    pass.draw(.{ .vertex_count = 3 });
    pass.end();

    const command_buffer = try command_encoder.finish(null);
    defer command_buffer.deinit();

    self.queue.submit(1, command_buffer);
    self.surface.present();
}
