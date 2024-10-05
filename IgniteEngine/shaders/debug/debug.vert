#version 450
#extension GL_ARB_separate_shader_objects : enable

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in uint object_id;
layout (location = 2) in uint material_id;

// OUT
layout(location = 1) out uint material_i;

// PUSH CONSTANT
layout(push_constant, std430) uniform pc {
    mat4 mvp;
};

// UNIFORM BUFFER
// layout (std140, binding = 0) uniform camera_buf{
//     mat4 mvp;
// } camera;

layout(std430, binding = 1) readonly buffer obj_tr_buff {
    mat4 tr[];
} obj_tr;


void main(){
    uint obj_i = object_id + gl_InstanceIndex;
    mat4 tr = obj_tr.tr[obj_i];

    material_i = material_id;

    gl_Position = mvp * tr * vec4(coord, 1.0);
}