const std = @import("std");
const c = @import("sokol_zig");

const App = struct {
    const window_title = "Hello";

    fn init(self: *@This()) !void {
        _ = self;
    }

    fn deinit(self: @This()) void {
        _ = self;
    }

    fn onInit(self: @This()) !void {
        _ = self;
        c.gfx.setup(.{
            .environment = appEnv(),
        });
        c.debugtext.setup(.{
            .fonts = .{
                c.debugtext.sdtx_font_kc853(),
                .{},
                .{},
                .{},
                .{},
                .{},
                .{},
                .{},
            },
        });
    }

    fn onFrame(self: @This()) !void {
        _ = self;
        const str = "hello world";
        c.debugtext.sdtx_color3b(0, 0, 0);
        c.debugtext.sdtx_puts(str);

        // Render pass
        {
            var action = c.gfx.PassAction{};
            action.colors[0] = .{
                .load_action = .CLEAR,
                .store_action = .DONTCARE,
                .clear_value = .{ .r = 255, .g = 255, .b = 255, .a = 1 },
            };
            c.gfx.beginPass(.{
                .action = action,
                .swapchain = appSwapchain(),
            });
            defer {
                c.gfx.endPass();
                c.gfx.commit();
            }

            c.debugtext.sdtx_draw();
        }
    }

    fn onEvent(self: @This(), event: c.app.Event) !void {
        _ = self;
        switch (event.type) {
            .INVALID,
            .KEY_DOWN,
            .KEY_UP,
            .CHAR,
            .MOUSE_DOWN,
            .MOUSE_UP,
            .MOUSE_SCROLL,
            .MOUSE_MOVE,
            .MOUSE_ENTER,
            .MOUSE_LEAVE,
            .TOUCHES_BEGAN,
            .TOUCHES_MOVED,
            .TOUCHES_ENDED,
            .TOUCHES_CANCELLED,
            .RESIZED,
            .ICONIFIED,
            .RESTORED,
            .FOCUSED,
            .UNFOCUSED,
            .SUSPENDED,
            .RESUMED,
            .QUIT_REQUESTED,
            .CLIPBOARD_PASTED,
            .FILES_DROPPED,
            .NUM,
            => {
                log.debug("event {s}\n", .{@tagName(event.type)});
            },
        }
    }
};

comptime {
    makeSokolApp(App);
}

// >> Sokol lib wrap
// ============================================================================

const log = std.log.scoped(.sokol);

fn errorWrap(err: anytype, msg: []const u8) void {
    std.io.getStdErr().writer().print("error: {any}\n", .{err}) catch {};
    @panic(msg);
}

fn makeSokolApp(comptime AppT: type) void {
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
            if (@hasDecl(AppT, "onInit")) app.onInit() catch |err| {
                errorWrap(err, "onInit failed");
            };
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

fn appEnv() c.gfx.Environment {
    log.debug("appEnv", .{});
    return .{
        .defaults = .{
            .color_format = @enumFromInt(c.app.sapp_color_format()),
            .depth_format = @enumFromInt(c.app.sapp_depth_format()),
            .sample_count = c.app.sapp_sample_count(),
        },
        .metal = .{
            .device = c.app.sapp_metal_get_device(),
        },
        .d3d11 = .{
            .device = c.app.sapp_d3d11_get_device(),
            .device_context = c.app.sapp_d3d11_get_device_context(),
        },
    };
}

fn appSwapchain() c.gfx.Swapchain {
    return .{
        .width = c.app.sapp_width(),
        .height = c.app.sapp_height(),
        .sample_count = c.app.sapp_sample_count(),
        .color_format = @enumFromInt(c.app.sapp_color_format()),
        .depth_format = @enumFromInt(c.app.sapp_depth_format()),
        .metal = .{
            .current_drawable = c.app.sapp_metal_get_current_drawable(),
            .depth_stencil_texture = c.app.sapp_metal_get_depth_stencil_texture(),
            .msaa_color_texture = c.app.sapp_metal_get_msaa_color_texture(),
        },
        .d3d11 = .{
            .render_view = c.app.sapp_d3d11_get_render_view(),
            .resolve_view = c.app.sapp_d3d11_get_resolve_view(),
            .depth_stencil_view = c.app.sapp_d3d11_get_depth_stencil_view(),
        },
        .gl = .{ .framebuffer = c.app.sapp_gl_get_framebuffer() },
    };
}
