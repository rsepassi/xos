const std = @import("std");
const sokol = @import("sokol.zig");
const text = @import("text.zig");

const shaderlib = sokol.c;

const log = std.log.scoped(.texthello);
pub const std_options = .{
    .log_level = .info,
};

const Resources = struct {
    const font = "CourierPrime-Regular.ttf";
};

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
    exepath: []const u8,
    resource_dir: std.fs.Dir,

    // Text
    ft: text.FreeType,
    font: text.Font,
    textbuf: text.Buffer,

    // State
    charbuf: std.ArrayList(u8),
    charbuf_pos: usize = 0,

    // Graphics
    sg_initialized: bool = false,
    frame_count: u64 = 0,

    pub fn init(self: *Self) !void {
        self.timer = try std.time.Timer.start();

        self.alloc = .{};

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
        var sg_desc = sokol.c.sg_desc{
            .environment = sokol.appEnv(),
            .logger = sokol.sgLogger(self),
        };
        sokol.c.sg_setup(&sg_desc);
        self.sg_initialized = true;
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

        var shaped = self.font.shape(self.textbuf);
        var iter = shaped.iterator();

        var char_name: [256:0]u8 = undefined;
        var bitmap: text.Glyph.Bitmap = undefined;
        while (iter.next() catch @panic("bad shaped glyph")) |*sglyph| {
            var glyph = sglyph.glyph;
            bitmap = glyph.render() catch @panic("bad render");
            const name = glyph.name(&char_name) catch @panic("bad name");
            log.info("{s} ({d}, {d})\n", .{ name, bitmap.rows, bitmap.cols });
            bitmap.ascii(std.io.getStdErr().writer()) catch @panic("bad write");
            break;
        }

        const bitmap_float = self.alloc.allocator().alloc(f32, bitmap.rows * bitmap.cols) catch @panic("no mem");
        for (0..bitmap.rows) |i| {
            for (0..bitmap.cols) |j| {
                const idx = i * bitmap.cols + j;
                const src = bitmap.buf[idx];
                const alpha = @as(f32, @floatFromInt(src)) / 255.0;
                bitmap_float[idx] = alpha;
            }
        }

        // For this single bitmap,
        // render it to screen

        var action = sokol.c.sg_pass_action{};
        action.colors[0] = .{
            .load_action = sokol.c.SG_LOADACTION_CLEAR,
            .clear_value = sokol.color(255, 255, 255, 1.0),
        };
        const swapchain = sokol.swapchain();

        const shader = sokol.c.sg_make_shader(shaderlib.loadchar_shader_desc(
            sokol.c.sg_query_backend(),
        ));
        var pipeline_desc = sokol.c.sg_pipeline_desc{
            .shader = shader,
            .primitive_type = sokol.c.SG_PRIMITIVETYPE_TRIANGLE_STRIP,
            .label = "pipeline",
        };
        pipeline_desc.layout.attrs[shaderlib.ATTR_vs_pos] = .{
            .format = sokol.c.SG_VERTEXFORMAT_FLOAT2,
        };
        pipeline_desc.layout.attrs[shaderlib.ATTR_vs_texuv] = .{
            .format = sokol.c.SG_VERTEXFORMAT_FLOAT2,
        };
        const pipeline = sokol.c.sg_make_pipeline(&pipeline_desc);

        const screen = sokol.screen();

        const vertices: []const f32 = &[_]f32{
            screen.bl.x, screen.bl.y, 0, 0,
            screen.tl.x, screen.tl.y, 0, 1,
            screen.br.x, screen.br.y, 1, 0,
            screen.tr.x, screen.tr.y, 1, 1,
        };
        const num_vertices = vertices.len / 4;
        const vertex_data = sokol.c.sg_range{
            .ptr = vertices.ptr,
            .size = vertices.len * @sizeOf(f32),
        };
        var vertex_buf_desc = sokol.c.sg_buffer_desc{
            .usage = sokol.c.SG_USAGE_IMMUTABLE,
            .size = vertex_data.size,
            .data = vertex_data,
            .label = "vertices",
        };
        const vertex_buf = sokol.c.sg_make_buffer(&vertex_buf_desc);

        var sampler_desc = sokol.c.sg_sampler_desc{
            .label = "sampler",
        };
        const sampler = sokol.c.sg_make_sampler(&sampler_desc);

        const vs_args = shaderlib.vs_params_t{
            .proj = .{
                2.0 / screen.width, 0,                   0, 0,
                0,                  2.0 / screen.height, 0, 0,
                0,                  0,                   1, 0,
                0,                  0,                   0, 1,
            },
        };
        const fs_args = shaderlib.fs_params_t{
            .color = sokol.colorVec(0, 0, 0),
        };
        var image_desc = sokol.c.sg_image_desc{
            .width = @intCast(bitmap.cols),
            .height = @intCast(bitmap.rows),
            .pixel_format = sokol.c.SG_PIXELFORMAT_R32F,
        };
        image_desc.data.subimage[0][0] = sokol.c.sg_range{
            .ptr = bitmap_float.ptr,
            .size = bitmap_float.len * @sizeOf(f32),
        };
        const image = sokol.c.sg_make_image(&image_desc);

        // TODO: On a pass, the only thing that should update is updating the
        // vertex buffer

        doPass(.{
            .action = action,
            .swapchain = swapchain,
            .pipeline = pipeline,
            .vertex_buf = vertex_buf,
            .num_vertices = @intCast(num_vertices),
            .sampler = sampler,
            .vs_args = &vs_args,
            .fs_args = &fs_args,
            .image = image,
        });

        // TODO: Cleanup? On shutdown only?
        // sokol.c.sg_destroy_*
    }

    pub fn onLog(self: Self, slog: sokol.Log) void {
        _ = self;
        sokol_log.info("sokol {any}", .{slog});
    }
};

const unknown_char = "\xEF\xBF\xBD";

const PassArgs = struct {
    action: sokol.c.sg_pass_action,
    swapchain: sokol.c.sg_swapchain,
    pipeline: sokol.c.sg_pipeline,
    vertex_buf: sokol.c.sg_buffer,
    num_vertices: c_int,
    image: sokol.c.sg_image,
    sampler: sokol.c.sg_sampler,
    vs_args: *const shaderlib.vs_params_t,
    fs_args: *const shaderlib.fs_params_t,
};
fn doPass(args: PassArgs) void {
    var pass = sokol.c.sg_pass{
        .action = args.action,
        .swapchain = args.swapchain,
    };
    sokol.c.sg_begin_pass(&pass);
    sokol.c.sg_apply_pipeline(args.pipeline);
    var bindings = sokol.c.sg_bindings{};
    bindings.vertex_buffers[0] = args.vertex_buf;
    bindings.fs.images[0] = args.image;
    bindings.fs.samplers[0] = args.sampler;
    sokol.c.sg_apply_bindings(&bindings);
    const vs_data = sokol.c.sg_range{
        .ptr = args.vs_args,
        .size = @sizeOf(shaderlib.vs_params_t),
    };
    sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_VS, shaderlib.SLOT_vs_params, &vs_data);
    const fs_data = sokol.c.sg_range{
        .ptr = args.fs_args,
        .size = @sizeOf(shaderlib.fs_params_t),
    };
    sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_FS, shaderlib.SLOT_fs_params, &fs_data);
    sokol.c.sg_draw(
        0, // base_element
        args.num_vertices, // num_elements
        1, // num_instances
    );
    sokol.c.sg_end_pass();
    sokol.c.sg_commit();
}
