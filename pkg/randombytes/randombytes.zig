const std = @import("std");

export fn randombytes(cbuf: ?*anyopaque, n: usize) c_int {
    var bufp: [*]u8 = @ptrCast(cbuf.?);
    std.crypto.random.bytes(bufp[0..n]);
    return 0;
}
