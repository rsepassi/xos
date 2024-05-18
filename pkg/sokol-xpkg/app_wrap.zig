// Wrapper for a sokol app

const std = @import("std");
const c = @import("sokol_zig");

const userlib = @import("app");
comptime {
    makeSokolApp(userlib.App);
}

// >> Sokol lib wrap
// ============================================================================

const log = std.log.scoped(.sokol);

fn errorWrap(err: anytype, msg: []const u8) void {
    std.io.getStdErr().writer().print("error: {any}\n", .{err}) catch {};
    @panic(msg);
}

pub fn makeSokolApp(comptime AppT: type) void {
    SokolApp(AppT).declare();
}

fn SokolApp(comptime AppT: type) type {
    const CCtx = struct {
        const Self = @This();

        fn sokolGetCtx() *AppT {
            return @ptrCast(@alignCast(c.app.userdata()));
        }

        fn sokolOnInit() callconv(.C) void {
            log.debug("onInit", .{});
            const app: *AppT = Self.sokolGetCtx();
            if (@hasDecl(AppT, "onInit")) {
                app.onInit() catch |err| {
                    errorWrap(err, "onInit failed");
                };
            }
            if (!c.gfx.sg_isvalid()) errorWrap(error.SokolGfxInvalid, "onInit left sokol gfx in an invalid state");
        }

        fn sokolOnFrame() callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            if (@hasDecl(AppT, "onFrame")) app.onFrame() catch |err| {
                errorWrap(err, "onFrame failed");
            };
        }

        fn sokolOnCleanup() callconv(.C) void {
            log.debug("onCleanup", .{});
            const app: *AppT = Self.sokolGetCtx();
            if (@hasDecl(AppT, "deinit")) app.deinit();
            std.heap.c_allocator.destroy(app);
            log.debug("goodbye", .{});
        }

        fn sokolOnEvent(event: [*c]const c.app.Event) callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            if (@hasDecl(AppT, "onEvent")) app.onEvent(event.*) catch |err| {
                errorWrap(err, "onEvent failed");
            };
        }
    };

    const Main = struct {
        fn sokol_main(argc: c_int, argv: [*][*:0]u8) callconv(.C) c.app.Desc {
            log.debug("sokol_main", .{});
            _ = argc;
            _ = argv;
            return appDesc();
        }

        fn appDesc() c.app.Desc {
            var ctx: *AppT = std.heap.c_allocator.create(AppT) catch @panic("alloc failed");
            if (@hasDecl(AppT, "init")) {
                ctx.init() catch |err| {
                    std.io.getStdErr().writer().print("error: {any}\n", .{err}) catch {};
                    @panic("init failed");
                };
            } else {
                ctx.* = .{};
            }
            log.debug("App struct initialized", .{});

            const window_title = if (@hasDecl(AppT, "window_title")) AppT.window_title else "xos-sokol";

            return .{
                .init_cb = CCtx.sokolOnInit,
                .frame_cb = CCtx.sokolOnFrame,
                .cleanup_cb = CCtx.sokolOnCleanup,
                .event_cb = CCtx.sokolOnEvent,
                .user_data = ctx,
                .window_title = window_title,
                .enable_clipboard = true,
                .enable_dragndrop = true,
            };
        }

        pub fn declare() void {
            @export(sokol_main, .{ .name = "sokol_main", .linkage = .strong });
        }
    };

    return Main;
}
