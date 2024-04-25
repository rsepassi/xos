const std = @import("std");
const wren = @import("wren");
const uv = @import("uv");
const coro = @import("zigcoro");
const wrensh = @import("wrensh.zig");

const log = std.log.scoped(.wrensh);

pub fn sleep(cvm: ?*wren.c.WrenVM) callconv(.C) void {
    const vm = wren.VM.get(cvm);
    _ = coro.xasync(sleepCoro, .{vm}, 1 << 14) catch {};
}

fn sleepCoro(vm: *wren.VM) !void {
    const fiber = vm.getSlot(1, .Handle);

    sleepCoroSafe(vm) catch |err| {
        wrensh.fiberError(vm, fiber, "process spawn failed err={any}", .{err});
        return;
    };

    coro.xsuspendBlock(corofinalize, .{ vm, coro.xframe(), fiber });
}

fn corofinalize(vm: *wren.VM, frame: coro.Frame, fiber: wren.Handle) void {
    vm.ensureSlots(1);
    vm.setSlot(0, fiber);
    const ctx = wrensh.getCtx(vm);
    vm.call(@ptrCast(ctx.wren_tx)) catch @panic("bad call");
    fiber.deinit();
    frame.deinit();
}

fn sleepCoroSafe(vm: *wren.VM) !void {
    const ctx = vm.getUser(wrensh.Ctx);
    const loop: *uv.uv_loop_t = @ptrCast(@alignCast(ctx.loop));

    const timeout: usize = @intFromFloat(vm.getSlot(2, .Num));
    try uv.coro.sleep(loop, timeout);
}
