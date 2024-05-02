const std = @import("std");

const log = std.log.scoped(.zstd);

// TODO: streaming ZSTD api + Reader/Writer
// TODO: ZSTD custom allocator
pub const dict = struct {
    pub const dc = @cImport({
        @cInclude("zdict.h");
    });

    pub fn dcheck(rc: usize) !usize {
        if (dc.ZDICT_isError(rc) != 0) {
            log.err("zstd dict {s}", .{dc.ZDICT_getErrorName(rc)});
            return error.ZstdDictFailed;
        }
        return rc;
    }

    pub fn train(dictbuf: []u8, samples: []const u8, samples_sizes: []const usize) ![]const u8 {
        const size = try dcheck(dc.ZDICT_trainFromBuffer(dictbuf.ptr, dictbuf.len, samples.ptr, samples_sizes.ptr, @intCast(samples_sizes.len)));
        return dictbuf[0..size];
    }
};

pub const c = @cImport({
    @cInclude("zstd.h");
});

pub fn check(rc: usize) !void {
    _ = try checkval(rc);
}

pub fn checkval(rc: usize) !usize {
    if (c.ZSTD_isError(rc) != 0) return error.ZstdError;
    return rc;
}

pub const CCtx = struct {
    ctx: *c.ZSTD_CCtx,

    pub fn init() !@This() {
        if (c.ZSTD_createCCtx()) |p| return .{ .ctx = p } else return error.OutOfMemory;
    }

    pub fn deinit(self: @This()) void {
        _ = c.ZSTD_freeCCtx(self.ctx);
    }

    pub fn loadDict(self: @This(), dictbuf: []const u8) !void {
        try check(c.ZSTD_CCtx_loadDictionary(self.ctx, dictbuf.ptr, dictbuf.len));
    }

    pub const Param = enum(c_uint) {
        CompressionLevel = c.ZSTD_c_compressionLevel,
    };

    pub fn setParam(self: @This(), param: Param, value: c_int) !void {
        try check(c.ZSTD_CCtx_setParameter(self.ctx, @intFromEnum(param), value));
    }

    pub fn compress(self: @This(), dst: []u8, src: []const u8) ![]u8 {
        const size = try checkval(c.ZSTD_compress2(self.ctx, dst.ptr, dst.len, src.ptr, src.len));
        return dst[0..size];
    }
};

pub const DCtx = struct {
    ctx: *c.ZSTD_DCtx,
    pub fn init() !@This() {
        if (c.ZSTD_createDCtx()) |p| return .{ .ctx = p } else return error.OutOfMemory;
    }
    pub fn deinit(self: @This()) void {
        _ = c.ZSTD_freeDCtx(self.ctx);
    }

    pub fn loadDict(self: @This(), dictbuf: []const u8) !void {
        try check(c.ZSTD_DCtx_loadDictionary(self.ctx, dictbuf.ptr, dictbuf.len));
    }

    pub fn decompress(ctx: DCtx, dst: []u8, src: []const u8) ![]const u8 {
        const size = try checkval(c.ZSTD_decompressDCtx(ctx.ctx, dst.ptr, dst.len, src.ptr, src.len));
        return dst[0..size];
    }
};
