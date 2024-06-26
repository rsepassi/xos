const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    const t = target.result;

    // libmbedcrypto.a: OBJS_CRYPTO + THIRDPARTY_CRYPTO_OBJECTS
    // libmbedx509.a: OBJS_X509
    // libmbedtls.a: OBJS_TLS

    const libcrypto = b.addStaticLibrary(.{
        .name = "mbedcrypto",
        .target = target,
        .optimize = optimize,
    });

    libcrypto.addIncludePath(.{ .path = "library" });
    libcrypto.addIncludePath(.{ .path = "include" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/everest/include" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/everest/include/everest" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/everest/include/everest/kremlib" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/p256-m/p256-m/include" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/p256-m/p256-m/include/p256-m" });
    libcrypto.addIncludePath(.{ .path = "3rdparty/p256-m/p256-m_driver_interface" });
    libcrypto.addCSourceFiles(.{ .files = &crypto_src_files });
    libcrypto.defineCMacro("_FILE_OFFSET_BITS", "64");
    libcrypto.linkLibC();

    switch (t.os.tag) {
        .windows => {
            libcrypto.linkSystemLibrary("bcrypt");
        },
        else => {},
    }

    const libx509 = b.addStaticLibrary(.{
        .name = "mbedx509",
        .target = target,
        .optimize = optimize,
    });
    libx509.addCSourceFiles(.{ .files = &x509_src_files });
    libx509.addIncludePath(.{ .path = "library" });
    libx509.addIncludePath(.{ .path = "include" });
    libx509.defineCMacro("_FILE_OFFSET_BITS", "64");
    libx509.linkLibC();

    const libtls = b.addStaticLibrary(.{
        .name = "mbedtls",
        .target = target,
        .optimize = optimize,
    });
    libtls.addCSourceFiles(.{ .files = &tls_src_files });
    libtls.addIncludePath(.{ .path = "library" });
    libtls.addIncludePath(.{ .path = "include" });
    libtls.defineCMacro("_FILE_OFFSET_BITS", "64");
    libtls.linkLibC();

    const libtest = b.addStaticLibrary(.{
        .name = "mbedtest",
        .target = target,
        .optimize = optimize,
    });
    libtest.addCSourceFiles(.{ .files = &testlib_src_files });
    libtest.addIncludePath(.{ .path = "library" });
    libtest.addIncludePath(.{ .path = "include" });
    libtest.addIncludePath(.{ .path = "tests/include" });
    libtest.defineCMacro("_FILE_OFFSET_BITS", "64");
    libtest.linkLibC();

    const gen_key = b.addExecutable(.{
        .name = "gen_key",
        .target = target,
        .optimize = optimize,
    });
    gen_key.addCSourceFiles(.{ .files = &.{"programs/pkey/gen_key.c"} });
    gen_key.addIncludePath(.{ .path = "include" });
    gen_key.linkLibrary(libcrypto);
    gen_key.linkLibC();

    const cert_write = b.addExecutable(.{
        .name = "cert_write",
        .target = target,
        .optimize = optimize,
    });
    cert_write.addCSourceFiles(.{ .files = &.{"programs/x509/cert_write.c"} });
    cert_write.addIncludePath(.{ .path = "include" });
    cert_write.addIncludePath(.{ .path = "tests/include" });
    cert_write.linkLibrary(libcrypto);
    cert_write.linkLibrary(libx509);
    cert_write.linkLibrary(libtest);
    cert_write.linkLibC();

    b.installArtifact(libcrypto);
    b.installArtifact(libx509);
    b.installArtifact(libtls);
    b.installArtifact(gen_key);
    b.installArtifact(cert_write);
}

const crypto_src_files = [_][]const u8{
    "3rdparty/everest/library/everest.c",
    "3rdparty/everest/library/x25519.c",
    "3rdparty/everest/library/Hacl_Curve25519_joined.c",
    "3rdparty/p256-m/p256-m_driver_entrypoints.c",
    "3rdparty/p256-m/p256-m/p256-m.c",
    "library/aes.c",
    "library/aesni.c",
    "library/aesce.c",
    "library/aria.c",
    "library/asn1parse.c",
    "library/asn1write.c",
    "library/base64.c",
    "library/bignum.c",
    "library/bignum_core.c",
    "library/bignum_mod.c",
    "library/bignum_mod_raw.c",
    "library/camellia.c",
    "library/ccm.c",
    "library/chacha20.c",
    "library/chachapoly.c",
    "library/cipher.c",
    "library/cipher_wrap.c",
    "library/cmac.c",
    "library/constant_time.c",
    "library/ctr_drbg.c",
    "library/des.c",
    "library/dhm.c",
    "library/ecdh.c",
    "library/ecdsa.c",
    "library/ecjpake.c",
    "library/ecp.c",
    "library/ecp_curves.c",
    "library/ecp_curves_new.c",
    "library/entropy.c",
    "library/entropy_poll.c",
    "library/error.c",
    "library/gcm.c",
    "library/hkdf.c",
    "library/hmac_drbg.c",
    "library/lmots.c",
    "library/lms.c",
    "library/md.c",
    "library/md5.c",
    "library/memory_buffer_alloc.c",
    "library/nist_kw.c",
    "library/oid.c",
    "library/padlock.c",
    "library/pem.c",
    "library/pk.c",
    "library/pk_wrap.c",
    "library/pkcs12.c",
    "library/pkcs5.c",
    "library/pkparse.c",
    "library/pkwrite.c",
    "library/platform.c",
    "library/platform_util.c",
    "library/poly1305.c",
    "library/psa_crypto.c",
    "library/psa_crypto_aead.c",
    "library/psa_crypto_cipher.c",
    "library/psa_crypto_client.c",
    "library/psa_crypto_driver_wrappers_no_static.c",
    "library/psa_crypto_ecp.c",
    "library/psa_crypto_ffdh.c",
    "library/psa_crypto_hash.c",
    "library/psa_crypto_mac.c",
    "library/psa_crypto_pake.c",
    "library/psa_crypto_rsa.c",
    "library/psa_crypto_se.c",
    "library/psa_crypto_slot_management.c",
    "library/psa_crypto_storage.c",
    "library/psa_its_file.c",
    "library/psa_util.c",
    "library/ripemd160.c",
    "library/rsa.c",
    "library/rsa_alt_helpers.c",
    "library/sha1.c",
    "library/sha256.c",
    "library/sha512.c",
    "library/sha3.c",
    "library/threading.c",
    "library/timing.c",
    "library/version.c",
    "library/version_features.c",
};

const x509_src_files = [_][]const u8{
    "library/x509.c",
    "library/x509_create.c",
    "library/x509_crl.c",
    "library/x509_crt.c",
    "library/x509_csr.c",
    "library/x509write.c",
    "library/x509write_crt.c",
    "library/x509write_csr.c",
    "library/pkcs7.c",
};

const tls_src_files = [_][]const u8{
    "library/debug.c",
    "library/mps_reader.c",
    "library/mps_trace.c",
    "library/net_sockets.c",
    "library/ssl_cache.c",
    "library/ssl_ciphersuites.c",
    "library/ssl_client.c",
    "library/ssl_cookie.c",
    "library/ssl_debug_helpers_generated.c",
    "library/ssl_msg.c",
    "library/ssl_ticket.c",
    "library/ssl_tls.c",
    "library/ssl_tls12_client.c",
    "library/ssl_tls12_server.c",
    "library/ssl_tls13_keys.c",
    "library/ssl_tls13_client.c",
    "library/ssl_tls13_server.c",
    "library/ssl_tls13_generic.c",
};

const testlib_src_files = [_][]const u8{
    "tests/src/asn1_helpers.c",
    "tests/src/bignum_helpers.c",
    "tests/src/certs.c",
    "tests/src/fake_external_rng_for_test.c",
    "tests/src/helpers.c",
    "tests/src/psa_crypto_helpers.c",
    "tests/src/psa_exercise_key.c",
    "tests/src/random.c",
    "tests/src/threading_helpers.c",
};
