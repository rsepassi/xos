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
    var udp: uv.coro.UDP = undefined;
    try udp.init(ctx.loop);
    defer udp.close();

    log.info("connecting", .{});
    var addr: uv.sockaddr_in = undefined;
    try uv.check(uv.uv_ip4_addr("127.0.0.1", 9000, &addr));
    try udp.send(payload, @ptrCast(&addr));
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
    defer alloc.free(stack);
    log.info("launching main coro", .{});
    const frame = try coro.xasync(mainCoro, .{ &ctx, payload }, stack);

    log.info("looping", .{});
    var live: c_int = 1;
    while (live > 0) live = uv.uv_run(&loop, uv.UV_RUN_ONCE);

    try coro.xawait(frame);
    log.info("shutting down", .{});
}
