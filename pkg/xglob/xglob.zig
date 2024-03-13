const std = @import("std");
const mem = std.mem;

const open_flags = .{
    .access_sub_paths = true,
};

pub const Iterator = struct {
    allocator: mem.Allocator,
    root: std.fs.Dir,
    segments: std.ArrayListUnmanaged([]const u8),
    stack: std.ArrayListUnmanaged(std.fs.Dir.Iterator),
    components: std.ArrayListUnmanaged([]const u8),
    path: ?[:0]const u8,
    done: bool,

    pub fn init(allocator: mem.Allocator, root: std.fs.Dir, pattern: []const u8) !Iterator {
        if (pattern.len > 0 and pattern[0] == '/') return error.NoAbsolutePatterns;

        var ret = Iterator{
            .allocator = allocator,
            .root = root,
            .segments = std.ArrayListUnmanaged([]const u8){},
            .stack = std.ArrayListUnmanaged(std.fs.Dir.Iterator){},
            .components = std.ArrayListUnmanaged([]const u8){},
            .path = null,
            .done = false,
        };
        errdefer ret.deinit();

        var it = mem.split(u8, pattern, "/");
        while (it.next()) |seg| {
            if (mem.indexOf(u8, seg, "**") != null)
                return error.NotSupported;

            try ret.segments.append(allocator, seg);
        }

        return ret;
    }

    pub fn deinit(self: *Iterator) void {
        self.segments.deinit(self.allocator);
        self.components.deinit(self.allocator);
        if (self.stack.items.len > 0) {
            for (self.stack.items[1..]) |*it| {
                it.dir.close();
            }
        }

        self.stack.deinit(self.allocator);
        if (self.path) |path| self.allocator.free(path);
    }

    pub fn match(pattern: []const u8, str: []const u8) bool {
        if (mem.eql(u8, pattern, "*")) return true;

        var i: usize = 0;
        var it = mem.tokenize(u8, pattern, "*");
        var exact_begin = pattern.len > 0 and pattern[0] != '*';

        while (it.next()) |substr| {
            if (mem.indexOf(u8, str[i..], substr)) |j| {
                if (exact_begin) {
                    if (j != 0) return false;
                    exact_begin = false;
                }

                i += j + substr.len;
            } else return false;
        }

        return if (pattern[pattern.len - 1] == '*') true else i == str.len;
    }

    pub fn next(self: *Iterator) !?[:0]const u8 {
        if (self.done) return null;

        if (self.stack.items.len == 0) {
            try self.stack.append(self.allocator, self.root.iterate());
        }

        var i = self.stack.items.len - 1;
        reset: while (true) {
            var it = &self.stack.items[i];
            while (try it.next()) |entry| {
                if (entry.kind != .file and entry.kind != .directory and entry.kind != .sym_link)
                    continue;

                if (match(self.segments.items[i], entry.name)) switch (entry.kind) {
                    .file, .sym_link => {
                        if (self.path) |path| {
                            self.allocator.free(path);
                            self.path = null;
                        }

                        try self.components.append(self.allocator, entry.name);
                        self.path = try std.fs.path.joinZ(self.allocator, self.components.items);
                        _ = self.components.pop();
                        return self.path;
                    },
                    .directory => {
                        if (i < self.segments.items.len - 1) {
                            const dir = try it.dir.openDir(entry.name, open_flags);
                            try self.stack.append(self.allocator, dir.iterate());
                            try self.components.append(self.allocator, entry.name);
                            i += 1;

                            continue :reset;
                        }
                    },
                    else => unreachable,
                };
            }

            if (i == 0) {
                self.done = true;
                return null;
            }

            i -= 1;
            _ = self.components.pop();
            var dir = self.stack.pop().dir;
            dir.close();
        }
    }
};

const CGlob = struct {
    alloc: std.mem.Allocator,
    from: std.fs.Dir,
    it: Iterator,
};

export fn xglob_init(pattern: [*:0]const u8, dir: [*:0]const u8) ?*anyopaque {
    var alloc = std.heap.c_allocator;
    const from = std.fs.cwd().openDir(std.mem.span(dir), .{ .iterate = true }) catch {
        return null;
    };
    const it = Iterator.init(alloc, from, std.mem.span(pattern)) catch {
        return null;
    };
    const ctx = alloc.create(CGlob) catch {
        return null;
    };
    ctx.* = .{ .alloc = alloc, .from = from, .it = it };
    return ctx;
}

export fn xglob_next(vctx: *anyopaque) ?[*:0]const u8 {
    const ctx: *CGlob = @ptrCast(@alignCast(vctx));
    const next = ctx.it.next() catch {
        return null;
    };
    if (next) |n| {
        return n.ptr;
    }
    return null;
}

export fn xglob_free(vctx: *anyopaque) void {
    const ctx: *CGlob = @ptrCast(@alignCast(vctx));
    ctx.it.deinit();
    ctx.from.close();
    ctx.alloc.destroy(ctx);
}

pub fn copy(
    allocator: mem.Allocator,
    pattern: []const u8,
    from: std.fs.Dir,
    to: std.fs.Dir,
) !void {
    var it = try Iterator.init(allocator, from, pattern);
    defer it.deinit();

    while (try it.next()) |subpath| {
        if (std.fs.path.dirname(subpath)) |dirname|
            try to.makePath(dirname);
        try from.copyFile(subpath, to, subpath, .{});
    }
}

test "no files" {
    try copy_test("", &[_][]const u8{}, &[_][]const u8{});
}

test "single file" {
    try copy_test("build.zig", &[_][]const u8{"build.zig"}, &[_][]const u8{"build.zig"});
}

test "single file in dir" {
    try copy_test("src/main.zig", &[_][]const u8{"src/main.zig"}, &[_][]const u8{"src/main.zig"});
}

test "glob all in root" {
    try copy_test(
        "*",
        &[_][]const u8{ "something.zig", "file", "src/main.zig" },
        &[_][]const u8{ "something.zig", "file" },
    );
}

test "glob single file with extension" {
    try copy_test(
        "*.zig",
        &[_][]const u8{ "build.zig", "README.md", "src/main.zig" },
        &[_][]const u8{"build.zig"},
    );
}

test "glob multiple files with extension" {
    try copy_test(
        "*.txt",
        &[_][]const u8{ "build.txt", "file.txt", "src/main.zig" },
        &[_][]const u8{ "build.txt", "file.txt" },
    );
}

test "glob single file with prefix" {
    try copy_test(
        "build*",
        &[_][]const u8{ "build.zig", "file.zig", "src/main.zig" },
        &[_][]const u8{"build.zig"},
    );
}

test "glob multiple files with prefix" {
    try copy_test(
        "ha*",
        &[_][]const u8{ "haha", "hahahaha.zig", "file", "src/hain.zig" },
        &[_][]const u8{ "haha", "hahahaha.zig" },
    );
}

test "glob all files in dir" {
    try copy_test(
        "src/*",
        &[_][]const u8{ "src/main.zig", "src/file.txt", "README.md", "build.zig" },
        &[_][]const u8{ "src/main.zig", "src/file.txt" },
    );
}

test "glob files with extension in dir" {
    try copy_test(
        "src/*.zig",
        &[_][]const u8{ "src/main.zig", "src/lib.zig", "src/file.txt", "README.md", "build.zig" },
        &[_][]const u8{ "src/main.zig", "src/lib.zig" },
    );
}

test "glob single file in multiple dirs" {
    try copy_test(
        "*/test.zig",
        &[_][]const u8{ "src/test.zig", "something/test.zig", "README.md", "src/a_file" },
        &[_][]const u8{ "src/test.zig", "something/test.zig" },
    );
}

test "glob beginning and end of a file" {
    try copy_test(
        "*hello*",
        &[_][]const u8{ "this_is_hello_file", "hello_world", "hello", "greeting_hello", "file" },
        &[_][]const u8{ "this_is_hello_file", "hello_world", "hello", "greeting_hello" },
    );
}

test "glob beginning and middle" {
    try copy_test(
        "*hello*file",
        &[_][]const u8{ "hellofile", "hello_world_file", "ahelloafile", "greeting_hellofile", "file" },
        &[_][]const u8{ "hellofile", "hello_world_file", "ahelloafile", "greeting_hellofile" },
    );
}

test "glob extension in multiple dirs" {
    try copy_test(
        "*/*.zig",
        &[_][]const u8{ "src/main.zig", "something/lib.zig", "README.md", "src/a_file" },
        &[_][]const u8{ "src/main.zig", "something/lib.zig" },
    );
}

fn copy_test(pattern: []const u8, fs: []const []const u8, expected: []const []const u8) !void {
    var dir = try setup_fs(fs);
    defer dir.cleanup();

    var dst = std.testing.tmpDir(open_flags);
    defer dst.cleanup();

    try copy(std.testing.allocator, pattern, dir.dir, dst.dir);
    try expect_fs(dst.dir, expected);
}

fn setup_fs(files: []const []const u8) !std.testing.TmpDir {
    var root = std.testing.tmpDir(open_flags);
    errdefer root.cleanup();

    for (files) |subpath| {
        if (subpath.len == 0) continue;

        var buf: [std.mem.page_size]u8 = undefined;
        const path = blk: {
            for (subpath, 0..) |c, i| buf[i] = if (c == '/') std.fs.path.sep else c;
            break :blk buf[0..subpath.len];
        };

        const kind: std.fs.File.Kind = if (path[path.len - 1] == std.fs.path.sep)
            .directory
        else
            .file;

        try touch(root.dir, path, kind);
    }

    return root;
}

fn expect_fs(root: std.fs.Dir, expected: []const []const u8) !void {
    for (expected) |subpath| try root.access(subpath, .{ .mode = .read_only });
}

fn touch(root: std.fs.Dir, subpath: []const u8, kind: std.fs.File.Kind) !void {
    switch (kind) {
        .directory => try root.makeDir(subpath),
        .file => {
            if (std.fs.path.dirname(subpath)) |dirname|
                try root.makePath(dirname);

            const file = try root.createFile(subpath, .{});
            file.close();
        },
        else => return error.OnlyDirOrFile,
    }
}
