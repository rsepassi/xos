const std = @import("std");

const ctext = @cImport({
    @cInclude("freetype/freetype.h");
    @cInclude("freetype/ftmodapi.h");
    @cInclude("freetype/ftglyph.h");
    @cInclude("harfbuzz/hb.h");
    @cInclude("harfbuzz/hb-ft.h");
});

const icu = @cImport({
    @cInclude("unicode/ustring.h");
});

const sokol = @cImport({
    @cInclude("sokol_app.h");
    @cInclude("sokol_gfx.h");
});

const log = std.log.scoped(.texthello);
pub const std_options = .{
    .log_level = .info,
};

const Resources = struct {
    const font = "CourierPrime-Regular.ttf";
};

const Ctx = struct {
    const Self = @This();

    const sokol_log = std.log.scoped(.sokol_app);

    timer: std.time.Timer,
    alloc: std.heap.GeneralPurposeAllocator(.{}),
    sokol_alloc: std.heap.GeneralPurposeAllocator(.{}),
    ft_alloc: std.heap.GeneralPurposeAllocator(.{}),
    exepath: []const u8,
    resource_dir: std.fs.Dir,
    text: Text,
    charbuf: std.ArrayList(u8),
    charbuf_pos: usize = 0,
    frame_count: u64 = 0,

    fn init(self: *Self) !void {
        self.timer = try std.time.Timer.start();

        // Allocators
        self.alloc = .{};
        self.sokol_alloc = .{};
        self.ft_alloc = .{};

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

        // Text
        try self.text.init(self.ft_alloc.allocator(), @ptrCast(&font_path_buf));

        self.charbuf = std.ArrayList(u8).init(alloc);
    }

    fn deinit(self: *Self) void {
        self.charbuf.deinit();
        self.text.deinit();
        self.resource_dir.close();
        self.alloc.allocator().free(self.exepath);
        sokol.sg_shutdown();
    }

    fn onInit(self: Self) void {
        log.info("onInit", .{});

        const sg_desc = sokol.sg_desc{
            .environment = self.sokolAppEnv(),
            .logger = .{
                .user_data = @ptrCast(@constCast(&self)),
                .func = Self.sokolLogC,
            },
        };
        sokol.sg_setup(&sg_desc);
        // self.text.renderText("hello world!") catch |err| {
        //     log.err("renderText failed {any}", .{err});
        // };

        // Make an image the size of the screen
        const frame_size = self.screenSize();
        const bitmap = [_]u8{ 0, 0, 0, 0 };
        var img_desc = sokol.sg_image_desc{
            .type = sokol.SG_IMAGETYPE_2D,
            .render_target = false,
            .width = frame_size.width,
            .height = frame_size.height,
            .num_slices = 1,
            .num_mipmaps = 1,
            .usage = sokol.SG_USAGE_DYNAMIC,
            .pixel_format = sokol.SG_PIXELFORMAT_RGBA8,
            .sample_count = 1,
            .data = std.mem.zeroes(sokol.sg_image_data),
            .label = "screen",
        };
        img_desc.data.subimage[0][0] = .{
            .ptr = &bitmap,
            .size = bitmap.len,
        };
        const img = sokol.sg_make_image(&img_desc);
        _ = img;

        // Pipeline
        var pipe_desc = std.mem.zeroes(sokol.sg_pipeline_desc);
        pipe_desc.primitive_type = sokol.SG_PRIMITIVETYPE_TRIANGLE_STRIP;
        pipe_desc.label = "pipeline";
        // sg_shader shader;
        // sg_vertex_layout_state layout;
        // sg_depth_state depth;
        // sg_stencil_state stencil;
        // int color_count;
        // sg_color_target_state colors[SG_MAX_COLOR_ATTACHMENTS];
        // sg_index_type index_type;
        // int sample_count;
        // sg_color blend_color;
        // bool alpha_to_coverage_enabled;
        // Defaults
        // .cull_mode = sokol.SG_CULLMODE_NONE,
        // .face_winding = sokol.SG_FACEWINDING_CW,
        const pipeline = sokol.sg_make_pipeline(&pipe_desc);
        _ = pipeline;
    }

    fn onEvent(self: *Self, event: Sokol.Event) void {
        self.frame_count = event.frame_count;
        switch (event.type) {
            .CHAR => {
                var utf8_buf: [4]u8 = undefined;
                const utf8 = utf8FromUtf32Char(&utf8_buf, event.char_code) orelse blk: {
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
                    sokol.sapp_request_quit();
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
                const n = sokol.sapp_get_num_dropped_files();
                log.info("{s} n={d} ({d}, {d})", .{ @tagName(event.type), n, x, y });
                for (0..@intCast(n)) |i| {
                    const f = sokol.sapp_get_dropped_file_path(@intCast(i));
                    log.info("  {s}", .{f});
                }
            },
            .CLIPBOARD_PASTED => {
                const s = sokol.sapp_get_clipboard_string();
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

    fn onFrame(self: *Self) void {
        _ = self;
        const width = sokol.sapp_width();
        const height = sokol.sapp_height();
        _ = width;
        _ = height;

        //        const bitmap: []const u8 = undefined;
        //
        //        createPipeline();
        //
        //const img_desc = sokol.sg_image_desc{
        //            .width = 256,
        //            .height = 256,
        //            .pixel_format = SG_PIXELFORMAT_RGBA8,
        //            .sample_count = 2,
        //        };
        //const img = sokol.sg_make_image(&img_desc);
        //
        //        const pass: sokol.sg_pass = .{
        //            .action = .{
        //            },
        //            .swapchain = self.sokolSwapchain(),
        //        };
        //        sokol.sg_begin_pass(&pass);
        //sokol.sg_apply_pipeline(pipe);
        //
        //// This represents the vertex data, where each quad has 4 vertices, and each vertex has a position (xy) and tex coord (uv)
        //const vertices = [_]const f32{
        //    // positions   // tex coords
        //    -1.0f,  1.0f,   0.0f, 1.0f,
        //     1.0f,  1.0f,   1.0f, 1.0f,
        //    -1.0f, -1.0f,   0.0f, 0.0f,
        //     1.0f, -1.0f,   1.0f, 0.0f,
        //};
        //
        //const buf_desc = sokol.sg_buffer_desc{
        //    .size = @sizeOf(vertices),
        //    .content = vertices,
        //};
        //const vertex_buffer = sokol.sg_make_buffer(&buf_desc);
        //
        //
        //        const bindings: sokol.sg_bindings = .{
        //.vertex_buffers = &.{vertex_buffer},
        //            .fs_images = &.{img},
        //        };
        //        sg_apply_bindings(&bindings);
        //        sokol.sg_draw(0, 4, 1);
        //        sokol.sg_end_pass();
        //        sokol.sg_commit();
    }

    fn sokolAppEnv(self: Self) sokol.sg_environment {
        _ = self;
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

    fn screenSize(self: Self) ScreenSize {
        _ = self;
        return .{
            .width = sokol.sapp_width(),
            .height = sokol.sapp_height(),
        };
    }

    fn sokolSwapchain(self: *Self) sokol.sg_swapchain {
        _ = self;
        return .{
            .width = sokol.sapp_width(),
            .height = sokol.sapp_height(),
            .sample_count = sokol.sapp_sample_count(),
            .color_format = sokol.sapp_color_format(),
            .depth_format = sokol.sapp_depth_format(),
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

    fn sokolGetCtx() *Ctx {
        return @ptrCast(@alignCast(sokol.sapp_userdata()));
    }

    fn sokolOnInit() callconv(.C) void {
        const self: *Self = sokolGetCtx();
        self.onInit();
    }

    fn sokolOnFrame() callconv(.C) void {
        const self: *Self = sokolGetCtx();
        self.onFrame();
    }

    fn sokolOnCleanup() callconv(.C) void {
        const self: *Self = sokolGetCtx();
        self.deinit();
        std.heap.c_allocator.destroy(self);
    }

    fn sokolOnEvent(cevent: [*c]const sokol.sapp_event) callconv(.C) void {
        const self: *Self = sokolGetCtx();
        const event: *const Sokol.Event = @ptrCast(cevent);
        self.onEvent(event.*);
    }

    fn sokolAlloc(size: usize, user_data: ?*anyopaque) callconv(.C) ?*anyopaque {
        const self: *Self = @ptrCast(@alignCast(user_data.?));
        const alloc = self.sokol_alloc.allocator();
        return cAlloc(alloc, size) catch null;
    }

    fn sokolFree(ptr_or_null: ?*anyopaque, user_data: ?*anyopaque) callconv(.C) void {
        const self: *Self = @ptrCast(@alignCast(user_data.?));
        const alloc = self.sokol_alloc.allocator();
        return cFree(alloc, ptr_or_null);
    }

    fn sokolLogC(tag: [*c]const u8, log_level: u32, log_item_id: u32, message_or_null: [*c]const u8, line_nr: u32, filename_or_null: [*c]const u8, user_data: ?*anyopaque) callconv(.C) void {
        const self: *Self = @ptrCast(@alignCast(user_data.?));
        _ = self;

        const level = switch (log_level) {
            0 => "panic",
            1 => "error",
            2 => "warn",
            3 => "info",
            else => unreachable,
        };

        sokol_log.info("{s}: [{s} {s}:{d}] id={d} {s}", .{ level, tag, filename_or_null, line_nr, log_item_id, message_or_null });
    }
};

export fn sokol_main(argc: c_int, argv: [*][*:0]u8) sokol.sapp_desc {
    _ = argc;
    _ = argv;
    // Create Ctx.
    // Deallocated in Ctx.sokolOnCleanup
    const ctx = std.heap.c_allocator.create(Ctx) catch @panic("alloc failed");
    ctx.init() catch @panic("init failed");

    return .{
        .init_cb = Ctx.sokolOnInit,
        .frame_cb = Ctx.sokolOnFrame,
        .cleanup_cb = Ctx.sokolOnCleanup,
        .event_cb = Ctx.sokolOnEvent,
        .user_data = ctx,
        .high_dpi = true,
        .alpha = true,
        .window_title = "texthello",
        .enable_clipboard = true,
        .clipboard_size = 1 << 16,
        .enable_dragndrop = true,
        .max_dropped_files = 32,
        .max_dropped_file_path_length = std.fs.MAX_PATH_BYTES,
        .allocator = .{
            .alloc_fn = Ctx.sokolAlloc,
            .free_fn = Ctx.sokolFree,
            .user_data = ctx,
        },
        .logger = .{
            .user_data = ctx,
            .func = Ctx.sokolLogC,
        },
        .win32_console_utf8 = true,
        .ios_keyboard_resizes_canvas = true,
    };
}

const Text = struct {
    const Self = @This();

    alloc: std.mem.Allocator,
    ft: ctext.FT_Library,
    ft_mem: ctext.FT_MemoryRec_,
    fontface: ctext.FT_Face,
    hb_face: *ctext.hb_face_t,
    hb_font: *ctext.hb_font_t,
    hb_buf: *ctext.hb_buffer_t,

    fn init(self: *Self, alloc: std.mem.Allocator, font_path: [:0]const u8) !void {
        self.alloc = alloc;
        // FreeType init
        self.ft_mem = .{
            .user = self,
            .alloc = Self.ftAlloc,
            .free = Self.ftFree,
            .realloc = Self.ftRealloc,
        };
        if (ctext.FT_New_Library(&self.ft_mem, &self.ft) != 0) return error.FTInitFail;
        ctext.FT_Add_Default_Modules(self.ft);
        ctext.FT_Set_Default_Properties(self.ft);
        log.info("FT init done", .{});

        // FreeType font load
        if (ctext.FT_New_Face(
            self.ft,
            font_path,
            0, // face index
            &self.fontface,
        ) != 0) return error.FTFontLoadFail;
        log.info("FT font load done", .{});
        // HarfBuzz font load
        self.hb_face = ctext.hb_ft_face_create_referenced(self.fontface) orelse return error.HBFontFail;
        self.hb_font = ctext.hb_font_create(self.hb_face) orelse return error.HBFontFail;
        log.info("HB font load done", .{});

        // HarfBuzz buffer init
        self.hb_buf = ctext.hb_buffer_create() orelse return error.HFBufFail;
        if (ctext.hb_buffer_allocation_successful(self.hb_buf) == 0) return error.HFBufFail;
        ctext.hb_buffer_set_direction(self.hb_buf, ctext.HB_DIRECTION_LTR);
        ctext.hb_buffer_set_script(self.hb_buf, ctext.HB_SCRIPT_LATIN);
        ctext.hb_buffer_set_language(self.hb_buf, ctext.hb_language_from_string("en", -1));
        log.info("HB buffer init done", .{});
    }

    fn deinit(self: *Self) void {
        ctext.hb_buffer_destroy(self.hb_buf);
        ctext.hb_font_destroy(self.hb_font);
        ctext.hb_face_destroy(self.hb_face);
        _ = ctext.FT_Done_Face(self.fontface);
        _ = ctext.FT_Done_Library(self.ft);
    }

    fn renderText(self: Self, text: [:0]const u8) !void {
        const buf = self.hb_buf;
        ctext.hb_buffer_clear_contents(buf);

        // Add text
        ctext.hb_buffer_add_utf8(buf, text, -1, 0, -1);

        // Shape
        ctext.hb_shape(self.hb_font, buf, null, 0);
        var glyph_count: u32 = 0;
        const glyph_infos = ctext.hb_buffer_get_glyph_infos(buf, &glyph_count);
        const glyph_poss = ctext.hb_buffer_get_glyph_positions(buf, &glyph_count);
        log.info("HB shape done", .{});

        // Render
        var cursor_x: ctext.hb_position_t = 0;
        var cursor_y: ctext.hb_position_t = 0;
        for (0..glyph_count) |i| {
            // Glyph info
            const glyph_info = glyph_infos[i];
            const glyph_pos = glyph_poss[i];
            const glyphid = glyph_info.codepoint;
            var char_name_buf: [256:0]u8 = undefined;
            const char_name: [*:0]u8 = &char_name_buf;
            if (ctext.FT_Get_Glyph_Name(self.fontface, glyphid, char_name, 16) != 0)
                return error.FTGetGlyphName;

            // Rasterize
            if (ctext.FT_Load_Glyph(self.fontface, glyphid, ctext.FT_LOAD_DEFAULT) != 0)
                return error.FTLoadGlyph;
            var glyph: ctext.FT_Glyph = undefined;
            if (ctext.FT_Get_Glyph(self.fontface.*.glyph, &glyph) != 0)
                return error.FTGetGlyph;
            var pen = ctext.FT_Vector{};
            if (glyph.*.format != ctext.FT_GLYPH_FORMAT_BITMAP)
                if (ctext.FT_Glyph_To_Bitmap(&glyph, ctext.FT_RENDER_MODE_NORMAL, &pen, 0) != 0)
                    return error.FTRender;
            const bit: ctext.FT_BitmapGlyph = @ptrCast(glyph);
            const bitmap = bit.*.bitmap;
            const glyph_metrics = self.fontface.*.glyph.*.metrics;

            // ASCII render glyph
            for (0..bitmap.rows) |bi| {
                for (0..bitmap.width) |bj| {
                    const val = bitmap.buffer[bi * bitmap.width + bj];
                    const str = if (val == 0) "_" else "X";
                    std.debug.print("{s}", .{str});
                }
                std.debug.print("\n", .{});
            }

            // Log info
            log.info("{s}", .{char_name});
            log.info("glyph info {any}", .{glyph_info});
            log.info("glyph pos {any}", .{glyph_pos});
            log.info("bitmap {any}", .{bitmap});
            log.info("glyph metrics {any}", .{glyph_metrics});

            // Advance
            cursor_x += glyph_pos.x_advance;
            cursor_y += glyph_pos.y_advance;
        }
    }

    fn ftAlloc(mem: ctext.FT_Memory, size: c_long) callconv(.C) ?*anyopaque {
        const self: *Self = @ptrCast(@alignCast(mem.*.user));
        return cAlloc(self.alloc, @intCast(size)) catch {
            log.err("ftAlloc failure", .{});
            return null;
        };
    }

    fn ftFree(mem: ctext.FT_Memory, block: ?*anyopaque) callconv(.C) void {
        const self: *Self = @ptrCast(@alignCast(mem.*.user));
        return cFree(self.alloc, block);
    }

    fn ftRealloc(mem: ctext.FT_Memory, cur_size: c_long, new_size: c_long, block: ?*anyopaque) callconv(.C) ?*anyopaque {
        const self: *Self = @ptrCast(@alignCast(mem.*.user));
        _ = cur_size;
        return cRealloc(self.alloc, block, @intCast(new_size)) catch {
            log.err("ftRealloc failure", .{});
            return null;
        };
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

// copied from sokol-zig
const Sokol = struct {
    const EventType = enum(i32) {
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
    const Keycode = enum(i32) {
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
    const AndroidTooltype = enum(i32) {
        UNKNOWN = 0,
        FINGER = 1,
        STYLUS = 2,
        MOUSE = 3,
    };
    const Touchpoint = extern struct {
        identifier: usize = 0,
        pos_x: f32 = 0.0,
        pos_y: f32 = 0.0,
        android_tooltype: AndroidTooltype = .UNKNOWN,
        changed: bool = false,
    };
    const Mousebutton = enum(i32) {
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
    const Modifier = enum {
        SHIFT,
        CTRL,
        ALT,
        SUPER,
        LEFT_MOUSE,
        RIGHT_MOUSE,
        MIDDLE_MOUSE,
    };
    const Event = extern struct {
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

        fn modifier(self: @This(), mod: Modifier) bool {
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
    };
};

const unknown_char = "\xEF\xBF\xBD";

fn utf8FromUtf32Char(buffer: *[4]u8, code: u32) ?[]u8 {
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
const ScreenSize = struct {
    width: c_int,
    height: c_int,
};
