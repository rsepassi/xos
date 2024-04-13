const std = @import("std");

const twod = @import("twod.zig");
const sokol = @import("sokol.zig");
const text = @import("text.zig");
const clipboard = @import("clipboard.zig");

const log = std.log.scoped(.texthello);
pub const std_options = .{
    .log_level = .info,
};

const Resources = struct {
    const font = "CourierPrime-Regular.ttf";
};

const unknown_char = "\xEF\xBF\xBD";
const font_override: ?[:0]const u8 = null;
//"/Users/ryan/fonts/notofonts.github.io-noto-monthly-release-24.4.1/fonts/NotoSerif/googlefonts/ttf/NotoSerif-Regular.ttf";

comptime {
    sokol.App(Ctx).declare();
}

const Ctx = struct {
    const Self = @This();

    pub const window_title = "texthello";
    const sokol_log = std.log.scoped(.sokol_app);

    // System
    timer: std.time.Timer,
    alloc: std.heap.GeneralPurposeAllocator(.{}),
    resource_dir: std.fs.Dir,
    clipboard: clipboard.Clipboard,

    // Text
    ft: text.FreeType,
    font: text.Font,
    usertext: std.ArrayList(u8),
    textbuf: text.Buffer,

    // Graphics
    need_render: bool,
    sg_initialized: bool,
    atlas: text.FontAtlas,
    vertex_list: std.ArrayList(f32),
    gfx: sokol.AlphaTexturePipeline,
    frame_count: u64,
    render_count: u64,
    last_render_frame: u64,

    pub fn init(self: *Self) !void {
        self.timer = try std.time.Timer.start();

        self.alloc = .{};
        const alloc = self.alloc.allocator();

        self.resource_dir = blk: {
            const exepath = try std.fs.selfExePathAlloc(self.alloc.allocator());
            defer alloc.free(exepath);
            const exedir = std.fs.path.dirname(exepath) orelse return error.NoResourceDir;
            const resource_dir_path = try std.fs.path.join(alloc, &.{ exedir, "resources" });
            defer alloc.free(resource_dir_path);
            break :blk try std.fs.cwd().openDir(resource_dir_path, .{});
        };

        self.clipboard = try clipboard.Clipboard.init();

        // Text
        self.ft = try text.FreeType.init();
        self.font = blk: {
            var font_path_buf: [std.fs.MAX_PATH_BYTES]u8 = undefined;
            const font_path = blk_path: {
                var font_path: []const u8 = undefined;
                if (font_override) |p| {
                    std.mem.copyForwards(u8, &font_path_buf, p);
                    font_path = @ptrCast(font_path_buf[0..p.len]);
                } else {
                    font_path = try self.resource_dir.realpath(Resources.font, &font_path_buf);
                }
                font_path_buf[font_path.len] = 0;
                break :blk_path font_path;
            };

            break :blk try self.ft.font(.{
                .path = @ptrCast(font_path),
                .pxsize = 24,
            });
        };
        self.usertext = std.ArrayList(u8).init(self.alloc.allocator());
        try self.usertext.appendSlice("welcome to xos");
        self.textbuf = try text.Buffer.init();
        self.textbuf.addText(self.usertext.items);

        self.need_render = true;
        self.sg_initialized = false;
        self.atlas = try text.buildAsciiAtlas(self.alloc.allocator(), self.font);
        self.vertex_list = std.ArrayList(f32).init(self.alloc.allocator());
        self.gfx = undefined;
        self.frame_count = 0;
        self.render_count = 0;
        self.last_render_frame = 0;

        log.info("init done at {d}ms", .{self.timer.read() / std.time.ns_per_ms});
    }

    pub fn deinit(self: *Self) void {
        self.vertex_list.deinit();
        self.atlas.deinit();
        self.usertext.deinit();
        self.textbuf.deinit();
        self.font.deinit();
        self.ft.deinit();
        self.clipboard.deinit();
        self.resource_dir.close();
        if (self.sg_initialized) {
            self.gfx.deinit();
            sokol.c.sgp_shutdown();
            sokol.c.sg_shutdown();
        }
        const leaked = self.alloc.detectLeaks();
        log.info("leak check: {s}", .{if (leaked) "leaked!" else "ok"});
    }

    pub fn onInit(self: *Self) void {
        var sg_desc = sokol.c.sg_desc{
            .environment = sokol.appEnv(),
            .logger = sokol.sgLogger(self),
        };
        sokol.c.sg_setup(&sg_desc);
        if (!sokol.c.sg_isvalid()) @panic("sokol init");
        const sgp_desc: sokol.c.sgp_desc = .{};
        sokol.c.sgp_setup(&sgp_desc);
        if (!sokol.c.sgp_is_valid()) @panic("sokol gp init");
        self.gfx = sokol.AlphaTexturePipeline.init(
            .{ .height = self.atlas.size.height, .width = self.atlas.size.width },
        ) catch @panic("pipe init");
        self.sg_initialized = true;
        log.info("sokol init done at {d}ms", .{self.timer.read() / std.time.ns_per_ms});
    }

    pub fn onEvent(self: *Self, event: sokol.Event) void {
        switch (event.type) {
            .CHAR => {
                var utf8_buf: [4]u8 = undefined;
                const utf8 = event.charutf8(&utf8_buf) orelse blk: {
                    log.err("bad char code {d}", .{event.char_code});
                    break :blk unknown_char;
                };
                log.info("CHAR code={d} str={s}", .{ event.char_code, utf8 });

                if (std.ascii.isPrint(utf8[0])) {
                    self.usertext.appendSlice(utf8) catch @panic("no mem");
                    self.need_render = true;
                }
            },
            .KEY_UP, .KEY_DOWN => {
                log.info("{s} {s}", .{ @tagName(event.type), @tagName(event.key_code) });
                if (event.key_code == .Q and event.modifier(.SUPER)) {
                    sokol.c.sapp_request_quit();
                }
            },
            .MOUSE_UP, .MOUSE_DOWN => {
                log.info("{s} ({d}, {d})", .{ @tagName(event.type), event.mouse_x, event.mouse_y });
            },
            .MOUSE_SCROLL => {
                log.info("{s} ({d}, {d}) dxy=({d}, {d})", .{ @tagName(event.type), event.mouse_x, event.mouse_y, event.scroll_x, event.scroll_y });
            },
            .RESIZED => {
                log.info("{s} ({d}, {d})", .{ @tagName(event.type), event.framebuffer_width, event.framebuffer_height });
                self.gfx.update(.{ .screen_size = .{
                    .width = @intCast(event.framebuffer_width),
                    .height = @intCast(event.framebuffer_height),
                } });
                self.need_render = true;
            },
            .FILES_DROPPED => {
                const x = event.mouse_x;
                const y = event.mouse_y;
                const n = sokol.c.sapp_get_num_dropped_files();
                log.info("{s} n={d} ({d}, {d})", .{ @tagName(event.type), n, x, y });
                for (0..@intCast(n)) |i| {
                    const f = sokol.c.sapp_get_dropped_file_path(@intCast(i));
                    log.info("  {s}", .{f});
                }
            },
            .CLIPBOARD_PASTED => {
                if (self.clipboard.get()) |s| {
                    log.info("{s} str={s}", .{ @tagName(event.type), s });
                    self.usertext.appendSlice(s) catch @panic("no mem");
                    self.need_render = true;
                }
            },
            .QUIT_REQUESTED => {
                log.info("{s} goodbye", .{@tagName(event.type)});
            },
            // Just log event type for now
            .MOUSE_ENTER,
            .MOUSE_LEAVE,
            .ICONIFIED,
            .RESTORED,
            .FOCUSED,
            .UNFOCUSED,
            .SUSPENDED,
            .RESUMED,
            => {
                log.info("{s}", .{@tagName(event.type)});
            },
            // Just log number of touches for now
            .TOUCHES_BEGAN,
            .TOUCHES_MOVED,
            .TOUCHES_ENDED,
            .TOUCHES_CANCELLED,
            => {
                log.info("{s} n={d}", .{ @tagName(event.type), event.num_touches });
            },
            // Noisy, pass in silence
            .MOUSE_MOVE => {},
            .INVALID, .NUM => {
                log.err("got unexpected event {s}", .{@tagName(event.type)});
            },
        }
    }

    pub fn onFrame(self: *Self) void {
        self.onFrameSafe() catch |err| {
            log.err("frame fail {any}", .{err});
            switch (err) {
                else => @panic("frame fail"),
            }
        };
    }

    fn onFrameSafe(self: *Self) !void {
        defer self.frame_count += 1;
        if (!self.need_render) {
            // To account for double/triple buffering
            if (self.frame_count >= self.last_render_frame + 3)
                return;
        }
        defer self.need_render = false;
        defer self.atlas.needs_update = false;

        // Compute origin (bottom left of line)
        const screen = sokol.screen_rect();
        const screen_pad = twod.Rect{
            .tl = screen.tl.down(5).right(10),
            .br = screen.br.up(5).left(10),
        };
        const line_height: f32 = @floatFromInt(self.font.face.*.size.*.metrics.height >> 6);
        const origin: twod.Point = screen_pad.tl.down(line_height);
        var cursor: twod.Point = origin;

        // Text
        {
            self.textbuf.reset();
            self.textbuf.addText(self.usertext.items);
            var shaped = self.font.shape(self.textbuf);

            var iter = shaped.iterator();
            while (iter.next()) |char| : ({
                const x_advance: f32 = @floatFromInt(char.pos.x_advance >> 6);
                cursor = cursor.right(x_advance);
            }) {
                // Get atlas rect
                const info = self.atlas.info.get(char.info.codepoint) orelse blk: {
                    log.err("missing glyph idx={d}", .{char.info.codepoint});
                    break :blk self.atlas.info.get(self.font.glyphIdx('?')).?;
                };
                const tex_rect = info.quad;

                // Get screen rect
                const glyph_rect = blk: {
                    const ybear: f32 = @floatFromInt(info.info.horiBearingY >> 6);
                    const ybear_offset = tex_rect.height() - ybear;
                    const x_offset: f32 = @floatFromInt((char.pos.x_offset + info.info.horiBearingX) >> 6);
                    const y_offset: f32 = @as(f32, @floatFromInt((char.pos.y_offset) >> 6)) - ybear_offset;

                    const glyph_origin = blk2: {
                        var glyph_origin = cursor.right(x_offset).up(y_offset);
                        const glyph_br = cursor.right(tex_rect.width());
                        if (glyph_br.x > screen_pad.br.x) {
                            // New line
                            cursor = twod.Point{
                                .x = screen_pad.tl.x,
                                .y = cursor.down(line_height).y,
                            };
                            glyph_origin = cursor.right(x_offset).up(y_offset);
                        }
                        break :blk2 glyph_origin;
                    };

                    break :blk twod.Rect{
                        .tl = glyph_origin.up(tex_rect.height()),
                        .br = glyph_origin.right(tex_rect.width()),
                    };
                };

                // Add triangles
                try self.vertex_list.appendSlice(&sokol.getRectVertices(glyph_rect, tex_rect));
            }
        }

        // 2D drawing
        {
            // Setup
            const width: c_int = @intFromFloat(screen.width());
            const height: c_int = @intFromFloat(screen.height());
            sokol.c.sgp_begin(width, height);
            sokol.c.sgp_viewport(0, 0, width, height);
            sokol.c.sgp_project(0, screen.width(), screen.height(), 0);

            // Draw
            sokol.c.sgp_set_color(0.3, 0.3, 0.3, 1.0);
            sokol.c.sgp_draw_filled_rect(origin.x, origin.y - 7, cursor.x - origin.x, 2.0);
        }

        // Render pass
        {
            // Some notes on SG_LOADACTION_LOAD
            // On Metal (and probably others) there is a default double (or triple)
            // buffering happening, so when you say action=LOAD, you'll load one
            // of those buffers, which may be uninitialized. To have more control
            // over what you're actually loading, you should probably use an
            // attachment, render to it, and load it. How exactly that should all
            // work I don't know yet.

            const background_color = sokol.color(255, 255, 255, 1.0);
            const text_color = sokol.colorVec(0, 128, 0);

            var action = sokol.c.sg_pass_action{};
            action.colors[0] = .{
                .load_action = sokol.c.SG_LOADACTION_CLEAR,
                .store_action = sokol.c.SG_STOREACTION_STORE,
                .clear_value = background_color,
            };
            const pass = sokol.RenderPass.begin(action, null);
            defer pass.endAndCommit();

            // Text
            self.gfx.update(.{
                .vertices = self.vertex_list.items,
                .texture = if (self.atlas.needs_update) self.atlas.data else null,
                .color = text_color,
            });
            self.gfx.apply();

            // 2D
            sokol.c.sgp_flush();
            sokol.c.sgp_end();
        }

        self.vertex_list.clearRetainingCapacity();
        self.render_count += 1;
        self.last_render_frame = self.frame_count;
        if (self.render_count == 1) {
            log.info("first render at {d}ms", .{self.timer.read() / std.time.ns_per_ms});
        }
    }

    pub fn onLog(self: Self, slog: sokol.Log) void {
        _ = self;
        sokol_log.info("sokol {any}", .{slog});
        if (slog.level == .panic) @panic("sokol failed");
    }
};

// TODO:
// * Updating FontAtlas, caching
// * Index buffer?
// * Scrolling
// * Line wrap
// * Text selection
// * Text attributes (color, bold, italics, etc)
// * Emoji
// * Accessibility
