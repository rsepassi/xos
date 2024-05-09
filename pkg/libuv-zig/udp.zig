const std = @import("std");
const uv = @import("uv.zig");
const uv_common = @import("common.zig");
const coro = @import("zigcoro");

const log = std.log.scoped(.uvzig);

const uv_udp_send_cb = struct {
    frame: coro.Frame,
    req: uv.uv_udp_send_t,
    status: c_int = 0,

    fn init(self: *@This()) void {
        uv.setReqData(&self.req, self);
        self.frame = coro.xframe();
    }

    fn cb(req: [*c]uv.uv_udp_send_t, status: c_int) callconv(.C) void {
        const self = uv.getReqData(req, @This());
        self.status = status;
        coro.xresume(self.frame);
    }
};

pub const UDP = struct {
    handle: uv.uv_udp_t,

    pub fn init(self: *@This(), loop: *uv.uv_loop_t) !void {
        try uv.check(uv.uv_udp_init(loop, &self.handle));
    }

    pub fn close(self: *@This()) void {
        var cb: uv_common.uv_close_cb = undefined;
        cb.init(@ptrCast(&self.handle));
        uv.uv_close(@ptrCast(&self.handle), uv_common.uv_close_cb.cb);
        coro.xsuspend();
    }

    pub fn send(self: *@This(), buf: []const u8, dst: ?*uv.sockaddr) !void {
        var req: uv_udp_send_cb = undefined;
        req.init();
        var uvbuf = uv.uv_buf_t{
            .base = @constCast(buf.ptr),
            .len = buf.len,
        };
        try uv.check(uv.uv_udp_send(&req.req, &self.handle, &uvbuf, 1, dst, uv_udp_send_cb.cb));
        coro.xsuspend();
        try uv.check(req.status);
    }

    const IpPort = struct {
        ip: []const u8,
        port: c_ushort,
    };
    pub fn getpeername(self: *@This(), addr_storage: *uv.sockaddr_storage) !?IpPort {
        return try self.getxname(addr_storage, .peer);
    }

    pub fn getsockname(self: *@This(), addr_storage: *uv.sockaddr_storage) !?IpPort {
        return try self.getxname(addr_storage, .sock);
    }

    pub fn getxname(self: *@This(), addr_storage: *uv.sockaddr_storage, xtype: enum { peer, sock }) !?IpPort {
        var addr_size: c_int = @sizeOf(uv.sockaddr_storage);
        switch (xtype) {
            .peer => try uv.check(uv.uv_udp_getpeername(&self.handle, @ptrCast(addr_storage), &addr_size)),
            .sock => try uv.check(uv.uv_udp_getsockname(&self.handle, @ptrCast(addr_storage), &addr_size)),
        }

        if (addr_storage.ss_family == uv.AF_INET) {
            const addr: *uv.sockaddr_in = @ptrCast(@alignCast(addr_storage));
            var ipbuf: [201:0]u8 = undefined;
            try uv.check(uv.uv_ip4_name(addr, &ipbuf, ipbuf.len));
            const iplen = std.mem.len(@as([*:0]u8, @ptrCast(&ipbuf)));
            const ip = ipbuf[0..iplen];

            const port = std.mem.bigToNative(c_ushort, addr.sin_port);

            return .{ .ip = ip, .port = port };
        } else if (addr_storage.ss_family == uv.AF_INET6) {
            const addr: *uv.sockaddr_in6 = @ptrCast(@alignCast(addr_storage));
            var ipbuf: [201:0]u8 = undefined;
            try uv.check(uv.uv_ip6_name(addr, &ipbuf, ipbuf.len));
            const iplen = std.mem.len(@as([*:0]u8, @ptrCast(&ipbuf)));
            const ip = ipbuf[0..iplen];

            const port = std.mem.bigToNative(c_ushort, addr.sin6_port);

            return .{ .ip = ip, .port = port };
        } else {
            return null;
        }
    }
};
