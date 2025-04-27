#include <iostream>
#include <cmath>
#define STB_PERLIN_IMPLEMENTATION
#include "./includes/STB/stb_perlin.h"
#define print(x) std::cout << x
#include <iomanip>

#define RESET   "\033[0m"
#define LIGHT_BLUE  "\033[38;5;81m"  // Light blue for water/low terrain
#define LIGHT_GREEN "\033[38;5;82m"  // Light green for grass/medium terrain
#define YELLOW      "\033[38;5;226m" // Yellow for higher plains
#define DARK_YELLOW "\033[38;5;130m" // Dark yellow for hilltops
#define ORANGE      "\033[38;5;214m" // Orange for mountainous terrain
#define BROWN       "\033[38;5;94m"  // Brown for very high terrain
#define DARK_BROWN  "\033[38;5;52m"  // Dark brown for extreme heights
#define WHITE       "\033[37m"       // White for snowy peaks
#define RESET_COLOR "\033[0m"

std::string noiseToChar(float h) {
    if (h >= 9.0f) return WHITE "■" RESET_COLOR;  // Snowy peak, high elevation
    if (h >= 8.0f) return DARK_BROWN "■" RESET_COLOR;  // Extreme heights
    if (h >= 7.0f) return BROWN "■" RESET_COLOR;   // Very high terrain
    if (h >= 6.0f) return ORANGE "■" RESET_COLOR;  // Mountainous terrain
    if (h >= 5.0f) return DARK_YELLOW "■" RESET_COLOR;  // Hilltops
    if (h >= 4.0f) return YELLOW "■" RESET_COLOR;  // High plains
    if (h >= 3.0f) return LIGHT_GREEN "■" RESET_COLOR; // Grassland
    if (h >= 2.0f) return LIGHT_BLUE "■" RESET_COLOR;  // Water or flat terrain
    return " ";  // Low terrain or flat land
}

int main() {
    int width = 100;
    int height = 100;
    int maxHeight = 10;

    float scale = 0.1f; // lower = smoother noise

    for (int x = 0; x < width; x++) {
        float nx = x * scale; // jump value in x for next pixel

        for (int y = 0; y < height; y++) {
            float ny = y * scale; // jump value in y for next pixel

            float noise = stb_perlin_noise3(nx, ny, 0.0f, 0, 0, 0);

            // Map the noise value to an ASCII character
            float normalized = (noise + 1.0f) / 2; // 0 - 1
            std::string terrainChar = noiseToChar(normalized * maxHeight);

            std::cout << terrainChar;
        }
        std::cout << '\n';
    }
}
