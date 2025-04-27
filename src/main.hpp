#pragma once

#include <iostream>
#include <cstring>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream> // file manipulator module
#include "utils/utils.cpp"
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

// shader program
unsigned int shaderProgramId;

// vertex buffer id
unsigned int squareVAO, squareVBO, squareEBO;
unsigned int triangleVAO, triangleVBO, triangleEBO;
unsigned int curTexLocation;

glm::mat4 trans;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camRight = glm::normalize(glm::cross(camFront, up));

float xOffset;
float yOffset;
float lastX;
float lastY;
float sensitivity = 0.1f;
float camSpeed = 10.0f;

void display();
void processInput(GLFWwindow* window);
void initCube();
void initTexture();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void initMatrixLocations();

