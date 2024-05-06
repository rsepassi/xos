const std = @import("std");

const blake3 = struct {
    pub const c = @cImport({
        @cInclude("blake3.h");
    });

    hasher: c.blake3_hasher,

    const Hash = [32]u8;

    pub fn init() @This() {
        var out: @This() = undefined;
        c.blake3_hasher_init(&out.hasher);
        return out;
    }

    pub fn finalize(self: *@This()) Hash {
        var out: [32]u8 = undefined;
        c.blake3_hasher_finalize(&self.hasher, @ptrCast(&out), out.len);
        return out;
    }

    pub fn update(self: *@This(), data: []const u8) void {
        c.blake3_hasher_update(&self.hasher, data.ptr, data.len);
    }

    pub fn hashReader(reader: anytype, comptime bufsize: usize) !Hash {
        var hasher = init();
        var buf: [bufsize]u8 = undefined;
        var nread: usize = 1;
        while (nread > 0) {
            nread = try reader.read(&buf);
            hasher.update(buf[0..nread]);
        }
        return hasher.finalize();
    }

    pub fn hash(data: []const u8) Hash {
        var hasher = init();
        hasher.update(data);
        return hasher.finalize();
    }

    pub fn deriveKey(context: []const u8, sk: [32]u8) Hash {
        var hasher: @This() = undefined;
        c.blake3_hasher_init_derive_key_raw(&hasher.hasher, context.ptr, context.len);
        hasher.update(&sk);
        return hasher.finalize();
    }
};

const monocypher = struct {
    pub const c = @cImport({
        @cInclude("monocypher.h");
        @cInclude("monocypher-ed25519.h");
    });
};

const mbedtls = struct {
    pub const c = @cImport({
        //@cInclude("mbedtls/x509.h");
    });
};

const crypt = struct {
    // TODO: possibly use mlock + equivalents to lock secret pages in memory
    // (i.e. not spill to swap). Maybe have a SecretAllocator that only
    // generates into locked pages and wipes on free.
    fn Secret(T: type) type {
        const I = @typeInfo(T);
        const comptime_len: ?comptime_int = if (I == .Array) I.Array.len else null;

        return struct {
            pub const len = comptime_len;

            bytes: T,

            fn deinit(self: @This()) void {
                wipe(@constCast(self.slice()));
            }

            fn eql(self: @This(), other_secret: anytype) bool {
                if (comptime_len == null) @compileError("eql requires comptime-known length");
                if (comptime_len != @TypeOf(other_secret).len) @compileError("secrets are of different length");

                const this = self.slice();
                const other = other_secret.slice();

                switch (comptime_len.?) {
                    16 => {
                        return monocypher.c.crypto_verify16(this.ptr, other.ptr) == 0;
                    },
                    32 => {
                        return monocypher.c.crypto_verify32(this.ptr, other.ptr) == 0;
                    },
                    64 => {
                        return monocypher.c.crypto_verify64(this.ptr, other.ptr) == 0;
                    },
                    else => @compileError("unsupported size"),
                }
            }

            inline fn slice(self: @This()) []const u8 {
                if (I == .Array) {
                    return &self.bytes;
                } else {
                    return self.bytes;
                }
            }
        };
    }

    pub const SecretBytes = Secret([]const u8);
    pub const PrivateKey = Secret([32]u8);
    pub const PublicKey = [32]u8;
    pub const Signature = [64]u8;
    pub const SigningKey = struct {
        // first half is sk, second half is pk
        fullkey: Secret([64]u8),

        fn deinit(self: @This()) void {
            self.fullkey.deinit();
        }

        fn sk(self: *const @This()) *const [32]u8 {
            return self.fullkey.bytes[0..32];
        }

        fn pk(self: *const @This()) *const [32]u8 {
            return self.fullkey.bytes[32..64];
        }

        fn sign(self: @This(), message: []const u8) Signature {
            return self.signHash(blake3.hash(message));
        }

        fn signHash(self: @This(), hash: blake3.Hash) Signature {
            var out: Signature = undefined;
            monocypher.c.crypto_eddsa_sign(&out, self.sk(), &hash, hash.len);
            return out;
        }

        fn check(self: @This(), sig: Signature, message: []const u8) bool {
            const hash = blake3.hash(message);
            return monocypher.c.crypto_eddsa_check(&sig, self.pk(), &hash, hash.len) == 0;
        }
    };

    pub fn newprivkey() PrivateKey {
        var buf: [32]u8 = undefined;
        random(&buf);
        return .{ .bytes = buf };
    }

    pub fn pubkey(sk: PrivateKey) PublicKey {
        var pk: PublicKey = undefined;
        monocypher.c.crypto_x25519_public_key(@ptrCast(&pk), @ptrCast(&sk.bytes));
        return pk;
    }

    pub fn newsignkey() SigningKey {
        var out: SigningKey = .{
            .fullkey = Secret([64]u8){ .bytes = undefined },
        };
        var seed = randomN(32);
        var tmppk: PublicKey = undefined;
        monocypher.c.crypto_eddsa_key_pair(&out.fullkey.bytes, &tmppk, &seed);
        return out;
    }

    pub fn random(buf: []u8) void {
        std.crypto.random.bytes(buf);
    }

    pub fn randomN(comptime n: usize) [n]u8 {
        var buf: [n]u8 = undefined;
        std.crypto.random.bytes(&buf);
        return buf;
    }

    pub fn wipe(buf: []u8) void {
        monocypher.c.crypto_wipe(buf.ptr, buf.len);
    }

    pub fn keyx(sk: PrivateKey, pk: PublicKey) Secret([32]u8) {
        var shared: [32]u8 = undefined;
        monocypher.c.crypto_x25519(&shared, &sk.bytes, &pk);
        return .{ .bytes = shared };
    }

    const Nonce = [24]u8;
    const Mac = [16]u8;

    const StreamCrypt = struct {
        ctx: monocypher.c.crypto_aead_ctx,
        nonce: Nonce,

        pub fn init(sk: PrivateKey, nonce: ?Nonce) @This() {
            var out: @This() = undefined;
            out.nonce = if (nonce) |n| n else randomN(24);
            monocypher.c.crypto_aead_init_x(&out.ctx, &sk.bytes, &out.nonce);
            return out;
        }

        pub fn deinit(self: @This()) void {
            const ptr: [*]u8 = @constCast(@ptrCast(&self.ctx));
            wipe(ptr[0..@sizeOf(@TypeOf(self.ctx))]);
        }

        const StreamCiphertext = struct {
            ciphertext: []const u8,
            mac: Mac,
        };

        pub fn authEncrypt(self: *@This(), out: []u8, data: []const u8) StreamCiphertext {
            std.debug.assert(out.len == data.len);
            var mac: Mac = undefined;
            monocypher.c.crypto_aead_write(
                &self.ctx,
                out.ptr,
                &mac,
                null,
                0,
                data.ptr,
                data.len,
            );
            return .{ .ciphertext = out, .mac = mac };
        }

        pub fn authDecrypt(self: *@This(), out: []u8, ciphertext: StreamCiphertext) ![]const u8 {
            std.debug.assert(out.len == ciphertext.ciphertext.len);
            const rc = monocypher.c.crypto_aead_read(
                &self.ctx,
                out.ptr,
                &ciphertext.mac,
                null,
                0,
                ciphertext.ciphertext.ptr,
                ciphertext.ciphertext.len,
            );
            if (rc != 0) return error.DecryptFailed;
            return out;
        }
    };

    const Ciphertext = struct {
        ciphertext: []const u8,
        ad: ?[]const u8,
        mac: Mac,
        nonce: Nonce,

        fn authDecrypt(self: @This(), out: []u8, sk: PrivateKey) ![]const u8 {
            std.debug.assert(out.len == self.ciphertext.len);
            var nonce = self.nonce;
            const data = self.ciphertext;
            const adhash = blk: {
                if (self.ad) |x| {
                    break :blk blake3.hash(x);
                } else {
                    break :blk null;
                }
            };
            const rc = monocypher.c.crypto_aead_unlock(
                out.ptr,
                &self.mac,
                &sk.bytes,
                &nonce,
                if (adhash) |x| &x else null,
                if (adhash) |x| x.len else 0,
                data.ptr,
                data.len,
            );
            if (rc != 0) return error.DecryptFailed;
            return out;
        }
    };

    pub fn authEncrypt(out: []u8, sk: PrivateKey, data: []const u8, ad: ?[]const u8) Ciphertext {
        std.debug.assert(out.len == data.len);
        const adhash = blk: {
            if (ad) |x| {
                break :blk blake3.hash(x);
            } else {
                break :blk null;
            }
        };
        var nonce = randomN(24);
        var mac: [16]u8 = undefined;
        monocypher.c.crypto_aead_lock(
            out.ptr,
            &mac,
            &sk.bytes,
            &nonce,
            if (adhash) |x| &x else null,
            if (adhash) |x| x.len else 0,
            data.ptr,
            data.len,
        );
        return .{ .ciphertext = out, .ad = ad, .mac = mac, .nonce = nonce };
    }

    const Argon2 = struct {
        config: Config,

        const Config = struct {
            algorithm: enum { Argon2d, Argon2i, Argon2id } = .Argon2id,
            nblocks: u32 = 300_000,
            npasses: u32 = 3,
            nlanes: u32 = 1,
        };
        fn init(config: Config) @This() {
            return .{ .config = config };
        }

        fn workAreaSize(self: @This()) usize {
            return self.config.nblocks * 1024;
        }

        const Salt = Secret([16]u8);
        const HashedPassword = struct {
            hash: Secret([32]u8),
            salt: Salt,

            fn deinit(self: @This()) void {
                self.hash.deinit();
                self.salt.deinit();
            }

            fn eql(self: @This(), other: HashedPassword) bool {
                return self.hash.eql(other.hash);
            }
        };

        fn hash(self: @This(), work_area: []u8, password: SecretBytes, salt: ?Salt) HashedPassword {
            std.debug.assert(work_area.len == self.config.nblocks * 1024);

            const config = monocypher.c.crypto_argon2_config{
                .algorithm = switch (self.config.algorithm) {
                    .Argon2d => monocypher.c.CRYPTO_ARGON2_D,
                    .Argon2i => monocypher.c.CRYPTO_ARGON2_I,
                    .Argon2id => monocypher.c.CRYPTO_ARGON2_ID,
                },
                .nb_blocks = self.config.nblocks,
                .nb_passes = self.config.npasses,
                .nb_lanes = self.config.nlanes,
            };

            const xsalt = if (salt) |s| s.bytes else randomN(16);
            const inputs = monocypher.c.crypto_argon2_inputs{
                .pass = password.bytes.ptr,
                .pass_size = @intCast(password.bytes.len),
                .salt = &xsalt,
                .salt_size = xsalt.len,
            };

            const extras = monocypher.c.crypto_argon2_extras{};

            var out: [32]u8 = undefined;
            monocypher.c.crypto_argon2(&out, out.len, work_area.ptr, config, inputs, extras);
            return .{ .hash = .{ .bytes = out }, .salt = .{ .bytes = xsalt } };
        }
    };
};

fn hexdigest(bytes: anytype) [bytes.len * 2]u8 {
    return std.fmt.bytesToHex(bytes, .lower);
}

fn printHex(writer: anytype, title: []const u8, contents: anytype) !void {
    try writer.print("{s}\n", .{title});
    _ = try writer.write(&hexdigest(contents));
    try writer.print("\n", .{});
}

fn base64EncodeAlloc(alloc: std.mem.Allocator, buf: []const u8) !struct { buf: []u8, encoded: []const u8 } {
    const encoder = std.base64.Base64Encoder.init(std.base64.url_safe_alphabet_chars, '=');
    const enc = try alloc.alloc(u8, encoder.calcSize(buf.len));
    const enc2 = encoder.encode(enc, buf);
    return .{ .encoded = enc2, .buf = enc };
}

pub fn main() !void {
    // cryptographic operations
    // * wordlists/mnemonics
    // * TLS x509 certificate
    // * ssh keys
    // * crypto wallets (btc, eth, cosmos)
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();

    const stdout = std.io.getStdOut().writer();

    const sk1 = crypt.newprivkey();
    defer sk1.deinit();
    const pk1 = crypt.pubkey(sk1);

    const pk1_b64 = try base64EncodeAlloc(alloc, &pk1);
    defer alloc.free(pk1_b64.buf);

    const sk2 = crypt.newprivkey();
    defer sk2.deinit();
    const pk2 = crypt.pubkey(sk2);

    const shared1 = crypt.keyx(sk1, pk2);
    defer shared1.deinit();
    const shared2 = crypt.keyx(sk2, pk1);
    defer shared2.deinit();

    const signk1 = crypt.newsignkey();
    defer signk1.deinit();

    const m1 = "hello world";
    const m1_hash = blake3.hash(m1);

    const m1_sig1 = signk1.sign(m1);
    const m1_hash_sig1 = signk1.signHash(m1_hash);

    const m2 = "fare thee well";
    const m3 = "foobar";

    const enc2_buf = try alloc.alloc(u8, m2.len);
    defer alloc.free(enc2_buf);
    var enc2 = crypt.authEncrypt(enc2_buf, sk1, m2, m3);

    const plain2_buf = try alloc.alloc(u8, enc2.ciphertext.len);
    defer alloc.free(plain2_buf);
    const plain2 = try enc2.authDecrypt(plain2_buf, sk1);

    const pw = crypt.SecretBytes{ .bytes = "boo-bar-baz" };
    defer pw.deinit();

    const argon2 = crypt.Argon2.init(.{});
    const argon2_work_area = try alloc.alloc(u8, argon2.workAreaSize());
    defer alloc.free(argon2_work_area);
    var pw_salt: crypt.Argon2.Salt = undefined;
    _ = try std.fmt.bufPrint(&pw_salt.bytes, "0123456789012345", .{});
    const pw_hash1 = argon2.hash(argon2_work_area, pw, pw_salt);
    defer pw_hash1.deinit();
    const pw_hash2 = argon2.hash(argon2_work_area, pw, pw_hash1.salt);
    defer pw_hash2.deinit();

    const derk1 = crypt.PrivateKey{ .bytes = blake3.deriveKey("a/1", pw_hash1.hash.bytes) };
    defer derk1.deinit();
    const derk2 = crypt.PrivateKey{ .bytes = blake3.deriveKey("a/2", pw_hash1.hash.bytes) };
    defer derk2.deinit();

    const f = try std.fs.cwd().openFile("readme.md", .{});
    defer f.close();
    const f_hash = try blake3.hashReader(f.reader(), 2048);

    const f_hash2: blake3.Hash = blk: {
        const f2 = try std.fs.cwd().openFile("readme.md", .{});
        defer f2.close();
        var stream1 = crypt.StreamCrypt.init(sk1, null);
        defer stream1.deinit();
        var stream2 = crypt.StreamCrypt.init(sk1, stream1.nonce);
        defer stream2.deinit();
        var buf: [128]u8 = undefined;
        var buf2: [128]u8 = undefined;
        var hasher = blake3.init();
        const reader = f2.reader();
        while (true) {
            const n = try reader.read(&buf);
            if (n == 0) break;
            const orig_message = buf[0..n];
            const stream_message = stream1.authEncrypt(buf2[0..n], orig_message);
            const plain_message = try stream2.authDecrypt(buf2[0..n], stream_message);
            hasher.update(plain_message);
        }
        break :blk hasher.finalize();
    };

    try printHex(stdout, "sk1", sk1.bytes);
    try printHex(stdout, "pk1", pk1);
    try stdout.print("pk1 b64\n{s}\n", .{pk1_b64.encoded});
    try printHex(stdout, "sk2", sk2.bytes);
    try stdout.print("sk1!=sk2 = {any}\n", .{!sk1.eql(sk2)});
    try printHex(stdout, "pk2", pk2);
    try printHex(stdout, "m1", m1_hash);
    try printHex(stdout, "signk1-full", signk1.fullkey.bytes);
    try printHex(stdout, "signk1-sk", signk1.sk());
    try printHex(stdout, "signk1-pk", signk1.pk());
    try printHex(stdout, "m1-sig", m1_sig1);
    try printHex(stdout, "m1-sig_hash", m1_hash_sig1);
    try stdout.print("sigcheck={any}\n", .{signk1.check(m1_sig1, m1)});
    try stdout.print("plain2 = {s}\n", .{plain2});
    try printHex(stdout, "pw_hash", pw_hash1.hash.bytes);
    try stdout.print("pw_hashes eq = {any}\n", .{pw_hash1.eql(pw_hash2)});
    try printHex(stdout, "a/1 key", derk1.bytes);
    try printHex(stdout, "a/2 key", derk2.bytes);
    try printHex(stdout, "f hash", f_hash);
    try printHex(stdout, "f hash", f_hash2);
    try printHex(stdout, "shared secret", shared1.bytes);
    try stdout.print("shared secret check = {any}\n", .{std.mem.eql(u8, &shared1.bytes, &shared2.bytes)});

    {
        var seed = crypt.randomN(32);
        try printHex(stdout, "ed25519-seed", seed);
        var sk: [64]u8 = undefined;
        var pk: [32]u8 = undefined;
        monocypher.c.crypto_ed25519_key_pair(&sk, &pk, &seed);
        try printHex(stdout, "ed25519-sk", sk);
        try printHex(stdout, "ed25519-pk", pk);
    }
}
