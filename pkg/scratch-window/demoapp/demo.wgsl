@vertex
fn vs_main(@location(0) xy: vec2f) -> @builtin(position) vec4f {
    return vec4f(xy.x, xy.y, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(1.0, 0.0, 0.0, 1.0);
}
