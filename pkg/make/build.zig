const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const os = target.result.os.tag;

    const lib = b.addStaticLibrary(.{
        .name = "gnu",
        .target = target,
        .optimize = optimize,
    });

    lib.defineCMacro("HAVE_CONFIG_H", "1");
    lib.addIncludePath(.{ .path = "lib" });
    lib.addIncludePath(.{ .path = "src" });
    lib.addCSourceFiles(.{ .files = &lib_src_files, .flags = &cflags });
    switch (os) {
        .linux => {
            lib.addCSourceFiles(.{ .files = &lib_linux_files, .flags = &cflags });
        },
        .windows => {
            lib.addCSourceFiles(.{ .files = &lib_windows_files, .flags = &cflags });
        },
        else => {},
    }
    lib.linkLibC();

    const exe = b.addExecutable(.{
        .name = "make",
        .target = target,
        .optimize = optimize,
        .linkage = if (os == .linux) .static else null,
        .strip = true,
    });
    exe.defineCMacro("HAVE_CONFIG_H", "1");
    exe.defineCMacro("LIBDIR", "\"./dummylib\"");
    exe.defineCMacro("LOCALEDIR", "\"./dummylocale\"");
    exe.defineCMacro("INCLUDEDIR", "\"./dummyinclude\"");
    exe.addIncludePath(.{ .path = "src" });
    exe.addIncludePath(.{ .path = "lib" });
    exe.addCSourceFiles(.{ .files = &exe_src_files });
    switch (os) {
        .linux, .macos => {
            exe.addCSourceFiles(.{ .files = &exe_unix_files });
        },
        .windows => {
            exe.addCSourceFiles(.{ .files = &exe_windows_files });
            exe.addIncludePath(.{ .path = "src/w32/include" });
        },
        else => {},
    }
    exe.linkLibrary(lib);
    exe.linkLibC();

    b.installArtifact(exe);
}

const cflags = [_][]const u8{
    "-Wno-cast-qual",
    "-Wno-conversion",
    "-Wno-float-equal",
    "-Wno-sign-compare",
    "-Wno-undef",
    "-Wno-unused-function",
    "-Wno-unused-parameter",
    "-Wno-float-conversion",
    "-Wimplicit-fallthrough",
    "-Wno-pedantic",
    "-Wno-sign-conversion",
    "-Wno-type-limits",
};

const lib_src_files = [_][]const u8{
    "lib/concat-filename.c",
    "lib/findprog-in.c",
    "lib/fnmatch.c",
    "lib/glob.c",
};

const lib_linux_files = [_][]const u8{
    "lib/alloca.c",
    "lib/getloadavg.c",
};

const lib_windows_files = [_][]const u8{
    "lib/getloadavg.c",
};

const exe_src_files = [_][]const u8{
    "src/ar.c",
    "src/arscan.c",
    "src/commands.c",
    "src/default.c",
    "src/dir.c",
    "src/expand.c",
    "src/file.c",
    "src/function.c",
    "src/getopt1.c",
    "src/getopt.c",
    "src/guile.c",
    "src/hash.c",
    "src/implicit.c",
    "src/job.c",
    "src/loadapi.c",
    "src/load.c",
    "src/main.c",
    "src/misc.c",
    "src/output.c",
    "src/read.c",
    "src/remake.c",
    "src/remote-stub.c",
    "src/rule.c",
    "src/shuffle.c",
    "src/signame.c",
    "src/strcache.c",
    "src/variable.c",
    "src/version.c",
    "src/vpath.c",
};

const exe_unix_files = [_][]const u8{
    "src/posixos.c",
};

const exe_windows_files = [_][]const u8{
    "src/w32/pathstuff.c",
    "src/w32/w32os.c",
    "src/w32/compat/dirent.c",
    "src/w32/compat/posixfcn.c",
    "src/w32/subproc/misc.c",
    "src/w32/subproc/sub_proc.c",
    "src/w32/subproc/w32err.c",
};
