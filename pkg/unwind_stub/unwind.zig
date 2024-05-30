fn dummyUnwind() callconv(.C) void {
    @panic("unwind unsupported");
}

fn exportUnwind() void {
    const unwind_syms = &.{
        "_Unwind_Backtrace",
        "_Unwind_FindEnclosingFunction",
        "_Unwind_GetCFA",
        "_Unwind_GetDataRelBase",
        "_Unwind_GetIP",
        "_Unwind_GetIPInfo",
        "_Unwind_GetLanguageSpecificData",
        "_Unwind_GetRegionStart",
        "_Unwind_GetTextRelBase",
        "_Unwind_Resume",
        "_Unwind_SetGR",
        "_Unwind_SetIP",
        "_Unwind_DeleteException",
        "_Unwind_RaiseException",
    };

    for (unwind_syms) |sym| {
        @export(dummyUnwind, .{ .name = sym });
    }
}
