const std = @import("std");
const builtin = @import("builtin");

const usage =
    \\
    \\xos [command]
    \\
    \\commands:
    \\* build
    \\
;

pub fn main() !void {
    const alloc = std.heap.c_allocator;
    var args = std.process.args();
    var env = try std.process.getEnvMap(alloc);
    const cwd = std.fs.cwd();

    _ = args.next(); // skip $0
    const maybe_cmd = args.next();

    // Usage
    if (maybe_cmd == null or
        std.mem.eql(u8, maybe_cmd.?, "-h") or
        std.mem.eql(u8, maybe_cmd.?, "--help"))
    {
        _ = try std.io.getStdOut().writer().write(usage);
        std.process.exit(0);
    }

    var root_buf: [std.fs.MAX_PATH_BYTES]u8 = undefined;
    const root = try std.fs.selfExeDirPath(&root_buf);
    const rootdir = try cwd.openDir(root, .{});
    const tools = try rootdir.openDir("tools", .{});
    const scripts = try tools.openDir("scripts", .{});

    const stderr = std.io.getStdErr().writer();
    _ = stderr;

    const script = "xos_main.wren";
    var script_buf: [std.fs.MAX_PATH_BYTES]u8 = undefined;
    const script_path = try scripts.realpath(script, &script_buf);

    var args_buf: [128][]const u8 = undefined;
    args_buf[0] = script_path;
    args_buf[1] = maybe_cmd.?;

    var i: usize = 2;
    while (args.next()) |arg| : (i += 1) {
        args_buf[i] = arg;
    }
    const exec_args = args_buf[0..i];

    const xos_tools = try rootdir.realpathAlloc(alloc, "tools");
    const xos_links = try rootdir.realpathAlloc(alloc, "tools/links");
    const path = try std.fmt.allocPrint(alloc, "{s}:{s}/scripts:{s}", .{
        xos_tools,
        xos_tools,
        xos_links,
    });

    try env.put("XOS_ROOT", root);
    try env.put("XOS_SYSTEM_HOME", env.get("XOS_SYSTEM_HOME") orelse env.get("HOME") orelse "");
    try env.put("XOS_HOST", getHostTriple());
    try env.put("PATH", path);
    std.process.execve(alloc, exec_args, &env) catch @panic("bad exec");
}

fn getHostTriple() []const u8 {
    return std.fmt.comptimePrint("{s}-{s}-{s}", .{
        @tagName(builtin.cpu.arch),
        @tagName(builtin.os.tag),
        @tagName(builtin.abi),
    });
}
