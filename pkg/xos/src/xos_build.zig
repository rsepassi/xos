const std = @import("std");
const builtin = @import("builtin");

const usage = @embedFile("usage.txt");

const str = []const u8;
const EnvMap = std.process.EnvMap;
const BuildId = str;

pub const std_options = struct {
    const logFn = xosLogFn;
};
const log = std.log.scoped(.xos_build);

pub fn main() !void {
    var timer = try std.time.Timer.start();
    log.debug("xos build", .{});
    const alloc = std.heap.c_allocator;
    const args = try std.process.argsAlloc(alloc);
    var env = try std.process.getEnvMap(alloc);
    const cwd = std.fs.cwd();

    // Usage
    if (args.len == 1 or
        std.mem.eql(u8, args[1], "-h") or
        std.mem.eql(u8, args[1], "--help"))
    {
        _ = try std.io.getStdErr().writer().write(usage);
        std.process.exit(1);
    }

    // xos installation directory
    const xos_root = try getXosRoot(alloc, cwd, env);

    // build artifact directory
    const build_root = try getBuildRoot(cwd, env);

    // pkg definitions directory
    const pkg_root = try getPkgRoot(cwd, env);

    // xos version
    const xos_id = try getXosId(alloc, xos_root, env);
    log.debug("XOS_ID={s}", .{xos_id});

    const cache_dir = try openMakeDir(build_root, "cache");
    const tools_all_dir = try openMakeDir(cache_dir, "tools");
    const tools_dir = try openMakeDir(tools_all_dir, xos_id);

    try linkInternalTools(alloc, xos_root, tools_dir, env);

    const build_id = try runBuild(
        alloc,
        args,
        &env,
        xos_root,
        build_root,
        pkg_root,
        tools_dir,
        xos_id,
    );
    log.debug("build_id={s}", .{build_id});

    try finalizeBuild(alloc, build_root, cache_dir, build_id);

    // put the build id on stdout
    const stdout = std.io.getStdOut().writer();
    try stdout.print("{s}\n", .{build_id});

    const duration = timer.read();
    log.debug("built in {d}ms", .{duration / std.time.ns_per_ms});
}

fn finalizeBuild(
    alloc: std.mem.Allocator,
    build_root: std.fs.Dir,
    cache_dir: std.fs.Dir,
    build_id: BuildId,
) !void {
    const build_out_dir = try getBuildOut(alloc, cache_dir, build_id);
    build_out_dir.access(".ok", .{}) catch {
        try build_out_dir.writeFile2(.{
            .sub_path = ".ok",
            .data = "",
            .flags = .{ .mode = default_mode },
        });
    };

    build_root.deleteFile("out") catch {};
    const build_out_path = try build_out_dir.realpathAlloc(alloc, "out");
    try build_root.symLink(build_out_path, "out", .{});
}

fn getPkgRoot(cwd: std.fs.Dir, env: EnvMap) !std.fs.Dir {
    if (env.get("XOS_PKG_ROOT")) |d| return try cwd.openDir(d, .{});
    const path = env.get("PKG") orelse "pkg";
    return try openMakeDir(cwd, path);
}

fn getBuildOut(
    alloc: std.mem.Allocator,
    cache_dir: std.fs.Dir,
    build_id: BuildId,
) !std.fs.Dir {
    const dir = try std.fs.path.join(alloc, &.{
        "pkg",
        build_id[0..2],
        build_id,
    });
    log.debug("build_out_dir={s}", .{dir});
    return try cache_dir.openDir(dir, .{});
}

fn getXosId(
    alloc: std.mem.Allocator,
    xos_root: std.fs.Dir,
    env: EnvMap,
) !str {
    if (env.get("XOS_ID")) |id| return id;
    const txt = try xos_root.readFileAlloc(alloc, ".xos_id", 1024);
    return std.mem.trimRight(u8, txt, "\n");
}

fn setIfDifferent(key: str, val: str, env: *EnvMap) !void {
    if (env.get(key)) |exist| {
        if (!std.mem.eql(u8, exist, val)) try env.put(key, val);
    } else {
        try env.put(key, val);
    }
}

fn getHostTriple() str {
    return std.fmt.comptimePrint("{s}-{s}-{s}", .{
        @tagName(builtin.cpu.arch),
        @tagName(builtin.os.tag),
        @tagName(builtin.abi),
    });
}

fn runBuild(
    alloc: std.mem.Allocator,
    oargs: []str,
    env: *EnvMap,
    xos_root: std.fs.Dir,
    build_root: std.fs.Dir,
    pkg_root: std.fs.Dir,
    tools_dir: std.fs.Dir,
    xos_id: str,
) !BuildId {
    const xos_root_path = try xos_root.realpathAlloc(alloc, ".");
    const build_root_path = try build_root.realpathAlloc(alloc, ".");
    const pkg_root_path = try pkg_root.realpathAlloc(alloc, ".");
    const tools_dir_path = try tools_dir.realpathAlloc(alloc, ".");
    const xos_tools = try xos_root.realpathAlloc(alloc, "tools");
    const path = try std.fmt.allocPrint(alloc, "{s}:{s}/scripts:{s}", .{
        xos_tools,
        xos_tools,
        tools_dir_path,
    });

    try setIfDifferent("XOS_ID", xos_id, env);
    try setIfDifferent("XOS_ROOT", xos_root_path, env);
    try setIfDifferent("XOS_BUILD_ROOT", build_root_path, env);
    try setIfDifferent("XOS_PKG_ROOT", pkg_root_path, env);
    try env.put("HOST", getHostTriple());
    try env.put("PATH", path);

    const args = try alloc.alloc(str, oargs.len + 1);
    const exe = try std.fs.path.join(alloc, &.{
        xos_tools,
        exename("wrensh"),
    });
    const script = try std.fs.path.join(alloc, &.{
        xos_tools,
        "scripts",
        exename("xos_build.wren"),
    });
    args[0] = exe;
    log.debug("exe={s} script={s}", .{ exe, script });
    args[1] = script;
    for (2.., oargs[1..]) |i, arg| args[i] = arg;

    var child = std.process.Child.init(args, alloc);
    child.env_map = env;
    child.stdout_behavior = .Pipe;
    try child.spawn();
    const stdout_contents = try child.stdout.?.readToEndAlloc(alloc, 1024);
    const out = child.wait();

    if (out) |term| {
        switch (term) {
            .Exited => |code| {
                if (code != 0) return error.NonZeroExitCode;
                return std.mem.trimRight(u8, stdout_contents, "\n");
            },
            else => {
                return error.NonZeroExitCode;
            },
        }
    } else |err| {
        return err;
    }
}

fn getXosRoot(
    alloc: std.mem.Allocator,
    cwd: std.fs.Dir,
    env: EnvMap,
) !std.fs.Dir {
    if (env.get("XOS_ROOT")) |root| return try cwd.openDir(root, .{});
    switch (builtin.os.tag) {
        .macos => {
            const path_max = std.os.PATH_MAX;
            const buf = try alloc.allocSentinel(u8, path_max, 0);
            var len: u32 = path_max;
            const rc = std.os.darwin._NSGetExecutablePath(buf.ptr, &len);
            if (rc != 0) return error.NotFound;
            var buf2 = try alloc.alloc(u8, path_max);
            const self = try std.os.realpathZ(buf, buf2[0..path_max]);
            const dir = std.fs.path.dirname(self).?;
            return try cwd.openDir(dir, .{});
        },
        .linux => {
            const path_max = std.os.PATH_MAX;
            const buf = try alloc.alloc(u8, path_max);
            const self = try std.os.readlink("/proc/self/exe", buf);
            const dir = std.fs.path.dirname(self).?;
            return try cwd.openDir(dir, .{});
        },
        .windows => {
            const path_max = std.os.windows.PATH_MAX_WIDE;
            const buf = try alloc.alloc(u16, path_max);
            const path = try std.os.windows.GetModuleFileNameW(null, buf.ptr, @intCast(buf.len));
            const max_bytes = path_max * 3 + 1;
            var buf2 = try alloc.alloc(u8, max_bytes);
            const self = try std.os.realpathW(path, buf2[0..max_bytes]);
            const dir = std.fs.path.dirname(self).?;
            return try cwd.openDir(dir, .{});
        },
        else => return error.Unimplemented,
    }
}

fn getBuildRoot(cwd: std.fs.Dir, env: EnvMap) !std.fs.Dir {
    if (env.get("XOS_BUILD_ROOT")) |b| return try cwd.openDir(b, .{});
    const path = env.get("BUILD") orelse "build";
    return try openMakeDir(cwd, path);
}

fn openMakeDir(cwd: std.fs.Dir, path: str) !std.fs.Dir {
    return cwd.openDir(path, .{}) catch {
        try cwd.makePath(path);
        return try cwd.openDir(path, .{});
    };
}

fn linkInternalTools(
    alloc: std.mem.Allocator,
    xos_root: std.fs.Dir,
    tools_dir: std.fs.Dir,
    env: EnvMap,
) !void {
    const exists = blk: {
        tools_dir.access(".ok", .{}) catch break :blk false;
        break :blk true;
    };
    if (exists) return;

    // Link all tools into tools_dir
    const pathvar = env.get("PATH") orelse "";

    // system
    const system_string = try std.fmt.allocPrint(
        alloc,
        system_template,
        .{pathvar},
    );
    try tools_dir.writeFile2(.{
        .sub_path = exename("system"),
        .data = system_string,
        .flags = .{ .mode = exe_mode },
    });

    // system_export
    const system_export_string = try std.fmt.allocPrint(
        alloc,
        system_export_template,
        .{pathvar},
    );
    try tools_dir.writeFile2(.{
        .sub_path = exename("system_export"),
        .data = system_export_string,
        .flags = .{ .mode = exe_mode },
    });

    const xos_tools = try xos_root.openDir("tools", .{});
    const bb = try xos_tools.realpathAlloc(alloc, exename("busybox"));
    const wrenshbox = try xos_tools.realpathAlloc(alloc, exename("wrenshbox"));
    const zig = try xos_root.realpathAlloc(
        alloc,
        "zig/" ++ comptime exename("zig"),
    );

    // busybox links
    inline for (bb_tools) |tool| {
        try tools_dir.symLink(bb, exename(tool), .{});
    }

    // wrenshbox links
    inline for (wrenshbox_tools) |tool| {
        try tools_dir.symLink(wrenshbox, exename(tool), .{});
    }

    // nproc
    if (builtin.os.tag == .macos) {
        try tools_dir.writeFile2(.{
            .sub_path = "nproc",
            .data = macos_nproc,
            .flags = .{ .mode = exe_mode },
        });
    } else {
        try tools_dir.symLink(bb, exename("nproc"), .{});
    }

    // zig
    try tools_dir.symLink(zig, exename("zig"), .{});

    // ok
    try tools_dir.writeFile2(.{
        .sub_path = ".ok",
        .data = "",
        .flags = .{ .mode = default_mode },
    });
}

const default_mode = switch (builtin.os.tag) {
    .windows => 0,
    else => 0o664,
};

const exe_mode = switch (builtin.os.tag) {
    .windows => 0,
    else => 0o764,
};

fn exename(comptime name: str) str {
    return if (builtin.os.tag == .windows) name ++ ".exe" else name;
}

const system_template =
    \\#!/usr/bin/env sh
    \\set -e
    \\cmd="$(PATH={s} which $1)"
    \\shift
    \\exec "$cmd" "$@"
;

const system_export_template =
    \\#!/usr/bin/env sh
    \\export PATH="{s}:$PATH"
    \\exec "$@"
;

const wrenshbox_tools = [_]str{
    "echo",
    "dirname",
    "basename",
};

const bb_tools = [_]str{
    "mkdir",
    "ls",
    "rm",
    "mv",
    "cp",
    "ln",
    "realpath",
    "tar",
    "gzip",
    "unzip",
    "wget",
    "cat",
    "cut",
    "grep",
    "head",
    "tail",
    "which",
    "env",
    "touch",
    "find",
    "sed",
    "sleep",
    "bzip2",
    "awk",
    "wc",
    "xargs",
    "sort",
    "uniq",
    "diff",
    "chmod",
    "sh",
    "xz",
    "cmp",
    "tr",
    "od",
    "readlink",
    "expr",
    "rmdir",
    "patch",
};

const macos_nproc =
    \\#!/usr/bin/env sh
    \\exec system sysctl -n hw.logicalcpu
;

pub fn xosLogFn(
    comptime level: std.log.Level,
    comptime scope: @TypeOf(.EnumLiteral),
    comptime format: []const u8,
    args: anytype,
) void {
    const prefix = std.fmt.comptimePrint(
        "[{s} {s}] ",
        level.asText(),
        @tagName(scope),
    );
    const stderr = std.io.getStdErr().writer();
    stderr.print(prefix ++ format ++ "\n", args) catch return;
}
