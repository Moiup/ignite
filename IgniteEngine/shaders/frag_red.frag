#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// IN
layout(location = 1) in vec2 uv_frag;
layout(location = 2) flat in uint tex_i;

// UNIFORM
layout(binding = 3) uniform sampler samp;
layout(binding = 4) uniform texture2D textures[7];

// OUT
layout (location = 0) out vec4 out_color;

void main(){
    vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
    // out_color = color;

    out_color = texture(sampler2D(textures[tex_i], samp), uv_frag);
}
