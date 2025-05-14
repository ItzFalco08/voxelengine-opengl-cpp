#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 localUv;
layout(location = 2) in float vFaceId;
layout(location = 3) in float blockType;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec2 finalUv;
float block_size = 1.0f / 8.0f; // Size of each block in the texture atlas

// Predefined UV coordinates for each block type and face
vec2 grassTopUv = vec2(block_size * 1.0f, 1.0f - block_size);
vec2 grassBottomUv = vec2(block_size * 2.0f, 1.0f - block_size);
vec2 grassSideUv = vec2(block_size * 0.0f, 1.0f - block_size);
vec2 stoneSideUv = vec2(block_size * 3.0f, 1.0f - block_size);
vec2 dirtSideUv = vec2(block_size * 2.0f, 1.0f - block_size);
vec2 bedrockSideUv = vec2(block_size * 4.0f, 1.0f - block_size);

void uvCalc() {
    // grass
    if (blockType == 1.0f) {
        if (vFaceId == 0.0f) {
            // top face
            finalUv = grassTopUv + block_size * localUv;
        } else if (vFaceId == 1.0f) {
            // bottom face
            finalUv = grassBottomUv + block_size * localUv;
        } else {
            // side face
            finalUv = grassSideUv + block_size * localUv;
        }
    }
    // dirt
    else if (blockType == 2.0f) {
        // always side face
        finalUv = dirtSideUv + block_size * localUv;
    }
    // stone
    else if (blockType == 3.0f) {
        // always side face
        finalUv = stoneSideUv + block_size * localUv;
    }

    else if (blockType == 4.0f) {
        finalUv = bedrockSideUv + block_size * localUv;
    }
}

void main()
{
    uvCalc();  // Call uvCalc to update the texture coordinates

    gl_Position = projection * view * model * vec4(vPos, 1.0);
    TexCoord = finalUv;
}
