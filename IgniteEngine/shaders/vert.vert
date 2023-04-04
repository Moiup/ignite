#version 450
#extension GL_ARB_separate_shader_objects : enable

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in uint object_id;

// UNIFORM BUFFER
layout (std140, binding = 0) uniform camera_buf{
    mat4 mvp;
} camera;

layout(std430, binding = 2) readonly buffer obj_tr_buff {
    mat4 tr[];
} obj_tr;

void main(){
    uint obj_i = object_id + gl_InstanceIndex;
    mat4 tr = obj_tr.tr[obj_i];
    
    gl_Position = camera.mvp * tr * vec4(coord, 1.0);
}