const std = @import("std");

const app = @import("app");
const gpu = @import("gpu");
const appgpu = @import("appgpu");
const DemoPipeline = @import("DemoPipeline.zig");

pub const std_options = .{
    .log_level = .debug,
};
const log = std.log.scoped(.app);

pub const App = @This();

ctx: *app.Ctx,
allocator: std.heap.GeneralPurposeAllocator(.{}),
gfx: appgpu.Gfx,
demo_pipeline: DemoPipeline,

pub fn appConfig() app.Config {
    return .{
        .window_title = "Hello",
        .window_size = .{ 640, 480 },
    };
}

pub fn init(self: *App, ctx: *app.Ctx) !void {
    const gfx = try appgpu.defaultGfx(ctx);
    errdefer gfx.deinit();

    const demo_pipeline = try DemoPipeline.init(gfx.device, gfx.queue, @enumFromInt(gfx.surface_config.format), ctx);
    errdefer demo_pipeline.deinit();

    self.* = .{
        .ctx = ctx,
        .allocator = .{},
        .gfx = gfx,
        .demo_pipeline = demo_pipeline,
    };

    try self.gfx.render(&.{self.demo_pipeline.renderPipeline()});
}

pub fn deinit(self: *App) void {
    defer if (self.allocator.deinit() == .leak) log.err("leak!", .{});
    defer self.gfx.deinit();
    defer self.demo_pipeline.deinit();
}

pub fn onEvent(self: *App, event: app.Event) !void {
    _ = self;
    switch (event) {
        .start,
        .char,
        => {},
    }
}
