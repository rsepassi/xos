const std = @import("std");

const twod = @import("twod.zig");

pub const c = @cImport({
    @cInclude("freetype/freetype.h");
    @cInclude("freetype/ftmodapi.h");
    @cInclude("freetype/ftglyph.h");
    @cInclude("harfbuzz/hb.h");
    @cInclude("harfbuzz/hb-ft.h");
});

pub const GlyphIndex = u32;

pub const FreeType = struct {
    const Self = @This();

    ft: c.FT_Library,

    pub fn init() !Self {
        var self = std.mem.zeroes(Self);
        if (c.FT_Init_FreeType(&self.ft) != 0) return error.FTInitFail;
        return self;
    }

    pub fn deinit(self: *Self) void {
        _ = c.FT_Done_Library(self.ft);
    }

    pub fn font(self: Self, args: Font.InitArgs) !Font {
        return try Font.init(&self, args);
    }
};

pub const Font = struct {
    const Self = @This();

    face: c.FT_Face,
    has_color: bool,
    hb_face: *c.hb_face_t,
    hb_font: *c.hb_font_t,

    const InitArgs = struct {
        path: [:0]const u8,
        face_index: u8 = 0,
        pxsize: usize = 12,
    };
    pub fn init(lib: *const FreeType, args: InitArgs) !Self {
        var self: Self = undefined;
        if (c.FT_New_Face(
            lib.ft,
            args.path,
            args.face_index,
            &self.face,
        ) != 0) return error.FTFontLoadFail;

        if (c.FT_HAS_COLOR(self.face)) self.has_color = true;
        if (c.FT_HAS_FIXED_SIZES(self.face)) {
            if (c.FT_Select_Size(self.face, 0) != 0) return error.FTFontSizeFail;
        } else {
            if (c.FT_Set_Char_Size(self.face, 0, @intCast(args.pxsize << 6), 0, 0) != 0)
                return error.FTFontSizeFail;
        }

        // Identity transform
        c.FT_Set_Transform(self.face, 0, 0);

        self.hb_face = c.hb_ft_face_create_referenced(self.face) orelse return error.HBFontFail;
        self.hb_font = c.hb_font_create(self.hb_face) orelse return error.HBFontFail;
        c.hb_font_set_scale(
            self.hb_font,
            @intCast(args.pxsize << 6),
            @intCast(args.pxsize << 6),
        );

        return self;
    }

    pub fn deinit(self: *Self) void {
        c.hb_font_destroy(self.hb_font);
        c.hb_face_destroy(self.hb_face);
        _ = c.FT_Done_Face(self.face);
    }

    pub fn glyphIdx(self: Self, char: usize) GlyphIndex {
        return c.FT_Get_Char_Index(self.face, char);
    }

    pub fn loadGlyph(self: Self, id: GlyphIndex) !void {
        var load_flags = c.FT_LOAD_DEFAULT;
        if (self.has_color) load_flags |= @intCast(c.FT_LOAD_COLOR);
        if (c.FT_Load_Glyph(self.face, id, load_flags) != 0)
            return error.FTLoadGlyph;
    }

    pub fn glyph(self: Self, id: u32) !Glyph {
        try self.loadGlyph(id);
        var g: Glyph = .{
            .id = id,
            .font = &self,
            .glyph = undefined,
        };
        if (c.FT_Get_Glyph(self.face.*.glyph, &g.glyph) != 0)
            return error.FTGetGlyph;
        return g;
    }

    pub const ShapedChar = struct {
        info: *const c.hb_glyph_info_t,
        pos: *const c.hb_glyph_position_t,
        fn flags(self: @This()) c.hb_glyph_flags_t {
            c.hb_glyph_info_get_glyph_flags(&self.info);
        }
    };

    pub const ShapedText = struct {
        font: *Font,
        info: []const c.hb_glyph_info_t,
        pos: []const c.hb_glyph_position_t,

        const Iterator = struct {
            shaped: *const ShapedText,
            i: usize = 0,

            pub fn next(self: *@This()) ?ShapedChar {
                if (self.i >= self.shaped.pos.len) return null;
                defer self.i += 1;
                return .{
                    .info = &self.shaped.info[self.i],
                    .pos = &self.shaped.pos[self.i],
                };
            }
        };

        pub fn iterator(self: *@This()) Iterator {
            return Iterator{ .shaped = self };
        }
    };

    pub fn shape(self: *Self, buf: Buffer) ShapedText {
        c.hb_shape(self.hb_font, buf.buf, null, 0);
        var glyph_count: u32 = 0;
        const glyph_infos = c.hb_buffer_get_glyph_infos(buf.buf, &glyph_count);
        const glyph_poss = c.hb_buffer_get_glyph_positions(buf.buf, &glyph_count);
        return .{
            .font = self,
            .info = glyph_infos[0..glyph_count],
            .pos = glyph_poss[0..glyph_count],
        };
    }
};

pub const Buffer = struct {
    const Self = @This();

    buf: *c.hb_buffer_t,

    pub fn init() !Self {
        const self: Self = .{
            .buf = c.hb_buffer_create() orelse return error.HFBufFail,
        };
        if (c.hb_buffer_allocation_successful(self.buf) == 0) return error.HFBufFail;

        c.hb_buffer_set_direction(self.buf, c.HB_DIRECTION_LTR);
        c.hb_buffer_set_script(self.buf, c.HB_SCRIPT_LATIN);
        c.hb_buffer_set_language(self.buf, c.hb_language_from_string("en", -1));

        return self;
    }

    pub fn deinit(self: *Self) void {
        c.hb_buffer_destroy(self.buf);
    }

    pub fn addText(self: Self, text: []const u8) void {
        c.hb_buffer_add_utf8(self.buf, text.ptr, @intCast(text.len), 0, @intCast(text.len));
    }

    pub fn reset(self: Self) void {
        c.hb_buffer_reset(self.buf);
        c.hb_buffer_set_direction(self.buf, c.HB_DIRECTION_LTR);
        c.hb_buffer_set_script(self.buf, c.HB_SCRIPT_LATIN);
        c.hb_buffer_set_language(self.buf, c.hb_language_from_string("en", -1));
    }

    pub fn len(self: Self) c_uint {
        return c.hb_buffer_get_length(self.buf);
    }
};

pub const Glyph = struct {
    font: *const Font,
    glyph: c.FT_Glyph,
    id: u32,

    pub fn deinit(self: @This()) void {
        c.FT_Done_Glyph(self.glyph);
    }

    pub fn name(self: @This(), buf: [:0]u8) ![]u8 {
        if (c.FT_Get_Glyph_Name(self.font.face, self.id, buf.ptr, @intCast(buf.len)) != 0)
            return error.FTGetGlyphName;
        return buf[0..std.mem.len(buf.ptr)];
    }

    pub const Bitmap = struct {
        glyph: *const Glyph,
        buf: []u8,
        nrows: u32,
        ncols: u32,
        pitch: i32,

        const Iterator = struct {
            bitmap: *const Bitmap,
            i: i32 = 0,

            pub fn next(self: *@This()) ?[]u8 {
                if (self.i >= self.bitmap.nrows) return null;
                const rowstart = self.i * self.bitmap.pitch;
                const rowend = rowstart + @as(i32, @intCast(self.bitmap.ncols));
                const out = self.bitmap.buf[@intCast(rowstart)..@intCast(rowend)];
                self.i += 1;
                return out;
            }
        };

        pub fn rows(self: *const @This()) Iterator {
            return .{ .bitmap = self };
        }

        pub fn ascii(self: @This(), writer: anytype) !void {
            var rows_ = self.rows();
            while (rows_.next()) |row| {
                for (row) |val| {
                    const s = if (val == 0) "_" else "X";
                    _ = try writer.write(s);
                }
                _ = try writer.write("\n");
            }
        }
    };

    pub fn render(self: *@This()) !Bitmap {
        if (self.glyph.*.format != c.FT_GLYPH_FORMAT_BITMAP) {
            // replaces self.glyph
            if (c.FT_Glyph_To_Bitmap(&self.glyph, c.FT_RENDER_MODE_NORMAL, null, 1) != 0)
                return error.FTRender;
        }
        const bit: *const c.FT_BitmapGlyph = @ptrCast(@alignCast(&self.glyph));
        const bm = bit.*.*.bitmap;
        return .{
            .glyph = self,
            .buf = bm.buffer[0 .. bm.rows * bm.width],
            .nrows = bm.rows,
            .ncols = bm.width,
            .pitch = bm.pitch,
        };
    }
};

pub const FontAtlas = struct {
    const RenderInfo = struct {
        horiBearingX: c_long,
        horiBearingY: c_long,
    };
    const AtlasInfo = struct {
        quad: twod.Rect,
        info: RenderInfo,
    };
    const InfoMap = std.hash_map.AutoHashMap(GlyphIndex, AtlasInfo);

    data: []u8,
    size: twod.Size,
    info: InfoMap,
    padpx: usize,
    needs_update: bool = true,
    col_offset: usize = 0,

    pub fn init(alloc: std.mem.Allocator, size: twod.Size, padpx: usize) !@This() {
        const info = FontAtlas.InfoMap.init(alloc);
        const data = try alloc.alloc(u8, size.area());
        return .{
            .data = data,
            .size = size,
            .info = info,
            .padpx = padpx,
        };
    }

    pub fn deinit(self: *@This()) void {
        self.info.allocator.free(self.data);
        self.info.deinit();
    }

    pub fn addGlyph(
        self: *@This(),
        glyph: GlyphIndex,
        bitmap: Glyph.Bitmap,
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

    pub fn ascii(self: @This(), quad: twod.Rect, writer: anytype) !void {
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

pub fn buildAsciiAtlas(alloc: std.mem.Allocator, font: Font) !FontAtlas {
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
