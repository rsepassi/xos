const std = @import("std");
const c = @import("sokol_zig");

const log = std.log.scoped(.app);

pub const App = struct {
    const window_title = "Hello";

    pub fn init(self: *@This()) !void {
        _ = self;
    }

    pub fn deinit(self: @This()) void {
        _ = self;
    }

    pub fn onInit(self: @This()) !void {
        _ = self;
        log.debug("onInit user", .{});
        c.gfx.setup(.{
            .environment = appEnv(),
        });
        if (!c.gfx.sg_isvalid()) @panic("zonInit left sokol gfx in an invalid state");
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
        if (!c.gfx.sg_isvalid()) @panic("zonInit left sokol gfx in an invalid state");
    }

    pub fn onFrame(self: @This()) !void {
        _ = self;

        const screen_size: struct { w: f32, h: f32 } = .{
            .w = @floatFromInt(c.app.sapp_width()),
            .h = @floatFromInt(c.app.sapp_height()),
        };

        const str = "hello world!";
        c.debugtext.sdtx_canvas(screen_size.w / 2.0, screen_size.h / 2.0);
        c.debugtext.sdtx_color3b(0, 0, 0);
        c.debugtext.sdtx_origin(8, 2);
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

    pub fn onEvent(self: @This(), event: c.app.Event) !void {
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
                log.debug("event {s}", .{@tagName(event.type)});
            },
        }
    }
};

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
