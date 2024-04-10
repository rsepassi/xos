// A Sokol app

const std = @import("std");

const sokol = @cImport({
    @cInclude("sokol_app.h");
    @cInclude("sokol_gfx.h");
    @cInclude("spritealpha_shader.h");
});

pub const c = sokol;

pub fn App(comptime AppT: type) type {
    const CCtx = struct {
        const Self = @This();

        fn sokolGetCtx() *AppT {
            return @ptrCast(@alignCast(sokol.sapp_userdata()));
        }

        fn sokolOnInit() callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            app.onInit();
        }

        fn sokolOnFrame() callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            app.onFrame();
        }

        fn sokolOnCleanup() callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            if (@hasDecl(AppT, "deinit")) app.deinit();
            std.heap.c_allocator.destroy(app);
        }

        fn sokolOnEvent(cevent: [*c]const sokol.sapp_event) callconv(.C) void {
            const app: *AppT = Self.sokolGetCtx();
            const event: *const Event = @ptrCast(cevent);
            app.onEvent(event.*);
        }

        fn sokolAlloc(size: usize, user_data: ?*anyopaque) callconv(.C) ?*anyopaque {
            const app: *AppT = @ptrCast(@alignCast(user_data.?));
            const alloc = app.sokol_alloc;
            return cAlloc(alloc, size) catch null;
        }

        fn sokolFree(ptr_or_null: ?*anyopaque, user_data: ?*anyopaque) callconv(.C) void {
            const app: *AppT = @ptrCast(@alignCast(user_data.?));
            const alloc = app.sokol_alloc;
            return cFree(alloc, ptr_or_null);
        }
    };

    const Main = struct {
        fn sokol_main(argc: c_int, argv: [*][*:0]u8) callconv(.C) sokol.sapp_desc {
            _ = argc;
            _ = argv;
            const ctx = std.heap.c_allocator.create(AppT) catch @panic("alloc failed");
            ctx.init() catch @panic("init failed");

            return .{
                .init_cb = CCtx.sokolOnInit,
                .frame_cb = CCtx.sokolOnFrame,
                .cleanup_cb = CCtx.sokolOnCleanup,
                .event_cb = CCtx.sokolOnEvent,
                .user_data = ctx,
                .high_dpi = true,
                .alpha = true,
                .window_title = AppT.window_title,
                .enable_clipboard = true,
                .clipboard_size = 1 << 16,
                .enable_dragndrop = true,
                .max_dropped_files = 32,
                .max_dropped_file_path_length = std.fs.MAX_PATH_BYTES,
                .allocator = if (@hasField(AppT, "sokol_alloc")) .{
                    .alloc_fn = CCtx.sokolAlloc,
                    .free_fn = CCtx.sokolFree,
                    .user_data = ctx,
                } else .{},
                .logger = logger(ctx),
                .win32_console_utf8 = true,
                .ios_keyboard_resizes_canvas = true,
            };
        }

        pub fn declare() void {
            @export(sokol_main, .{ .name = "sokol_main", .linkage = .strong });
        }
    };

    return Main;
}

pub const Log = struct {
    pub const Level = enum { panic, err, warn, info };
    tag: [*c]const u8,
    level: Level,
    item_id: u32,
    message: ?[*c]const u8,
    line_nr: u32,
    filename: ?[*c]const u8,

    pub fn format(self: @This(), comptime fmt: []const u8, options: std.fmt.FormatOptions, writer: anytype) !void {
        _ = fmt;
        _ = options;

        var filename: [:0]const u8 = "";
        if (self.filename) |cfilename| {
            filename = cfilename[0..std.mem.len(cfilename) :0];
        }
        var message: [:0]const u8 = "";
        if (self.message) |cmessage| {
            message = cmessage[0..std.mem.len(cmessage) :0];
        }
        _ = try writer.print(
            "[{s} {s}:{d}] tag={s} id={d} {s}",
            .{
                @tagName(self.level),
                filename,
                self.line_nr,
                self.tag,
                self.item_id,
                message,
            },
        );
    }
};

pub const EventType = enum(i32) {
    INVALID,
    KEY_DOWN,
    KEY_UP,
    CHAR,
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_SCROLL,
    MOUSE_MOVE,
    MOUSE_ENTER,
    MOUSE_LEAVE,
    TOUCHES_BEGAN,
    TOUCHES_MOVED,
    TOUCHES_ENDED,
    TOUCHES_CANCELLED,
    RESIZED,
    ICONIFIED,
    RESTORED,
    FOCUSED,
    UNFOCUSED,
    SUSPENDED,
    RESUMED,
    QUIT_REQUESTED,
    CLIPBOARD_PASTED,
    FILES_DROPPED,
    NUM,
};

pub const Keycode = enum(i32) {
    INVALID = 0,
    SPACE = 32,
    APOSTROPHE = 39,
    COMMA = 44,
    MINUS = 45,
    PERIOD = 46,
    SLASH = 47,
    _0 = 48,
    _1 = 49,
    _2 = 50,
    _3 = 51,
    _4 = 52,
    _5 = 53,
    _6 = 54,
    _7 = 55,
    _8 = 56,
    _9 = 57,
    SEMICOLON = 59,
    EQUAL = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LEFT_BRACKET = 91,
    BACKSLASH = 92,
    RIGHT_BRACKET = 93,
    GRAVE_ACCENT = 96,
    WORLD_1 = 161,
    WORLD_2 = 162,
    ESCAPE = 256,
    ENTER = 257,
    TAB = 258,
    BACKSPACE = 259,
    INSERT = 260,
    DELETE = 261,
    RIGHT = 262,
    LEFT = 263,
    DOWN = 264,
    UP = 265,
    PAGE_UP = 266,
    PAGE_DOWN = 267,
    HOME = 268,
    END = 269,
    CAPS_LOCK = 280,
    SCROLL_LOCK = 281,
    NUM_LOCK = 282,
    PRINT_SCREEN = 283,
    PAUSE = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_DECIMAL = 330,
    KP_DIVIDE = 331,
    KP_MULTIPLY = 332,
    KP_SUBTRACT = 333,
    KP_ADD = 334,
    KP_ENTER = 335,
    KP_EQUAL = 336,
    LEFT_SHIFT = 340,
    LEFT_CONTROL = 341,
    LEFT_ALT = 342,
    LEFT_SUPER = 343,
    RIGHT_SHIFT = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT = 346,
    RIGHT_SUPER = 347,
    MENU = 348,
};

pub const AndroidTooltype = enum(i32) {
    UNKNOWN = 0,
    FINGER = 1,
    STYLUS = 2,
    MOUSE = 3,
};

pub const Touchpoint = extern struct {
    identifier: usize = 0,
    pos_x: f32 = 0.0,
    pos_y: f32 = 0.0,
    android_tooltype: AndroidTooltype = .UNKNOWN,
    changed: bool = false,
};

pub const Mousebutton = enum(i32) {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2,
    INVALID = 256,
};
const modifier_shift = 1;
const modifier_ctrl = 2;
const modifier_alt = 4;
const modifier_super = 8;
const modifier_lmb = 256;
const modifier_rmb = 512;
const modifier_mmb = 1024;
pub const Modifier = enum {
    SHIFT,
    CTRL,
    ALT,
    SUPER,
    LEFT_MOUSE,
    RIGHT_MOUSE,
    MIDDLE_MOUSE,
};

pub const Event = extern struct {
    frame_count: u64 = 0,
    type: EventType = .INVALID,
    key_code: Keycode = .INVALID,
    char_code: u32 = 0,
    key_repeat: bool = false,
    modifiers: u32 = 0,
    mouse_button: Mousebutton = .LEFT,
    mouse_x: f32 = 0.0,
    mouse_y: f32 = 0.0,
    mouse_dx: f32 = 0.0,
    mouse_dy: f32 = 0.0,
    scroll_x: f32 = 0.0,
    scroll_y: f32 = 0.0,
    num_touches: i32 = 0,
    touches: [8]Touchpoint = [_]Touchpoint{.{}} ** 8,
    window_width: i32 = 0,
    window_height: i32 = 0,
    framebuffer_width: i32 = 0,
    framebuffer_height: i32 = 0,

    pub fn modifier(self: @This(), mod: Modifier) bool {
        const code: u32 = switch (mod) {
            .SHIFT => modifier_shift,
            .CTRL => modifier_ctrl,
            .ALT => modifier_alt,
            .SUPER => modifier_super,
            .LEFT_MOUSE => modifier_lmb,
            .RIGHT_MOUSE => modifier_rmb,
            .MIDDLE_MOUSE => modifier_mmb,
        };
        return self.modifiers & code != 0;
    }

    pub fn charutf8(self: @This(), buffer: *[4]u8) ?[]u8 {
        const code = self.char_code;
        if (code <= 0x7F) {
            buffer[0] = @intCast(code);
            return buffer[0..1];
        }
        if (code <= 0x7FF) {
            buffer[0] = @intCast(0xC0 | (code >> 6)); // 110xxxxx
            buffer[1] = @intCast(0x80 | (code & 0x3F)); // 10xxxxxx
            return buffer[0..2];
        }
        if (code <= 0xFFFF) {
            buffer[0] = @intCast(0xE0 | (code >> 12)); // 1110xxxx
            buffer[1] = @intCast(0x80 | ((code >> 6) & 0x3F)); // 10xxxxxx
            buffer[2] = @intCast(0x80 | (code & 0x3F)); // 10xxxxxx
            return buffer[0..3];
        }
        if (code <= 0x10FFFF) {
            buffer[0] = @intCast(0xF0 | (code >> 18)); // 11110xxx
            buffer[1] = @intCast(0x80 | ((code >> 12) & 0x3F)); // 10xxxxxx
            buffer[2] = @intCast(0x80 | ((code >> 6) & 0x3F)); // 10xxxxxx
            buffer[3] = @intCast(0x80 | (code & 0x3F)); // 10xxxxxx
            return buffer[0..4];
        }
        return null;
    }
};

// malloc/realloc/free compatible allocation:
//
// Allocations with requested size size are structed as:
//
// header: {
//   true_size: usize = size+sizeOf(header)
// }
// buf: [size]u8
//
// buf is returned to C code
fn cAlloc(alloc: std.mem.Allocator, size: usize) ![*]u8 {
    const true_size = size + @sizeOf(usize);
    const buf = try alloc.alloc(u8, true_size);
    const out = buf.ptr + @sizeOf(usize);
    const true_size_buf: [*]u8 = @ptrCast(@constCast(&true_size));
    std.mem.copyForwards(u8, buf, true_size_buf[0..@sizeOf(usize)]);
    return out;
}

fn cRealloc(alloc: std.mem.Allocator, ptr_or_null: ?*anyopaque, new_size: usize) ![*]u8 {
    const ptr: [*]u8 = if (ptr_or_null) |p| @ptrCast(p) else {
        return try cAlloc(alloc, new_size);
    };
    const head = ptr - @sizeOf(usize);
    const true_size: *usize = @ptrCast(@alignCast(head));

    const new_true_size = new_size + @sizeOf(usize);
    const newbuf = try alloc.realloc(head[0..true_size.*], new_true_size);
    const out = newbuf.ptr + @sizeOf(usize);
    const new_true_size_buf: [*]u8 = @ptrCast(@constCast(&new_true_size));
    std.mem.copyForwards(u8, newbuf, new_true_size_buf[0..@sizeOf(usize)]);
    return out;
}

fn cFree(alloc: std.mem.Allocator, ptr_or_null: ?*anyopaque) void {
    const ptr: [*]u8 = if (ptr_or_null) |p| @ptrCast(p) else return;
    const head = ptr - @sizeOf(usize);
    const true_size: *usize = @ptrCast(@alignCast(head));
    alloc.free(head[0..true_size.*]);
}

pub fn appEnv() sokol.sg_environment {
    return .{
        .defaults = .{
            .color_format = @intCast(sokol.sapp_color_format()),
            .depth_format = @intCast(sokol.sapp_depth_format()),
            .sample_count = @intCast(sokol.sapp_sample_count()),
        },
        .metal = .{
            .device = sokol.sapp_metal_get_device(),
        },
        .d3d11 = .{
            .device = sokol.sapp_d3d11_get_device(),
            .device_context = sokol.sapp_d3d11_get_device_context(),
        },
        .wgpu = .{
            .device = sokol.sapp_wgpu_get_device(),
        },
    };
}

pub const Quad = struct {
    tl: Point2D,
    br: Point2D,

    pub fn height(self: @This()) f32 {
        return self.tl.y - self.br.y;
    }

    pub fn width(self: @This()) f32 {
        return self.br.x - self.tl.x;
    }
};

pub const Point2D = struct {
    x: f32,
    y: f32,

    pub fn down(self: @This(), delta: f32) Point2D {
        return .{ .x = self.x, .y = self.y - delta };
    }
    pub fn up(self: @This(), delta: f32) Point2D {
        return .{ .x = self.x, .y = self.y + delta };
    }
    pub fn left(self: @This(), delta: f32) Point2D {
        return .{ .x = self.x - delta, .y = self.y };
    }
    pub fn right(self: @This(), delta: f32) Point2D {
        return .{ .x = self.x + delta, .y = self.y };
    }
};

pub fn screen() Quad {
    const width: f32 = @floatFromInt(sokol.sapp_width());
    const height: f32 = @floatFromInt(sokol.sapp_height());
    return .{
        .tl = .{ .x = 0, .y = height },
        .br = .{ .x = width, .y = 0 },
    };
}

pub fn swapchain() sokol.sg_swapchain {
    return .{
        .width = sokol.sapp_width(),
        .height = sokol.sapp_height(),
        .sample_count = sokol.sapp_sample_count(),
        .color_format = @intCast(sokol.sapp_color_format()),
        .depth_format = @intCast(sokol.sapp_depth_format()),
        .metal = .{
            .current_drawable = sokol.sapp_metal_get_current_drawable(),
            .depth_stencil_texture = sokol.sapp_metal_get_depth_stencil_texture(),
            .msaa_color_texture = sokol.sapp_metal_get_msaa_color_texture(),
        },
        .d3d11 = .{
            .render_view = sokol.sapp_d3d11_get_render_view(),
            .resolve_view = sokol.sapp_d3d11_get_resolve_view(),
            .depth_stencil_view = sokol.sapp_d3d11_get_depth_stencil_view(),
        },
        .wgpu = .{
            .render_view = sokol.sapp_wgpu_get_render_view(),
            .resolve_view = sokol.sapp_wgpu_get_resolve_view(),
            .depth_stencil_view = sokol.sapp_wgpu_get_depth_stencil_view(),
        },
        .gl = .{ .framebuffer = sokol.sapp_gl_get_framebuffer() },
    };
}

const ScreenSize = struct {
    width: c_int,
    height: c_int,
};

pub fn sgLogger(app: anytype) c.sg_logger {
    return loggerInternal(app, c.sg_logger);
}

pub fn logger(app: anytype) c.sapp_logger {
    return loggerInternal(app, c.sapp_logger);
}

pub fn loggerInternal(app: anytype, comptime T: type) T {
    const X = struct {
        fn sokolLogC(tag: [*c]const u8, log_level: u32, log_item_id: u32, message_or_null: [*c]const u8, line_nr: u32, filename_or_null: [*c]const u8, user_data: ?*anyopaque) callconv(.C) void {
            const app_: *@TypeOf(app) = @ptrCast(@alignCast(user_data.?));
            app_.*.onLog(.{
                .tag = tag,
                .level = switch (log_level) {
                    0 => .panic,
                    1 => .err,
                    2 => .warn,
                    3 => .info,
                    else => unreachable,
                },
                .item_id = log_item_id,
                .message = message_or_null,
                .line_nr = line_nr,
                .filename = filename_or_null,
            });
        }
    };
    return .{
        .func = X.sokolLogC,
        .user_data = @ptrCast(@constCast(app)),
    };
}

pub fn color(r: u8, g: u8, b: u8, a: f32) c.sg_color {
    return .{
        .r = @as(f32, @floatFromInt(r)) / 255.0,
        .g = @as(f32, @floatFromInt(g)) / 255.0,
        .b = @as(f32, @floatFromInt(b)) / 255.0,
        .a = a,
    };
}

pub fn colorVec(r: u8, g: u8, b: u8) [3]f32 {
    return .{
        @as(f32, @floatFromInt(r)) / 255.0,
        @as(f32, @floatFromInt(g)) / 255.0,
        @as(f32, @floatFromInt(b)) / 255.0,
    };
}
