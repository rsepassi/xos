const std = @import("std");
const wren = @import("wren");
const uv = @import("uv");
const coro = @import("zigcoro");
const wrensh = @cImport(@cInclude("wrensh.h"));

const log = std.log.scoped(.wrensh);

pub fn run(vm: ?*wren.c.WrenVM) callconv(.C) void {
    runSafe(wren.VM.get(vm)) catch {};
}

fn runSafe(vm: *wren.VM) !void {
    _ = try coro.xasync(runCoro, .{vm}, 1 << 16);
}

fn runCoro(vm: *wren.VM) void {
    const fiber = vm.getSlot(Slots.fiber.i(), .Handle);
    defer fiber.deinit();

    runCoroSafe(vm) catch |err| {
        const msg = std.fmt.allocPrintZ(vm.args.allocator, "process spawn failed err={any}", .{err}) catch @panic("no mem");
        defer vm.args.allocator.free(msg);
        log.debug("run failed {s}", .{msg});

        const ctx = vm.getUser(wrensh.Ctx);
        vm.ensureSlots(2);
        vm.setSlot(0, fiber);
        vm.setSlot(1, msg);
        vm.call(@ptrCast(ctx.wren_tx_err)) catch {
            @panic("bad call");
        };

        return;
    };

    vm.setSlot(0, fiber);
    coro.xsuspendBlock(corofinalize, .{ vm, coro.xframe() });
}

fn corofinalize(vm: *wren.VM, frame: coro.Frame) void {
    const ctx = vm.getUser(wrensh.Ctx);
    vm.call(@ptrCast(ctx.wren_tx_val)) catch {
        vm.abortFiber("error", .{});
    };
    frame.deinit();
}

const Slots = enum(u8) {
    fiber = 1,
    args,
    env,
    rc,
    stdout,
    stderr,
    scratch,

    fn i(self: @This()) usize {
        return @intFromEnum(self);
    }
};

fn runCoroSafe(vm: *wren.VM) !void {
    log.debug("run", .{});
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));
    const alloc = vm.args.allocator;

    const wargs = vm.getSlot(Slots.args.i(), .List);
    const wargs_len = wargs.len();

    const args = try alloc.alloc([:0]const u8, wargs_len);
    defer alloc.free(args);

    vm.ensureSlots(Slots.scratch.i());
    for (0..wargs_len) |i| {
        const arg = wargs.get(i, Slots.scratch.i(), .String);
        log.debug("- {s}", .{arg});
        args[i] = arg;
    }

    const env = blk: {
        var env: ?[][:0]const u8 = null;
        if (vm.getSlot(Slots.env.i(), .Type) == .Null) {
            break :blk env;
        }

        const wenv = vm.getSlot(Slots.env.i(), .List);
        const wenv_len = wenv.len();
        env = try alloc.alloc([:0]const u8, wenv_len);
        for (0..wenv_len) |i| {
            const entry = wenv.get(i, Slots.scratch.i(), .String);
            log.debug("- {s}", .{entry});
            env.?[i] = entry;
        }
        break :blk env;
    };
    defer if (env) |e| alloc.free(e);

    const return_code = vm.getSlot(Slots.rc.i(), .Bool);

    var stdout: struct {
        opts: ?uv.coro.Process.StdioOpts = null,
        state: ?union(enum) {
            file: uv.coro.File,
            pipe: uv.coro.Pipe,
        } = null,
    } = .{ .opts = null, .state = null };
    switch (vm.getSlot(Slots.stdout.i(), .Type)) {
        .Num => {
            const fd: i32 = @intFromFloat(vm.getSlot(Slots.stdout.i(), .Num));
            log.debug("stdout redirect fd={d}", .{fd});
            stdout = .{ .opts = .{ .inherit_fd = fd } };
        },
        .String => {
            const path = vm.getSlot(Slots.stdout.i(), .String);

            if (std.mem.eql(u8, path, "/dev/null")) {} else {
                const file = try uv.coro.File.open(loop, path, uv.File.Flags.TRUNC | uv.File.Flags.WRONLY | uv.File.Flags.CREAT, 0o664);
                log.debug("stdout redirect path={s}", .{path});
                stdout = .{
                    .opts = .{ .inherit_fd = file.fd },
                    .state = .{ .file = file },
                };
            }
        },
        .Null => {
            log.debug("stdout will be read", .{});
            stdout.state = .{ .pipe = undefined };

            var pipe = &stdout.state.?.pipe;
            try pipe.init(loop, false);

            stdout.opts = .{ .create_pipe = .{ .pipe = stdout.state.?.pipe.stream().handle, .flow = .WO } };
        },
        else => {
            return error.UnsupportedType;
        },
    }
    defer {
        if (stdout.state) |*state| {
            switch (state.*) {
                .file => |f| {
                    log.debug("file close {d}", .{&state.file.fd});
                    f.close();
                },
                .pipe => |p| {
                    _ = p;
                    log.debug("pipe close {*}", .{&state.pipe.handle});
                    state.pipe.close();
                },
            }
        }
    }

    if (stdout.state != null and stdout.state.? == .pipe) {
        const pframe = try coro.xasync(prun, .{ loop, alloc, args, env, stdout.opts }, 1 << 15);
        defer pframe.deinit();

        var stream = stdout.state.?.pipe.stream();
        const reader = stream.reader();
        const contents = try reader.readAllAlloc(alloc, 1 << 30);
        defer alloc.free(contents);

        const out = try coro.xawait(pframe);
        if (out.exit_status != 0) {
            if (return_code) {
                vm.ensureSlots(2);
                vm.setSlot(1, out.exit_status);
            }
            return error.ProcessFailed;
        } else {
            vm.ensureSlots(2);
            vm.setSlot(1, contents);
        }
    } else {
        const out = try prun(loop, alloc, args, env, stdout.opts);
        vm.ensureSlots(2);
        vm.setSlot(1, out.exit_status);
    }
}

fn prun(
    loop: *uv.uv_loop_t,
    alloc: std.mem.Allocator,
    args: [][:0]const u8,
    env: ?[][:0]const u8,
    stdout: ?uv.coro.Process.StdioOpts,
) !uv.coro.Process.Status {
    const out = try uv.coro.Process.run(loop, alloc, args, .{
        .env = env,
        .stdio = .{ null, stdout, null },
    });
    return out;
}
