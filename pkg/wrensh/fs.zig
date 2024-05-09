const std = @import("std");
const wren = @import("wren");
const uv = @import("uv");
const coro = @import("zigcoro");
const wrensh = @cImport(@cInclude("wrensh.h"));

const log = std.log.scoped(.wrensh_fs);
const stack_size = 1 << 15;

const Method = *const fn (vm: ?*wren.c.WrenVM) callconv(.C) void;

pub fn bindMethod(signature: []const u8) ?Method {
    if (std.mem.eql(u8, signature, "read_(_,_)")) return readFile;
    if (std.mem.eql(u8, signature, "write_(_,_,_)")) return writeFile;
    if (std.mem.eql(u8, signature, "append_(_,_,_)")) return appendFile;
    if (std.mem.eql(u8, signature, "mkdir_(_,_,_)")) return mkdir;
    if (std.mem.eql(u8, signature, "mkdtemp_(_,_)")) return mkdtemp;
    if (std.mem.eql(u8, signature, "mkstemp_(_,_)")) return mkstemp;
    return null;
}

pub fn readFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    readFileSafe(wren.VM.get(vm)) catch {};
}

pub fn writeFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    writeFileSafe(wren.VM.get(vm)) catch {};
}

pub fn appendFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    appendFileSafe(wren.VM.get(vm)) catch {};
}

pub fn mkdir(vm: ?*wren.c.WrenVM) callconv(.C) void {
    _ = coro.xasync(mkdirCoro, .{wren.VM.get(vm)}, stack_size) catch {};
}

pub fn mkdtemp(vm: ?*wren.c.WrenVM) callconv(.C) void {
    _ = coro.xasync(mkdtempCoro, .{wren.VM.get(vm)}, stack_size) catch {};
}

pub fn mkstemp(vm: ?*wren.c.WrenVM) callconv(.C) void {
    _ = coro.xasync(mkstempCoro, .{wren.VM.get(vm)}, stack_size) catch {};
}

fn readFileSafe(vm: *wren.VM) !void {
    _ = try coro.xasync(readFileCoro, .{vm}, stack_size);
}

fn writeFileSafe(vm: *wren.VM) !void {
    _ = try coro.xasync(writeFileCoro, .{vm}, stack_size);
}

fn appendFileSafe(vm: *wren.VM) !void {
    _ = try coro.xasync(appendFileCoro, .{vm}, stack_size);
}

fn readFileCoro(vm: *wren.VM) void {
    readFileCoroSafe(vm) catch |err| {
        log.debug("read coro err", .{});
        vm.abortFiber("read failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn writeFileCoro(vm: *wren.VM) void {
    writeFileCoroSafe(vm, false) catch |err| {
        log.debug("write coro err", .{});
        vm.abortFiber("write failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn appendFileCoro(vm: *wren.VM) void {
    writeFileCoroSafe(vm, true) catch |err| {
        log.debug("append coro err", .{});
        vm.abortFiber("append failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn mkdirCoro(vm: *wren.VM) void {
    mkdirCoroSafe(vm) catch |err| {
        log.debug("mkdir coro err", .{});
        vm.abortFiber("mkdir failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn mkdtempCoro(vm: *wren.VM) void {
    mkdtempCoroSafe(vm) catch |err| {
        log.debug("mkdtemp coro err", .{});
        vm.abortFiber("mkdtemp failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn mkstempCoro(vm: *wren.VM) void {
    mkstempCoroSafe(vm) catch |err| {
        log.debug("mkstemp coro err", .{});
        vm.abortFiber("mkstemp failed err={any}", .{err});
        return;
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn mkdirCoroSafe(vm: *wren.VM) !void {
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const handle = vm.getSlot(1, .Handle);
    defer handle.deinit();

    const path = vm.getSlot(2, .String);

    const mode: c_int = switch (vm.getSlot(3, .Type)) {
        .Null => 0o775,
        .Num => @intFromFloat(vm.getSlot(3, .Num)),
        else => return error.WrongType,
    };

    try uv.coro.fs.mkdir(loop, path, mode);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, 0);
}

fn mkdtempCoroSafe(vm: *wren.VM) !void {
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const handle = vm.getSlot(1, .Handle);
    defer handle.deinit();

    const template = vm.getSlot(2, .String);

    var buf: [std.fs.max_path_bytes]u8 = undefined;
    const path = try uv.coro.fs.mkdtemp(loop, template, &buf);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, path);
}

fn mkstempCoroSafe(vm: *wren.VM) !void {
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const handle = vm.getSlot(1, .Handle);
    defer handle.deinit();

    const template = vm.getSlot(2, .String);

    var buf: [std.fs.max_path_bytes]u8 = undefined;
    const info = try uv.coro.fs.mkstemp(loop, template, &buf);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, info.path.?);
}

fn readFileCoroSafe(vm: *wren.VM) !void {
    const alloc = vm.args.allocator;
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const handle = vm.getSlot(1, .Handle);
    defer handle.deinit();

    const path = vm.getSlot(2, .String);
    const file = try uv.coro.File.open(loop, path, uv.File.Flags.RDONLY, 0);
    defer file.close();

    const reader = file.reader();
    const contents = try reader.readAllAlloc(alloc, 1 << 30);
    defer alloc.free(contents);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, contents);
}

fn finalize(vm: *wren.VM, frame: coro.Frame) void {
    const ctx = vm.getUser(wrensh.Ctx);
    vm.call(@ptrCast(ctx.wren_tx_val)) catch {
        vm.abortFiber("error", .{});
    };
    frame.deinit();
}

fn writeFileCoroSafe(vm: *wren.VM, append: bool) !void {
    log.debug("write file append={any}", .{append});
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const handle = vm.getSlot(1, .Handle);
    defer handle.deinit();

    const path = vm.getSlot(2, .String);
    const contents = vm.getSlot(3, .Bytes);

    const flag = if (append) uv.File.Flags.APPEND else uv.File.Flags.TRUNC;

    const file = try uv.coro.File.open(loop, path, flag | uv.File.Flags.WRONLY | uv.File.Flags.CREAT, 0o664);
    defer file.close();

    const writer = file.writer();
    const len = try writer.write(contents);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, len);
}
