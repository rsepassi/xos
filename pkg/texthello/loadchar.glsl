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
    vec3 color;
    vec2 tex_size;
};

in vec2 uv;

out vec4 pix;

void main() {
    vec2 nuv = vec2(uv.x / tex_size.x, 1.0 - uv.y / tex_size.y);
    float alpha = texture(usampler2D(tex, smp), nuv).r / 255.0;
    if (alpha > 0) {
      pix = vec4(color, alpha);
    } else {
      discard;
    }
}

@end

@program loadchar vs fs
