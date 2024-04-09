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

    // Graphics
    need_render: bool = false,
    sg_initialized: bool = false,
    pipeline_state: PipelineState = undefined,
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
        self.font = try self.ft.font(.{
            .path = @ptrCast(font_path),
            .pxheight = 64,
        });
        self.textbuf = try text.Buffer.init();
        self.textbuf.addText("h");
    }

    pub fn deinit(self: *Self) void {
        self.textbuf.deinit();
        self.font.deinit();
        self.ft.deinit();
        self.resource_dir.close();
        self.alloc.allocator().free(self.exepath);
        if (self.sg_initialized) {
            self.pipeline_state.deinit();
            sokol.c.sg_shutdown();
        }
    }

    pub fn onInit(self: *Self) void {
        var sg_desc = sokol.c.sg_desc{
            .environment = sokol.appEnv(),
            .logger = sokol.sgLogger(self),
        };
        sokol.c.sg_setup(&sg_desc);
        self.pipeline_state = PipelineState.init(
            self.alloc.allocator(),
            .{ .width = 100, .height = 100 },
        ) catch @panic("pipe init");
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

                self.textbuf.reset();
                self.textbuf.addText(utf8);
                self.need_render = true;
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
                self.pipeline_state.updateScreenSize(.{
                    .width = @intCast(event.framebuffer_width),
                    .height = @intCast(event.framebuffer_height),
                });
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
        if (!self.need_render) return;
        defer self.need_render = false;
        if (self.textbuf.len() < 1) return;

        var shaped = self.font.shape(self.textbuf);
        var iter = shaped.iterator();
        var bitmap: text.Glyph.Bitmap = undefined;
        while (iter.next() catch @panic("bad shaped glyph")) |*sglyph| {
            var glyph = sglyph.glyph;
            bitmap = glyph.render() catch @panic("bad render");
            break;
        }

        const char_height: f32 = @floatFromInt(bitmap.rows);
        const char_width: f32 = @floatFromInt(bitmap.cols);

        // Screen coordinates
        const screen = sokol.screen();
        const tl = screen.tl;
        const bl = tl.down(char_height);
        const br = bl.right(char_width);
        const tr = tl.right(char_width);

        // Texture coordinates
        const tex_tl = sokol.Point2D{ .x = 0, .y = @floatFromInt(self.pipeline_state.image_size.height) };
        const tex_bl = tex_tl.down(char_height);
        const tex_br = tex_bl.right(char_width);
        const tex_tr = tex_tl.right(char_width);

        const vertices: []const f32 = &[_]f32{
            bl.x, bl.y, tex_bl.x, tex_bl.y,
            tl.x, tl.y, tex_tl.x, tex_tl.y,
            br.x, br.y, tex_br.x, tex_br.y,
            tr.x, tr.y, tex_tr.x, tex_tr.y,
        };
        const num_vertices = vertices.len / 4;

        const tex_width = self.pipeline_state.image_size.width;
        for (0..bitmap.rows) |i| {
            for (0..bitmap.cols) |j| {
                const src = bitmap.buf[i * bitmap.cols + j];
                self.pipeline_state.image_data[i * tex_width + j] = src;
            }
        }

        doGfxPass(.{
            .state = &self.pipeline_state,
            .num_vertices = @intCast(num_vertices),
            .vertices = vertices,
            .update_image = true,
        });
    }

    pub fn onLog(self: Self, slog: sokol.Log) void {
        _ = self;
        sokol_log.info("sokol {any}", .{slog});
    }
};

const unknown_char = "\xEF\xBF\xBD";

const Size2D = struct {
    height: usize,
    width: usize,
    fn area(self: @This()) usize {
        return self.height * self.width;
    }
};

const PipelineState = struct {
    action: sokol.c.sg_pass_action,
    pipeline: sokol.c.sg_pipeline,
    shader: sokol.c.sg_shader,
    vertex_buf: sokol.c.sg_buffer,
    image: sokol.c.sg_image,
    sampler: sokol.c.sg_sampler,
    vs_args: shaderlib.vs_params_t,
    fs_args: shaderlib.fs_params_t,
    image_size: Size2D,
    image_data: []u8,
    alloc: std.mem.Allocator,

    fn init(alloc: std.mem.Allocator, image_size: Size2D) !@This() {
        var action = sokol.c.sg_pass_action{};
        action.colors[0] = .{
            .load_action = sokol.c.SG_LOADACTION_CLEAR,
            .clear_value = sokol.color(255, 255, 255, 1.0),
        };

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
        pipeline_desc.colors[0].blend = .{
            .enabled = true,
            .src_factor_rgb = sokol.c.SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = sokol.c.SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .op_rgb = sokol.c.SG_BLENDOP_ADD,
            .src_factor_alpha = sokol.c.SG_BLENDFACTOR_ONE,
            .dst_factor_alpha = sokol.c.SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            .op_alpha = sokol.c.SG_BLENDOP_ADD,
        };
        const pipeline = sokol.c.sg_make_pipeline(&pipeline_desc);

        const max_vertices = 4;
        var vertex_buf_desc = sokol.c.sg_buffer_desc{
            .usage = sokol.c.SG_USAGE_DYNAMIC,
            .size = 4 * @sizeOf(f32) * max_vertices,
            .label = "vertices",
        };
        const vertex_buf = sokol.c.sg_make_buffer(&vertex_buf_desc);

        var sampler_desc = sokol.c.sg_sampler_desc{
            .label = "sampler",
        };
        const sampler = sokol.c.sg_make_sampler(&sampler_desc);

        const screen = sokol.screen();
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
            .tex_size = .{
                @floatFromInt(image_size.width),
                @floatFromInt(image_size.height),
            },
        };
        var image_desc = sokol.c.sg_image_desc{
            .width = @intCast(image_size.width),
            .height = @intCast(image_size.height),
            .usage = sokol.c.SG_USAGE_DYNAMIC,
            .pixel_format = sokol.c.SG_PIXELFORMAT_R8UI,
        };
        const image = sokol.c.sg_make_image(&image_desc);
        const image_data = try alloc.alloc(u8, image_size.area());
        for (0..image_data.len) |i| image_data[i] = 0;

        return .{
            .shader = shader,
            .alloc = alloc,
            .action = action,
            .pipeline = pipeline,
            .vertex_buf = vertex_buf,
            .image = image,
            .sampler = sampler,
            .vs_args = vs_args,
            .fs_args = fs_args,
            .image_data = image_data,
            .image_size = image_size,
        };
    }

    fn deinit(self: @This()) void {
        self.alloc.free(self.image_data);
        sokol.c.sg_destroy_buffer(self.vertex_buf);
        sokol.c.sg_destroy_image(self.image);
        sokol.c.sg_destroy_sampler(self.sampler);
        sokol.c.sg_destroy_shader(self.shader);
        sokol.c.sg_destroy_pipeline(self.pipeline);
    }

    fn updateImage(self: *const @This()) void {
        const data = sokol.c.sg_range{
            .ptr = self.image_data.ptr,
            .size = self.image_data.len * @sizeOf(u8),
        };
        sokol.c.sg_update_image(self.image, @ptrCast(&data));
    }

    fn updateScreenSize(self: *@This(), size: Size2D) void {
        const width: f32 = @floatFromInt(size.width);
        const height: f32 = @floatFromInt(size.height);
        self.vs_args.proj = .{
            2.0 / width, 0,            0, 0,
            0,           2.0 / height, 0, 0,
            0,           0,            1, 0,
            0,           0,            0, 1,
        };
    }
};

const PassArgs = struct {
    state: *const PipelineState,
    vertices: []const f32,
    num_vertices: c_int,
    update_image: bool = false,
};
fn doGfxPass(args: PassArgs) void {
    var pass = sokol.c.sg_pass{
        .action = args.state.action,
        .swapchain = sokol.swapchain(),
    };
    sokol.c.sg_begin_pass(&pass);
    sokol.c.sg_apply_pipeline(args.state.pipeline);

    // Buffer updates
    const vertex_data = sokol.c.sg_range{
        .ptr = args.vertices.ptr,
        .size = args.vertices.len * @sizeOf(f32),
    };
    sokol.c.sg_update_buffer(args.state.vertex_buf, &vertex_data);
    if (args.update_image) args.state.updateImage();

    // Bindings
    var bindings = sokol.c.sg_bindings{};
    bindings.vertex_buffers[0] = args.state.vertex_buf;
    bindings.fs.images[0] = args.state.image;
    bindings.fs.samplers[0] = args.state.sampler;
    sokol.c.sg_apply_bindings(&bindings);

    // Uniforms
    const vs_data = sokol.c.sg_range{
        .ptr = &args.state.vs_args,
        .size = @sizeOf(shaderlib.vs_params_t),
    };
    sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_VS, shaderlib.SLOT_vs_params, &vs_data);
    const fs_data = sokol.c.sg_range{
        .ptr = &args.state.fs_args,
        .size = @sizeOf(shaderlib.fs_params_t),
    };
    sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_FS, shaderlib.SLOT_fs_params, &fs_data);

    // Draw
    sokol.c.sg_draw(
        0, // base_element
        args.num_vertices, // num_elements
        1, // num_instances
    );
    sokol.c.sg_end_pass();
    sokol.c.sg_commit();
}
