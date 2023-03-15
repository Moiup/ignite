#version 450
#extension GL_ARB_separate_shader_objects : enable

// IN

// OUT
layout (location = 0) out vec4 out_color;

void main(){
    vec4 color = vec4(0.0, 1.0, 0.0, 1.0);
    out_color = color;
}
