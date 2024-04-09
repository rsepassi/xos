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

uniform texture2D tex;
uniform sampler smp;
uniform fs_params {
    vec3 color;
};

in vec2 uv;

out vec4 pix;

void main() {
    float alpha = texture(sampler2D(tex, smp), vec2(uv.x, 1.0 - uv.y)).r;
    if (alpha > 0) {
      pix = vec4(color, alpha);
    } else {
      discard;
    }
}

@end

@program loadchar vs fs
