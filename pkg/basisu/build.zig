// https://github.com/hexops/basisu
const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const build_encoder = b.option(bool, "encoder", "Build the basisu encoder") orelse true;
    const build_transcoder = b.option(bool, "transcoder", "build the basisu transcoder") orelse true;

    if (!build_encoder and !build_transcoder) {
        std.log.err("I'm supposed to build... nothing?", .{});
        return error.NothingToBuild;
    }

    const lib = b.addStaticLibrary(.{
        .name = "basisu",
        .target = target,
        .optimize = optimize,
    });

    lib.linkLibCpp();

    lib.defineCMacro("BASISU_FORCE_DEVEL_MESSAGES", "0");
    lib.defineCMacro("BASISU_SUPPORT_KTX2_ZSTD", "0");

    lib.addIncludePath(.{ .path = "encoder" });
    lib.addIncludePath(.{ .path = "transcoder" });

    lib.addCSourceFile(.{ .file = .{ .path = "zstd/zstd.c" }, .flags = &.{} });

    if (build_encoder) {
        lib.addCSourceFiles(.{ .files = &encoder_sources, .flags = &.{} });
        lib.installHeadersDirectory(
            .{ .path = "encoder" },
            "encoder",
            .{
                .exclude_extensions = &.{ "inc", "cpp" },
            },
        );
    }

    if (build_transcoder) {
        lib.addCSourceFiles(.{
            .files = &transcoder_sources,
            .flags = &.{
                "-Wno-deprecated-builtins",
                "-Wno-deprecated-declarations",
                "-Wno-array-bounds",
                "-fno-strict-aliasing",
                "-fno-sanitize=undefined", // there is some UB in transcoder
            },
        });
        lib.installHeadersDirectory(
            .{ .path = "transcoder" },
            "transcoder",
            .{
                .exclude_extensions = &.{ "inc", "cpp" },
            },
        );
    }

    b.installArtifact(lib);
}

const encoder_sources = [_][]const u8{
    "encoder/basisu_backend.cpp",
    "encoder/basisu_basis_file.cpp",
    "encoder/basisu_bc7enc.cpp",
    "encoder/basisu_comp.cpp",
    "encoder/basisu_enc.cpp",
    "encoder/basisu_etc.cpp",
    "encoder/basisu_frontend.cpp",
    "encoder/basisu_gpu_texture.cpp",
    "encoder/basisu_kernels_sse.cpp",
    "encoder/basisu_opencl.cpp",
    "encoder/basisu_pvrtc1_4.cpp",
    "encoder/basisu_resample_filters.cpp",
    "encoder/basisu_resampler.cpp",
    "encoder/basisu_ssim.cpp",
    "encoder/basisu_uastc_enc.cpp",
    "encoder/jpgd.cpp",
    "encoder/pvpngreader.cpp",
};

const transcoder_sources = [_][]const u8{
    "transcoder/basisu_transcoder.cpp",
};
