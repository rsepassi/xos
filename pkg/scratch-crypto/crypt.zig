const std = @import("std");
const builtin = @import("builtin");

const log = std.log.scoped(.crypt);

const WordlistConfig = struct {
    nwords: ?usize = null,
};
fn Wordlist(config: WordlistConfig) type {
    return struct {
        arr: std.ArrayList([]const u8),
        map: std.StringHashMap(usize),

        fn fromStr(alloc: std.mem.Allocator, src: []const u8) !@This() {
            var arr = std.ArrayList([]const u8).init(alloc);
            errdefer arr.deinit();
            var map = std.StringHashMap(usize).init(alloc);
            errdefer map.deinit();

            if (config.nwords) |n| {
                try arr.ensureTotalCapacity(n);
                try map.ensureTotalCapacity(n);
            }

            var iter = std.mem.splitScalar(u8, src, '\n');
            var i: usize = 0;
            while (iter.next()) |word| : (i += 1) {
                if (config.nwords == null) {
                    try arr.append(word);
                    try map.put(word, i);
                } else {
                    arr.appendAssumeCapacity(word);
                    map.putAssumeCapacity(word, i);
                }
            }

            return .{
                .arr = arr,
                .map = map,
            };
        }

        fn getIdx(self: @This(), word: []const u8) ?usize {
            return self.map.get(word);
        }

        fn getWord(self: @This(), idx: usize) []const u8 {
            return self.arr.items[@intCast(idx)];
        }
    };
}

const hashlib = struct {
    pub fn hashReader(hasher: anytype, reader: anytype, comptime bufsize: usize) ![digestLength(@TypeOf(hasher))]u8 {
        var buf: [bufsize]u8 = undefined;
        var nread: usize = 1;
        while (nread > 0) {
            nread = try reader.read(&buf);
            hasher.update(buf[0..nread]);
        }
        return hasher.finalResult();
    }

    pub fn hash(hasher: anytype, data: []const u8) [digestLength(@TypeOf(hasher))]u8 {
        hasher.update(data);
        return hasher.finalResult();
    }

    fn digestLength(T: type) usize {
        const I = @typeInfo(T);
        if (I == .Pointer) {
            return I.Pointer.child.digest_length;
        } else {
            return T.digest_length;
        }
    }
};

const blake3 = struct {
    pub const c = @cImport({
        @cInclude("blake3.h");
    });

    hasher: c.blake3_hasher,

    const digest_length = 32;
    const Digest = [digest_length]u8;

    pub fn init() @This() {
        var out: @This() = undefined;
        c.blake3_hasher_init(&out.hasher);
        return out;
    }

    pub fn finalResult(self: *@This()) Digest {
        var out: [digest_length]u8 = undefined;
        c.blake3_hasher_finalize(&self.hasher, @ptrCast(&out), out.len);
        return out;
    }

    pub fn finalResultBuf(self: *@This(), out: []u8) void {
        c.blake3_hasher_finalize(&self.hasher, out.ptr, out.len);
    }

    pub fn update(self: *@This(), data: []const u8) void {
        c.blake3_hasher_update(&self.hasher, data.ptr, data.len);
    }

    pub fn reset(self: *@This()) void {
        c.blake3_hasher_reset(&self.hasher);
    }

    pub fn hash(data: []const u8) Digest {
        var hasher = init();
        hasher.update(data);
        return hasher.finalResult();
    }

    pub fn initDeriveKey(context: []const u8) @This() {
        var out: @This() = undefined;
        c.blake3_hasher_init_derive_key_raw(&out.hasher, context.ptr, context.len);
        return out;
    }

    pub fn deriveKey(sk: []const u8, context: []const u8) Digest {
        var hasher = initDeriveKey(context);
        hasher.update(sk);
        return hasher.finalResult();
    }

    pub fn deriveKeyBuf(sk: []const u8, context: []const u8, out: []u8) void {
        var hasher = initDeriveKey(context);
        hasher.update(sk);
        hasher.finalResultBuf(out);
    }
};

const blake2b = struct {
    hasher: monocypher.c.crypto_blake2b_ctx,

    const digest_length = 64;
    const Digest = [digest_length]u8;

    pub fn init() @This() {
        var out: @This() = undefined;
        out.reset();
        return out;
    }

    pub fn finalResult(self: *@This()) Digest {
        var out: [digest_length]u8 = undefined;
        monocypher.c.crypto_blake2b_final(&self.hasher, @ptrCast(&out));
        return out;
    }

    pub fn update(self: *@This(), data: []const u8) void {
        monocypher.c.crypto_blake2b_update(&self.hasher, data.ptr, data.len);
    }

    pub fn reset(self: *@This()) void {
        monocypher.c.crypto_blake2b_init(&self.hasher, digest_length);
    }
};

const monocypher = struct {
    pub const c = @cImport({
        @cInclude("monocypher.h");
        @cInclude("monocypher-ed25519.h");
    });

    pub fn wipe(buf: []u8) void {
        monocypher.c.crypto_wipe(buf.ptr, buf.len);
    }

    fn eql(comptime n: usize, a: *const [n]u8, b: *const [n]u8) bool {
        switch (n) {
            16 => {
                return monocypher.c.crypto_verify16(a, b) == 0;
            },
            32 => {
                return monocypher.c.crypto_verify32(a, b) == 0;
            },
            64 => {
                return monocypher.c.crypto_verify64(a, b) == 0;
            },
            else => @compileError("unsupported comparison length"),
        }
    }
};

const MemoryLock = struct {
    buf: []const u8,

    fn deinit(self: @This()) void {
        const munlock_sig = *const fn (addr: ?*anyopaque, len: usize) callconv(.C) c_int;
        const munlock_fn = switch (builtin.os.tag) {
            .linux, .macos => @extern(munlock_sig, .{
                .name = "munlock",
            }),
            .windows => @extern(munlock_sig, .{
                .name = "VirtualUnlock",
            }),
            else => @compileError("no munlock"),
        };
        _ = munlock_fn(@ptrCast(@constCast(self.buf.ptr)), self.buf.len);
    }
};

fn mlock(buf: []const u8) !MemoryLock {
    const mlock_sig = *const fn (addr: ?*anyopaque, len: usize) callconv(.C) c_int;
    const mlock_fn = switch (builtin.os.tag) {
        .linux, .macos => @extern(mlock_sig, .{
            .name = "mlock",
        }),
        .windows => @extern(mlock_sig, .{
            .name = "VirtualLock",
        }),
        else => @compileError("no mlock"),
    };

    const rc = mlock_fn(@ptrCast(@constCast(buf.ptr)), buf.len);
    if (rc != 0) return error.MlockFailed;
    return .{ .buf = buf };
}

const crypt = struct {
    const SecretAllocator = struct {
        alloc: ?std.mem.Allocator,
        end_index: usize,
        buf: []u8,

        fn initFromAllocator(a: std.mem.Allocator, npages: usize) !@This() {
            const buf = try a.alignedAlloc(u8, std.mem.page_size, npages * std.mem.page_size);
            var out = try init(buf);
            out.alloc = a;
            return out;
        }

        fn init(buf: []align(std.mem.page_size) u8) !@This() {
            if (buf.len % std.mem.page_size != 0) @panic("SecretAllocator page-aligned buffer must be a multiple of the page size");
            _ = try mlock(buf);
            return .{
                .alloc = null,
                .end_index = 0,
                .buf = buf,
            };
        }

        fn deinit(self: @This()) void {
            monocypher.wipe(self.buf);
            const ml = MemoryLock{ .buf = self.buf };
            ml.deinit();
            if (self.alloc) |a| a.free(self.buf);
        }

        fn allocator(self: *@This()) std.mem.Allocator {
            return .{
                .ptr = self,
                .vtable = &.{
                    .alloc = alloc,
                    .resize = resize,
                    .free = free,
                },
            };
        }

        fn alloc(ctx: *anyopaque, n: usize, log2_ptr_align: u8, ra: usize) ?[*]u8 {
            _ = ra;
            const self: *@This() = @ptrCast(@alignCast(ctx));
            log.debug("SecretAllocator [{d}]{*} end_index={d} n={d} l2align={d}", .{ self.buf.len, self.buf, self.end_index, n, log2_ptr_align });
            const ptr_align = @as(usize, 1) << @as(std.mem.Allocator.Log2Align, @intCast(log2_ptr_align));
            const adjust_off = std.mem.alignPointerOffset(self.buf.ptr + self.end_index, ptr_align) orelse return null;
            const adjusted_index = self.end_index + adjust_off;
            const new_end_index = adjusted_index + n;
            if (new_end_index > self.buf.len) return null;
            self.end_index = new_end_index;
            return self.buf.ptr + adjusted_index;
        }

        fn resize(
            ctx: *anyopaque,
            buf: []u8,
            log2_buf_align: u8,
            new_size: usize,
            return_address: usize,
        ) bool {
            _ = ctx;
            _ = buf;
            _ = log2_buf_align;
            _ = new_size;
            _ = return_address;
            return false;
        }

        fn free(
            ctx: *anyopaque,
            buf: []u8,
            log2_buf_align: u8,
            ra: usize,
        ) void {
            _ = ctx;
            _ = log2_buf_align;
            _ = ra;
            monocypher.wipe(buf);
        }
    };

    fn Secret(comptime T: type) type {
        return struct {
            const len: usize = @sizeOf(T);
            alloc: std.mem.Allocator,
            bytes: []u8,

            fn init(alloc: std.mem.Allocator, value: *T) !@This() {
                const src_ptr: [*]u8 = @ptrCast(value);
                const src_buf = src_ptr[0..len];

                const obj = try alloc.create(T);
                const buf = (@as([*]u8, @ptrCast(obj)))[0..len];
                std.mem.copyForwards(u8, buf, src_buf);
                monocypher.wipe(src_buf);
                return .{
                    .bytes = buf,
                    .alloc = alloc,
                };
            }

            fn initRandom(alloc: std.mem.Allocator) !@This() {
                const obj = try alloc.create(T);
                const buf = (@as([*]u8, @ptrCast(obj)))[0..len];
                std.crypto.random.bytes(buf);
                return .{
                    .bytes = buf,
                    .alloc = alloc,
                };
            }

            fn initEmpty(alloc: std.mem.Allocator) !@This() {
                const obj = try alloc.create(T);
                const buf = (@as([*]u8, @ptrCast(obj)))[0..len];
                return .{
                    .bytes = buf,
                    .alloc = alloc,
                };
            }

            fn deinit(self: @This()) void {
                self.alloc.free(self.bytes);
            }

            fn get(self: @This()) *T {
                return @ptrCast(@alignCast(self.bytes.ptr));
            }

            fn eql(self: @This(), other: @This()) bool {
                return monocypher.eql(len, self.bytes[0..len], other.bytes[0..len]);
            }

            fn arr(self: @This()) *const [len]u8 {
                return self.bytes[0..len];
            }
        };
    }

    pub const PrivateKey = Secret([32]u8);
    pub const PublicKey = [32]u8;
    pub const SigningKey = Secret([64]u8);
    pub const Signature = [64]u8;
    const sign = struct {
        const EdDSA = struct {
            key: union(enum) {
                sk: SigningKey,
                pk: PublicKey,
            },

            fn init(seed: PrivateKey) !@This() {
                const out: @This() = .{
                    .key = .{ .sk = try SigningKey.initEmpty(seed.alloc) },
                };
                var tmppk: PublicKey = undefined;
                monocypher.c.crypto_eddsa_key_pair(out.key.sk.bytes.ptr, &tmppk, seed.bytes.ptr);
                return out;
            }

            fn initPubkey(pk: PublicKey) @This() {
                return .{ .key = .{ .pk = pk } };
            }

            fn deinit(self: @This()) void {
                switch (self.key) {
                    .sk => |key| key.deinit(),
                    .pk => {},
                }
            }

            fn pubkey(self: @This()) *const PublicKey {
                return switch (self.key) {
                    .sk => |key| key.bytes[32..64],
                    .pk => |*key| key,
                };
            }

            fn sign(self: @This(), message: []const u8) Signature {
                if (self.key == .pk) @panic("cannot use sign with only a public key");
                var out: Signature = undefined;
                monocypher.c.crypto_eddsa_sign(
                    &out,
                    self.key.sk.bytes.ptr,
                    message.ptr,
                    message.len,
                );
                return out;
            }

            fn check(self: @This(), sig: Signature, message: []const u8) bool {
                const pk: []const u8 = switch (self.key) {
                    .sk => |key| key.bytes[32..],
                    .pk => |key| &key,
                };
                const rc = monocypher.c.crypto_eddsa_check(
                    &sig,
                    pk.ptr,
                    message.ptr,
                    message.len,
                );
                return rc == 0;
            }
        };

        const Ed25519 = struct {
            key: union(enum) {
                sk: SigningKey,
                pk: PublicKey,
            },

            fn init(seed: PrivateKey) !@This() {
                const out: @This() = .{
                    .key = .{ .sk = try SigningKey.initEmpty(seed.alloc) },
                };
                var tmppk: PublicKey = undefined;
                monocypher.c.crypto_ed25519_key_pair(out.key.sk.bytes.ptr, &tmppk, seed.bytes.ptr);
                return out;
            }

            fn initPubkey(pk: PublicKey) @This() {
                return .{ .key = .{ .pk = pk } };
            }

            fn deinit(self: @This()) void {
                switch (self.key) {
                    .sk => |key| key.deinit(),
                    .pk => {},
                }
            }

            fn pubkey(self: @This()) *const PublicKey {
                return switch (self.key) {
                    .sk => |key| key.bytes[32..64],
                    .pk => |*key| key,
                };
            }

            fn sign(self: @This(), message: []const u8) Signature {
                if (self.key == .pk) @panic("cannot use sign with only a public key");
                var out: Signature = undefined;
                monocypher.c.crypto_ed25519_sign(
                    &out,
                    self.key.sk.bytes.ptr,
                    message.ptr,
                    message.len,
                );
                return out;
            }

            fn check(self: @This(), sig: Signature, message: []const u8) bool {
                const pk: []const u8 = switch (self.key) {
                    .sk => |key| key.bytes[32..],
                    .pk => |key| &key,
                };
                const rc = monocypher.c.crypto_ed25519_check(
                    &sig,
                    pk.ptr,
                    message.ptr,
                    message.len,
                );
                return rc == 0;
            }
        };
    };

    const keyx = struct {
        const SharedKey = PrivateKey;

        const X25519 = struct {
            fn pubkey(sk: PrivateKey) PublicKey {
                var pk: PublicKey = undefined;
                monocypher.c.crypto_x25519_public_key(&pk, sk.bytes.ptr);
                return pk;
            }

            fn sharedkey(sk: PrivateKey, pk: PublicKey) !SharedKey {
                const shared = try SharedKey.initEmpty(sk.alloc);
                monocypher.c.crypto_x25519(shared.bytes.ptr, sk.bytes.ptr, &pk);
                return shared;
            }
        };
    };

    const kdf = struct {
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

            const HashedPassword = struct {
                hash: Secret([32]u8),
                salt: Secret([16]u8),

                fn deinit(self: @This()) void {
                    self.hash.deinit();
                    self.salt.deinit();
                }

                fn eql(self: @This(), other: HashedPassword) bool {
                    return self.hash.eql(other.hash);
                }
            };

            pub fn hashSalt(
                self: @This(),
                work_area: []u8,
                password: []u8,
                salt: *const Secret([16]u8),
                alloc: std.mem.Allocator,
            ) !HashedPassword {
                const out: HashedPassword = .{
                    .hash = try Secret([32]u8).initEmpty(alloc),
                    .salt = try Secret([16]u8).initEmpty(alloc),
                };
                std.mem.copyForwards(u8, out.salt.bytes, salt.bytes);
                self.hashInternal(
                    work_area,
                    password,
                    out.salt.bytes,
                    out.hash.bytes,
                );
                return out;
            }

            pub fn hash(
                self: @This(),
                work_area: []u8,
                password: []u8,
                alloc: std.mem.Allocator,
            ) !HashedPassword {
                const out: HashedPassword = .{
                    .hash = try Secret([32]u8).initEmpty(alloc),
                    .salt = try Secret([16]u8).initRandom(alloc),
                };
                self.hashInternal(
                    work_area,
                    password,
                    out.salt.bytes,
                    out.hash.bytes,
                );
                return out;
            }

            fn hashInternal(
                self: @This(),
                work_area: []u8,
                password: []u8,
                salt: []const u8,
                hbuf: []u8,
            ) void {
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

                const inputs = monocypher.c.crypto_argon2_inputs{
                    .pass = password.ptr,
                    .pass_size = @intCast(password.len),
                    .salt = salt.ptr,
                    .salt_size = @intCast(salt.len),
                };

                const extras = monocypher.c.crypto_argon2_extras{};

                monocypher.c.crypto_argon2(
                    hbuf.ptr,
                    @intCast(hbuf.len),
                    work_area.ptr,
                    config,
                    inputs,
                    extras,
                );

                monocypher.wipe(password);
            }
        };
    };

    const aead = struct {
        const XChaCha20 = struct {
            const Ctx = Secret(monocypher.c.crypto_aead_ctx);
            const Nonce = Secret([24]u8);
            const Mac = [16]u8;

            ctx: Ctx,
            nonce: Nonce,

            pub fn init(sk: PrivateKey) !@This() {
                var out: @This() = .{
                    .ctx = try Ctx.initEmpty(sk.alloc),
                    .nonce = try Nonce.initRandom(sk.alloc),
                };
                monocypher.c.crypto_aead_init_x(out.ctx.get(), sk.bytes.ptr, out.nonce.bytes.ptr);
                return out;
            }

            pub fn initNonce(sk: PrivateKey, nonce: *const Nonce) !@This() {
                var out: @This() = .{
                    .ctx = try Ctx.initEmpty(sk.alloc),
                    .nonce = try Nonce.init(sk.alloc, nonce.bytes[0..24]),
                };
                monocypher.c.crypto_aead_init_x(out.ctx.get(), sk.bytes.ptr, out.nonce.bytes.ptr);
                return out;
            }

            pub fn deinit(self: @This()) void {
                self.ctx.deinit();
                self.nonce.deinit();
            }

            const Parent = @This();
            fn EncryptedReader(T: type) type {
                return struct {
                    const ReadError = error{
                        BufTooSmall,
                    } || T.Error;
                    const Reader = std.io.Reader(*@This(), ReadError, read);

                    parent: *Parent,
                    src_reader: T,

                    pub fn reader(self: *@This()) Reader {
                        return .{ .context = self };
                    }

                    fn read(self: *@This(), buf: []u8) ReadError!usize {
                        if (buf.len <= 16) return ReadError.BufTooSmall;
                        const readbuf = buf[0 .. buf.len - 16];
                        const n = try self.src_reader.read(readbuf);
                        const macbuf = buf[n .. n + 16];
                        monocypher.c.crypto_aead_write(
                            self.parent.ctx.get(),
                            buf.ptr,
                            macbuf.ptr,
                            null,
                            0,
                            buf.ptr,
                            n,
                        );
                        return n + 16;
                    }
                };
            }

            fn DecryptedReader(T: type) type {
                return struct {
                    const ReadError = error{
                        BufTooSmall,
                        DecryptFailed,
                    } || T.Error;
                    const Reader = std.io.Reader(*@This(), ReadError, read);

                    parent: *Parent,
                    src_reader: T,

                    pub fn reader(self: *@This()) Reader {
                        return .{ .context = self };
                    }

                    fn read(self: *@This(), buf: []u8) ReadError!usize {
                        if (buf.len <= 16) return ReadError.BufTooSmall;
                        const n = try self.src_reader.read(buf);
                        if (n == 0) return 0;
                        const macbuf = buf[n - 16 .. n];
                        const rc = monocypher.c.crypto_aead_read(
                            self.parent.ctx.get(),
                            buf.ptr,
                            macbuf.ptr,
                            null,
                            0,
                            buf.ptr,
                            n - 16,
                        );
                        if (rc != 0) return ReadError.DecryptFailed;
                        return n - 16;
                    }
                };
            }

            fn encryptStream(self: *@This(), reader: anytype) EncryptedReader(@TypeOf(reader)) {
                return .{
                    .parent = self,
                    .src_reader = reader,
                };
            }

            fn encrypt(self: *@This(), message: []const u8, out: []u8) ![]u8 {
                var src_stream = std.io.fixedBufferStream(message);
                const src_reader = src_stream.reader();
                var stream = self.encryptStream(src_reader);
                const n = try stream.read(out);
                return out[0..n];
            }

            fn decrypt(self: *@This(), message: []const u8, out: []u8) ![]u8 {
                var src_stream = std.io.fixedBufferStream(message);
                const src_reader = src_stream.reader();
                var stream = self.decryptStream(src_reader);
                const n = try stream.read(out);
                return out[0..n];
            }

            fn decryptStream(self: *@This(), reader: anytype) DecryptedReader(@TypeOf(reader)) {
                return .{
                    .parent = self,
                    .src_reader = reader,
                };
            }
        };
    };
};

fn hexdigest(bytes: anytype) [bytes.len * 2]u8 {
    return std.fmt.bytesToHex(bytes, .lower);
}

fn debugHex(title: []const u8, contents: anytype) void {
    std.debug.print("{s}=", .{title});
    std.debug.print("{s}\n", .{&hexdigest(contents)});
}

fn printHex(writer: anytype, title: []const u8, contents: anytype) !void {
    try writer.print("{s}\n", .{title});
    _ = try writer.write(&hexdigest(contents));
    try writer.print("\n", .{});
}

fn base64EncodeAlloc(
    alloc: std.mem.Allocator,
    buf: []const u8,
) !struct { buf: []u8, encoded: []const u8 } {
    const encoder = std.base64.Base64Encoder.init(std.base64.url_safe_alphabet_chars, '=');
    const enc = try alloc.alloc(u8, encoder.calcSize(buf.len));
    const enc2 = encoder.encode(enc, buf);
    return .{ .encoded = enc2, .buf = enc };
}

const TestCtx = struct {
    alloc: std.mem.Allocator,
    secret_alloc: crypt.SecretAllocator,
    stderr: @TypeOf(std.io.getStdErr().writer()),

    fn init() !@This() {
        return initConfig(.{});
    }

    const Config = struct {
        npages: usize = 32,
    };
    fn initConfig(config: Config) !@This() {
        const alloc = std.testing.allocator;
        return .{
            .alloc = alloc,
            .secret_alloc = try crypt.SecretAllocator.initFromAllocator(alloc, config.npages),
            .stderr = std.io.getStdErr().writer(),
        };
    }

    fn deinit(self: @This()) void {
        self.secret_alloc.deinit();
    }
};

fn testSign(Signer: type) !void {
    var ctx = try TestCtx.init();
    defer ctx.deinit();
    const secret_alloc = ctx.secret_alloc.allocator();

    const sk = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk.deinit();

    const signkey = try Signer.init(sk);
    defer signkey.deinit();

    const m1 = "hello world";
    const sig = signkey.sign(m1);

    const pk = signkey.pubkey();
    const checkkey = Signer.initPubkey(pk.*);

    try std.testing.expect(checkkey.check(sig, m1));

    const m2 = "hello worl!";
    try std.testing.expect(!checkkey.check(sig, m2));
}

test "sign eddsa" {
    try testSign(crypt.sign.EdDSA);
}

test "sign ed25519" {
    try testSign(crypt.sign.Ed25519);
}

test "keyx x25519" {
    var ctx = try TestCtx.init();
    defer ctx.deinit();
    const secret_alloc = ctx.secret_alloc.allocator();

    const sk1 = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk1.deinit();
    const sk2 = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk2.deinit();

    const pk1 = crypt.keyx.X25519.pubkey(sk1);
    const pk2 = crypt.keyx.X25519.pubkey(sk2);

    const share1 = try crypt.keyx.X25519.sharedkey(sk1, pk2);
    defer share1.deinit();
    const share2 = try crypt.keyx.X25519.sharedkey(sk2, pk1);
    defer share2.deinit();

    try std.testing.expect(share1.eql(share2));
}

test "kdf argon2" {
    var ctx = try TestCtx.init();
    defer ctx.deinit();
    const secret_alloc = ctx.secret_alloc.allocator();

    const argon2 = crypt.kdf.Argon2.init(.{});
    // TODO: work area should be under memlock
    const work_area = try ctx.alloc.alloc(u8, argon2.workAreaSize());
    defer ctx.alloc.free(work_area);

    const hash1 = blk: {
        const pw_const = "foo-bar-baz";
        const pw = try ctx.alloc.alloc(u8, pw_const.len);
        defer ctx.alloc.free(pw);
        std.mem.copyForwards(u8, pw, pw_const);

        break :blk try argon2.hash(work_area, pw, secret_alloc);
    };
    defer hash1.deinit();

    const hash2 = blk: {
        const pw_const = "foo-bar-baz";
        const pw = try ctx.alloc.alloc(u8, pw_const.len);
        defer ctx.alloc.free(pw);
        std.mem.copyForwards(u8, pw, pw_const);

        break :blk try argon2.hashSalt(work_area, pw, &hash1.salt, secret_alloc);
    };
    defer hash2.deinit();

    try std.testing.expect(hash1.eql(hash2));
}

const lorem =
    \\Lorem ipsum dolor sit amet, consectetur adipiscing elit.
    \\Morbi elementum dui lacus, euismod dapibus lectus varius sed. Mauris
    \\ultrices ornare neque id lobortis. Donec eu mollis diam, id efficitur
    \\mauris. Ut a felis in felis porttitor fringilla. Donec consequat ipsum
    \\quis pulvinar feugiat. Proin in elit urna. Pellentesque turpis tortor,
    \\us ut posuere eu, sagittis convallis dui. Pellentesque quis lorem et
    \\justo fringilla vulputate."
;

test "aead xchacha20" {
    var ctx = try TestCtx.initConfig(.{ .npages = 32 });
    defer ctx.deinit();
    const secret_alloc = ctx.secret_alloc.allocator();

    const src_buf = lorem;

    var src_stream = std.io.fixedBufferStream(src_buf);
    const src_reader = src_stream.reader();

    const sk1 = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk1.deinit();

    const sk2 = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk2.deinit();

    const pk1 = crypt.keyx.X25519.pubkey(sk1);
    const pk2 = crypt.keyx.X25519.pubkey(sk2);

    const share1 = try crypt.keyx.X25519.sharedkey(sk1, pk2);
    defer share1.deinit();
    const share2 = try crypt.keyx.X25519.sharedkey(sk2, pk1);
    defer share2.deinit();

    try std.testing.expect(share1.eql(share2));

    var aead1 = try crypt.aead.XChaCha20.init(share1);
    var aead2 = try crypt.aead.XChaCha20.initNonce(share2, &aead1.nonce);

    var encStream = aead1.encryptStream(src_reader);
    const encReader = encStream.reader();

    var decStream = aead2.decryptStream(encReader);
    const decReader = decStream.reader();

    var decrypted = std.ArrayList(u8).init(ctx.alloc);
    defer decrypted.deinit();

    var buf: [1 << 12]u8 = undefined;
    while (true) {
        const n = try decReader.read(&buf);
        if (n == 0) break;
        const result = buf[0..n];
        try decrypted.appendSlice(result);
    }

    try std.testing.expect(std.mem.eql(u8, src_buf, decrypted.items));

    const msg = "hello world";
    var out1: [msg.len + 16]u8 = undefined;
    const enc1 = try aead1.encrypt(msg, &out1);
    var out2: [msg.len + 16]u8 = undefined;
    const dec1 = try aead2.decrypt(enc1, &out2);
    try std.testing.expect(std.mem.eql(u8, msg, dec1));
}

fn testHasher(hasher: anytype) !void {
    const data = lorem;
    var stream = std.io.fixedBufferStream(data);
    const reader = stream.reader();

    const h1 = try hashlib.hashReader(hasher, reader, 8);
    hasher.reset();
    const h2 = hashlib.hash(hasher, data);

    try std.testing.expect(std.mem.eql(u8, &h1, &h2));
}

test "blake2b" {
    var hasher = blake2b.init();
    try testHasher(&hasher);
}

test "blake3" {
    var hasher = blake3.init();
    try testHasher(&hasher);
}

test "blake3 kdf" {
    var ctx = try TestCtx.init();
    defer ctx.deinit();
    const secret_alloc = ctx.secret_alloc.allocator();

    const sk = try crypt.PrivateKey.initRandom(secret_alloc);
    defer sk.deinit();

    const k1 = blake3.deriveKey(sk.bytes, "a");
    const k2 = blake3.deriveKey(sk.bytes, "a");
    const k11 = blake3.deriveKey(&k1, "b");
    const k21 = blake3.deriveKey(&k2, "b");
    try std.testing.expect(std.mem.eql(u8, &k11, &k21));
}

test "wordlist" {
    const src =
        \\hello
        \\goodbye
        \\world
    ;

    const alloc = std.testing.allocator;
    const wordlist = try Wordlist(.{}).fromStr(alloc, src);
    try std.testing.expect(std.mem.eql(u8, wordlist.getWord(0), "hello"));
    try std.testing.expect(wordlist.getIdx("hello") == 0);
}

const BipWordlist = Wordlist(.{ .nwords = 2048 });

fn bipMnemonic(alloc: std.mem.Allocator, wordlist: BipWordlist) ![]u8 {
    var ent: [32]u8 = undefined;
    std.crypto.random.bytes(&ent);
    const hash = blake3.hash(&ent);

    var seed8: std.PackedIntArrayEndian(u8, .big, 33) = undefined;
    std.mem.copyForwards(u8, seed8.bytes[0..32], &ent);
    seed8.bytes[32] = hash[0];

    var seed11 = std.PackedIntArrayEndian(u11, .big, 24){ .bytes = seed8.bytes };

    var mnemonic = std.ArrayList(u8).init(alloc);
    for (0..24) |i| {
        const idx = seed11.get(i);
        const word = wordlist.getWord(@intCast(idx));
        try mnemonic.appendSlice(word);
        try mnemonic.appendSlice(" ");
    }
    return mnemonic.toOwnedSlice();
}

fn mnemonicToSeed(alloc: std.mem.Allocator, mnemonic: []u8, passphrase: []const u8) !crypt.PrivateKey {
    const argon = crypt.kdf.Argon2.init(.{});
    const work_area = try alloc.alloc(u8, argon.workAreaSize());
    defer alloc.free(work_area);
    // TODO: passphrase has to be 7 chars currently
    const salt = try crypt.Secret([16]u8).initEmpty(alloc);
    defer salt.deinit();
    _ = try std.fmt.bufPrint(salt.bytes, "mnemonic {s}", .{passphrase});
    const out = try argon.hashSalt(work_area, mnemonic, &salt, alloc);
    return out.hash;
}

test "mnemonic seed" {
    const src = @embedFile("bip39_english.txt");

    const alloc = std.testing.allocator;
    const wordlist = try BipWordlist.fromStr(alloc, src);

    // generate a mnemonic from the wordlist
    const mnemonic = try bipMnemonic(alloc, wordlist);

    // convert to seed using argon2 (pw=mnemonic, salt=mnemonic+passphrase)
    const passphrase = "0123456";
    const seed = try mnemonicToSeed(alloc, mnemonic, passphrase);

    // derive private key and chain code
    var derived: [512]u8 = undefined;
    blake3.deriveKeyBuf(seed.bytes, "master", &derived);
    const sk = derived[0..256];
    const chain = derived[256..];

    _ = sk;
    _ = chain;
}

const mbedtls = struct {
    const c = @cImport({
        @cInclude("mbedtls/x509.h");
    });
};

// TODO:
// * TLS x509 certificate
// * ssh keys
// * crypto wallets (btc, eth, cosmos)
