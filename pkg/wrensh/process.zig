const std = @import("std");
const wren = @import("wren");
const uv = @import("uv");
const coro = @import("zigcoro");
const wrensh = @cImport(@cInclude("wrensh.h"));

const log = std.log.scoped(.wrensh);

pub fn runc(vm: ?*wren.c.WrenVM) callconv(.C) void {
    runcSafe(wren.VM.get(vm)) catch {};
}

fn runcSafe(vm: *wren.VM) !void {
    _ = try coro.xasync(runCoro, .{vm}, 1 << 14);
}

fn runCoro(vm: *wren.VM) void {
    const fiber = vm.getSlot(Slots.fiber.i(), .Handle);
    defer fiber.deinit();

    runCoroSafe(vm) catch |err| {
        const msg = std.fmt.allocPrintZ(vm.args.allocator, "process spawn failed err={any}", .{err}) catch @panic("no mem");
        defer vm.args.allocator.free(msg);
        log.debug("abort fiber {s}", .{msg});

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

fn runCoroSafe(vm: *wren.VM) !void {
    log.debug("run", .{});
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));
    const alloc = vm.args.allocator;

    const wargs = vm.getSlot(Slots.args.i(), .List);
    const wargs_len = wargs.len();

    const args = try alloc.alloc([:0]const u8, wargs_len);
    defer alloc.free(args);

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

    const out = try uv.coro.Process.run(loop, alloc, args, .{
        .env = env,
    });
    vm.ensureSlots(2);
    vm.setSlot(1, out.exit_status);
}

pub fn run(vm: ?*wren.c.WrenVM) callconv(.C) void {
    runSafe(wren.VM.get(vm)) catch |err| {
        log.err("{any}", .{err});
    };
}

fn runSafe(vm: *wren.VM) !void {
    log.debug("run", .{});
    errdefer vm.abortFiber("failed to spawn process", .{});

    vm.ensureSlots(vm.numSlots() + 1);

    const ctx = vm.getUser(wrensh.Ctx);
    const alloc = vm.args.allocator;

    const args = blk: {
        const wargs = vm.getSlot(Slots.args.i(), .List);
        const wargs_len = wargs.len();
        const args = try alloc.alloc(?*anyopaque, wargs_len + 1);
        args[args.len - 1] = null;

        for (0..wargs_len) |i| {
            const arg = wargs.get(i, Slots.scratch.i(), .String);
            log.debug("- {s}", .{arg});
            args[i] = @constCast(arg.ptr);
        }
        break :blk args;
    };
    defer alloc.free(args);

    const env = blk: {
        var env: ?[]?*anyopaque = null;
        const maybe_wenv = if (vm.getSlot(Slots.env.i(), .Type) == .Null) null else vm.getSlot(Slots.env.i(), .List);
        if (maybe_wenv) |wenv| {
            const wenv_len = wenv.len();
            env = try alloc.alloc(?*anyopaque, wenv_len + 1);
            env.?[env.?.len - 1] = null;
            for (0..wenv_len) |i| {
                const entry = wenv.get(i, Slots.scratch.i(), .String);
                log.debug("- {s}", .{entry});
                env.?[i] = @constCast(entry.ptr);
            }
        }
        break :blk env;
    };
    defer if (env) |e| alloc.free(e);

    var state = try alloc.create(RunState);
    errdefer alloc.destroy(state);
    state.* = .{
        .vm = vm,
        .fiber = vm.getSlot(Slots.fiber.i(), .Handle),
        .handle = .{},
        .return_code = vm.getSlot(Slots.rc.i(), .Bool),
    };

    var stdio: [3]uv.uv_stdio_container_t = undefined;
    stdio[0].flags = uv.UV_IGNORE;

    var stdout_f: ?std.fs.File = null;
    switch (vm.getSlot(Slots.stdout.i(), .Type)) {
        .Num => {
            const fd: i32 = @intFromFloat(vm.getSlot(Slots.stdout.i(), .Num));
            log.debug("stdout redirect fd={d}", .{fd});
            stdio[1].data.fd = fd;
            stdio[1].flags = uv.UV_INHERIT_FD;
        },
        .String => {
            const path = vm.getSlot(Slots.stdout.i(), .String);
            log.debug("stdout redirect path={s}", .{path});
            stdout_f = try std.fs.cwd().createFile(path, .{});
            stdio[1].data.fd = stdout_f.?.handle;
            stdio[1].flags = uv.UV_INHERIT_FD;
        },
        .Null => {
            log.debug("stdout will be read", .{});
            state.stdout_state = .{
                .str = std.ArrayList(u8).init(alloc),
            };
            const stdout_state = &state.stdout_state.?;
            stdio[1].flags = uv.UV_CREATE_PIPE | uv.UV_WRITABLE_PIPE;
            stdio[1].data.stream = @ptrCast(&stdout_state.pipe);
            _ = uv.uv_pipe_init(@ptrCast(@alignCast(ctx.loop)), &stdout_state.pipe, 0);
            uv.uv_handle_set_data(@ptrCast(&stdout_state.pipe), state);
        },
        else => {
            return error.UnsupportedType;
        },
    }
    defer if (stdout_f) |f| f.close();

    var stderr_f: ?std.fs.File = null;
    switch (vm.getSlot(Slots.stderr.i(), .Type)) {
        .Num => {
            const fd: i32 = @intFromFloat(vm.getSlot(Slots.stderr.i(), .Num));
            log.debug("stderr redirect fd={d}", .{fd});
            stdio[2].data.fd = fd;
            stdio[2].flags = uv.UV_INHERIT_FD;
        },
        .String => {
            const path = vm.getSlot(Slots.stderr.i(), .String);
            log.debug("stderr redirect path={s}", .{path});
            stderr_f = try std.fs.cwd().createFile(path, .{});
            stdio[2].data.fd = stderr_f.?.handle;
            stdio[2].flags = uv.UV_INHERIT_FD;
        },
        .Null => {
            stdio[2].data.fd = 2;
            stdio[2].flags = uv.UV_INHERIT_FD;
        },
        else => {
            return error.UnsupportedType;
        },
    }
    defer if (stderr_f) |f| f.close();

    var opts = uv.uv_process_options_t{};
    opts.file = @ptrCast(args[0]);
    opts.args = @ptrCast(args.ptr);
    opts.env = if (env) |e| @ptrCast(e) else null;
    opts.cwd = null;
    opts.stdio_count = 3;
    opts.stdio = &stdio;
    opts.exit_cb = exitCb;

    uv.uv_handle_set_data(@ptrCast(&state.handle), state);

    {
        const rc = uv.uv_spawn(@ptrCast(@alignCast(ctx.loop)), &state.handle, &opts);
        if (rc != 0) {
            log.err("uv_spawn fail: {s}", .{uv.uv_strerror(rc)});
            return error.UVSpawnFailed;
        }
    }

    if (state.stdout_state) |*stdout_state| {
        const rc = uv.uv_read_start(@ptrCast(&stdout_state.pipe), stdoutAlloc, stdoutRead);
        if (rc != 0) return error.UVReadFail;
    }
}

const RunState = struct {
    fiber: wren.Handle,
    vm: *wren.VM,
    return_code: bool,
    handle: uv.uv_process_t,

    exit_status: ?i64 = null,

    stdout_state: ?struct {
        str: std.ArrayList(u8),
        pipe: uv.uv_pipe_t = undefined,
        done: bool = false,
        readbuf: [1 << 16]u8 = undefined,
    } = null,
};

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

fn finalize(state: *RunState) !void {
    const vm = state.vm;
    const alloc = vm.args.allocator;
    const ctx = vm.getUser(wrensh.Ctx);

    defer state.fiber.deinit();
    defer uv.uv_close(@ptrCast(&state.handle), null);
    defer alloc.destroy(state);

    vm.ensureSlots(2);
    vm.setSlot(0, state.fiber);
    const success = state.exit_status.? == 0;

    if (success) {
        if (state.stdout_state) |stdout_state| {
            defer stdout_state.str.deinit();
            vm.setSlot(1, stdout_state.str.items);
            try vm.call(@ptrCast(ctx.wren_tx_val));
        } else if (state.return_code) {
            vm.setSlot(1, 0);
            try vm.call(@ptrCast(ctx.wren_tx_val));
        } else {
            try vm.call(@ptrCast(ctx.wren_tx));
        }
    } else {
        if (state.return_code) {
            vm.setSlot(1, state.exit_status.?);
            try vm.call(@ptrCast(ctx.wren_tx_val));
        } else {
            const err = try std.fmt.allocPrintZ(alloc, "process failed with code={d}", .{state.exit_status.?});
            defer alloc.free(err);
            vm.setSlot(1, err);
            try vm.call(@ptrCast(ctx.wren_tx_err));
        }
    }
}

fn exitCb(process: [*c]uv.uv_process_t, exit_status: i64, term_signal: c_int) callconv(.C) void {
    _ = term_signal;
    const state: *RunState = @ptrCast(@alignCast(uv.uv_handle_get_data(@ptrCast(process))));
    state.exit_status = exit_status;

    if (state.stdout_state == null or state.stdout_state.?.done)
        finalize(state) catch @panic("bad process exit");
}

fn stdoutAlloc(process: [*c]uv.uv_handle_t, suggested_size: usize, buf: [*c]uv.uv_buf_t) callconv(.C) void {
    _ = suggested_size;
    const state: *RunState = @ptrCast(@alignCast(uv.uv_handle_get_data(@ptrCast(process))));
    const readbuf = &state.stdout_state.?.readbuf;
    const sz = @sizeOf(@TypeOf(readbuf.*));
    buf.* = uv.uv_buf_init(readbuf.ptr, sz);
}

fn stdoutRead(cstream: *anyopaque, nread: isize, buf: [*c]const uv.uv_buf_t) callconv(.C) void {
    const stream: *uv.uv_stream_t = @ptrCast(@alignCast(cstream));
    const state: *RunState = @ptrCast(@alignCast(uv.uv_handle_get_data(@ptrCast(stream))));
    const stdout_state = &state.stdout_state.?;

    if (nread < 0) {
        stdout_state.done = true;
        _ = uv.uv_read_stop(stream);
        uv.uv_close(@ptrCast(stream), null);
        if (state.exit_status != null) finalize(state) catch @panic("bad exit");
        return;
    }

    if (nread > 0) {
        stdout_state.str.appendSlice(buf.*.base[0..@intCast(nread)]) catch @panic("out of memory");
    }
}
