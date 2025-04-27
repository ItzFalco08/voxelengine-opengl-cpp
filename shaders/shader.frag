#version 330 core

out vec4 FragColor;

flat in int vFaceId;

uniform sampler2D curTexture;
uniform int blockType;

const float tileSize = 1.0 / 8.0; // 128px texture with 16px blocks (8x8 grid)

in vec2 vUV;

#define GRASS 1
#define DIRT 2
#define STONE 3

vec2 getTileOffset(int blockType, int faceId) {
    if (blockType == GRASS) {
        if (faceId == 0) return vec2(1.0, 7.0) * tileSize;    // Grass Top (column 1, row 7)
        if (faceId == 1) return vec2(2.0, 7.0) * tileSize;    // Dirt Bottom (column 2, row 7)
        return vec2(0.0, 7.0) * tileSize;                     // Grass Side (column 0, row 7)
    }
    else if (blockType == DIRT) {
        return vec2(2.0, 7.0) * tileSize;    // All dirt faces use dirt texture
    }
    else if (blockType == STONE) {
        return vec2(3.0, 7.0) * tileSize;    // All stone faces use stone texture
    }
    return vec2(0.0);
}

void main() {
    vec2 tileOffset = getTileOffset(blockType, vFaceId);
    vec2 finalUV = tileOffset + (vUV * tileSize);
    FragColor = texture(curTexture, finalUV);
}