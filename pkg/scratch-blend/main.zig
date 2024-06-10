const b2d = struct {
    const c = @cImport(@cInclude("blend2d.h"));
};

pub fn main() !void {
    var img: b2d.c.BLImageCore = undefined;
    _ = b2d.c.blImageInit(&img);
    _ = b2d.c.blImageCreate(&img, 128, 128, b2d.c.BL_FORMAT_PRGB32);
    defer _ = b2d.c.blImageDestroy(&img);
}

export fn domain() void {
    var img: b2d.c.BLImageCore = undefined;
    _ = b2d.c.blImageInit(&img);
    _ = b2d.c.blImageCreate(&img, 128, 128, b2d.c.BL_FORMAT_PRGB32);
    defer _ = b2d.c.blImageDestroy(&img);
}
