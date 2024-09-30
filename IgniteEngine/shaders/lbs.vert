#version 450
#extension GL_ARB_separate_shader_objects : enable

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in uint object_id;
layout (location = 2) in uint material_id;
layout (location = 3) in vec2 uv;
layout (location = 4) in uvec4 joints;
layout (location = 5) in vec4 weights;

// OUT
layout(location = 1) out uint material_i;
layout(location = 2) out vec2 uv_frag;

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

layout(std430, binding = 2) readonly buffer joint_tr_buff
{
    mat4 jtr[];
} joint_tr;

void main(){
    uint obj_i = object_id + gl_InstanceIndex;
    mat4 tr = obj_tr.tr[obj_i];

    mat4 skin_mat =
        weights.x * joint_tr.jtr[joints.x] +
        weights.y * joint_tr.jtr[joints.y] +
        weights.z * joint_tr.jtr[joints.z] +
        weights.w * joint_tr.jtr[joints.w];

    material_i = material_id;
    uv_frag = vec2(uv.x, 1.0 - uv.y);
    
    gl_Position = mvp * tr * skin_mat * vec4(coord, 1.0);
}