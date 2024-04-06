const std = @import("std");
const sokol = @import("sokol.zig");
const text = @import("text.zig");

const log = std.log.scoped(.texthello);
pub const std_options = .{
    .log_level = .info,
};

const Resources = struct {
    const font = "CourierPrime-Regular.ttf";
};

const Ctx = struct {
    const Self = @This();

    pub const window_title = "texthello";
    const sokol_log = std.log.scoped(.sokol_app);

    timer: std.time.Timer,
    alloc: std.heap.GeneralPurposeAllocator(.{}),
    exepath: []const u8,
    resource_dir: std.fs.Dir,

    sg_initialized: bool = false,

    ft: text.FreeType,
    font: text.Font,
    textbuf: text.Buffer,

    charbuf: std.ArrayList(u8),
    charbuf_pos: usize = 0,
    frame_count: u64 = 0,

    pub fn init(self: *Self) !void {
        self.timer = try std.time.Timer.start();

        // Allocators
        self.alloc = .{};

        // Resources
        self.exepath = try std.fs.selfExePathAlloc(self.alloc.allocator());
        const exedir = std.fs.path.dirname(self.exepath) orelse return error.NoResourceDir;
        const alloc = self.alloc.allocator();
        const resource_dir_path = try std.fs.path.join(alloc, &.{ exedir, "resources" });
        defer alloc.free(resource_dir_path);
        self.resource_dir = try std.fs.cwd().openDir(resource_dir_path, .{});
        var font_path_buf: [std.fs.MAX_PATH_BYTES]u8 = undefined;
        const font_path = try self.resource_dir.realpath(Resources.font, &font_path_buf);
        font_path_buf[font_path.len] = 0;

        self.sg_initialized = false;

        // Text
        self.ft = try text.FreeType.init();
        self.font = try self.ft.font(.{ .path = @ptrCast(font_path) });
        self.textbuf = try text.Buffer.init();
        self.textbuf.addText("hello world");

        self.charbuf = std.ArrayList(u8).init(alloc);
    }

    pub fn deinit(self: *Self) void {
        self.charbuf.deinit();
        self.textbuf.deinit();
        self.font.deinit();
        self.ft.deinit();
        self.resource_dir.close();
        self.alloc.allocator().free(self.exepath);
        if (self.sg_initialized) sokol.c.sg_shutdown();
    }

    pub fn onInit(self: *Self) void {
        self.sg_initialized = true;
        const sg_desc = sokol.c.sg_desc{
            .environment = sokol.appEnv(),
            .logger = sokol.sgLogger(self),
        };
        sokol.c.sg_setup(&sg_desc);
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

                self.charbuf.insertSlice(self.charbuf_pos, utf8) catch @panic("insertSlice oom");
                self.charbuf_pos += utf8.len;
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
                const s = sokol.c.sapp_get_clipboard_string();
                log.info("{s} {s}", .{ @tagName(event.type), s });
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
            .MOUSE_MOVE,
            => {},
            .INVALID, .NUM => {
                log.err("got unexpected event {s}", .{@tagName(event.type)});
            },
        }
    }

    pub fn onFrame(self: *Self) void {
        defer self.frame_count += 1;
        if (self.frame_count > 0) return;

        log.info("first frame", .{});
        var shaped = self.font.shape(self.textbuf);
        var iter = shaped.iterator();

        var char_name: [256:0]u8 = undefined;
        while (iter.next() catch @panic("bad shaped glyph")) |*sglyph| {
            var glyph = sglyph.glyph;
            const bitmap = glyph.render() catch @panic("bad render");
            const name = glyph.name(&char_name) catch @panic("bad name");
            log.info("{s} ({d}, {d})\n", .{ name, bitmap.rows, bitmap.cols });
            bitmap.ascii(std.io.getStdErr().writer()) catch @panic("bad write");
        }

        // // Make an image the size of the screen
        // const frame_size = sokol.screenSize();
        // const bitmap = [_]u8{ 0, 0, 0, 0 };
        // var img_desc = sokol.c.sg_image_desc{
        //     .type = sokol.c.SG_IMAGETYPE_2D,
        //     .render_target = false,
        //     .width = frame_size.width,
        //     .height = frame_size.height,
        //     .num_slices = 1,
        //     .num_mipmaps = 1,
        //     .usage = sokol.c.SG_USAGE_DYNAMIC,
        //     .pixel_format = sokol.c.SG_PIXELFORMAT_RGBA8,
        //     .sample_count = 1,
        //     .data = std.mem.zeroes(sokol.c.sg_image_data),
        //     .label = "screen",
        // };
        // img_desc.data.subimage[0][0] = .{
        //     .ptr = &bitmap,
        //     .size = bitmap.len,
        // };
        // const img = sokol.c.sg_make_image(&img_desc);
        // _ = img;

        // // Pipeline
        // var pipe_desc = std.mem.zeroes(sokol.c.sg_pipeline_desc);
        // pipe_desc.primitive_type = sokol.c.SG_PRIMITIVETYPE_TRIANGLE_STRIP;
        // pipe_desc.label = "pipeline";
        // // sg_shader shader;
        // // sg_vertex_layout_state layout;
        // // sg_depth_state depth;
        // // sg_stencil_state stencil;
        // // int color_count;
        // // sg_color_target_state colors[SG_MAX_COLOR_ATTACHMENTS];
        // // sg_index_type index_type;
        // // int sample_count;
        // // sg_color blend_color;
        // // bool alpha_to_coverage_enabled;
        // // Defaults
        // // .cull_mode = sokol.SG_CULLMODE_NONE,
        // // .face_winding = sokol.SG_FACEWINDING_CW,
        // const pipeline = sokol.c.sg_make_pipeline(&pipe_desc);
        // _ = pipeline;

        // //        const bitmap: []const u8 = undefined;
        // //
        // //        createPipeline();
        // //
        // //const img_desc = sokol.sg_image_desc{
        // //            .width = 256,
        // //            .height = 256,
        // //            .pixel_format = SG_PIXELFORMAT_RGBA8,
        // //            .sample_count = 2,
        // //        };
        // //const img = sokol.sg_make_image(&img_desc);
        // //
        // //        const pass: sokol.sg_pass = .{
        // //            .action = .{
        // //            },
        // //            .swapchain = self.sokolSwapchain(),
        // //        };
        // //        sokol.sg_begin_pass(&pass);
        // //sokol.sg_apply_pipeline(pipe);
        // //
        // //// This represents the vertex data, where each quad has 4 vertices, and each vertex has a position (xy) and tex coord (uv)
        // //const vertices = [_]const f32{
        // //    // positions   // tex coords
        // //    -1.0f,  1.0f,   0.0f, 1.0f,
        // //     1.0f,  1.0f,   1.0f, 1.0f,
        // //    -1.0f, -1.0f,   0.0f, 0.0f,
        // //     1.0f, -1.0f,   1.0f, 0.0f,
        // //};
        // //
        // //const buf_desc = sokol.sg_buffer_desc{
        // //    .size = @sizeOf(vertices),
        // //    .content = vertices,
        // //};
        // //const vertex_buffer = sokol.sg_make_buffer(&buf_desc);
        // //
        // //
        // //        const bindings: sokol.sg_bindings = .{
        // //.vertex_buffers = &.{vertex_buffer},
        // //            .fs_images = &.{img},
        // //        };
        // //        sg_apply_bindings(&bindings);
        // //        sokol.sg_draw(0, 4, 1);
        // //        sokol.sg_end_pass();
        // //        sokol.sg_commit();
        // log.info("first frame done", .{});
    }

    pub fn onLog(self: Self, slog: sokol.Log) void {
        _ = self;
        sokol_log.info("sokol {any}", .{slog});
    }
};

comptime {
    sokol.App(Ctx).declare();
}

const unknown_char = "\xEF\xBF\xBD";
