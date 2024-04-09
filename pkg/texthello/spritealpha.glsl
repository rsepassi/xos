@vs vs

uniform vs_params {
    mat4 proj;
};

in vec2 pos;
in vec2 texuv;

out vec2 uv;

void main() {
    gl_Position = proj * vec4(pos, 0.0, 1.0) - vec4(1.0, 1.0, 0.0, 0.0);
    uv = texuv;
}

@end

@fs fs

uniform utexture2D tex;
@sampler_type smp nonfiltering
uniform sampler smp;
uniform fs_params {
    int alpha_only;
    vec3 color;
    vec2 tex_size;
};

in vec2 uv;

out vec4 pix;

void main() {
    vec2 norm_uv = vec2(uv.x / tex_size.x, 1.0 - uv.y / tex_size.y);
    uvec4 texval = texture(usampler2D(tex, smp), norm_uv);

    if (alpha_only != 0) {
      float alpha = float(texval.r) / 255.0f;
      if (alpha > 0) {
        pix = vec4(color, alpha);
      } else {
        discard;
      }
    } else {
      pix = vec4(texval) / 255.0f;
    }
}

@end

@program spritealpha vs fs
