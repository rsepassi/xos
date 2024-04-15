const twod = @import("twod.zig");

pub const RGBA = extern struct {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
};

pub const Image = struct {
    data: []RGBA,
    size: twod.Size,
};
