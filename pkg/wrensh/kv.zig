const std = @import("std");
const lmdb = @import("lmdb");
const wren = @import("wren");

const log = std.log.scoped(.wrensh);

pub const KV = struct {
    pub fn alloc(cvm: ?*wren.c.WrenVM) callconv(.C) void {
        allocSafe(wren.VM.get(cvm)) catch {};
    }

    fn allocSafe(vm: *wren.VM) !void {
        log.info("kv init", .{});
        errdefer vm.abortFiber("failed to open kv db", .{});
        const db = try vm.setSlotNewForeign(0, 0, lmdb.Environment);
        const path = vm.getSlot(1, .String);
        db.* = try lmdb.Environment.init(path, .{});
    }

    pub fn final(data: ?*anyopaque) callconv(.C) void {
        const db: *lmdb.Environment = @ptrCast(@alignCast(data));
        db.deinit();
    }

    pub fn get(vm: ?*wren.c.WrenVM) callconv(.C) void {
        getSafe(wren.VM.get(vm)) catch {};
    }

    fn getSafe(vm: *wren.VM) !void {
        log.info("kv get", .{});
        errdefer vm.abortFiber("kv get failed", .{});
        const db = vm.getSlotForeign(0, lmdb.Environment);
        const txn = try lmdb.Transaction.init(db.*, .{ .mode = .ReadOnly });
        errdefer txn.abort();

        const key = vm.getSlot(1, .Bytes);
        log.debug("kv get key={s}", .{key});

        const val = try txn.get(key);
        if (val) |v| {
            vm.setSlot(0, v);
        } else {
            vm.setSlot(0, null);
        }

        try txn.commit();
    }

    pub fn getp(vm: ?*wren.c.WrenVM) callconv(.C) void {
        getpSafe(wren.VM.get(vm)) catch {};
    }

    fn getpSafe(vm: *wren.VM) !void {
        log.info("kv getp", .{});
        errdefer vm.abortFiber("kv getp failed", .{});

        const db = vm.getSlotForeign(0, lmdb.Environment);
        const txn = try lmdb.Transaction.init(db.*, .{ .mode = .ReadOnly });
        errdefer txn.abort();

        var cursor = try txn.cursor();
        defer cursor.deinit();

        const prefix = vm.getSlot(1, .Bytes);

        wren.c.wrenEnsureSlots(vm.vm, 3);
        wren.c.wrenSetSlotNewMap(vm.vm, 0);

        var key = try cursor.seek(prefix);

        while (key != null and std.mem.startsWith(u8, key.?, prefix)) : (key = try cursor.goToNext()) {
            const val = try cursor.getCurrentValue();

            vm.setSlot(1, key.?);
            vm.setSlot(2, val);
            wren.c.wrenSetMapValue(vm.vm, 0, 1, 2);
        }

        try txn.commit();
    }

    pub fn set(vm: ?*wren.c.WrenVM) callconv(.C) void {
        setSafe(wren.VM.get(vm)) catch {};
    }

    fn setSafe(vm: *wren.VM) !void {
        log.info("kv set", .{});
        errdefer vm.abortFiber("kv set failed", .{});
        const db = vm.getSlotForeign(0, lmdb.Environment);
        const txn = try lmdb.Transaction.init(db.*, .{ .mode = .ReadWrite });
        errdefer txn.abort();

        const key = vm.getSlot(1, .Bytes);
        const val = vm.getSlot(2, .Bytes);

        try txn.set(key, val);
        try txn.commit();
    }
};
