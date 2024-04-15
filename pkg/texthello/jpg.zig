const std = @import("std");
const c = @cImport({
    @cInclude("stb_image.h");
});
const twod = @import("twod.zig");
pub const Image = struct {
    const RGBA = extern struct {
        r: u8,
        g: u8,
        b: u8,
        a: u8,
    };
    data: []RGBA,
    size: twod.Size,

    pub fn deinit(self: @This()) void {
        c.stbi_image_free(self.data.ptr);
    }
    pub fn fromFile(alloc: std.mem.Allocator, path: [:0]const u8) !@This() {
        _ = alloc;
        var x: c_int = 0;
        var y: c_int = 0;
        var n: c_int = 0;
        const data = c.stbi_load(path.ptr, &x, &y, &n, 4) orelse return error.ImageRead;
        const len = x * y;
        const rgb_data: [*c]RGBA = @ptrCast(data);
        const slice = rgb_data[0..@intCast(len)];
        return .{
            .data = @ptrCast(slice),
            .size = .{ .width = @intCast(x), .height = @intCast(y) },
        };
    }
};
