const std = @import("std");
const uv = @import("uv.zig");
const coro = @import("zigcoro");

const log = std.log.scoped(.uvzig);

pub const Process = struct {
    const RunOpts = struct {
        env: ?[][:0]const u8 = null,
        cwd: ?[:0]const u8 = null,
    };

    const ProcessOut = struct {
        exit_status: i64,
        term_signal: c_int,
    };

    pub fn run(loop: *uv.uv_loop_t, alloc: std.mem.Allocator, args: [][:0]const u8, opts: RunOpts) !ProcessOut {
        var handle = uv.uv_process_t{};
        defer {
            uv.uv_close(@ptrCast(&handle), xresume2);
            coro.xsuspend();
        }
        var data = Data.init();
        uv.setHandleData(&handle, &data);

        const cargs = try alloc.alloc([*c]const u8, args.len + 1);
        defer alloc.free(cargs);
        cargs[args.len] = null;
        for (args, 0..) |a, i| {
            log.debug("- {s}", .{a});
            cargs[i] = a;
        }

        var cenv: ?[][*c]const u8 = null;
        if (opts.env) |env| {
            cenv = try alloc.alloc([*c]const u8, env.len + 1);
            cenv.?[env.len] = null;
            for (env, 0..) |e, i| cenv.?[i] = e;
        }
        defer if (cenv) |e| alloc.free(e);

        var stdio: [3]uv.uv_stdio_container_t = undefined;
        stdio[0].flags = uv.UV_IGNORE;
        stdio[1].flags = uv.UV_IGNORE;
        stdio[2].flags = uv.UV_IGNORE;

        var o = uv.uv_process_options_t{};
        o.file = cargs[0];
        o.args = @constCast(@ptrCast(cargs.ptr));
        o.env = if (cenv) |env| @constCast(@ptrCast(env.ptr)) else null;
        o.cwd = if (opts.cwd) |cwd| cwd.ptr else null;
        o.stdio_count = 3;
        o.stdio = &stdio;
        o.exit_cb = xresume;

        try uv.check(uv.uv_spawn(loop, &handle, &o));
        coro.xsuspend();

        return data.out.?;
    }

    fn xresume2(process: [*c]uv.uv_handle_t) callconv(.C) void {
        const data = uv.getHandleData(process, Data);
        coro.xresume(data.frame);
    }

    fn xresume(process: [*c]uv.uv_process_t, exit_status: i64, term_signal: c_int) callconv(.C) void {
        const data = uv.getHandleData(process, Data);
        data.out = .{
            .exit_status = exit_status,
            .term_signal = term_signal,
        };
        coro.xresume(data.frame);
    }

    const Data = struct {
        frame: coro.Frame,
        out: ?ProcessOut = null,
        fn init() @This() {
            return .{ .frame = coro.xframe() };
        }
    };
};
