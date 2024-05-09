const std = @import("std");
const uv = @import("uv");
const coro = @import("coro");

const log = std.log.scoped(.server);
pub const std_options = .{
    .log_level = .info,
};

const Ctx = struct {
    loop: *uv.uv_loop_t,
    alloc: std.mem.Allocator,
};

const ClientCtx = struct {
    ctx: *Ctx,
    server: *uv.coro.TCP,
};

pub fn connCoroSafe(ctx: ClientCtx) !void {
    connCoro(ctx) catch |err| {
        log.err("error {any}", .{err});
    };
    coro.xsuspendBlock(connFinalize, .{coro.xframe()});
}

fn connFinalize(frame: coro.Frame) void {
    log.info("client cleanup", .{});
    frame.deinit();
}

pub fn connCoro(ctx: ClientCtx) !void {
    var client: uv.coro.TCP = undefined;
    try client.init(ctx.ctx.loop);
    defer client.close();

    const server_handle = ctx.server.stream().handle;
    const stream = client.stream();
    try uv.check(uv.uv_accept(server_handle, stream.handle));

    var addr_storage: uv.sockaddr_storage = undefined;
    if (try client.getpeername(@ptrCast(&addr_storage))) |ipport| {
        log.info("ip:port={s}:{d}", .{ ipport.ip, ipport.port });
    }

    const reader = stream.reader();
    var buf: [1024]u8 = undefined;
    const msg = try reader.readUntilDelimiter(&buf, '\n');
    log.info("request: {s}", .{msg});

    const writer = stream.writer();
    try writer.print("hi {s}", .{msg});
    log.info("response: hi {s}", .{msg});
}

pub fn mainCoro(ctx: *Ctx) !void {
    log.info("mainCoro", .{});
    var tcp: uv.coro.TCP = undefined;
    try tcp.init(ctx.loop);
    defer tcp.close();

    var addr: uv.sockaddr_in = undefined;
    try uv.check(uv.uv_ip4_addr("127.0.0.1", 9000, &addr));
    try uv.check(uv.uv_tcp_bind(&tcp.handle, @ptrCast(&addr), 0));
    log.info("bind", .{});

    const stream = tcp.stream();
    var listener = stream.listener(32);
    log.info("listener", .{});

    while (try listener.next()) |_| {
        log.info("new connection", .{});
        _ = try coro.xasync(connCoroSafe, .{.{ .ctx = ctx, .server = &tcp }}, 1 << 15);
    }
}

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer if (gpa.deinit() == .leak) log.err("leak!", .{});
    const alloc = gpa.allocator();

    coro.initEnv(.{
        .stack_allocator = alloc,
    });

    var loop: uv.uv_loop_t = undefined;
    try uv.check(uv.uv_loop_init(&loop));
    defer _ = uv.uv_loop_close(&loop);

    const stack = try coro.stackAlloc(alloc, 1 << 15);
    defer alloc.free(stack);

    var ctx = Ctx{
        .loop = &loop,
        .alloc = alloc,
    };
    const frame = try coro.xasync(mainCoro, .{&ctx}, 1 << 15);
    defer frame.deinit();

    var live: c_int = 1;
    while (live > 0) {
        if (std_options.log_level == .debug) {
            log.debug("tick", .{});
            uv.uv_print_all_handles(&loop, uv.__stderrp);
        }
        live = uv.uv_run(&loop, uv.UV_RUN_ONCE);
    }
    log.info("shutdown", .{});
    uv.uv_print_all_handles(&loop, uv.__stderrp);

    try coro.xawait(frame);
}

// TODO:
// reader: start/stop in a context? suspend in alloc?
// cleanup connCoro stack: suspendBlock? xasyncDetached?
