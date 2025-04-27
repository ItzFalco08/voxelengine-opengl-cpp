#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aFaceId;
layout(location = 2) in vec2 aUV;

flat out int vFaceId;
out vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vFaceId = int(aFaceId);
    vec3 p = aPos + vec3(0.5); // Convert from [-0.5,0.5] to [0,1]

    // Calculate UVs based on face
    if (vFaceId == 0) {         // Top face
        vUV = vec2(p.x, 1.0 - p.z); // Flip Z to match texture orientation
    } 
    else if (vFaceId == 1) {    // Bottom face
        vUV = vec2(p.x, p.z);
    }
    else {                      // Side faces (front/back/left/right)
        vUV = vec2(p.x + p.z, p.y); // Combined coordinate for sides
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}