const std = @import("std");
const c = @cImport(@cInclude("png.h"));
const twod = @import("twod.zig");

pub const PNG = struct {
    const Row = [*]twod.RGBA;

    alloc: std.mem.Allocator,
    data: []twod.RGBA,
    rows: []Row,
    size: twod.Size,

    pub fn init(alloc: std.mem.Allocator, size: twod.Size) !@This() {
        const data = try alloc.alloc(twod.RGBA, size.area());
        errdefer alloc.free(data);
        const rows = try alloc.alloc(Row, size.height);
        for (rows, 0..) |*row, i| {
            row.* = data.ptr + (i * size.width);
        }
        return .{
            .alloc = alloc,
            .rows = rows,
            .data = data,
            .size = size,
        };
    }

    pub fn deinit(self: @This()) void {
        self.alloc.free(self.data);
        self.alloc.free(self.rows);
    }

    pub fn image(self: @This()) twod.Image {
        return .{
            .data = self.data,
            .size = self.size,
        };
    }

    pub fn ascii(self: @This(), writer: anytype) !void {
        for (0..self.size.height) |i| {
            for (0..self.size.width) |j| {
                const val = self.rows[i][j];
                if ((val.r + val.g + val.b) > 50 and val.a > 50)
                    _ = try writer.write("X")
                else
                    _ = try writer.write("_");
            }
            _ = try writer.write("|\n");
        }
    }

    pub fn fromFile(alloc: std.mem.Allocator, path: []const u8) !@This() {
        var f = try std.fs.cwd().openFile(path, .{});
        defer f.close();
        var png_ptr = c.png_create_read_struct(c.PNG_LIBPNG_VER_STRING, null, errorFn, warnFn);
        if (png_ptr == null) return error.PngCreateFail;
        defer c.png_destroy_read_struct(&png_ptr, null, null);
        const info_ptr = c.png_create_info_struct(png_ptr);
        {
            var buftag = c.png_jmpbuf(png_ptr);
            if (c.setjmp(&buftag[0]) != 0) return error.PngSetJmpFail;
        }
        c.png_set_read_fn(png_ptr, @ptrCast(&f), customRead);

        c.png_read_info(png_ptr, info_ptr);
        const size = twod.Size{
            .width = c.png_get_image_width(png_ptr, info_ptr),
            .height = c.png_get_image_height(png_ptr, info_ptr),
        };
        const color_type = c.png_get_color_type(png_ptr, info_ptr);
        const bit_depth = c.png_get_bit_depth(png_ptr, info_ptr);

        {
            if (bit_depth == 16) c.png_set_strip_16(png_ptr);
            if (color_type == c.PNG_COLOR_TYPE_PALETTE)
                c.png_set_palette_to_rgb(png_ptr);
            // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
            if (color_type == c.PNG_COLOR_TYPE_GRAY and bit_depth < 8)
                c.png_set_expand_gray_1_2_4_to_8(png_ptr);
            if (c.png_get_valid(png_ptr, info_ptr, c.PNG_INFO_tRNS) != 0)
                c.png_set_tRNS_to_alpha(png_ptr);

            // These color_type don't have an alpha channel then fill it with 0xff.
            if (color_type == c.PNG_COLOR_TYPE_RGB or
                color_type == c.PNG_COLOR_TYPE_GRAY or
                color_type == c.PNG_COLOR_TYPE_PALETTE)
                c.png_set_filler(png_ptr, 0xFF, c.PNG_FILLER_AFTER);

            if (color_type == c.PNG_COLOR_TYPE_GRAY or
                color_type == c.PNG_COLOR_TYPE_GRAY_ALPHA)
                c.png_set_gray_to_rgb(png_ptr);
            c.png_read_update_info(png_ptr, info_ptr);
        }

        const img = try PNG.init(alloc, size);
        c.png_read_image(png_ptr, @ptrCast(@alignCast(img.rows.ptr)));
        return img;
    }
};

fn customRead(png_ptr: c.png_structp, data: c.png_bytep, length: c.png_size_t) callconv(.C) void {
    customReadSafe(png_ptr, data, length) catch {
        std.debug.print("PNG READ FAIL\n", .{});
        c.png_error(png_ptr, "Error reading file");
    };
}

fn customReadSafe(png_ptr: c.png_structp, data: c.png_bytep, length: c.png_size_t) !void {
    const f: *std.fs.File = @ptrCast(@alignCast(c.png_get_io_ptr(png_ptr)));
    const slice = data[0..length];
    const read = try f.read(slice);
    if (read != length) return error.PngReadFail;
}

fn errorFn(png_ptr: c.png_structp, msg: c.png_const_charp) callconv(.C) void {
    _ = png_ptr;
    std.debug.print("PNG ERROR {s}\n", .{msg});
}

fn warnFn(png_ptr: c.png_structp, msg: c.png_const_charp) callconv(.C) void {
    _ = png_ptr;
    std.debug.print("PNG WARN {s}\n", .{msg});
}
