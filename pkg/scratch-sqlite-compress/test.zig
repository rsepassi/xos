const std = @import("std");
const sqlite = @import("sqlite");
const zstd = @import("zstd");

const log = std.log;
pub const std_options = .{
    //.log_level = .info,
};

const Samples = struct {
    text: []const u8,
    lens: std.ArrayList(usize),

    fn deinit(self: @This()) void {
        self.lens.allocator.free(self.text);
        self.lens.deinit();
    }
};

fn readSamples(allocator: std.mem.Allocator) !Samples {
    const contents = try std.fs.cwd().readFileAlloc(allocator, "/tmp/lorem.txt", 100_000_000);
    errdefer allocator.free(contents);

    var lens = std.ArrayList(usize).init(allocator);
    var iter = std.mem.splitScalar(u8, contents, '\n');
    while (iter.next()) |s| {
        try lens.append(s.len);
    }

    return .{
        .text = contents,
        .lens = lens,
    };
}

const SqliteCompressCtx = struct {
    alloc: std.mem.Allocator,
    cctx: zstd.CCtx,

    fn sqliteCompress(ctx: ?*sqlite.c.sqlite3_context, argc: c_int, argv: [*c]?*sqlite.c.sqlite3_value) callconv(.C) void {
        _ = argc;

        const uctx: *SqliteCompressCtx = @ptrCast(@alignCast(sqlite.c.sqlite3_user_data(ctx)));

        const src_n: usize = @intCast(sqlite.c.sqlite3_value_bytes(argv[0]));
        const text = sqlite.c.sqlite3_value_text(argv[0])[0..src_n];

        const dst_capacity = zstd.c.ZSTD_compressBound(src_n);
        const compressed_buf = CBuf.alloc(dst_capacity) catch @panic("oom");

        const compressed = uctx.cctx.compress(compressed_buf, text) catch @panic("compress fail");
        log.info("compressed {d}->{d} bytes", .{ src_n, compressed.len });

        sqlite.c.sqlite3_result_blob(ctx, compressed.ptr, @intCast(compressed.len), CBuf.free);
    }
};

const SqliteDecompressCtx = struct {
    alloc: std.mem.Allocator,
    dctx: zstd.DCtx,

    // TODO: if can't determine a max_len, need to use the incremental
    // decompress apis
    const max_len = 1024;

    fn sqliteDecompress(ctx: ?*sqlite.c.sqlite3_context, argc: c_int, argv: [*c]?*sqlite.c.sqlite3_value) callconv(.C) void {
        _ = argc;

        const uctx: *SqliteDecompressCtx = @ptrCast(@alignCast(sqlite.c.sqlite3_user_data(ctx)));

        const src_n: usize = @intCast(sqlite.c.sqlite3_value_bytes(argv[0]));
        const compressed = @as([*]const u8, @ptrCast(sqlite.c.sqlite3_value_blob(argv[0])))[0..src_n];

        const decompressed_buf = CBuf.alloc(max_len) catch @panic("oom");

        const decompressed = uctx.dctx.decompress(decompressed_buf, compressed) catch @panic("decompress fail");
        log.info("decompressed to {d} bytes", .{decompressed.len});

        sqlite.c.sqlite3_result_text(ctx, decompressed.ptr, @intCast(decompressed.len), CBuf.free);
    }
};

const CBuf = struct {
    fn alloc(size: usize) ![]u8 {
        const buf: [*]u8 = @ptrCast(sqlite.c.sqlite3_malloc(@intCast(size + @sizeOf(usize))));
        const header = @as(*usize, @ptrCast(@alignCast(buf)));
        header.* = size;
        return buf[@sizeOf(usize) .. size + @sizeOf(usize)];
    }

    fn free(cptr: ?*anyopaque) callconv(.C) void {
        const ptr: [*]const u8 = @ptrCast(cptr);
        const buf_ptr = ptr - @sizeOf(usize);
        const header: *const usize = @ptrCast(@alignCast(buf_ptr));
        const buf = buf_ptr[0 .. header.* + @sizeOf(usize)];
        sqlite.c.sqlite3_free(@ptrCast(@constCast(buf.ptr)));
    }
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();
    defer if (gpa.deinit() == .leak) log.err("leak!", .{});

    // Build dictionary
    const samples = try readSamples(alloc);
    defer samples.deinit();
    log.info("samples n={d} total={d} bytes", .{ samples.lens.items.len, samples.text.len });

    const dict_buf = try alloc.alloc(u8, 100_000);
    defer alloc.free(dict_buf);

    const dict = try zstd.dict.train(dict_buf, samples.text, samples.lens.items);
    log.info("dict built {d} bytes", .{dict.len});

    // Compress
    const cctx = try zstd.CCtx.init();
    defer cctx.deinit();
    try cctx.loadDict(dict);
    try cctx.setParam(.CompressionLevel, 19);

    const dst_capacity = zstd.c.ZSTD_compressBound(samples.text.len);
    const compressed_buf = try alloc.alloc(u8, dst_capacity);
    defer alloc.free(compressed_buf);

    const compressed = try cctx.compress(compressed_buf, samples.text);
    log.info("compressed {d}->{d} bytes", .{ samples.text.len, compressed.len });

    // Decompress
    const dctx = try zstd.DCtx.init();
    defer dctx.deinit();
    try dctx.loadDict(dict);

    const max_len = 200_000;
    const decompressed_buf = try alloc.alloc(u8, max_len);
    defer alloc.free(decompressed_buf);

    const decompressed = try dctx.decompress(decompressed_buf, compressed);
    log.info("decompressed to {d} bytes", .{decompressed.len});

    // SQLite
    const db = try sqlite.init(
        "mem",
        sqlite.c.SQLITE_OPEN_READWRITE | sqlite.c.SQLITE_OPEN_MEMORY,
        null,
    );
    defer db.deinit();

    // Bind compress/decompress
    var compress_ctx = SqliteCompressCtx{
        .alloc = alloc,
        .cctx = cctx,
    };
    try db.check(sqlite.c.sqlite3_create_function_v2(
        db.db,
        "compress",
        1,
        sqlite.c.SQLITE_UTF8 | sqlite.c.SQLITE_DETERMINISTIC,
        &compress_ctx,
        SqliteCompressCtx.sqliteCompress,
        null,
        null,
        null,
    ));

    var decompress_ctx = SqliteDecompressCtx{
        .alloc = alloc,
        .dctx = dctx,
    };
    try db.check(sqlite.c.sqlite3_create_function_v2(
        db.db,
        "decompress",
        1,
        sqlite.c.SQLITE_UTF8 | sqlite.c.SQLITE_DETERMINISTIC,
        &decompress_ctx,
        SqliteDecompressCtx.sqliteDecompress,
        null,
        null,
        null,
    ));
    log.info("functions bound", .{});

    // Create table with generated column
    {
        const sql =
            \\create table stuff (
            \\contents_compressed BLOB,
            \\contents text as (decompress(contents_compressed)) virtual
            \\) strict;
        ;
        try db.run(sql);
    }
    log.info("table created", .{});

    // Insert compressed content
    {
        const sql =
            \\insert into stuff (
            \\contents_compressed
            \\)
            \\values(
            \\compress(?1)
            \\);
        ;
        const stmt = try db.prepare(sql);
        defer stmt.deinit();
        const text: []const u8 = "lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsum lorem ipsum";
        try stmt.bind(1, text, .{ .lifetime = .Static });
        _ = try stmt.step();
    }
    log.info("data inserted", .{});

    // Create FTS5 table
    {
        const sql =
            \\CREATE VIRTUAL TABLE search USING fts5(contents, content='stuff', content_rowid='rowid', detail=none);
        ;
        try db.run(sql);
    }
    log.info("fts created", .{});

    // Populate FTS table
    {
        const sql =
            \\insert into search(rowid, contents) select rowid, contents from stuff;
        ;
        try db.run(sql);
    }
    log.info("fts populated", .{});

    // Query uncompressed content
    {
        const sql =
            \\select contents, contents_compressed from stuff;
        ;
        const stmt = try db.prepare(sql);
        defer stmt.deinit();
        while (try stmt.step()) |_| {
            const text = stmt.text(0);
            std.debug.print("{s}\n", .{text});

            const blob = stmt.blob(1);
            std.debug.print("{any}\n", .{blob});
        }
    }
    log.info("query done", .{});

    // Query search index
    {
        const sql =
            \\SELECT contents FROM search where search MATCH 'ips*';
        ;
        const stmt = try db.prepare(sql);
        defer stmt.deinit();

        var i: usize = 0;
        while (try stmt.step()) |_| : (i += 1) {
            const text = stmt.text(0);
            std.debug.print("{s}\n", .{text});
        }
        log.info("search returned {d} results", .{i});
    }
    log.info("fts query done", .{});

    log.info("ok", .{});
}
