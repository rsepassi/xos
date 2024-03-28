const builtin = @import("builtin");
const std = @import("std");

const lib = @cImport({
    @cInclude("build_internal.h");
    @cInclude("build.h");
    @cInclude("env.h");
    @cInclude("stdio.h");
});

const JobState = struct {
    process: std.ChildProcess,
    poller: std.io.Poller(StreamEnum),
    ready: bool = false,

    fn kill(self: *@This()) void {
        _ = self.process.kill() catch {};
    }
};

const StreamEnum = enum {
    stdout,
};

export fn poller_init(p: **lib.poller_t) void {
    p.* = @ptrCast(std.heap.c_allocator.create(JobState) catch @panic("bad malloc"));
}

export fn poller_destroy(p: *lib.poller_t) void {
    const poller: *JobState = @ptrCast(@alignCast(p));
    std.heap.c_allocator.destroy(poller);
}

export fn poller_ready(p: *lib.poller_t) bool {
    const job: *JobState = @alignCast(@ptrCast(p));
    return job.ready;
}

export fn jobstart(j: *lib.job, e: *lib.edge, out: *lib.poller_t) c_int {
    jobstartZig(j, e, @ptrCast(@alignCast(out))) catch {
        return -1;
    };
    return 0;
}

fn jobstartZig(j: *lib.job, e: *lib.edge, out: *JobState) !void {
    lib.nstarted += 1;
    for (0..e.nout) |i| {
        const n: *lib.node = e.out[i];
        if (n.mtime == lib.MTIME_MISSING) {
            const path = n.path.*.s()[0..n.path.*.n];
            try std.fs.cwd().makePath(path);
        }
    }

    const respfile = lib.edgevar(e, @constCast("rspfile"), false);
    if (respfile != 0) {
        const content = lib.edgevar(e, @constCast("rspfile_content"), true);
        if (lib.writefile(respfile.*.s(), content) < 0) {
            return error.WriteFile;
        }
    }
    errdefer {
        if (respfile != 0 and !lib.buildopts.keeprsp) {
            _ = lib.remove(respfile.*.s());
        }
    }

    j.edge = e;
    j.cmd = lib.edgevar(e, @constCast("command"), true);

    if (!lib.consoleused)
        lib.printstatus(e, j.cmd);

    // TODO
    // if (e.pool != &lib.consolepool) {}

    const argv = [_][]const u8{ "sh", "-c", j.cmd.*.s()[0..j.cmd.*.n] };
    var process = std.ChildProcess.init(&argv, std.heap.c_allocator);
    process.stdin_behavior = .Ignore;
    process.stdout_behavior = .Pipe;
    process.stderr_behavior = .Ignore;
    try process.spawn();
    j.failed = false;
    if (e.pool == &lib.consolepool) lib.consoleused = true;

    out.process = process;
    out.poller = std.io.poll(std.heap.c_allocator, StreamEnum, .{
        .stdout = process.stdout.?,
    });
}

export fn jobwork(j: *lib.job) bool {
    return jobworkZig(j, @ptrCast(@alignCast(j.poller))) catch false;
}

const BUFSIZ = 4096;

fn jobDone(j: *lib.job, job: *JobState) void {
    lib.nfinished += 1;
    if (job.process.wait()) |term| {
        switch (term) {
            .Exited => |c| {
                if (c != 0) {
                    lib.warn("job failed with status {d}: {s}", c, j.cmd.*.s());
                    j.failed = true;
                }
            },
            .Signal => |c| {
                lib.warn("job terminated due to signal {d}: {s}", c, j.cmd.*.s());
                j.failed = true;
            },
            else => {
                lib.warn("job status unknown: {s}", j.cmd.*.s());
                j.failed = true;
            },
        }
    } else |err| {
        err catch {};
        lib.warn("waitpid {d}", job.process.id);
        j.failed = true;
    }

    if (j.buf.len > 0 and (!lib.consoleused or j.failed)) {
        const buf = j.buf.data[0..j.buf.len];
        _ = std.io.getStdOut().writer().write(buf) catch {};
    }
    j.buf.len = 0;
    const e: *lib.edge = j.edge;
    if (e.pool != 0) {
        const p: *lib.pool = e.pool;
        if (p == &lib.consolepool) lib.consoleused = false;
        if (p.work != 0) {
            const new: *lib.edge = p.work;
            p.work = p.work.*.worknext;
            new.worknext = lib.work;
            lib.work = new;
        } else {
            p.numjobs -= 1;
        }
    }
    if (!j.failed) lib.edgedone(e);
}

var lastInstant: ?i128 = null;

comptime {
    if (builtin.os.tag == .windows) {
        @export(clock_gettime, .{ .name = "clock_gettime", .linkage = .strong });
    }
}

fn clock_gettime(clock: c_int, ts: *lib.timespec) callconv(.C) c_int {
    _ = clock;
    const now = std.time.nanoTimestamp();
    if (lastInstant) |last| {
        if (std.math.order(now, last) == .gt) {
            lastInstant = now;
        }
    } else {
        lastInstant = now;
    }

    const last = lastInstant.?;
    ts.tv_sec = @intCast(@divFloor(last, std.time.ns_per_s));
    ts.tv_nsec = @intCast(last - (ts.tv_sec * std.time.ns_per_s));

    return 0;
}

fn jobworkZig(j: *lib.job, job: *JobState) !bool {
    errdefer {
        job.kill();
        j.failed = true;
        jobDone(j, job);
    }
    var newdata: []u8 = undefined;
    var newcap: usize = 0;
    if (j.buf.cap - j.buf.len < BUFSIZ / 2) {
        const buf = j.buf.data[0..j.buf.cap];
        newcap = j.buf.cap + BUFSIZ;
        newdata = try std.heap.c_allocator.realloc(buf, newcap);
        j.buf.cap = newcap;
        j.buf.data = newdata.ptr;
    }

    const buf = j.buf.data[j.buf.len..j.buf.cap];
    const n = try job.process.stdout.?.read(buf);
    if (n > 0) {
        j.buf.len += n;
        return true;
    } else {
        jobDone(j, job);
        return false;
    }
}

export fn poll(pollers: [*c]*lib.poller_t, pollers_len: usize, timeout: c_int) c_int {
    var timer = std.time.Timer.start() catch @panic("bad timer");
    const timeout_ns = @as(u64, @intCast(timeout)) * std.time.ns_per_ms;
    const jobs: []*JobState = @ptrCast(pollers[0..pollers_len]);
    for (jobs) |job| {
        job.ready = job.poller.pollTimeout(500 * std.time.ns_per_ms) catch {
            return -1;
        };
        if (job.ready) return 0;
        if (timer.read() > timeout_ns) break;
    }
    return 0;
}
