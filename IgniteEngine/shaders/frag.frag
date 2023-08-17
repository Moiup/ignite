#version 450
#extension GL_ARB_separate_shader_objects : enable

// BUFFERS
struct Material {
    vec3 Ka;
    int map_Ka;

    vec3 Kd;
    int map_Kd;

    vec3 Ks;
    int map_Ks;

    vec3 Ke;
    int map_Ke;

    vec3 Kt;
    int map_Kt;

    vec3 Tf;
    
    float Ns;
    float Ni;
    float d;
    float illum;

    int map_Ns;
    int map_Ni;
    int map_d;
    int map_bump;
};

layout(std430, binding = 2) readonly buffer MaterialsBuffer {
    Material materials[];
};

// IN
layout(location = 1) flat in uint material_i;

// OUT
layout (location = 0) out vec4 out_color;

void main(){
    // vec4 color = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 color = vec4(materials[material_i].Kd, 1.0);
    out_color = color;
}
