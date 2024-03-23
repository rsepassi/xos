const std = @import("std");

const wren = @cImport(
    @cInclude("wren.h"),
);

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

export fn timerAlloc(vm: *wren.WrenVM) void {
    const ptr = wren.wrenSetSlotNewForeign(vm, 0, 0, @sizeOf(std.time.Timer));
    const timer: *std.time.Timer = @ptrCast(@alignCast(ptr));
    timer.* = std.time.Timer.start() catch @panic("bad timer");
}

export fn timerFinal(data: *anyopaque) void {
    _ = data;
}

export fn timerLap(vm: *wren.WrenVM) void {
    const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
    const t = timer.lap();
    wren.wrenSetSlotDouble(vm, 0, @floatFromInt(t / std.time.ns_per_ms));
}

export fn timerReset(vm: *wren.WrenVM) void {
    const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
    timer.reset();
}

export fn timerRead(vm: *wren.WrenVM) void {
    const timer: *std.time.Timer = @ptrCast(@alignCast(wren.wrenGetSlotForeign(vm, 0)));
    const t = timer.read();
    wren.wrenSetSlotDouble(vm, 0, @floatFromInt(t / std.time.ns_per_ms));
}
