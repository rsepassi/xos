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

const font_override: ?[:0]const u8 = null;
// "/Users/ryan/fonts/notofonts.github.io-noto-monthly-release-24.4.1/fonts/NotoSerif/googlefonts/ttf/NotoSerif-Regular.ttf";

comptime {
    sokol.App(Ctx).declare();
}

const ascii_chars = blk: {
    @setEvalBranchQuota(9999);
    var n = 0;
    for (0..256) |i| {
        if (std.ascii.isPrint(i)) n += 1;
    }
    var chars: [n]u8 = undefined;
    var i = 0;
    for (0..256) |x| {
        if (std.ascii.isPrint(x)) {
            chars[i] = x;
            i += 1;
        }
    }
    break :blk chars;
};

const FontAtlas = struct {
    const GlyphRenderInfo = struct {
        quad: sokol.Rect,
        horiBearingX: c_long,
        horiBearingY: c_long,
    };
    const InfoMap = std.hash_map.AutoHashMap(text.GlyphIndex, GlyphRenderInfo);

    data: []u8,
    width: usize,
    height: usize,
    info: InfoMap,
    needs_update: bool = true,

    fn deinit(self: *@This()) void {
        self.info.allocator.free(self.data);
        self.info.deinit();
    }

    fn ascii(self: @This(), quad: sokol.Rect, writer: anytype) !void {
        const offset: usize = @intFromFloat(quad.tl.x);
        const width: usize = @intFromFloat(quad.width());
        const height: usize = @intFromFloat(quad.height());

        for (0..height) |i| {
            const row_start = i * self.width + offset;
            for (0..width) |j| {
                const val = self.data[row_start + j];
                const s = if (val == 0) "_" else "X";
                _ = try writer.write(s);
            }
            _ = try writer.write("\n");
        }
    }
};

fn buildAsciiAtlas(alloc: std.mem.Allocator, font: text.Font) !FontAtlas {
    const padpx = 2;

    var max_height: c_long = 0;
    var total_width: c_long = 0;
    for (ascii_chars) |char| {
        const idx = font.glyphIdx(char);
        try font.loadGlyph(idx);
        const metrics = font.face.*.glyph.*.metrics;
        max_height = @max(max_height, metrics.height);
        total_width += metrics.width + (padpx << 6);
    }

    const data_height: usize = @intCast((max_height + 1) >> 6);
    const data_width: usize = @intCast((total_width + 1) >> 6);
    const data = try alloc.alloc(u8, data_height * data_width);

    var info = FontAtlas.InfoMap.init(alloc);

    var offset: usize = 0;
    for (ascii_chars) |char| {
        const idx = font.glyphIdx(char);
        var glyph = try font.glyph(idx);
        defer glyph.deinit();
        const bitmap = try glyph.render();
        const start = data_width * (data_height - bitmap.nrows);

        var i: usize = 0;
        var rows = bitmap.rows();
        while (rows.next()) |row| : (i += 1) {
            const row_start = start + offset + i * data_width;
            std.mem.copyForwards(u8, data[row_start .. row_start + bitmap.ncols], row);
        }

        try info.put(idx, .{
            .horiBearingX = font.face.*.glyph.*.metrics.horiBearingX,
            .horiBearingY = font.face.*.glyph.*.metrics.horiBearingY,
            .quad = .{
                .tl = .{ .x = @floatFromInt(start + offset), .y = @floatFromInt(bitmap.nrows) },
                .br = .{ .x = @floatFromInt(start + offset + bitmap.ncols), .y = 0 },
            },
        });

        // To render to ascii on stderr, uncomment this line
        // try bitmap.ascii(std.io.getStdErr().writer());

        offset += bitmap.ncols + padpx;
    }

    return .{
        .data = data,
        .width = data_width,
        .height = data_height,
        .info = info,
    };
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

    // Graphics
    need_render: bool,
    sg_initialized: bool,
    atlas: FontAtlas,
    vertex_list: std.ArrayList(f32),
    gfx: GfxPipeline = undefined,
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
        var font_path: []const u8 = undefined;
        if (font_override) |p| {
            std.mem.copyForwards(u8, &font_path_buf, p);
            font_path = @ptrCast(font_path_buf[0..p.len]);
        } else {
            font_path = try self.resource_dir.realpath(Resources.font, &font_path_buf);
        }
        font_path_buf[font_path.len] = 0;

        // Text
        self.ft = try text.FreeType.init();
        self.font = try self.ft.font(.{
            .path = @ptrCast(font_path),
            .pxsize = 24,
        });
        self.textbuf = try text.Buffer.init();
        self.textbuf.addText("hello world");

        self.need_render = true;
        self.sg_initialized = false;
        self.atlas = try buildAsciiAtlas(self.alloc.allocator(), self.font);
        self.vertex_list = std.ArrayList(f32).init(self.alloc.allocator());
        self.gfx = undefined;
        self.frame_count = 0;
    }

    pub fn deinit(self: *Self) void {
        self.vertex_list.deinit();
        self.atlas.deinit();
        self.textbuf.deinit();
        self.font.deinit();
        self.ft.deinit();
        self.resource_dir.close();
        self.alloc.allocator().free(self.exepath);
        if (self.sg_initialized) {
            self.gfx.deinit();
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
        self.gfx = GfxPipeline.init(
            self.alloc.allocator(),
            self.atlas,
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
                self.gfx.updateScreenSize(.{
                    .width = @intCast(event.framebuffer_width),
                    .height = @intCast(event.framebuffer_height),
                });
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
        self.onFrameSafe() catch |err| {
            log.err("frame fail {any}", .{err});
            switch (err) {
                error.GlyphNotInAtlas => {},
                else => @panic("frame fail"),
            }
        };
    }

    fn onFrameSafe(self: *Self) !void {
        defer self.frame_count += 1;
        if (!self.need_render) return;
        defer self.need_render = false;
        if (self.textbuf.len() < 1) return;
        defer self.atlas.needs_update = false;

        // Build up our vertices
        var vertices = &self.vertex_list;
        defer vertices.clearRetainingCapacity();

        {
            // Compute origin (bottom left of line)
            var origin: sokol.Point2D = blk: {
                const screen = sokol.screen();
                const line_height: f32 = @floatFromInt(self.font.face.*.size.*.metrics.height >> 6);
                break :blk screen.tl.down(line_height);
            };
            var x_advance: f32 = 0;
            var shaped = self.font.shape(self.textbuf);
            var iter = shaped.iterator();
            while (iter.next()) |char| : ({
                x_advance = @floatFromInt(char.pos.x_advance >> 6);
            }) {
                // Line advance
                origin = origin.right(x_advance);

                // Get atlas rect
                const info = self.atlas.info.get(char.info.codepoint) orelse {
                    return error.GlyphNotInAtlas;
                };
                const tex_rect = info.quad;

                // Get screen rect
                const glyph_rect = blk: {
                    const ybear: f32 = @floatFromInt(info.horiBearingY >> 6);
                    const ybear_offset = tex_rect.height() - ybear;
                    const x_offset: f32 = @floatFromInt((char.pos.x_offset + info.horiBearingX) >> 6);
                    const y_offset: f32 = @as(f32, @floatFromInt((char.pos.y_offset) >> 6)) - ybear_offset;
                    const glyph_origin = origin.right(x_offset).up(y_offset);
                    break :blk sokol.Rect{
                        .tl = glyph_origin.up(tex_rect.height()),
                        .br = glyph_origin.right(tex_rect.width()),
                    };
                };

                // Add triangles
                try vertices.appendSlice(&sokol.getRectVertices(glyph_rect, tex_rect));
            }
        }

        self.gfx.doPass(.{
            .vertices = vertices.items,
            .update_texture = self.atlas.needs_update,
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

const GfxPipeline = struct {
    action: sokol.c.sg_pass_action,
    pipeline: sokol.c.sg_pipeline,
    shader: sokol.c.sg_shader,
    vertex_buf: sokol.c.sg_buffer,
    texture: sokol.c.sg_image,
    sampler: sokol.c.sg_sampler,
    vs_args: shaderlib.vs_params_t,
    fs_args: shaderlib.fs_params_t,
    atlas: FontAtlas,
    alloc: std.mem.Allocator,

    fn init(alloc: std.mem.Allocator, atlas: FontAtlas) !@This() {
        var action = sokol.c.sg_pass_action{};
        action.colors[0] = .{
            .load_action = sokol.c.SG_LOADACTION_CLEAR,
            .clear_value = sokol.color(255, 255, 255, 1.0),
        };

        const shader = sokol.c.sg_make_shader(shaderlib.spritealpha_shader_desc(
            sokol.c.sg_query_backend(),
        ));
        var pipeline_desc = sokol.c.sg_pipeline_desc{
            .shader = shader,
            .primitive_type = sokol.c.SG_PRIMITIVETYPE_TRIANGLES,
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

        const max_quads = 256;
        const vertices_per_quad = 6;
        const vertex_vals = 4;
        var vertex_buf_desc = sokol.c.sg_buffer_desc{
            .usage = sokol.c.SG_USAGE_DYNAMIC,
            .size = @sizeOf(f32) * max_quads * vertices_per_quad * vertex_vals,
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
                2.0 / screen.width(), 0,                     0, 0,
                0,                    2.0 / screen.height(), 0, 0,
                0,                    0,                     1, 0,
                0,                    0,                     0, 1,
            },
        };
        const fs_args = shaderlib.fs_params_t{
            .color = sokol.colorVec(0, 0, 0),
            .tex_size = .{
                @floatFromInt(atlas.width),
                @floatFromInt(atlas.height),
            },
        };
        var image_desc = sokol.c.sg_image_desc{
            .width = @intCast(atlas.width),
            .height = @intCast(atlas.height),
            .usage = sokol.c.SG_USAGE_DYNAMIC,
            .pixel_format = sokol.c.SG_PIXELFORMAT_R8UI,
        };
        const image = sokol.c.sg_make_image(&image_desc);

        return .{
            .shader = shader,
            .alloc = alloc,
            .action = action,
            .pipeline = pipeline,
            .vertex_buf = vertex_buf,
            .texture = image,
            .sampler = sampler,
            .vs_args = vs_args,
            .fs_args = fs_args,
            .atlas = atlas,
        };
    }

    fn deinit(self: @This()) void {
        sokol.c.sg_destroy_buffer(self.vertex_buf);
        sokol.c.sg_destroy_image(self.texture);
        sokol.c.sg_destroy_sampler(self.sampler);
        sokol.c.sg_destroy_shader(self.shader);
        sokol.c.sg_destroy_pipeline(self.pipeline);
    }

    fn updateTexture(self: *const @This()) void {
        const data = sokol.c.sg_range{
            .ptr = self.atlas.data.ptr,
            .size = self.atlas.data.len * @sizeOf(u8),
        };
        sokol.c.sg_update_image(self.texture, @ptrCast(&data));
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

    const PassArgs = struct {
        vertices: []const f32,
        update_texture: bool = false,
    };
    fn doPass(self: *const @This(), args: PassArgs) void {
        var pass = sokol.c.sg_pass{
            .action = self.action,
            .swapchain = sokol.swapchain(),
        };
        sokol.c.sg_begin_pass(&pass);
        sokol.c.sg_apply_pipeline(self.pipeline);

        // Buffers
        const vertex_data = sokol.c.sg_range{
            .ptr = args.vertices.ptr,
            .size = args.vertices.len * @sizeOf(f32),
        };
        sokol.c.sg_update_buffer(self.vertex_buf, &vertex_data);
        if (args.update_texture) self.updateTexture();

        // Bindings
        var bindings = sokol.c.sg_bindings{};
        bindings.vertex_buffers[0] = self.vertex_buf;
        bindings.fs.images[0] = self.texture;
        bindings.fs.samplers[0] = self.sampler;
        sokol.c.sg_apply_bindings(&bindings);

        // Uniforms
        const vs_data = sokol.c.sg_range{
            .ptr = &self.vs_args,
            .size = @sizeOf(shaderlib.vs_params_t),
        };
        sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_VS, shaderlib.SLOT_vs_params, &vs_data);
        const fs_data = sokol.c.sg_range{
            .ptr = &self.fs_args,
            .size = @sizeOf(shaderlib.fs_params_t),
        };
        sokol.c.sg_apply_uniforms(sokol.c.SG_SHADERSTAGE_FS, shaderlib.SLOT_fs_params, &fs_data);

        // Draw
        sokol.c.sg_draw(
            0, // base_element
            @intCast(args.vertices.len / 4), // num_elements
            1, // num_instances
        );

        sokol.c.sg_end_pass();
        sokol.c.sg_commit();
    }
};

// TODO:
// * Updating FontAtlas, caching
// * Index buffer
// * Line wrap
// * Text selection
// * Emoji
