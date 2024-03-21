const std = @import("std");
const builtin = @import("builtin");
const usage = @embedFile("usage.txt");
const str = []const u8;

// User-facing build entrypoint

pub fn main() !void {
    const alloc = std.heap.c_allocator;
    const args = try std.process.argsAlloc(alloc);
    var env = try std.process.getEnvMap(alloc);

    // Usage
    if (args.len == 1 or std.mem.eql(u8, args[1], "-h") or std.mem.eql(u8, args[1], "--help")) {
        _ = try std.io.getStdErr().writer().write(usage);
        std.process.exit(1);
    }

    const xos_root = try getXosRoot(alloc);
    const build_root = try getBuildRoot(alloc, env);
    const cache_dir = try std.fs.path.join(alloc, &.{ build_root, "cache" });
    const tools_dir = try std.fs.path.join(alloc, &.{ cache_dir, "tools" });
    try linkInternalTools(alloc, xos_root, tools_dir);
    try runBuild(alloc, args, &env, xos_root, build_root);
}

fn runBuild(alloc: std.mem.Allocator, args: []str, env: *std.process.EnvMap, xos_root: str, build_root: str) !void {
    const path = try std.fs.path.join(alloc, &.{ xos_root, "tools" });

    try env.put("XOS_SEAL", "1");
    try env.put("XOS_ROOT", xos_root);
    try env.put("XOS_BUILD_ROOT", build_root);
    try env.put("PATH", path);

    const exe = try std.fs.path.join(alloc, &.{ xos_root, "tools", "xos_internal_build" });
    args[0] = exe;
    var child = std.process.Child.init(args, alloc);
    child.env_map = env;
    _ = try child.spawnAndWait();
}

fn getXosRoot(alloc: std.mem.Allocator) !str {
    const self = switch (builtin.os.tag) {
        .macos => blk: {
            const path_max = std.os.PATH_MAX;
            const buf = try alloc.allocSentinel(u8, path_max, 0);
            var len: u32 = path_max;
            const rc = std.os.darwin._NSGetExecutablePath(buf.ptr, &len);
            if (rc != 0) return error.NotFound;
            var buf2 = try alloc.alloc(u8, path_max);
            break :blk try std.os.realpathZ(buf, buf2[0..path_max]);
        },
        .linux => blk: {
            const path_max = std.os.PATH_MAX;
            const buf = try alloc.alloc(u8, path_max);
            break :blk try std.os.readlink("/proc/self/exe", buf);
        },
        .windows => blk: {
            const path_max = std.os.windows.PATH_MAX_WIDE;
            const buf = try alloc.alloc(u16, path_max);
            const path = try std.os.windows.GetModuleFileNameW(null, buf.ptr, @intCast(buf.len));
            const max_bytes = path_max * 3 + 1;
            var buf2 = try alloc.alloc(u8, max_bytes);
            break :blk try std.os.realpathW(path, buf2[0..max_bytes]);
        },
        else => return error.Unimplemented,
    };
    return std.fs.path.dirname(self).?;
}

fn getBuildRoot(alloc: std.mem.Allocator, env: std.process.EnvMap) !str {
    var build = env.get("XOS_BUILD_ROOT");
    if (build) |b| return b;
    build = env.get("BUILD");
    if (build) |b| return b;
    return try std.fs.path.join(alloc, &.{ try std.process.getCwdAlloc(alloc), "build" });
}

fn linkInternalTools(alloc: std.mem.Allocator, xos_root: str, tools_dir: str) !void {
    const okpath = try std.fs.path.join(alloc, &.{ tools_dir, ".ok" });
    const exists = blk: {
        std.fs.cwd().access(okpath, .{}) catch break :blk false;
        break :blk true;
    };
    if (!exists) {
        const exe = try std.fs.path.join(alloc, &.{ xos_root, "tools", "xos_internal_link_tools" });
        var child = std.process.Child.init(&.{
            exe,
            tools_dir,
        }, alloc);
        _ = try child.spawnAndWait();
    }
}
