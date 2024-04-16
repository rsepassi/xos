const std = @import("std");

const wren = @cImport(
    @cInclude("wren.h"),
);

const cjson = @cImport(@cInclude("cJSON.h"));

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

export fn jsonEncode(vm: *wren.WrenVM) void {
    jsonEncodeSafe(vm) catch {
        wren.wrenSetSlotString(vm, 0, "failed to encode JSON");
        wren.wrenAbortFiber(vm, 0);
    };
}

export fn jsonDecode(vm: *wren.WrenVM) void {
    jsonDecodeSafe(vm) catch {
        const err = cjson.cJSON_GetErrorPtr();
        const err_slice = err[0..@min(std.mem.len(err), 10)];
        var err_buf: [256]u8 = undefined;
        const err_str = std.fmt.bufPrintZ(&err_buf, "failed to parse JSON, near: {s}", .{err_slice}) catch "failed to parse JSON";
        wren.wrenSetSlotString(vm, 0, err_str);
        wren.wrenAbortFiber(vm, 0);
    };
}

fn jsonParseValue(vm: *wren.WrenVM, json: *cjson.cJSON, slot: c_int) void {
    if (cjson.cJSON_IsBool(json) == 1) {
        wren.wrenSetSlotBool(vm, slot, cjson.cJSON_IsTrue(json) == 1);
    } else if (cjson.cJSON_IsNull(json) == 1) {
        wren.wrenSetSlotNull(vm, slot);
    } else if (cjson.cJSON_IsNumber(json) == 1) {
        wren.wrenSetSlotDouble(vm, slot, json.*.valuedouble);
    } else if (cjson.cJSON_IsString(json) == 1) {
        wren.wrenSetSlotString(vm, slot, json.*.valuestring);
    } else if (cjson.cJSON_IsArray(json) == 1) {
        const list = slot;
        wren.wrenSetSlotNewList(vm, list);

        const val = slot + 1;
        wren.wrenEnsureSlots(vm, val + 1);

        var element = json.child;
        while (element) |el| : (element = el.*.next) {
            jsonParseValue(vm, el, val);
            wren.wrenInsertInList(vm, list, -1, val);
        }
    } else if (cjson.cJSON_IsObject(json) == 1) {
        const map = slot;
        wren.wrenSetSlotNewMap(vm, map);
        const key = slot + 1;
        const val = slot + 2;

        wren.wrenEnsureSlots(vm, val + 1);

        var element = json.child;
        while (element) |el| : (element = el.*.next) {
            jsonParseValue(vm, el, val);
            wren.wrenSetSlotString(vm, key, el.*.string);
            wren.wrenSetMapValue(vm, map, key, val);
        }
    }
}

fn jsonDecodeSafe(vm: *wren.WrenVM) !void {
    const s = wren.wrenGetSlotString(vm, 1);
    const json = cjson.cJSON_Parse(s) orelse return error.JSONParse;
    defer cjson.cJSON_Delete(json);
    jsonParseValue(vm, json, 0);
}

fn jsonEncodeValue(vm: *wren.WrenVM, slot: c_int) !*cjson.cJSON {
    switch (wren.wrenGetSlotType(vm, slot)) {
        wren.WREN_TYPE_BOOL => {
            return cjson.cJSON_CreateBool(if (wren.wrenGetSlotBool(vm, slot)) 1 else 0);
        },
        wren.WREN_TYPE_NUM => {
            return cjson.cJSON_CreateNumber(wren.wrenGetSlotDouble(vm, slot));
        },
        wren.WREN_TYPE_NULL => {
            return cjson.cJSON_CreateNull();
        },
        wren.WREN_TYPE_STRING => {
            return cjson.cJSON_CreateString(wren.wrenGetSlotString(vm, slot));
        },
        wren.WREN_TYPE_LIST => {
            const arr = cjson.cJSON_CreateArray();
            const n: usize = @intCast(wren.wrenGetListCount(vm, slot));
            const val_slot = slot + 1;
            wren.wrenEnsureSlots(vm, val_slot + 1);
            for (0..n) |i| {
                wren.wrenGetListElement(vm, slot, @intCast(i), val_slot);
                const item = try jsonEncodeValue(vm, val_slot);
                _ = cjson.cJSON_AddItemToArray(arr, item);
            }
            return arr;
        },
        wren.WREN_TYPE_MAP => {
            const obj = cjson.cJSON_CreateObject();
            const n: usize = @intCast(wren.wrenGetMapCount(vm, slot));
            const key_slot = slot + 1;
            const val_slot = slot + 2;
            wren.wrenEnsureSlots(vm, val_slot + 1);

            // TODO: Need keys...call keys.toList() on the map?
            _ = key_slot;
            for (0..n) |i| {
                _ = i;
                std.debug.print("JSON map encode not yet implemented\n", .{});
                return error.JSONMapNotImplemented;
                // wren.wrenGetMapValue(vm, slot, key_slot, val_slot);
                // const item = try jsonEncodeValue(vm, val_slot);
                // cjson.cJSON_AddItemToObject(obj, key, item);
            }
            return obj;
        },
        wren.WREN_TYPE_FOREIGN, wren.WREN_TYPE_UNKNOWN => {
            return error.JSONUnencodable;
        },
        else => unreachable,
    }
    unreachable;
}

fn jsonEncodeSafe(vm: *wren.WrenVM) !void {
    const json = try jsonEncodeValue(vm, 1);
    defer cjson.cJSON_Delete(json);
    const out = cjson.cJSON_Print(json);
    const out_slice = out[0..std.mem.len(out)];
    defer std.heap.c_allocator.free(out_slice);
    wren.wrenSetSlotString(vm, 0, out);
}
