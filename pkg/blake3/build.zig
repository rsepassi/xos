const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const cpu = target.result.cpu.arch;
    const os = target.result.os.tag;

    const lib = b.addStaticLibrary(.{
        .name = "blake3",
        .target = target,
        .optimize = optimize,
    });

    lib.addIncludePath(.{ .path = "c" });
    lib.addCSourceFiles(.{ .files = &lib_src_files, .flags = &cflags });
    switch (os) {
        .linux => {
            switch (cpu) {
                .x86_64 => {
                    lib.addCSourceFiles(.{ .files = &linux_src_files, .flags = &cflags });
                    lib.defineCMacro("BLAKE3_NO_AVX512", null);
                },
                else => {
                    for (default_defines) |d| lib.defineCMacro(d, null);
                },
            }
        },
        .windows => {
            switch (cpu) {
                //.x86_64 => {
                // error: unrecognized file extension of parameter
                // lib.addCSourceFiles(.{ .files = &windows_src_files, .flags = &cflags });
                //},
                else => {
                    for (default_defines) |d| lib.defineCMacro(d, null);
                },
            }
        },
        .macos => {
            switch (cpu) {
                .aarch64 => {
                    lib.addCSourceFile(.{ .file = .{ .path = "c/blake3_neon.c" } });
                    lib.defineCMacro("BLAKE3_USE_NEON", "1");
                },
                else => {
                    for (default_defines) |d| lib.defineCMacro(d, null);
                },
            }
        },
        else => {},
    }
    lib.linkLibC();

    const h1 = b.addInstallHeaderFile("c/blake3.h", "blake3.h");
    const h2 = b.addInstallHeaderFile("c/blake3_impl.h", "blake3_impl.h");
    b.default_step.dependOn(&h1.step);
    b.default_step.dependOn(&h2.step);

    b.installArtifact(lib);
}

const default_defines = [_][]const u8{
    "BLAKE3_NO_SSE2",
    "BLAKE3_NO_SSE41",
    "BLAKE3_NO_AVX2",
    "BLAKE3_NO_AVX512",
};

const cflags = [_][]const u8{};

const lib_src_files = [_][]const u8{
    "c/blake3.c",
    "c/blake3_dispatch.c",
    "c/blake3_portable.c",
};

const linux_src_files = [_][]const u8{
    "c/blake3_avx2_x86-64_unix.S",
    "c/blake3_avx512_x86-64_unix.S",
    "c/blake3_sse2_x86-64_unix.S",
    "c/blake3_sse41_x86-64_unix.S",
};

const windows_src_files = [_][]const u8{
    "c/blake3_avx2_x86-64_windows_msvc.asm",
    "c/blake3_avx512_x86-64_windows_msvc.asm",
    "c/blake3_sse2_x86-64_windows_msvc.asm",
    "c/blake3_sse41_x86-64_windows_msvc.asm",
};
