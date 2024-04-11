pub const Size = struct {
    height: usize,
    width: usize,
    pub fn area(self: @This()) usize {
        return self.height * self.width;
    }
};

pub const Rect = struct {
    tl: Point,
    br: Point,

    pub fn height(self: @This()) f32 {
        return self.tl.y - self.br.y;
    }

    pub fn width(self: @This()) f32 {
        return self.br.x - self.tl.x;
    }
};

pub const Point = struct {
    x: f32,
    y: f32,

    pub fn down(self: @This(), delta: f32) Point {
        return .{ .x = self.x, .y = self.y - delta };
    }
    pub fn up(self: @This(), delta: f32) Point {
        return .{ .x = self.x, .y = self.y + delta };
    }
    pub fn left(self: @This(), delta: f32) Point {
        return .{ .x = self.x - delta, .y = self.y };
    }
    pub fn right(self: @This(), delta: f32) Point {
        return .{ .x = self.x + delta, .y = self.y };
    }
};
