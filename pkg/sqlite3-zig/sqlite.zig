const std = @import("std");

const log = std.log.scoped(.sqlite);

const DB = @This();

db: ?*c.sqlite3 = null,

pub const c = @cImport(@cInclude("sqlite3.h"));

pub const ErrorCode = enum(c_int) {
    Error = c.SQLITE_ERROR,
    Internal = c.SQLITE_INTERNAL,
    Perm = c.SQLITE_PERM,
    Abort = c.SQLITE_ABORT,
    Busy = c.SQLITE_BUSY,
    Locked = c.SQLITE_LOCKED,
    Nomem = c.SQLITE_NOMEM,
    Readonly = c.SQLITE_READONLY,
    Interrupt = c.SQLITE_INTERRUPT,
    Ioerr = c.SQLITE_IOERR,
    Corrupt = c.SQLITE_CORRUPT,
    Notfound = c.SQLITE_NOTFOUND,
    Full = c.SQLITE_FULL,
    Cantopen = c.SQLITE_CANTOPEN,
    Protocol = c.SQLITE_PROTOCOL,
    Empty = c.SQLITE_EMPTY,
    Schema = c.SQLITE_SCHEMA,
    Toobig = c.SQLITE_TOOBIG,
    Constraint = c.SQLITE_CONSTRAINT,
    Mismatch = c.SQLITE_MISMATCH,
    Misuse = c.SQLITE_MISUSE,
    Nolfs = c.SQLITE_NOLFS,
    Auth = c.SQLITE_AUTH,
    Format = c.SQLITE_FORMAT,
    Range = c.SQLITE_RANGE,
    Notadb = c.SQLITE_NOTADB,
    Notice = c.SQLITE_NOTICE,
    Warning = c.SQLITE_WARNING,
};

pub const Error = error{
    Error,
    Internal,
    Perm,
    Abort,
    Busy,
    Locked,
    Nomem,
    Readonly,
    Interrupt,
    Ioerr,
    Corrupt,
    Notfound,
    Full,
    Cantopen,
    Protocol,
    Empty,
    Schema,
    Toobig,
    Constraint,
    Mismatch,
    Misuse,
    Nolfs,
    Auth,
    Format,
    Range,
    Notadb,
    Notice,
    Warning,
};

pub fn errFromCode(rc: c_int) Error {
    const err: ErrorCode = @enumFromInt(rc);
    return switch (err) {
        .Error => Error.Error,
        .Internal => Error.Internal,
        .Perm => Error.Perm,
        .Abort => Error.Abort,
        .Busy => Error.Busy,
        .Locked => Error.Locked,
        .Nomem => Error.Nomem,
        .Readonly => Error.Readonly,
        .Interrupt => Error.Interrupt,
        .Ioerr => Error.Ioerr,
        .Corrupt => Error.Corrupt,
        .Notfound => Error.Notfound,
        .Full => Error.Full,
        .Cantopen => Error.Cantopen,
        .Protocol => Error.Protocol,
        .Empty => Error.Empty,
        .Schema => Error.Schema,
        .Toobig => Error.Toobig,
        .Constraint => Error.Constraint,
        .Mismatch => Error.Mismatch,
        .Misuse => Error.Misuse,
        .Nolfs => Error.Nolfs,
        .Auth => Error.Auth,
        .Format => Error.Format,
        .Range => Error.Range,
        .Notadb => Error.Notadb,
        .Notice => Error.Notice,
        .Warning => Error.Warning,
    };
}

pub fn init(fname: [:0]const u8, flags: c_int, vfs_name: ?[:0]const u8) !@This() {
    var out = @This(){};
    const cvfs = if (vfs_name) |x| x.ptr else null;
    const rc = c.sqlite3_open_v2(fname, &out.db, flags, cvfs);
    if (rc != c.SQLITE_OK) return error.DbOpen;
    return out;
}

pub fn deinit(self: @This()) void {
    _ = c.sqlite3_close(self.db);
}

pub fn check(self: @This(), rc: c_int) !void {
    if (rc != c.SQLITE_OK) {
        log.err("code={s} msg={s}", .{ @tagName(@as(ErrorCode, @enumFromInt(rc))), c.sqlite3_errmsg(self.db) });
        return errFromCode(rc);
    }
}

pub const Statement = struct {
    stmt: ?*c.sqlite3_stmt = null,

    pub fn deinit(self: @This()) void {
        _ = c.sqlite3_finalize(self.stmt);
    }

    pub fn step(self: @This()) !?void {
        const rc = c.sqlite3_step(self.stmt);
        if (rc == c.SQLITE_ROW) return void{};
        if (rc == c.SQLITE_DONE) return null;
        return errFromCode(rc);
    }

    pub fn text(self: @This(), col: usize) []const u8 {
        const text_ptr = c.sqlite3_column_text(self.stmt, @intCast(col));
        const text_n = c.sqlite3_column_bytes(self.stmt, @intCast(col));
        return text_ptr[0..@intCast(text_n)];
    }

    pub fn blob(self: @This(), col: usize) []const u8 {
        const blob_ptr: [*]const u8 = @ptrCast(c.sqlite3_column_blob(self.stmt, @intCast(col)));
        const blob_n = c.sqlite3_column_bytes(self.stmt, @intCast(col));
        return blob_ptr[0..@intCast(blob_n)];
    }

    pub const Lifetime = enum {
        Static,
        Transient,
    };

    // TODO: text destructor
    const Destructor = *const fn (?*anyopaque) callconv(.C) void;
    const BindOptions = struct {
        blob: ?enum { data, zero } = null,
        lifetime: ?Lifetime = null,
        dtor: ?Destructor = null,
    };
    pub fn bind(self: @This(), col: usize, data: anytype, opts: BindOptions) !void {
        const ccol: c_int = @intCast(col);
        const cdb = c.sqlite3_db_handle(self.stmt);
        const db = DB{ .db = cdb };
        switch (@TypeOf(data)) {
            f32, f64, comptime_float => {
                try db.check(c.sqlite3_bind_double(self.stmt, ccol, @floatCast(data)));
            },
            usize => {
                if (opts.blob) |b| {
                    switch (b) {
                        .zero => {
                            try db.check(c.sqlite3_bind_zeroblob64(self.stmt, ccol, data));
                        },
                        .data => return error.WrongBlobDatar,
                    }
                } else {
                    try db.check(c.sqlite3_bind_int64(self.stmt, ccol, data));
                }
            },
            i8, i16, i32, i64, u8, u16, u32, u64, comptime_int => {
                try db.check(c.sqlite3_bind_int64(self.stmt, ccol, @intCast(data)));
            },
            @TypeOf(null) => {
                try db.check(c.sqlite3_bind_null(self.stmt, ccol));
            },
            []const u8 => {
                const lifetime = if (opts.lifetime) |l| switch (l) {
                    .Static => c.SQLITE_STATIC,
                    .Transient => c.SQLITE_TRANSIENT,
                } else c.SQLITE_TRANSIENT;
                std.debug.print("{*}\n", .{lifetime});
                if (opts.blob) |b| {
                    switch (b) {
                        .data => {
                            try db.check(c.sqlite3_bind_blob64(self.stmt, ccol, data.ptr, data.len, opts.dtor orelse lifetime));
                        },
                        else => return error.WrongBlobData,
                    }
                } else {
                    try db.check(c.sqlite3_bind_text64(self.stmt, ccol, data.ptr, data.len, opts.dtor orelse lifetime, c.SQLITE_UTF8));
                }
            },
            else => @compileError("unsupported bind type"),
        }
    }
};

pub fn prepare(self: @This(), sql: []const u8) !Statement {
    var out = Statement{};
    try self.check(c.sqlite3_prepare_v3(self.db, sql.ptr, @intCast(sql.len), 0, &out.stmt, null));
    return out;
}

pub fn preparePersistent(self: @This(), sql: []const u8) !Statement {
    var out = Statement{};
    try self.check(c.sqlite3_prepare_v3(self.db, sql.ptr, @intCast(sql.len), c.SQLITE_PREPARE_PERSISTENT, &out.stmt, null));
    return out;
}

pub fn run(self: @This(), sql: []const u8) !void {
    const stmt = try self.prepare(sql);
    defer stmt.deinit();
    _ = try stmt.step();
}
