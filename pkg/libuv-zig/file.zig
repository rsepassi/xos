const std = @import("std");
const uv = @import("uv.zig");
const coro = @import("zigcoro");

const log = std.log.scoped(.uvzig);

pub const File = struct {
    loop: *uv.uv_loop_t,
    fd: c_int,

    const Data = struct {
        frame: coro.Frame,
        fn init() @This() {
            return .{ .frame = coro.xframe() };
        }
    };

    pub fn open(loop: *uv.uv_loop_t, path: [:0]const u8, flags: c_int, mode: c_int) !@This() {
        log.debug("open {s}", .{path});
        var data = Data.init();
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        uv.setReqData(&req, &data);
        try uv.check(uv.uv_fs_open(loop, &req, path, flags, mode, xresume));
        coro.xsuspend();
        if (req.result < 0) {
            return error.FileOpen;
        }

        return .{
            .loop = loop,
            .fd = @intCast(req.result),
        };
    }

    pub fn close(self: @This()) void {
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);
        uv.check(uv.uv_fs_close(self.loop, &req, self.fd, xresume)) catch {};
        coro.xsuspend();
    }

    fn xresume(req: [*c]uv.uv_fs_t) callconv(.C) void {
        const data = uv.getReqData(req, Data);
        coro.xresume(data.frame);
    }

    const Error = error{
        Read,
        Write,
    } || uv.Error;
    const Reader = std.io.Reader(@This(), Error, read);

    fn read(self: @This(), buf: []u8) Error!usize {
        log.debug("read {d}", .{buf.len});
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);
        var uvbuf = uv.newbuf(buf);
        try uv.check(uv.uv_fs_read(self.loop, &req, self.fd, &uvbuf, 1, -1, xresume));
        coro.xsuspend();
        if (req.result < 0) {
            return Error.Read;
        } else {
            return @intCast(req.result);
        }
    }

    const Writer = std.io.Writer(@This(), Error, write);

    fn write(self: @This(), buf: []const u8) Error!usize {
        log.debug("write", .{});
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);
        var uvbuf = uv.newbuf(@constCast(buf));
        std.debug.print("pre fs_write\n", .{});
        try uv.check(uv.uv_fs_write(self.loop, &req, self.fd, &uvbuf, 1, -1, xresume));
        std.debug.print("post fs_write\n", .{});
        coro.xsuspend();
        if (req.result < 0) {
            std.debug.print("post resume fs_write err\n", .{});
            return Error.Write;
        } else {
            std.debug.print("post resume fs_write len {d}\n", .{req.result});
            return @intCast(req.result);
        }
    }

    pub fn writer(self: @This()) Writer {
        return .{ .context = self };
    }

    pub fn reader(self: @This()) Reader {
        return .{ .context = self };
    }

    pub fn stat(self: @This()) !uv.uv_stat_t {
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);
        try uv.check(uv.uv_fs_fstat(self.loop, &req, self.fd, xresume));
        coro.xsuspend();
        try uv.check(req.result);
        return uv.uv_fs_get_statbuf(&req).*;
    }

    pub fn fsync(self: @This()) !void {
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);
        try uv.check(uv.uv_fs_fsync(self.loop, &req, self.fd, xresume));
        coro.xsuspend();
        try uv.check(req.result);
    }

    pub fn sendfile(self: @This(), dst: c_int, src_offset: ?usize, n: usize) !usize {
        log.debug("send {d}", .{n});
        var req = uv.uv_fs_t{};
        defer uv.uv_fs_req_cleanup(&req);
        var data = Data.init();
        uv.setReqData(&req, &data);

        try uv.check(uv.uv_fs_sendfile(self.loop, &req, dst, self.fd, @intCast(src_offset orelse 0), n, xresume));
        coro.xsuspend();

        try uv.check(req.result);
        return @intCast(req.result);
    }

    // TODO:
    // chown
    // utime
    // chmod
    // datasync
    // truncate
    // sendfile
};
