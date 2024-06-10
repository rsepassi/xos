const std = @import("std");

const app = @import("app");
const gpu = @import("gpu");
const appgpu = @import("appgpu");
const twod = app.twod;

const dummydata = @import("data.zig");
const text = @import("text.zig");

const DemoPipeline = @import("DemoPipeline.zig");
const ImagePipeline = @import("ImagePipeline.zig");
const SpritePipeline = @import("SpritePipeline.zig");
const GlyphPipeline = @import("GlyphPipeline.zig");

// TODO: resources
const font_path = "/Users/ryan/code/xos/pkg/texthello/CourierPrime-Regular.ttf";

pub const std_options = .{
    .log_level = .debug,
};
const log = std.log.scoped(.app);

pub const App = @This();

pub const PipelineCtx = struct {
    gfx: appgpu.Gfx,
    allocator: std.mem.Allocator,
};

// App
appctx: *app.Ctx,
allocator: std.heap.GeneralPurposeAllocator(.{}),
// Graphics
pipectx: PipelineCtx,
demo_pipeline: DemoPipeline,
image_pipeline: ImagePipeline,
sprite_pipeline: SpritePipeline,
glyph_pipeline: GlyphPipeline,
// Text
ft: text.FreeType,
font: text.Font,
atlas: text.FontAtlas,

pub fn appConfig() app.Config {
    return .{
        .window_title = "Hello",
        .window_size = .{ 640, 480 },
    };
}

pub fn init(self: *App, appctx: *app.Ctx) !void {
    // Assign to self for pointer stability
    self.allocator = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = self.allocator.allocator();

    const gfx = try appgpu.defaultGfx(appctx);
    errdefer gfx.deinit();

    const pipectx = PipelineCtx{
        .gfx = gfx,
        .allocator = allocator,
    };

    log.debug("DemoPipeline", .{});
    const demo_pipeline = try DemoPipeline.init(pipectx);
    errdefer demo_pipeline.deinit();

    log.debug("ImagePipeline", .{});
    const image_pipeline = try ImagePipeline.init(pipectx);
    errdefer image_pipeline.deinit();

    log.debug("SpritePipeline", .{});
    const sprite_pipeline = try SpritePipeline.init(pipectx);
    errdefer sprite_pipeline.deinit();

    log.debug("GlyphPipeline", .{});
    const glyph_pipeline = try GlyphPipeline.init(pipectx);
    errdefer glyph_pipeline.deinit();

    log.debug("FreeType", .{});
    const ft = try text.FreeType.init();
    errdefer ft.deinit();
    const font = try ft.font(.{
        .path = font_path,
        .pxsize = 40,
    });
    errdefer font.deinit();
    log.debug("ascii atlas", .{});
    const atlas = try text.buildAsciiAtlas(allocator, font);
    errdefer atlas.deinit();

    self.* = .{
        .appctx = appctx,
        .allocator = self.allocator,
        .pipectx = pipectx,
        .demo_pipeline = demo_pipeline,
        .image_pipeline = image_pipeline,
        .sprite_pipeline = sprite_pipeline,
        .glyph_pipeline = glyph_pipeline,
        .ft = ft,
        .font = font,
        .atlas = atlas,
    };
}

pub fn deinit(self: *App) void {
    defer if (self.allocator.deinit() == .leak) log.err("leak!", .{});
    defer self.pipectx.gfx.deinit();
    defer self.demo_pipeline.deinit();
    defer self.image_pipeline.deinit();
    defer self.sprite_pipeline.deinit();
    defer self.glyph_pipeline.deinit();
    defer self.ft.deinit();
    defer self.font.deinit();
    defer self.atlas.deinit();
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
    {
        const box = twod.Rect.fromSize(.{ .width = 100, .height = 100 });
        try sprite_locs.write(&.{
            .{ .pos = box, .uv = box },
            .{ .pos = box.up(100), .uv = box.right(200) },
        });
    }
    const sprite_args = self.sprite_pipeline.makeArgs(pipeline_spritesheet, &sprite_locs);
    defer sprite_args.deinit();

    log.debug("glyphs", .{});
    const pipeline_atlas = try GlyphPipeline.Atlas.init(self.pipectx.gfx, .{ .data = self.atlas.data, .size = self.atlas.size });
    defer pipeline_atlas.deinit();
    var glyph_locs = try GlyphPipeline.GlyphLocs.init(self.pipectx, 100);
    defer glyph_locs.deinit();
    {
        const colors = twod.color(twod.RGBf);
        const xinfo = self.atlas.info.get(self.font.glyphIdx('x')).?;
        const xbox = xinfo.quad;
        const abox = self.atlas.info.get(self.font.glyphIdx('a')).?.quad;
        const box = twod.Rect.fromSize(xbox.size());

        try glyph_locs.write(&.{
            .{ .pos = box, .uv = xbox, .color = colors.green() },
            .{ .pos = box.right(@floatFromInt(xinfo.info.advance_width)), .uv = abox, .color = colors.red() },
        });
    }
    const glyph_args = self.glyph_pipeline.makeArgs(pipeline_atlas, &glyph_locs);
    defer glyph_args.deinit();

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
            appgpu.Gfx.PipelineRun.init(&self.glyph_pipeline, &glyph_args, GlyphPipeline.run),
        },
    });
}
