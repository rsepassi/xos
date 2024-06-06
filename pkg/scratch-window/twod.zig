pub const Size = extern struct {
    width: f32,
    height: f32,

    pub fn init(width: f32, height: f32) @This() {
        return .{ .width = width, .height = height };
    }

    pub inline fn area(self: @This()) f32 {
        return self.height * self.width;
    }
};

pub const Rect = struct {
    tl: Point,
    br: Point,

    pub inline fn height(self: @This()) f32 {
        return self.tl.y - self.br.y;
    }

    pub inline fn width(self: @This()) f32 {
        return self.br.x - self.tl.x;
    }

    pub inline fn bl(self: @This()) Point {
        return self.tl.down(self.height());
    }

    pub inline fn tr(self: @This()) Point {
        return self.tl.right(self.width());
    }

    pub inline fn down(self: @This(), delta: f32) @This() {
        return .{ .tl = self.tl.down(delta), .br = self.br.down(delta) };
    }

    pub inline fn right(self: @This(), delta: f32) @This() {
        return .{ .tl = self.tl.right(delta), .br = self.br.right(delta) };
    }

    pub inline fn up(self: @This(), delta: f32) @This() {
        return .{ .tl = self.tl.up(delta), .br = self.br.up(delta) };
    }

    pub inline fn left(self: @This(), delta: f32) @This() {
        return .{ .tl = self.tl.left(delta), .br = self.br.left(delta) };
    }

    pub fn fromSize(size: Size) @This() {
        const tr_ = Point{
            .x = size.width,
            .y = size.height,
        };
        return .{
            .tl = tr_.left(tr_.x),
            .br = tr_.down(tr_.y),
        };
    }

    pub fn toTriangles(self: @This()) [2][3]Point {
        return .{
            .{ self.bl(), self.br, self.tl },
            .{ self.tl, self.br, self.tr() },
        };
    }
};

pub const Point = extern struct {
    x: f32,
    y: f32,

    pub inline fn down(self: @This(), delta: f32) Point {
        return .{ .x = self.x, .y = self.y - delta };
    }
    pub inline fn up(self: @This(), delta: f32) Point {
        return .{ .x = self.x, .y = self.y + delta };
    }
    pub inline fn left(self: @This(), delta: f32) Point {
        return .{ .x = self.x - delta, .y = self.y };
    }
    pub inline fn right(self: @This(), delta: f32) Point {
        return .{ .x = self.x + delta, .y = self.y };
    }
};

pub const RGBAf = extern struct {
    r: f64,
    g: f64,
    b: f64,
    a: f64,
};

pub const RGBA = extern struct {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
};

pub const Image = struct {
    data: []const RGBA,
    size: Size,

    pub fn tou8slice(self: @This()) []const u8 {
        const len = self.data.len * @sizeOf(RGBA);
        const ptr: [*]const u8 = @ptrCast(self.data.ptr);
        return ptr[0..len];
    }
};
