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
    const stdout_info = try getVmIO(alloc, vm, Slots.stdout.i(), true);
    defer stdout_info.deinit();
    const stderr_info = try getVmIO(alloc, vm, Slots.stderr.i(), false);
    defer stderr_info.deinit();

    var stdout = Stdio{};
    try stdout.init(stdout_info, loop, Slots.stdout.i());
    defer stdout.deinit();
    var stderr = Stdio{};
    try stderr.init(stderr_info, loop, Slots.stderr.i());
    defer stderr.deinit();

    if (stdout.state != null and stdout.state.? == .pipe) {
        log.debug("spawning async process run", .{});
        const pframe = try coro.xasync(prun, .{ loop, alloc, args, env, stdout.opts, stderr.opts }, 1 << 15);
        defer pframe.deinit();

        var stream = stdout.state.?.pipe.stream();
        const reader = stream.reader();
        const contents = try reader.readAllAlloc(alloc, 1 << 30);
        defer alloc.free(contents);

        const out = try coro.xawait(pframe);
        if (return_code) {
            vm.ensureSlots(2);
            vm.setSlot(1, out.exit_status);
        } else {
            if (out.exit_status != 0) {
                return error.ProcessFailed;
            } else {
                vm.ensureSlots(2);
                vm.setSlot(1, contents);
            }
        }
    } else {
        const out = try prun(loop, alloc, args, env, stdout.opts, stderr.opts);
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
    stderr: ?uv.coro.Process.StdioOpts,
) !uv.coro.Process.Status {
    const out = try uv.coro.Process.run(loop, alloc, args, .{
        .env = env,
        .stdio = .{ null, stdout, stderr },
    });
    return out;
}

const VmIO = struct {
    alloc: std.mem.Allocator,
    io: union(enum) {
        inherit: i32,
        path: [:0]const u8,
        read: void,
        ignore: void,
    },

    fn deinit(self: @This()) void {
        if (self.io == .path) {
            self.alloc.free(self.io.path);
        }
    }
};
fn getVmIO(alloc: std.mem.Allocator, vm: *wren.VM, slot: usize, isout: bool) !VmIO {
    var out = VmIO{ .alloc = alloc, .io = undefined };
    switch (vm.getSlot(slot, .Type)) {
        .Num => {
            const fd: i32 = @intFromFloat(vm.getSlot(Slots.stdout.i(), .Num));
            out.io = .{ .inherit = fd };
        },
        .String => {
            const path = vm.getSlot(Slots.stdout.i(), .String);

            if (std.mem.eql(u8, path, "/dev/null")) {
                out.io = .{ .ignore = void{} };
            } else {
                const p = try alloc.allocSentinel(u8, path.len, 0);
                std.mem.copyForwards(u8, p, path);
                p[p.len] = 0;
                out.io = .{ .path = p };
            }
        },
        .Null => {
            if (isout) {
                out.io = .{ .read = void{} };
            } else {
                out.io = .{ .inherit = 2 };
            }
        },
        else => {
            return error.UnsupportedType;
        },
    }
    return out;
}

const Stdio = struct {
    opts: ?uv.coro.Process.StdioOpts = null,
    state: ?union(enum) {
        file: uv.coro.File,
        pipe: uv.coro.Pipe,
    } = null,

    fn init(
        stdout: *@This(),
        vmio: VmIO,
        loop: *uv.uv_loop_t,
        slot: usize,
    ) !void {
        switch (vmio.io) {
            .ignore => {},
            .inherit => |fd| {
                log.debug("stdout redirect fd={d}", .{fd});
                stdout.* = .{ .opts = .{ .inherit_fd = fd } };
            },
            .path => |path| {
                log.debug("stdout redirect path={s}", .{path});
                const file = try uv.coro.File.open(loop, path, uv.File.Flags.TRUNC | uv.File.Flags.WRONLY | uv.File.Flags.CREAT, 0o664);
                stdout.* = .{
                    .opts = .{ .inherit_fd = file.fd },
                    .state = .{ .file = file },
                };
            },
            .read => {
                log.debug("stdout will be read", .{});
                stdout.state = .{ .pipe = undefined };

                var pipe = &stdout.state.?.pipe;
                try pipe.init(loop, false);

                stdout.opts = .{ .create_pipe = .{ .pipe = pipe.stream().handle, .flow = .WO } };
            },
        }
    }

    fn deinit(self: *const @This()) void {
        var s: *@This() = @constCast(self);
        if (s.state) |*state| {
            switch (state.*) {
                .file => |f| {
                    log.debug("file close {d}", .{state.file.fd});
                    f.close();
                },
                .pipe => |p| {
                    _ = p;
                    state.pipe.close();
                    log.debug("pipe close {*}", .{&state.pipe.handle});
                },
            }
        }
    }
};
