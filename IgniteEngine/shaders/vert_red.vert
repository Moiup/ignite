#version 450 core
#extension GL_ARB_separate_shader_objects : enable

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in uint mesh_offsets;
layout (location = 2) in vec2 uv;

// UNIFORM BUFFER
layout (std140, binding = 0) uniform camera_buf{
    mat4 mvp;
} camera;

layout(std430, binding = 1) readonly buffer obj_tr_i_buff {
    uint tr_i[];
} obj_tr_i;

layout(std430, binding = 2) readonly buffer obj_tr_buff {
    mat4 tr[];
} obj_tr;

layout(std430, binding = 3) readonly buffer texture_i_buff {
    uint tex_i[];
} texture_i;

// OUT
layout(location = 1) out vec2 uv_frag;
layout(location = 2) out uint tex_i;

void main(){
    uint obj_i = gl_VertexIndex + gl_InstanceIndex * mesh_offsets;
    
    uv_frag = uv;
    tex_i = texture_i.tex_i[obj_i];

    uint tr_i = obj_tr_i.tr_i[obj_i];
    mat4 tr = obj_tr.tr[tr_i];

    gl_Position = camera.mvp * tr * vec4(coord, 1.0);
}