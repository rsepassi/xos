const std = @import("std");

const app = @import("app");
const gpu = @import("gpu");
const appgpu = @import("appgpu");
const twod = appgpu.twod;
const DemoPipeline = @import("DemoPipeline.zig");
const ImagePipeline = @import("ImagePipeline.zig");

pub const std_options = .{
    .log_level = .debug,
};
const log = std.log.scoped(.app);

pub const App = @This();

pub const PipelineCtx = struct {
    gfx: appgpu.Gfx,
};

appctx: *app.Ctx,
allocator: std.heap.GeneralPurposeAllocator(.{}),
gfx: appgpu.Gfx,
demo_pipeline: DemoPipeline,
image_pipeline: ImagePipeline,

pub fn appConfig() app.Config {
    return .{
        .window_title = "Hello",
        .window_size = .{ 640, 480 },
    };
}

pub fn init(self: *App, appctx: *app.Ctx) !void {
    const gfx = try appgpu.defaultGfx(appctx);
    errdefer gfx.deinit();

    const pipectx = PipelineCtx{
        .gfx = gfx,
    };

    const demo_pipeline = try DemoPipeline.init(pipectx);
    errdefer demo_pipeline.deinit();

    const image_pipeline = try ImagePipeline.init(pipectx);
    errdefer image_pipeline.deinit();

    self.* = .{
        .appctx = appctx,
        .allocator = .{},
        .gfx = gfx,
        .demo_pipeline = demo_pipeline,
        .image_pipeline = image_pipeline,
    };
}

pub fn deinit(self: *App) void {
    defer if (self.allocator.deinit() == .leak) log.err("leak!", .{});
    defer self.gfx.deinit();
    defer self.demo_pipeline.deinit();
    defer self.image_pipeline.deinit();
}

pub fn onEvent(self: *App, event: app.Event) !void {
    switch (event) {
        .start => {
            const imageA = getImageData(.a);
            const pipeline_imageA = try ImagePipeline.PipelineImage.init(self.gfx, imageA.size);
            defer pipeline_imageA.deinit();
            pipeline_imageA.writeImage(imageA);
            pipeline_imageA.writePos(.{ .x = 0, .y = imageA.size.height });

            const imageB = getImageData(.b);
            const pipeline_imageB = try ImagePipeline.PipelineImage.init(self.gfx, imageB.size);
            defer pipeline_imageB.deinit();
            pipeline_imageB.writeImage(imageB);
            pipeline_imageB.writePos(.{ .x = 256, .y = imageB.size.height });

            const image_argsA = self.image_pipeline.makeArgs(pipeline_imageA);
            defer image_argsA.deinit();
            const image_argsB = self.image_pipeline.makeArgs(pipeline_imageB);
            defer image_argsB.deinit();

            try self.gfx.render(.{
                .load = .{ .Clear = .{
                    .r = 0.05,
                    .g = 0.05,
                    .b = 0.05,
                    .a = 1,
                } },
                .piperuns = &.{
                    appgpu.Gfx.PipelineRun.init(&self.demo_pipeline, &void{}, DemoPipeline.run),
                    appgpu.Gfx.PipelineRun.init(&self.image_pipeline, &image_argsA, ImagePipeline.run),
                    appgpu.Gfx.PipelineRun.init(&self.image_pipeline, &image_argsB, ImagePipeline.run),
                },
            });
        },
        .char,
        => {},
    }
}

// TODO:
// * screen size buffer and onResize updates
// * Pipelines:
//   * Text
//   * Sprite

fn getImageData(comptime which: enum { a, b }) twod.Image {
    // Create image data
    const width = 256;
    const height = 256;
    const pixels = comptime blk: {
        @setEvalBranchQuota(100000);
        var pixels: [width * height]twod.RGBA = undefined;
        for (0..width) |i| {
            for (0..height) |j| {
                const idx = j * width + i;
                if (which == .a) {
                    pixels[idx].r = @intCast(i);
                    pixels[idx].g = @intCast(j);
                    pixels[idx].b = 128;
                    pixels[idx].a = 255;
                } else {
                    pixels[idx].r = 0;
                    pixels[idx].g = 0;
                    pixels[idx].b = 255;
                    pixels[idx].a = 255;
                }
            }
        }
        break :blk pixels;
    };
    return .{ .data = &pixels, .size = .{ .width = width, .height = height } };
}
