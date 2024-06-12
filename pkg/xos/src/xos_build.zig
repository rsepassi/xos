const std = @import("std");
const builtin = @import("builtin");

const usage = @embedFile("usage.txt");

const str = []const u8;
const EnvMap = std.process.EnvMap;
const BuildId = str;

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

    const build_id = try runBuild(
        alloc,
        args,
        &env,
        xos_root,
        build_root,
        pkg_root,
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
    xos_id: str,
) !BuildId {
    const xos_root_path = try xos_root.realpathAlloc(alloc, ".");
    const build_root_path = try build_root.realpathAlloc(alloc, ".");
    const pkg_root_path = try pkg_root.realpathAlloc(alloc, ".");
    const xos_tools = try xos_root.realpathAlloc(alloc, "tools");
    const xos_links = try xos_root.realpathAlloc(alloc, "tools/links");
    const path = try std.fmt.allocPrint(alloc, "{s}:{s}/scripts:{s}", .{
        xos_tools,
        xos_tools,
        xos_links,
    });

    try setIfDifferent("XOS_ID", xos_id, env);
    try setIfDifferent("XOS_ROOT", xos_root_path, env);
    try setIfDifferent("XOS_BUILD_ROOT", build_root_path, env);
    try setIfDifferent("XOS_PKG_ROOT", pkg_root_path, env);
    try env.put("XOS_SYSTEM_PATH", env.get("XOS_SYSTEM_PATH") orelse env.get("PATH") orelse "");
    try env.put("XOS_SYSTEM_HOME", env.get("XOS_SYSTEM_HOME") orelse env.get("HOME") orelse "");
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
    const term = try child.wait();

    switch (term) {
        .Exited => |code| {
            if (code != 0) std.process.exit(code);
            return std.mem.trimRight(u8, stdout_contents, "\n");
        },
        else => {
            log.err("{any}\n", .{term});
            std.process.exit(1);
        },
    }
}

fn getXosRoot(
    alloc: std.mem.Allocator,
    cwd: std.fs.Dir,
    env: EnvMap,
) !std.fs.Dir {
    if (env.get("XOS_ROOT")) |root| return try cwd.openDir(root, .{});
    const dir = try std.fs.selfExeDirPathAlloc(alloc);
    return try cwd.openDir(std.fs.path.dirname(dir).?, .{});
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
