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
//"/Users/ryan/fonts/notofonts.github.io-noto-monthly-release-24.4.1/fonts/NotoSerif/googlefonts/ttf/NotoSerif-Regular.ttf";

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
    const RenderInfo = struct {
        horiBearingX: c_long,
        horiBearingY: c_long,
    };
    const AtlasInfo = struct {
        quad: sokol.Rect,
        info: RenderInfo,
    };
    const InfoMap = std.hash_map.AutoHashMap(text.GlyphIndex, AtlasInfo);

    data: []u8,
    size: Size2D,
    info: InfoMap,
    padpx: usize,
    needs_update: bool = true,
    col_offset: usize = 0,

    fn init(alloc: std.mem.Allocator, size: Size2D, padpx: usize) !@This() {
        const info = FontAtlas.InfoMap.init(alloc);
        const data = try alloc.alloc(u8, size.area());
        return .{
            .data = data,
            .size = size,
            .info = info,
            .padpx = padpx,
        };
    }

    fn deinit(self: *@This()) void {
        self.info.allocator.free(self.data);
        self.info.deinit();
    }

    fn addGlyph(
        self: *@This(),
        glyph: text.GlyphIndex,
        bitmap: text.Glyph.Bitmap,
        info: RenderInfo,
    ) !void {
        const data_width = self.size.width;
        const data_height = self.size.height;
        if (bitmap.nrows > data_height) return error.CharTooTall;
        if (self.col_offset + bitmap.ncols > data_width) return error.CharTooWide;

        const start = data_width * (data_height - bitmap.nrows);

        var i: usize = 0;
        var rows = bitmap.rows();
        while (rows.next()) |row| : (i += 1) {
            const row_start = start + self.col_offset + i * data_width;
            std.mem.copyForwards(u8, self.data[row_start .. row_start + bitmap.ncols], row);
        }

        try self.info.put(glyph, .{
            .info = info,
            .quad = .{
                .tl = .{ .x = @floatFromInt(start + self.col_offset), .y = @floatFromInt(bitmap.nrows) },
                .br = .{ .x = @floatFromInt(start + self.col_offset + bitmap.ncols), .y = 0 },
            },
        });

        // To render to ascii on stderr, uncomment this line
        // try bitmap.ascii(std.io.getStdErr().writer());

        self.col_offset += bitmap.ncols + self.padpx;
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
        try font.loadGlyph(font.glyphIdx(char));
        const metrics = font.face.*.glyph.*.metrics;
        max_height = @max(max_height, metrics.height);
        total_width += metrics.width + (padpx << 6);
    }

    const data_height: usize = @intCast((max_height + 1) >> 6);
    const data_width: usize = @intCast((total_width + 1) >> 6);

    var atlas = try FontAtlas.init(
        alloc,
        .{ .width = data_width, .height = data_height },
        padpx,
    );

    for (ascii_chars) |char| {
        const idx = font.glyphIdx(char);
        var glyph = try font.glyph(idx);
        defer glyph.deinit();
        const bitmap = try glyph.render();
        try atlas.addGlyph(idx, bitmap, .{
            .horiBearingX = font.face.*.glyph.*.metrics.horiBearingX,
            .horiBearingY = font.face.*.glyph.*.metrics.horiBearingY,
        });
    }

    return atlas;
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
    usertext: std.ArrayList(u8),
    textbuf: text.Buffer,

    // Graphics
    need_render: bool,
    sg_initialized: bool,
    atlas: FontAtlas,
    vertex_list: std.ArrayList(f32),
    gfx: AlphaTexturePipeline = undefined,
    frame_count: u64 = 0,
    render_count: u64 = 0,

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
        self.usertext = std.ArrayList(u8).init(self.alloc.allocator());
        try self.usertext.appendSlice("welcome to xos");
        self.textbuf = try text.Buffer.init();
        self.textbuf.addText(self.usertext.items);

        self.need_render = true;
        self.sg_initialized = false;
        self.atlas = try buildAsciiAtlas(self.alloc.allocator(), self.font);
        self.vertex_list = std.ArrayList(f32).init(self.alloc.allocator());
        self.gfx = undefined;
        self.frame_count = 0;
        self.render_count = 0;
    }

    pub fn deinit(self: *Self) void {
        self.vertex_list.deinit();
        self.atlas.deinit();
        self.usertext.deinit();
        self.textbuf.deinit();
        self.font.deinit();
        self.ft.deinit();
        self.resource_dir.close();
        self.alloc.allocator().free(self.exepath);
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
        self.gfx = AlphaTexturePipeline.init(
            .{ .height = self.atlas.size.height, .width = self.atlas.size.width },
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
                self.gfx.updateScreenSize(.{
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
        self.onFrameSafe() catch |err| {
            log.err("frame fail {any}", .{err});
            switch (err) {
                error.GlyphNotInAtlas => {
                    @panic("bad glyph");
                },
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

        // Compute origin (bottom left of line)
        const origin: sokol.Point2D = blk: {
            const screen = sokol.screen();
            const line_height: f32 = @floatFromInt(self.font.face.*.size.*.metrics.height >> 6);
            break :blk screen.tl.right(10).down(line_height);
        };
        var cursor: sokol.Point2D = origin;

        // Text
        {
            self.textbuf.reset();
            self.textbuf.addText(self.usertext.items);
            var shaped = self.font.shape(self.textbuf);

            var x_advance: f32 = 0;
            var iter = shaped.iterator();
            while (iter.next()) |char| : ({
                x_advance = @floatFromInt(char.pos.x_advance >> 6);
            }) {
                // Line advance
                cursor = cursor.right(x_advance);

                // Get atlas rect
                const info = self.atlas.info.get(char.info.codepoint) orelse {
                    return error.GlyphNotInAtlas;
                };
                const tex_rect = info.quad;

                // Get screen rect
                const glyph_rect = blk: {
                    const ybear: f32 = @floatFromInt(info.info.horiBearingY >> 6);
                    const ybear_offset = tex_rect.height() - ybear;
                    const x_offset: f32 = @floatFromInt((char.pos.x_offset + info.info.horiBearingX) >> 6);
                    const y_offset: f32 = @as(f32, @floatFromInt((char.pos.y_offset) >> 6)) - ybear_offset;
                    const glyph_origin = cursor.right(x_offset).up(y_offset);
                    break :blk sokol.Rect{
                        .tl = glyph_origin.up(tex_rect.height()),
                        .br = glyph_origin.right(tex_rect.width()),
                    };
                };

                // Add triangles
                try self.vertex_list.appendSlice(&sokol.getRectVertices(glyph_rect, tex_rect));
            }
            cursor = cursor.right(x_advance);
        }

        // 2D drawing
        {
            const screen = sokol.screen();
            const width: c_int = @intFromFloat(screen.width());
            const height: c_int = @intFromFloat(screen.height());
            sokol.c.sgp_begin(width, height);
            sokol.c.sgp_viewport(0, 0, width, height);
            sokol.c.sgp_project(0, screen.width(), screen.height(), 0);

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

            var action = sokol.c.sg_pass_action{};
            action.colors[0] = .{
                .load_action = sokol.c.SG_LOADACTION_CLEAR,
                .store_action = sokol.c.SG_STOREACTION_STORE,
                .clear_value = sokol.color(255, 255, 255, 1.0),
            };
            const pass = sokol.RenderPass.begin(action, null);
            defer pass.endAndCommit();

            // Text
            self.gfx.update(.{
                .vertices = self.vertex_list.items,
                .texture = if (self.atlas.needs_update) self.atlas.data else null,
            });
            self.gfx.apply();

            // 2D
            sokol.c.sgp_flush();
            sokol.c.sgp_end();
        }

        self.vertex_list.clearRetainingCapacity();
        self.render_count += 1;
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

const AlphaTexturePipeline = struct {
    pipeline: sokol.c.sg_pipeline,
    shader: sokol.c.sg_shader,
    vertex_buf: sokol.c.sg_buffer,
    texture: sokol.c.sg_image,
    sampler: sokol.c.sg_sampler,
    vs_args: shaderlib.vs_params_t,
    fs_args: shaderlib.fs_params_t,
    nvertices: usize = 0,

    fn init(texture_size: Size2D) !@This() {
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
                @floatFromInt(texture_size.width),
                @floatFromInt(texture_size.height),
            },
        };
        var image_desc = sokol.c.sg_image_desc{
            .width = @intCast(texture_size.width),
            .height = @intCast(texture_size.height),
            .usage = sokol.c.SG_USAGE_DYNAMIC,
            .pixel_format = sokol.c.SG_PIXELFORMAT_R8UI,
        };
        const image = sokol.c.sg_make_image(&image_desc);

        return .{
            .shader = shader,
            .pipeline = pipeline,
            .vertex_buf = vertex_buf,
            .texture = image,
            .sampler = sampler,
            .vs_args = vs_args,
            .fs_args = fs_args,
        };
    }

    fn deinit(self: @This()) void {
        sokol.c.sg_destroy_buffer(self.vertex_buf);
        sokol.c.sg_destroy_image(self.texture);
        sokol.c.sg_destroy_sampler(self.sampler);
        sokol.c.sg_destroy_shader(self.shader);
        sokol.c.sg_destroy_pipeline(self.pipeline);
    }

    fn updateTexture(self: *const @This(), tex_data: []const u8) void {
        const data = sokol.c.sg_range{
            .ptr = tex_data.ptr,
            .size = tex_data.len * @sizeOf(u8),
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

    const UpdateArgs = struct {
        vertices: []const f32,
        texture: ?[]const u8 = null,
    };
    fn update(self: *@This(), args: UpdateArgs) void {
        // Buffers
        const vertex_data = sokol.c.sg_range{
            .ptr = args.vertices.ptr,
            .size = args.vertices.len * @sizeOf(f32),
        };
        sokol.c.sg_update_buffer(self.vertex_buf, &vertex_data);
        if (args.texture) |tex| self.updateTexture(tex);
        self.nvertices = args.vertices.len / 4;
    }

    fn apply(self: *const @This()) void {
        sokol.c.sg_apply_pipeline(self.pipeline);

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
            @intCast(self.nvertices), // num_elements
            1, // num_instances
        );
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
