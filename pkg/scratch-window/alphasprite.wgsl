// Vertex shader
// Inputs:
//   Vertex
//   Texture coordinates
// Uniforms:
//   Orthographic projection matrix
// Outputs
//   Normalized device coordinates
//   Texture coordinates

@binding(0) @group(0) var<uniform> proj : mat4x4f;

@vertex
fn vtx_main() -> @builtin(position) vec4f {
    // glsl version
    // gl_Position = proj * vec4(pos, 0.0, 1.0) - vec4(1.0, 1.0, 0.0, 0.0);
    // uv = texuv;
  return vec4(
}

// Fragment shader
// Inputs:
//   Texture coordinate
// Uniforms:
//   Texture
//   Texture size
//   Sampler
//   Color

@fragment
fn frag_main() -> @location(0) vec4f {

    // glsl version
    // vec2 norm_uv = vec2(uv.x / tex_size.x, 1.0 - uv.y / tex_size.y);
    // uvec4 texval = texture(usampler2D(tex, smp), norm_uv);
    // float alpha = float(texval.r) / 255.0f;

    // if (alpha > 0) {
    //   pix = vec4(color, alpha);
    // } else {
    //   discard;
    // }
}
