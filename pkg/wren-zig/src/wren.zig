const std = @import("std");
pub const c = @cImport(@cInclude("wren.h"));

const str = []const u8;
const cstr = [:0]const u8;

const log = std.log.scoped(.wren);

threadlocal var tls_alloc: std.mem.Allocator = undefined;

pub const ForeignClassMethods = c.WrenForeignClassMethods;
pub const ForeignMethod = *const fn (?*c.WrenVM) callconv(.C) void;

pub const VM = struct {
    vm: *c.WrenVM,
    args: InitArgs,

    const Self = @This();

    const WriteFn = *const fn (vm: *Self, s: cstr) void;
    const ErrorFn = *const fn (vm: *Self, err_type: ErrorType, module: ?cstr, line: ?usize, msg: cstr) void;
    const ForeignMethodFn = *const fn (vm: *Self, module: cstr, class_name: cstr, is_static: bool, signature: cstr) ?ForeignMethod;
    const ForeignClassFn = *const fn (vm: *Self, module: cstr, class_name: cstr) ForeignClassMethods;

    const InitArgs = struct {
        allocator: std.mem.Allocator,
        write_fn: ?WriteFn = null,
        error_fn: ?ErrorFn = null,
        foreign_method_fn: ?ForeignMethodFn = null,
        foreign_class_fn: ?ForeignClassFn = null,
        user_data: ?*anyopaque = null,
        use_tls_allocator: bool = true,
    };

    pub fn init(args: InitArgs) !*Self {
        var config: c.WrenConfiguration = undefined;
        c.wrenInitConfiguration(&config);

        const self = try args.allocator.create(Self);
        errdefer args.allocator.destroy(self);

        config.userData = self;
        if (args.use_tls_allocator) {
            tls_alloc = args.allocator;
            config.reallocateFn = realloc;
        }
        if (args.write_fn != null) config.writeFn = userWriteFn;
        if (args.error_fn != null) config.errorFn = userErrorFn;
        if (args.foreign_method_fn != null) config.bindForeignMethodFn = userForeignMethodFn;
        if (args.foreign_class_fn != null) config.bindForeignClassFn = userForeignClassFn;

        const vm = c.wrenNewVM(&config) orelse return error.WrenInit;
        self.* = .{ .vm = vm, .args = args };
        return self;
    }

    pub fn deinit(self: *Self) void {
        c.wrenFreeVM(self.vm);
        self.args.allocator.destroy(self);
    }

    pub fn get(vm: ?*c.WrenVM) *Self {
        return @ptrCast(@alignCast(c.wrenGetUserData(vm)));
    }

    pub fn getUser(self: *Self, comptime T: type) *T {
        return @ptrCast(@alignCast(self.args.user_data));
    }

    pub fn collectGarbase(self: *Self) void {
        c.wrenCollectGarbage(self.vm);
    }

    const ResultError = error{
        Compile,
        Runtime,
    };
    pub fn interpret(self: *Self, module: cstr, src: cstr) ResultError!void {
        return switch (c.wrenInterpret(self.vm, module.ptr, src.ptr)) {
            c.WREN_RESULT_SUCCESS => void{},
            c.WREN_RESULT_COMPILE_ERROR => ResultError.Compile,
            c.WREN_RESULT_RUNTIME_ERROR => ResultError.Runtime,
            else => unreachable,
        };
    }

    pub fn makeCallHandle(self: *Self, signature: cstr) Handle {
        return .{
            .vm = self,
            .handle = c.wrenMakeCallHandle(self.vm, signature.ptr),
        };
    }

    pub fn numSlots(self: *Self) usize {
        return @intCast(c.wrenGetSlotCount(self.vm));
    }

    pub fn ensureSlots(self: *Self, n: usize) void {
        return c.wrenEnsureSlots(self.vm, @intCast(n));
    }

    const Type = enum {
        Bool,
        Num,
        Foreign,
        List,
        Map,
        Null,
        String,
        Unknown,

        fn fromInt(x: c_uint) @This() {
            return switch (x) {
                c.WREN_TYPE_BOOL => .Bool,
                c.WREN_TYPE_NUM => .Num,
                c.WREN_TYPE_FOREIGN => .Foreign,
                c.WREN_TYPE_LIST => .List,
                c.WREN_TYPE_MAP => .Map,
                c.WREN_TYPE_NULL => .Null,
                c.WREN_TYPE_STRING => .String,
                c.WREN_TYPE_UNKNOWN => .Unknown,
                else => unreachable,
            };
        }
    };

    const SlotType = enum {
        Bool,
        Bytes,
        String,
        Num,
        Handle,
        List,
        Type,

        fn getType(x: @This()) type {
            return switch (x) {
                .Bool => bool,
                .Bytes => []const u8,
                .String => [:0]const u8,
                .Num => f64,
                .Handle => Handle,
                .List => List,
                .Type => Type,
            };
        }
    };
    pub fn getSlot(self: *Self, i: usize, comptime T: SlotType) T.getType() {
        return getSlot2(self, i, T.getType());
    }

    pub fn getSlotForeign(self: *Self, idx: usize, comptime T: type) *T {
        const i: c_int = @intCast(idx);
        return @ptrCast(@alignCast(c.wrenGetSlotForeign(self.vm, i)));
    }

    pub fn setSlot(self: *Self, idx: usize, val: anytype) void {
        const i: c_int = @intCast(idx);
        switch (@TypeOf(val)) {
            bool => {
                c.wrenSetSlotBool(self.vm, i, @intFromBool(val));
            },
            @TypeOf(null) => {
                c.wrenSetSlotNull(self.vm, i);
            },
            []const u8, []u8 => {
                c.wrenSetSlotBytes(self.vm, i, val.ptr, val.len);
            },
            [:0]u8, [:0]const u8 => {
                c.wrenSetSlotString(self.vm, i, val.ptr);
            },
            Handle => {
                c.wrenSetSlotHandle(self.vm, i, val.handle);
            },
            i8, i16, i32, i64, u8, u16, u32, u64, usize => {
                c.wrenSetSlotDouble(self.vm, i, @floatFromInt(val));
            },
            else => {
                @compileLog(@TypeOf(val));
                @compileError("unsupported wren type");
            },
        }
    }

    pub fn setSlotNewForeign(self: *Self, i: usize, classSlot: usize, comptime T: type) !*T {
        const ptr = c.wrenSetSlotNewForeign(self.vm, @intCast(i), @intCast(classSlot), @sizeOf(T));
        if (ptr == null) return error.WrenAlloc;
        return @ptrCast(@alignCast(ptr));
    }

    pub fn abortFiber(self: *Self, comptime fmt: str, args: anytype) void {
        const msg = std.fmt.allocPrint(self.args.allocator, fmt, args) catch @panic("no mem");
        defer self.args.allocator.free(msg);
        self.setSlot(0, msg);
        self.abortFiberSlot(0);
    }

    pub fn abortFiberSlot(self: *Self, i: usize) void {
        c.wrenAbortFiber(self.vm, @intCast(i));
    }

    pub fn call(self: *Self, handle: ?*c.WrenHandle) !void {
        const rc = c.wrenCall(self.vm, handle);
        return switch (rc) {
            c.WREN_RESULT_SUCCESS => void{},
            c.WREN_RESULT_COMPILE_ERROR => ResultError.Compile,
            c.WREN_RESULT_RUNTIME_ERROR => ResultError.Runtime,
            else => unreachable,
        };
    }

    const List = struct {
        vm: *Self,
        slot: c_int,

        pub fn len(self: @This()) usize {
            return @intCast(c.wrenGetListCount(self.vm.vm, self.slot));
        }

        pub fn get(self: @This(), i: usize, scratch: usize, comptime T: SlotType) T.getType() {
            c.wrenGetListElement(self.vm.vm, self.slot, @intCast(i), @intCast(scratch));
            return self.vm.getSlot(scratch, T);
        }
    };

    fn getSlot2(self: *Self, idx: usize, comptime T: type) T {
        const i: c_int = @intCast(idx);
        if (T == bool) {
            return c.wrenGetSlotBool(self.vm, i);
        } else if (T == []const u8) {
            var len: c_int = 0;
            const buf = c.wrenGetSlotBytes(self.vm, i, &len);
            return buf[0..@intCast(len)];
        } else if (T == [:0]const u8) {
            const buf = c.wrenGetSlotString(self.vm, i);
            return buf[0..std.mem.len(buf) :0];
        } else if (T == f64) {
            return c.wrenGetSlotDouble(self.vm, i);
        } else if (T == Handle) {
            return .{
                .vm = self,
                .handle = c.wrenGetSlotHandle(self.vm, i),
            };
        } else if (T == List) {
            return .{
                .vm = self,
                .slot = i,
            };
        } else if (T == Type) {
            return Type.fromInt(c.wrenGetSlotType(self.vm, i));
        } else {
            @compileError("invalid slot type");
        }
    }

    fn realloc(memory: ?*anyopaque, new_size: usize, _: ?*anyopaque) callconv(.C) ?*anyopaque {
        const alloc = tls_alloc;

        const Header = struct {
            sz: usize,
        };
        const hsz = @sizeOf(Header);

        if (memory == null) {
            const buf = alloc.alloc(u8, new_size + hsz) catch return null;
            const header: *Header = @ptrCast(@alignCast(buf.ptr));
            header.sz = new_size + hsz;
            return buf.ptr + hsz;
        } else if (new_size == 0) {
            var buf: [*]u8 = @ptrCast(memory.?);
            buf -= hsz;
            const header: *Header = @ptrCast(@alignCast(buf));
            alloc.free(buf[0..header.sz]);
            return null;
        } else {
            var buf: [*]u8 = @ptrCast(memory.?);
            buf -= hsz;
            var header: *Header = @ptrCast(@alignCast(buf));
            const new_buf = alloc.realloc(buf[0..header.sz], new_size + hsz) catch return null;
            header = @ptrCast(@alignCast(new_buf.ptr));
            header.sz = new_size + hsz;
            return new_buf.ptr + hsz;
        }
    }

    fn userWriteFn(vm: ?*c.WrenVM, s: [*c]const u8) callconv(.C) void {
        const self = get(vm);
        self.args.write_fn.?(self, getcStr(s));
    }

    fn userErrorFn(vm: ?*c.WrenVM, err_type: c_uint, module: [*c]const u8, line: c_int, msg: [*c]const u8) callconv(.C) void {
        const self = get(vm);
        self.args.error_fn.?(
            self,
            ErrorType.fromInt(err_type),
            getcStr(module),
            if (line >= 0) @intCast(line) else null,
            getcStr(msg),
        );
    }

    fn userForeignMethodFn(vm: ?*c.WrenVM, module: [*c]const u8, class_name: [*c]const u8, is_static: bool, signature: [*c]const u8) callconv(.C) ?ForeignMethod {
        const self = get(vm);
        return self.args.foreign_method_fn.?(
            self,
            getcStr(module),
            getcStr(class_name),
            is_static,
            getcStr(signature),
        );
    }

    fn userForeignClassFn(vm: ?*c.WrenVM, module: [*c]const u8, class_name: [*c]const u8) callconv(.C) ForeignClassMethods {
        const self = get(vm);
        return self.args.foreign_class_fn.?(
            self,
            getcStr(module),
            getcStr(class_name),
        );
    }

    fn getcStr(s: [*c]const u8) [:0]const u8 {
        if (s == 0) return "";
        return s[0..std.mem.len(s) :0];
    }
};

pub const Handle = struct {
    vm: *VM,
    handle: ?*c.WrenHandle,

    pub fn call(self: @This()) VM.ResultError!void {
        c.wrenCall(self.vm.vm, self.handle);
    }

    pub fn deinit(self: @This()) void {
        c.wrenReleaseHandle(self.vm.vm, self.handle);
    }
};

pub const ErrorType = enum {
    Compile,
    Runtime,
    StackTrace,

    fn fromInt(e: c_uint) @This() {
        return switch (e) {
            c.WREN_ERROR_COMPILE => .Compile,
            c.WREN_ERROR_RUNTIME => .Runtime,
            c.WREN_ERROR_STACK_TRACE => .StackTrace,
            else => unreachable,
        };
    }
};
