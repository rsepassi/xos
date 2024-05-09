const std = @import("std");

const Hash = std.crypto.hash.sha2.Sha256;
const bufsize = 1 << 15;

pub fn main() !void {
    const alloc = std.heap.c_allocator;
    const args = try std.process.argsAlloc(alloc);
    const stdout = std.io.getStdOut().writer();

    if (args.len == 1) {
        const stdin = std.io.getStdIn().reader();
        const hash = try getHash(stdin);
        try printEntry(stdout, hash, "-");
        return;
    }

    if (args.len == 2 and std.mem.eql(u8, args[1], "-")) {
        const stdin = std.io.getStdIn().reader();
        const hash = try getHash(stdin);
        try printEntry(stdout, hash, "-");
        return;
    } else if (args.len == 3 and std.mem.eql(u8, args[1], "-c")) {
        var stream = std.io.fixedBufferStream(args[2]);
        const reader = stream.reader();
        const hash = try getHash(reader);
        try printEntry(stdout, hash, "-");
        return;
    }

    for (args[1..]) |arg| {
        var file = try std.fs.cwd().openFile(arg, .{});
        defer file.close();
        const reader = file.reader();
        const hash = try getHash(reader);
        try printEntry(stdout, hash, arg);
    }
}

fn printEntry(writer: anytype, hash: [Hash.digest_length]u8, file: []const u8) !void {
    const hex = std.fmt.bytesToHex(hash, .lower);
    try writer.print("{s}  {s}\n", .{ hex, file });
}

fn getHash(unbuf: anytype) ![Hash.digest_length]u8 {
    var hash = Hash.init(.{});
    var reader = std.io.bufferedReaderSize(bufsize, unbuf);
    var buf: [bufsize]u8 = undefined;
    while (true) {
        const len = try reader.read(@ptrCast(&buf));
        if (len == 0) break;
        hash.update(buf[0..len]);
    }
    return hash.finalResult();
}
