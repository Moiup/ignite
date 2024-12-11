#version 450
#extension GL_ARB_separate_shader_objects : enable

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in uvec4 joints;
layout (location = 2) in vec4 weights;

layout (push_constant, std430) uniform pc {
    mat4 mvp;
};

layout (std430, binding = 1) readonly buffer obj_tr_buff {
    mat4 tr[];
} obj_tr;

layout (std430, binding = 2) readonly buffer joint_tr_buff {
    mat4 jtr[];
} joint_tr;

void main(){

    mat4 tr = obj_tr.tr[0];
    mat4 skin_mat =
        weights.x * joint_tr.jtr[joints.x] +
        weights.y * joint_tr.jtr[joints.y] +
        weights.z * joint_tr.jtr[joints.z] +
        weights.w * joint_tr.jtr[joints.w];

    gl_Position = mvp * tr * skin_mat * vec4(coord, 1.0);
}