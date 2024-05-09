const std = @import("std");
const uv = @import("uv");
const coro = @import("coro");

const log = std.log.scoped(.client);
pub const std_options = .{
    .log_level = .debug,
};

const Ctx = struct {
    loop: *uv.uv_loop_t,
};

pub fn mainCoro(ctx: *Ctx, payload: []const u8) !void {
    var tcp: uv.coro.TCP = undefined;
    try tcp.init(ctx.loop);
    defer tcp.close();

    log.info("connecting", .{});
    var addr: uv.sockaddr_in = undefined;
    try uv.check(uv.uv_ip4_addr("127.0.0.1", 9000, &addr));
    try tcp.connect(@ptrCast(&addr));
    log.info("connected", .{});

    const stream = tcp.stream();
    _ = try stream.write(payload);
    _ = try stream.write("\n");
    var buf: [1024]u8 = undefined;
    const n = try stream.read(&buf);
    log.info("message: {s}", .{buf[0..n]});
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer if (gpa.deinit() == .leak) log.err("leak!", .{});
    const alloc = gpa.allocator();

    const args = try std.process.argsAlloc(alloc);
    defer std.process.argsFree(alloc, args);
    if (args.len != 2) @panic("must provide message");

    var loop: uv.uv_loop_t = undefined;
    try uv.check(uv.uv_loop_init(&loop));
    defer _ = uv.uv_loop_close(&loop);

    const payload = args[1];
    var ctx = Ctx{ .loop = &loop };

    const stack = try coro.stackAlloc(alloc, 1 << 15);
    log.info("launching main coro", .{});
    const frame = try coro.xasync(mainCoro, .{ &ctx, payload }, stack);

    log.info("looping", .{});
    var live: c_int = 1;
    while (live > 0) live = uv.uv_run(&loop, uv.UV_RUN_ONCE);

    try coro.xawait(frame);
}
