#pragma once
#define SDL_MAIN_HANDLED


#include <iostream>
#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream> // file manipulator module
#include <cmath>
#include "Shader/Shader.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_PERLIN_IMPLEMENTATION
#include "STB/stb_perlin.h"

// glm (opengl Mathematic) library
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// audio system
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Nuklear includes with implementation
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

// texture object id
unsigned int textureId1;
unsigned int textureId2;

// for ease
#define print(x) std::cout << x << std::endl

// default screen dimensions
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 450;

// shaders
std::string vertexSource;
std::string fragmentSource;

// vertex buffer id
unsigned int cubeVAO, cubeVBO;
unsigned int triangleVAO, triangleVBO, triangleEBO;
unsigned int curTexLocation;

// matrix locations
int modelLoc;
int viewLoc;
int projectionLoc;

// matrix
glm::mat4 trans;
glm::mat4 view;
glm::mat4 projection;

// delta time
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

// camera vectors
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camRight = glm::normalize(glm::cross(camFront, up));

// camera variables
float xOffset;
float yOffset;
float lastX;
float lastY;
float sensitivity = 0.1f;
float camSpeed = 10.0f;

// player chunk position
glm::vec2 playerChunkPos = glm::vec2(0.0f, 0.0f); // x-> x , y-> z

// background music file pointer
Mix_Music* bgm = nullptr;

// helper functions
void display();
void processInput(GLFWwindow* window);
void initCube();
void initTexture();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void initMatrixLocations();
