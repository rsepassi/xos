const std = @import("std");
const app = @import("app");
const gpu = @import("gpu");
const DemoPipeline = @This();

const log = std.log.scoped(.demo_pipeline);

ctx: *app.Ctx,
queue: gpu.Queue,
vertex_buf: gpu.Buffer,
ssize_buf: gpu.Buffer,
bind_group: gpu.BindGroup,
pipeline: gpu.RenderPipeline,

const ScreenSize = [2]f32;
const Vertex = extern struct {
    pos: [2]f32,
    color: [3]f32,
};
const nvertices = 6;

pub fn init(device: gpu.Device, queue: gpu.Queue, texture_format: gpu.TextureFormat, ctx: *app.Ctx) !@This() {
    const vertex_data = [_]Vertex{
        .{ .pos = .{ 0, 0 }, .color = .{ 1, 0, 0 } },
        .{ .pos = .{ 640, 0 }, .color = .{ 0, 1, 0 } },
        .{ .pos = .{ 320, 480 }, .color = .{ 0, 0, 1 } },
    };
    const vertex_buf = try device.createBuffer(&.{
        .label = "demo vertices",
        .size = @sizeOf(Vertex) * vertex_data.len,
        .usage = @intFromEnum(gpu.BufferUsage.CopyDst) | @intFromEnum(gpu.BufferUsage.Vertex),
        .mappedAtCreation = 0,
    });
    queue.writeBuffer(vertex_buf, 0, &vertex_data);

    const ssize_buf = try device.createBuffer(&.{
        .label = "demo screen size",
        .size = @sizeOf(ScreenSize),
        .usage = @intFromEnum(gpu.BufferUsage.CopyDst) | @intFromEnum(gpu.BufferUsage.Uniform),
        .mappedAtCreation = 0,
    });

    const wgsl = @embedFile("demo.wgsl");
    const shader = try device.createShaderModule("demo", .{ .wgsl = wgsl });
    defer shader.deinit();

    const bind_layout = device.createBindGroupLayout(&.{
        .entryCount = 1,
        .entries = &.{
            .binding = 0,
            .visibility = @intFromEnum(gpu.ShaderStage.Vertex),
            .buffer = .{
                .type = @intFromEnum(gpu.BufferBindingType.Uniform),
                .minBindingSize = @sizeOf(ScreenSize),
            },
        },
    });
    defer bind_layout.deinit();
    const pipeline_layout = try device.createPipelineLayout(&.{
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &bind_layout.ptr,
    });
    defer pipeline_layout.deinit();

    const bind_group = device.createBindGroup(&.{
        .layout = bind_layout.ptr,
        .entryCount = 1,
        .entries = &.{
            .binding = 0,
            .buffer = ssize_buf.ptr,
            .offset = 0,
            .size = @sizeOf(ScreenSize),
        },
    });

    const demo_pipeline = try device.createRenderPipeline(&.{
        .label = "demo_pipeline",
        .layout = pipeline_layout.ptr,
        .vertex = .{
            .module = shader.ptr,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = &.{
                .arrayStride = @sizeOf(Vertex),
                .stepMode = @intFromEnum(gpu.VertexStepMode.Vertex),
                .attributeCount = 2,
                .attributes = &[_]gpu.c.WGPUVertexAttribute{
                    .{
                        .format = @intFromEnum(gpu.VertexFormat.Float32x2),
                        .shaderLocation = 0,
                        .offset = @offsetOf(Vertex, "pos"),
                    },
                    .{
                        .format = @intFromEnum(gpu.VertexFormat.Float32x3),
                        .shaderLocation = 1,
                        .offset = @offsetOf(Vertex, "color"),
                    },
                },
            },
        },
        .fragment = &.{
            .module = shader.ptr,
            .entryPoint = "fs_main",
            .targetCount = 1,
            .targets = &.{
                .format = @intFromEnum(texture_format),
                .blend = &.{
                    .color = .{
                        .operation = @intFromEnum(gpu.BlendOperation.Add),
                        .srcFactor = @intFromEnum(gpu.BlendFactor.SrcAlpha),
                        .dstFactor = @intFromEnum(gpu.BlendFactor.OneMinusSrcAlpha),
                    },
                    .alpha = .{
                        .operation = @intFromEnum(gpu.BlendOperation.Add),
                        .srcFactor = @intFromEnum(gpu.BlendFactor.Zero),
                        .dstFactor = @intFromEnum(gpu.BlendFactor.One),
                    },
                },
                .writeMask = @intFromEnum(gpu.ColorWriteMask.All),
            },
        },
        .primitive = .{
            .topology = @intFromEnum(gpu.PrimitiveTopology.TriangleList),
            .stripIndexFormat = @intFromEnum(gpu.IndexFormat.Undefined),
        },
        .multisample = .{
            .count = 1,
            .mask = 0xFFFFFFFF,
            .alphaToCoverageEnabled = 0,
        },
    });
    return .{
        .ctx = ctx,
        .queue = queue,
        .pipeline = demo_pipeline,
        .vertex_buf = vertex_buf,
        .ssize_buf = ssize_buf,
        .bind_group = bind_group,
    };
}

pub fn deinit(self: @This()) void {
    defer self.vertex_buf.deinit();
    defer self.ssize_buf.deinit();
    defer self.bind_group.deinit();
    defer self.pipeline.deinit();
}

pub usingnamespace gpu.RenderPipeline.Interface.mixin(@This());

pub fn run(self: @This(), pass: gpu.RenderPassEncoder) !void {
    log.debug("DemoPipeline.run", .{});
    const ssize = self.ctx.getWindowSize();
    self.queue.writeBuffer(self.ssize_buf, 0, &ScreenSize{
        @floatFromInt(ssize.width),
        @floatFromInt(ssize.height),
    });
    pass.setPipeline(self.pipeline);
    pass.setBindGroup(.{
        .group = self.bind_group,
    });
    pass.setVertexBuffer(.{
        .buf = self.vertex_buf,
        .size = @sizeOf(Vertex) * nvertices,
    });
    pass.draw(.{ .vertex_count = nvertices });
}
