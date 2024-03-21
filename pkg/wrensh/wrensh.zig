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
