const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "nghttp2",
        .target = target,
        .optimize = optimize,
    });

    lib.defineCMacro("HAVE_CONFIG_H", null);
    lib.defineCMacro("BUILDING_NGHTTP2", null);
    lib.defineCMacro("NGHTTP2_STATICLIB", null);
    lib.addIncludePath(.{ .path = "." });
    lib.addIncludePath(.{ .path = "lib" });
    lib.addIncludePath(.{ .path = "lib/includes" });
    lib.addCSourceFiles(.{ .files = &lib_src_files, .flags = &cflags });
    lib.linkLibC();
    b.installArtifact(lib);
}

const cflags = [_][]const u8{};

const lib_src_files = [_][]const u8{
    "lib/nghttp2_alpn.c",
    "lib/nghttp2_buf.c",
    "lib/nghttp2_callbacks.c",
    "lib/nghttp2_debug.c",
    "lib/nghttp2_extpri.c",
    "lib/nghttp2_frame.c",
    "lib/nghttp2_hd.c",
    "lib/nghttp2_hd_huffman.c",
    "lib/nghttp2_hd_huffman_data.c",
    "lib/nghttp2_helper.c",
    "lib/nghttp2_http.c",
    "lib/nghttp2_map.c",
    "lib/nghttp2_mem.c",
    "lib/nghttp2_option.c",
    "lib/nghttp2_outbound_item.c",
    "lib/nghttp2_pq.c",
    "lib/nghttp2_priority_spec.c",
    "lib/nghttp2_queue.c",
    "lib/nghttp2_ratelim.c",
    "lib/nghttp2_rcbuf.c",
    "lib/nghttp2_session.c",
    "lib/nghttp2_stream.c",
    "lib/nghttp2_submit.c",
    "lib/nghttp2_time.c",
    "lib/nghttp2_version.c",
    "lib/sfparse.c",
};
