const std = @import("std");
const c = @cImport(@cInclude("libclipboard.h"));

pub const Clipboard = struct {
    clipboard: *c.clipboard_c,

    pub fn init() !@This() {
        return .{
            .clipboard = c.clipboard_new(null) orelse return error.BadClipboard,
        };
    }

    pub fn deinit(self: @This()) void {
        c.clipboard_free(self.clipboard);
    }

    pub fn clear(self: @This()) void {
        c.clipboard_clear(self.clipboard, c.LCB_CLIPBOARD);
    }

    pub fn get(self: @This()) ?[]const u8 {
        const ctext = c.clipboard_text(self.clipboard) orelse return null;
        return ctext[0..std.mem.len(ctext)];
    }

    pub fn set(self: @This(), text: []const u8) !void {
        const done = c.clipboard_set_text_ex(self.clipboard, text.ptr, text.len, c.LCB_CLIPBOARD);
        if (!done) return error.BadClipboardPaste;
    }
};
