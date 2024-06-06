@group(0) @binding(0) var<uniform> ssize: vec2f;
@group(0) @binding(1) var tex: texture_2d<f32>;
@group(0) @binding(2) var smp: sampler;
@group(0) @binding(3) var<uniform> texsize: vec2f;

struct VertexInput {
    @location(0) pos: vec2f,
    @location(1) uv: vec2f,
};

struct VertexOutput {
    @builtin(position) pos: vec4f,
    @location(0) uv: vec2f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;

    out.pos = vec4f(
        (in.pos.x / ssize.x * 2) - 1,
        1 - (in.pos.y / ssize.y * 2),
        0.0,
        1.0
    );
    out.uv = vec2f(
        in.uv.x / texsize.x,
        in.uv.y / texsize.y,
    );

    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    var color = textureSample(tex, smp, in.uv);

    // Gamma correction, if needed
    // gamma or 1/gamma depending on which correction is needed
    let gamma: f32 = 2.2;
    color = vec4f(pow(color.rgb, vec3f(gamma)), color.a);

    return color;
}
