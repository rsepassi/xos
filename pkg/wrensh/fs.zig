const std = @import("std");
const wren = @import("wren");
const uv = @import("uv");
const coro = @import("zigcoro");
const wrensh = @cImport(@cInclude("wrensh.h"));

const log = std.log.scoped(.wrensh_fs);
const stack_size = 1 << 15;

pub fn readFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    readFileSafe(wren.VM.get(vm)) catch {};
}

pub fn writeFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    writeFileSafe(wren.VM.get(vm)) catch {};
}

pub fn appendFile(vm: ?*wren.c.WrenVM) callconv(.C) void {
    appendFileSafe(wren.VM.get(vm)) catch {};
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
        vm.abortFiber("read failed err={any}", .{err});
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn writeFileCoro(vm: *wren.VM) void {
    writeFileCoroSafe(vm, false) catch |err| {
        vm.abortFiber("write failed err={any}", .{err});
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
}

fn appendFileCoro(vm: *wren.VM) void {
    writeFileCoroSafe(vm, true) catch |err| {
        vm.abortFiber("append failed err={any}", .{err});
    };
    coro.xsuspendBlock(finalize, .{ vm, coro.xframe() });
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

    const flag = if (append) uv.File.Flags.APPEND else (uv.File.Flags.WRONLY | uv.File.Flags.TRUNC);

    const file = try uv.coro.File.open(loop, path, flag | uv.File.Flags.CREAT, 0o664);
    defer file.close();

    const writer = file.writer();
    const len = try writer.write(contents);

    vm.ensureSlots(2);
    vm.setSlot(0, handle);
    vm.setSlot(1, len);
}
