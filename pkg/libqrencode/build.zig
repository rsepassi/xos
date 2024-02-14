const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const os = target.result.os.tag;

    const lib = b.addStaticLibrary(.{
        .name = "qrencode",
        .target = target,
        .optimize = optimize,
    });

    lib.defineCMacro("HAVE_CONFIG_H", null);
    lib.addIncludePath(.{ .path = "." });
    lib.addCSourceFiles(.{ .files = &lib_src_files, .flags = &cflags });
    lib.linkLibC();
    b.installArtifact(lib);

    const exe = b.addExecutable(.{
        .name = "qrenc",
        .target = target,
        .optimize = optimize,
        .linkage = if (os == .linux) .static else null,
        .strip = true,
    });
    exe.defineCMacro("HAVE_CONFIG_H", "1");
    exe.addIncludePath(.{ .path = "." });
    exe.addCSourceFiles(.{ .files = &exe_src_files, .flags = &cflags });

    exe.linkLibrary(lib);
    exe.linkLibC();
    b.installArtifact(exe);
}

const cflags = [_][]const u8{};

const lib_src_files = [_][]const u8{
    "bitstream.c",
    "mask.c",
    "mmask.c",
    "mqrspec.c",
    "qrencode.c",
    "qrinput.c",
    "qrspec.c",
    "rsecc.c",
    "split.c",
};

const exe_src_files = [_][]const u8{
    "qrenc.c",
};
