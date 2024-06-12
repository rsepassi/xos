const std = @import("std");
const builtin = @import("builtin");

const uv = @cImport(@cInclude("uv.h"));
pub const c = @cImport(@cInclude("wrensh.h"));
pub const Ctx = c.Ctx;
const json = @import("json.zig");
const kv = @import("kv.zig");
const process = @import("process.zig");
const fs = @import("fs.zig");
const wtimer = @import("timer.zig");
const coro = @import("zigcoro");

const wren = wren2.c;
const wren2 = @import("wren");

const log = std.log.scoped(.wrensh);
pub const std_options = .{
    //    .log_level = .debug,
};

extern const wrensh_src_io: [*c]const u8;
extern const wrensh_src_usage: [*c]const u8;
extern const wrensh_src_user: [*c]const u8;
extern fn wrenMetaSource() [*c]const u8;
extern fn wrenMetaBindForeignMethod(
    vm: *wren2.c.WrenVM,
    className: [*c]const u8,
    isStatic: bool,
    sig: [*c]const u8,
) wren2.c.WrenForeignMethodFn;
extern fn cBindForeignMethod(
    vm: *wren2.c.WrenVM,
    module: [*c]const u8,
    className: [*c]const u8,
    isStatic: bool,
    sig: [*c]const u8,
) wren2.c.WrenForeignMethodFn;
extern fn cBindForeignClass(
    vm: *wren2.c.WrenVM,
    module: [*c]const u8,
    className: [*c]const u8,
    m: *wren2.c.WrenForeignClassMethods,
) bool;

export fn wrenshEnvMap(vm: *wren.WrenVM) void {
    const alloc = std.heap.c_allocator;
    var env = std.process.getEnvMap(alloc) catch @panic("no mem");
    defer env.deinit();

    wren.wrenEnsureSlots(vm, 3);
    wren.wrenSetSlotNewMap(vm, 0);

    var it = env.iterator();
    while (it.next()) |el| {
        wren.wrenSetSlotBytes(vm, 1, el.key_ptr.ptr, el.key_ptr.len);
        wren.wrenSetSlotBytes(vm, 2, el.value_ptr.ptr, el.value_ptr.len);
        wren.wrenSetMapValue(vm, 0, 1, 2);
    }
}

const Timer = struct {
    fn timerAlloc(vm: ?*wren.WrenVM) callconv(.C) void {
        const ptr = wren.wrenSetSlotNewForeign(vm, 0, 0, @sizeOf(std.time.Timer));
        const timer: *std.time.Timer = @ptrCast(@alignCast(ptr));
        timer.* = std.time.Timer.start() catch @panic("bad timer");
    }

    fn timerFinal(data: ?*anyopaque) callconv(.C) void {
        _ = data;
    }

    fn timerLap(vm: ?*wren.WrenVM) callconv(.C) void {
        const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
        const t = timer.lap();
        wren.wrenSetSlotDouble(vm, 0, @floatFromInt(t / std.time.ns_per_ms));
    }

    fn timerReset(vm: ?*wren.WrenVM) callconv(.C) void {
        const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
        timer.reset();
    }

    fn timerRead(vm: ?*wren.WrenVM) callconv(.C) void {
        const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
        const t = timer.read();
        wren.wrenSetSlotDouble(vm, 0, @floatFromInt(t / std.time.ns_per_ms));
    }
};

fn readFile(alloc: std.mem.Allocator, path: []const u8) ![:0]const u8 {
    const f = try std.fs.cwd().openFile(path, .{});
    defer f.close();
    const buf = try f.readToEndAlloc(alloc, 1 << 20);
    var new_buf = try alloc.realloc(buf, buf.len + 1);
    new_buf[buf.len] = 0;
    new_buf.len = buf.len;
    return @ptrCast(new_buf);
}

fn uvcall(rc: c_int) !void {
    if (rc != 0) return error.UVError;
}

fn uvGetCtx(handle: anytype) *c.Ctx {
    return @ptrCast(@alignCast(uv.uv_handle_get_data(@ptrCast(handle))));
}

export fn wrenshArgs(vm: *wren.WrenVM) void {
    const ctx = wrenshGetCtx(vm);

    wren.wrenEnsureSlots(vm, 2);
    wren.wrenSetSlotNewList(vm, 0);
    const argv: [*][:0]const u8 = @ptrCast(@alignCast(ctx.argv));
    for (0..@intCast(ctx.argc)) |i| {
        wren.wrenSetSlotString(vm, 1, argv[i]);
        wren.wrenInsertInList(vm, 0, -1, 1);
    }
}

export fn wrenshArg(vm: *wren.WrenVM) void {
    const n: usize = @intFromFloat(wren.wrenGetSlotDouble(vm, 1));
    const ctx = wrenshGetCtx(vm);
    if (n >= ctx.argc) {
        const err = "args index out of bounds";
        wren.wrenSetSlotBytes(vm, 0, err.ptr, err.len);
        wren.wrenAbortFiber(vm, 0);
    } else {
        const argv: [*][:0]const u8 = @ptrCast(@alignCast(ctx.argv));
        wren.wrenSetSlotBytes(vm, 0, argv[n], argv[n].len);
    }
}

pub fn getCtx(vm: *wren2.VM) *Ctx {
    return vm.getUser(Ctx);
}

pub fn fiberError(vm: *wren2.VM, fiber: wren2.Handle, comptime fmt: []const u8, args: anytype) void {
    const msg = std.fmt.allocPrintZ(vm.args.allocator, fmt, args) catch @panic("no mem");
    defer vm.args.allocator.free(msg);
    log.debug("abort fiber {s}", .{msg});

    const ctx = getCtx(vm);
    vm.setSlot(0, fiber);
    vm.setSlot(1, msg);
    vm.call(@ptrCast(ctx.wren_tx_err)) catch @panic("bad call");
}

const WrenshSrc = struct {
    has_baked_src: bool,
    alloc: std.mem.Allocator,
    user_src: ?[:0]const u8 = null,
    file_src: ?[:0]const u8 = null,

    fn init(alloc: std.mem.Allocator, args: [][:0]const u8) !@This() {
        const argc = args.len;
        const has_baked_src = wrensh_src_user != null;
        var user_src: ?[:0]const u8 = null;
        var file_src: ?[:0]const u8 = null;
        if (has_baked_src) {
            user_src = wrensh_src_user[0..std.mem.len(wrensh_src_user) :0];
        } else if (argc > 2 and std.mem.eql(u8, args[1], "-c")) {
            user_src = args[2];
        } else if (argc > 1) {
            file_src = try readFile(alloc, args[1]);
            user_src = file_src.?;
        } else {
            user_src = null;
        }

        return .{
            .has_baked_src = has_baked_src,
            .user_src = user_src,
            .file_src = file_src,
            .alloc = alloc,
        };
    }

    fn deinit(self: @This()) void {
        if (self.file_src) |s| self.alloc.free(s);
    }
};

fn zigBindForeignClass(
    vm: *wren2.c.WrenVM,
    cmodule: [*c]const u8,
    cclassName: [*c]const u8,
    m: *wren2.c.WrenForeignClassMethods,
) bool {
    _ = vm;
    _ = cmodule;
    const className = cclassName[0..std.mem.len(cclassName)];
    if (std.mem.eql(u8, className, "Timer")) {
        m.*.allocate = Timer.timerAlloc;
        m.*.finalize = Timer.timerFinal;
        return true;
    }
    if (std.mem.eql(u8, className, "KV")) {
        m.*.allocate = kv.KV.alloc;
        m.*.finalize = kv.KV.final;
        return true;
    }

    return false;
}

fn zigBindForeignMethod(
    vm: *wren2.c.WrenVM,
    cmodule: [*c]const u8,
    cclassName: [*c]const u8,
    isStatic: bool,
    csignature: [*c]const u8,
) wren2.c.WrenForeignMethodFn {
    _ = vm;
    _ = cmodule;

    const className = cclassName[0..std.mem.len(cclassName)];
    const signature = csignature[0..std.mem.len(csignature)];

    // Timer
    if (std.mem.eql(u8, className, "Timer") and !isStatic) {
        if (std.mem.eql(u8, signature, "lap()")) return Timer.timerLap;
        if (std.mem.eql(u8, signature, "read()")) return Timer.timerRead;
        if (std.mem.eql(u8, signature, "reset()")) return Timer.timerReset;
    }

    // KV
    if (std.mem.eql(u8, className, "KV") and !isStatic) {
        if (std.mem.eql(u8, signature, "get(_)")) return kv.KV.get;
        if (std.mem.eql(u8, signature, "getp(_)")) return kv.KV.getp;
        if (std.mem.eql(u8, signature, "set(_,_)")) return kv.KV.set;
    }

    // JSON
    if (std.mem.eql(u8, className, "JSON") and isStatic) {
        if (std.mem.eql(u8, signature, "encode(_)")) return json.jsonEncode;
        if (std.mem.eql(u8, signature, "decode(_)")) return json.jsonDecode;
    }

    // IO
    if (std.mem.eql(u8, className, "IO") and isStatic) {
        if (std.mem.eql(u8, signature, "run_(_,_,_,_,_,_)")) return process.run;
        if (std.mem.eql(u8, signature, "exec_(_,_)")) return process.exec;
        if (std.mem.eql(u8, signature, "sleep_(_,_)")) return wtimer.sleep;

        if (fs.bindMethod(signature)) |f| return f;
    }

    return null;
}

fn usage(args: []const [:0]const u8, writer: anytype) !bool {
    const argc = args.len;
    if (argc == 1 or
        (argc == 2 and (std.mem.eql(u8, args[1], "-h") or std.mem.eql(u8, args[1], "--help"))))
    {
        _ = try writer.write(wrensh_src_usage[0..std.mem.len(wrensh_src_usage)]);
        return true;
    }
    return false;
}

fn wrenWrite(vm: *wren2.VM, s: [:0]const u8) void {
    _ = vm;
    const stderr = std.io.getStdErr().writer();
    _ = stderr.write(s) catch @panic("stderr write failed");
}

fn wrenErrorFn(vm: *wren2.VM, err_type: wren2.ErrorType, module: ?[:0]const u8, line: ?usize, msg: [:0]const u8) void {
    _ = vm;
    const stderr = std.io.getStdErr().writer();
    if (module != null and line != null) {
        _ = stderr.print("error: {any} {s}:{d}  {s}\n", .{
            err_type,
            module.?,
            line.?,
            msg,
        }) catch @panic("stderr write failed");
    } else {
        _ = stderr.print("error: {any}  {s}\n", .{
            err_type,
            msg,
        }) catch @panic("stderr write failed");
    }
}

const cstr = [:0]const u8;

fn bindForeignMethods(vm: *wren2.VM, module: cstr, class_name: cstr, is_static: bool, signature: cstr) ?wren2.ForeignMethod {
    if (!std.mem.eql(u8, module, "io")) @panic("unexpected foreign method");

    var out = zigBindForeignMethod(vm.vm, module.ptr, class_name.ptr, is_static, signature.ptr);
    if (out != null) return out;

    out = cBindForeignMethod(vm.vm, module.ptr, class_name.ptr, is_static, signature.ptr);
    if (out != null) return out;

    out = wrenMetaBindForeignMethod(vm.vm, class_name.ptr, is_static, signature.ptr);
    if (out != null) return out;

    @panic("unexpected foreign method");
}

fn bindForeignClasses(vm: *wren2.VM, module: cstr, class_name: cstr) wren2.ForeignClassMethods {
    if (!std.mem.eql(u8, module, "io")) @panic("unexpected foreign method");

    var out = wren2.ForeignClassMethods{};

    var done = zigBindForeignClass(vm.vm, module.ptr, class_name.ptr, &out);
    if (done) return out;

    done = cBindForeignClass(vm.vm, module.ptr, class_name.ptr, &out);
    if (done) return out;

    @panic("unexpected foreign class");
}

export fn wrenshGetCtx(vm: ?*wren.WrenVM) *c.Ctx {
    return wren2.VM.get(vm).getUser(c.Ctx);
}

fn setupWren(alloc: std.mem.Allocator, ctx: *c.Ctx) !*wren2.VM {
    const vm = try wren2.VM.init(.{
        .allocator = alloc,
        .write_fn = wrenWrite,
        .error_fn = wrenErrorFn,
        .user_data = ctx,
        .foreign_method_fn = bindForeignMethods,
        .foreign_class_fn = bindForeignClasses,
        .use_tls_allocator = false,
    });

    ctx.wren_tx_val = @ptrCast(vm.makeCallHandle("transfer(_)").handle);
    ctx.wren_tx_err = @ptrCast(vm.makeCallHandle("transferError(_)").handle);
    ctx.wren_tx = @ptrCast(vm.makeCallHandle("transfer()").handle);
    ctx.wren_call = @ptrCast(vm.makeCallHandle("call()").handle);
    ctx.wren_call_val = @ptrCast(vm.makeCallHandle("call(_)").handle);
    ctx.wren_call2_val = @ptrCast(vm.makeCallHandle("call(_,_)").handle);

    const meta_src = wrenMetaSource();
    try vm.interpret("io", meta_src[0..std.mem.len(meta_src) :0]);
    try vm.interpret("io", wrensh_src_io[0..std.mem.len(wrensh_src_io) :0]);
    try vm.interpret("main", "import \"io\" for IO, X, Data, JSON, KV");

    return vm;
}

fn cleanupWren(vm: *wren2.VM, ctx: c.Ctx) void {
    log.debug("cleanupWren", .{});
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_tx_val) }).deinit();
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_tx_err) }).deinit();
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_tx) }).deinit();
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_call) }).deinit();
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_call_val) }).deinit();
    (wren2.Handle{ .vm = vm, .handle = @ptrCast(ctx.wren_call2_val) }).deinit();
}

pub fn main() !void {
    defer log.debug("ok", .{});
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();
    defer if (gpa.deinit() == .leak) log.err("leak!", .{});

    coro.initEnv(.{
        .stack_allocator = alloc,
    });

    const args = try std.process.argsAlloc(alloc);
    defer std.process.argsFree(alloc, args);
    const argc = args.len;
    log.debug("wrensh argc={d}", .{argc});

    const stderr = std.io.getStdErr().writer();
    const stdout = std.io.getStdOut().writer();
    _ = stdout;

    const src = try WrenshSrc.init(alloc, args);
    defer src.deinit();

    if (src.user_src == null and try usage(args, stderr)) return;

    var ctx = std.mem.zeroes(c.Ctx);
    ctx.argc = @intCast(argc);
    ctx.argv = @ptrCast(args.ptr);

    log.debug("uv init", .{});
    var loop: uv.uv_loop_t = undefined;
    try uvcall(uv.uv_loop_init(&loop));
    ctx.loop = @ptrCast(&loop);
    defer c.cleanupUV(&ctx);

    log.debug("stdio setup", .{});
    ctx.stdio = c.setupStdio(@ptrCast(&loop));
    defer c.cleanupStdio(ctx.stdio);

    log.debug("wren setup", .{});
    const wrenvm = try setupWren(alloc, &ctx);
    defer wrenvm.deinit();
    defer cleanupWren(wrenvm, ctx);

    log.debug("wren interpret", .{});
    try wrenvm.interpret("main", src.user_src.?);

    log.debug("loop", .{});
    var live: c_int = 1;
    while (live > 0) {
        if (builtin.mode == .Debug) {
            log.debug("tick", .{});
            uv.uv_print_all_handles(&loop, uv.__stderrp);
        }
        live = uv.uv_run(&loop, uv.UV_RUN_ONCE);
    }

    log.debug("loop exit", .{});
}
