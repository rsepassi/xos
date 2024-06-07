const std = @import("std");

const app = @import("app");
const gpu = @import("gpu");
const appgpu = @import("appgpu");
const twod = appgpu.twod;

const dummydata = @import("data.zig");

const DemoPipeline = @import("DemoPipeline.zig");
const ImagePipeline = @import("ImagePipeline.zig");
const SpritePipeline = @import("SpritePipeline.zig");

pub const std_options = .{
    .log_level = .debug,
};
const log = std.log.scoped(.app);

pub const App = @This();

pub const PipelineCtx = struct {
    gfx: appgpu.Gfx,
    allocator: std.mem.Allocator,
};

appctx: *app.Ctx,
allocator: std.heap.GeneralPurposeAllocator(.{}),
pipectx: PipelineCtx,
demo_pipeline: DemoPipeline,
image_pipeline: ImagePipeline,
sprite_pipeline: SpritePipeline,

pub fn appConfig() app.Config {
    return .{
        .window_title = "Hello",
        .window_size = .{ 640, 480 },
    };
}

pub fn init(self: *App, appctx: *app.Ctx) !void {
    // Assign to self for pointer stability
    self.allocator = std.heap.GeneralPurposeAllocator(.{}){};

    const gfx = try appgpu.defaultGfx(appctx);
    errdefer gfx.deinit();

    const pipectx = PipelineCtx{
        .gfx = gfx,
        .allocator = self.allocator.allocator(),
    };

    const demo_pipeline = try DemoPipeline.init(pipectx);
    errdefer demo_pipeline.deinit();

    const image_pipeline = try ImagePipeline.init(pipectx);
    errdefer image_pipeline.deinit();

    const sprite_pipeline = try SpritePipeline.init(pipectx);
    errdefer sprite_pipeline.deinit();

    self.* = .{
        .appctx = appctx,
        .pipectx = pipectx,
        .allocator = self.allocator,
        .demo_pipeline = demo_pipeline,
        .image_pipeline = image_pipeline,
        .sprite_pipeline = sprite_pipeline,
    };
}

pub fn deinit(self: *App) void {
    defer if (self.allocator.deinit() == .leak) log.err("leak!", .{});
    defer self.pipectx.gfx.deinit();
    defer self.demo_pipeline.deinit();
    defer self.image_pipeline.deinit();
    defer self.sprite_pipeline.deinit();
}

pub fn onEvent(self: *App, event: app.Event) !void {
    switch (event) {
        .start => {
            try self.render();
        },
        .resize => {
            self.pipectx.gfx.updateWindowSize();
            try self.render();
        },
        .char,
        => {},
    }
}

fn render(self: *App) !void {
    log.debug("render", .{});

    log.debug("imageA", .{});
    const imageA = dummydata.getImageData(.a);
    const pipeline_imageA = try ImagePipeline.PipelineImage.init(self.pipectx.gfx, imageA.size);
    defer pipeline_imageA.deinit();
    pipeline_imageA.writeImage(imageA);
    pipeline_imageA.writePos(.{ .x = 0, .y = imageA.size.height });
    const image_argsA = self.image_pipeline.makeArgs(pipeline_imageA);
    defer image_argsA.deinit();

    log.debug("imageB", .{});
    const imageB = dummydata.getImageData(.b);
    const pipeline_imageB = try ImagePipeline.PipelineImage.init(self.pipectx.gfx, imageB.size);
    defer pipeline_imageB.deinit();
    pipeline_imageB.writeImage(imageB);
    pipeline_imageB.writePos(.{ .x = 256, .y = imageB.size.height });
    const image_argsB = self.image_pipeline.makeArgs(pipeline_imageB);
    defer image_argsB.deinit();

    log.debug("spritesheet", .{});
    const spritesheet = dummydata.getSpriteSheet();
    const pipeline_spritesheet = try SpritePipeline.SpriteSheet.init(self.pipectx.gfx, spritesheet);
    defer pipeline_spritesheet.deinit();
    var sprite_locs = try SpritePipeline.SpriteLocs.init(self.pipectx, 100);
    defer sprite_locs.deinit();
    const box = twod.Rect.fromSize(.{ .width = 100, .height = 100 });
    try sprite_locs.write(&.{
        .{ .pos = box, .uv = box },
        .{ .pos = box.up(100), .uv = box.right(200) },
    });
    const sprite_args = self.sprite_pipeline.makeArgs(pipeline_spritesheet, sprite_locs);
    defer sprite_args.deinit();

    log.debug("gfx.render", .{});
    try self.pipectx.gfx.render(.{
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
            appgpu.Gfx.PipelineRun.init(&self.sprite_pipeline, &sprite_args, SpritePipeline.run),
        },
    });
}
