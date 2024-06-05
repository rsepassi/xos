const log = @import("std").log.scoped(.demo_pipeline);
const gpu = @import("gpu");
const DemoPipeline = @This();

pipeline: gpu.RenderPipeline,
vertex_buf: gpu.Buffer,

const Vertex = [2]f32;
const nvertices = 3;

pub fn init(device: gpu.Device, queue: gpu.Queue, texture_format: gpu.TextureFormat) !@This() {
    const vertex_data = [_]Vertex{
        // bl
        .{ -1, -1 },
        // br
        .{ 1, -1 },
        // tc
        .{ 0, 1 },
    };
    const vertex_buf = try device.createBuffer(&.{
        .label = "demo vertices",
        .size = @sizeOf(Vertex) * vertex_data.len,
        .usage = @intFromEnum(gpu.BufferUsage.CopyDst) | @intFromEnum(gpu.BufferUsage.Vertex),
        .mappedAtCreation = 0,
    });
    queue.writeBuffer(vertex_buf, 0, &vertex_data);

    const wgsl = @embedFile("demo.wgsl");
    const shader = try device.createShaderModule("demo", .{ .wgsl = wgsl });
    defer shader.deinit();

    const pipeline_layout = try device.createPipelineLayout(&.{});
    defer pipeline_layout.deinit();

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
                .attributeCount = 1,
                .attributes = &.{
                    .format = @intFromEnum(gpu.VertexFormat.Float32x2),
                    .shaderLocation = 0,
                    .offset = 0,
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
        .pipeline = demo_pipeline,
        .vertex_buf = vertex_buf,
    };
}

pub fn deinit(self: @This()) void {
    self.pipeline.deinit();
    self.vertex_buf.deinit();
}

pub usingnamespace gpu.RenderPipeline.Interface.mixin(@This());

pub fn run(self: @This(), pass: gpu.RenderPassEncoder) !void {
    log.debug("DemoPipeline.run", .{});
    pass.setPipeline(self.pipeline);
    pass.setVertexBuffer(.{
        .buf = self.vertex_buf,
        .size = @sizeOf(Vertex) * nvertices,
    });
    pass.draw(.{ .vertex_count = 3 });
}
