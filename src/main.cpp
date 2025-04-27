#include "main.hpp"

glm::mat4 view;
glm::mat4 projection;

int modelLoc;
int viewLoc;
int projectionLoc;

// Defining constants for block types
const int AIR = 0;
const int GRASS = 1;
const int DIRT = 2;
const int STONE = 3;

// Chunk size
const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 16; // 16 until we add caves via 3D noise

Shader* shader;

// Chunk Class
class Chunk{
public:
    int initialX;
    int initialZ;
    int finalX;
    int finalZ;

    float scale = 0.1f;
    int maxHeight = 10;
    
    Chunk(int x, int z) {
        // generate form (16x, 16z) to (16x + 15, 16z + 15)     
        initialX = x * 16;
        initialZ = z * 16;
        finalX = x * 16 + 15;
        finalZ = z * 16 + 15;
    }

    void genChunk() {
        for (int x = initialX; x <= finalX; x++) {
            float nx = x * scale;
            for (int z = initialZ; z <= finalZ ; z++) {
                float nz = z * scale;

                // generate noise
                float noise = stb_perlin_noise3(nx, nz, 0.0f, 0, 0, 0); // -1 -> +1

                float normalized = (noise + 1) / 2.0f; // 0 -> 1
                int height = (int) (normalized * maxHeight); // 0 -> maxHeight(10)
                
                // render block upto x, height, z 
                for (int y = -CHUNK_HEIGHT; y <= height; y++) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                    if (y == height) {
                        shader->setInt("blockType", GRASS); // 1 = grass block

                    } else if (y < height && y >= height - 3) {
                        shader->setInt("blockType", DIRT); // 2 = dirt block
                        
                    } else if (y < height - 3) {
                        shader->setInt("blockType", STONE); // 3 = stone block
                    }

                    glDrawArrays(GL_TRIANGLES, 0, 36);

                }
            }
        }
    }
};

void initialization(GLFWwindow* window) {
    
    // initialize shader
    shaderProgramId = shader->programId;
    shader->use();

    initCube();

    // load textures
    initTexture();

    // enable z-buffer
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // init matrix location
    initMatrixLocations();

    glClearColor(110.0f / 255.0f, 177.0f / 255.0f, 255.0f / 255.0f, 1.0f); // background color

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    glfwSetCursorPosCallback(window, mouse_callback); 


    // set the texture to fragment shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId1);
    shader->setInt("curTexture", 0); 

    glBindVertexArray(squareVAO);

}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft C++", NULL, NULL); 

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);

    // ✅ GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    shader = new Shader("../shaders/shader.vert", "../shaders/shader.frag");

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    initialization(window);

    // game loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glfwPollEvents();
        display();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camPos += camFront * deltaTime * camSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camPos -= camFront * deltaTime * camSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camPos += camRight * deltaTime * camSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camPos -= camRight * deltaTime * camSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camPos += up * deltaTime * camSpeed;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camPos -= up * deltaTime * camSpeed;
    }
}

unsigned int loadTexture(const char* texLocation) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind to modify settings

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load image data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texLocation, &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << texLocation << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

void initTexture() {
    stbi_set_flip_vertically_on_load(true); // Flip image

    // Load textures without specifying texture units
    textureId1 = loadTexture("../src/textures/texture.png");

    if (!textureId1 ) {
        std::cerr << "Failed to load one or more textures!" << std::endl;
    }
}

void initCube() {
    float vertices[] = {
        // Positions          FaceID  U     V
        // Top Face (faceId = 0)
        -0.5f, 0.5f, -0.5f,   0,     0.0f, 0.0f,
        0.5f, 0.5f, -0.5f,    0,     1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     0,     1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,   0,     0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     0,     1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,    0,     0.0f, 1.0f,

        // Bottom Face (faceId = 1)
        -0.5f, -0.5f, -0.5f,  1,     0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1,     1.0f, 0.0f,
        0.5f, -0.5f, 0.5f,    1,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1,     0.0f, 0.0f,
        0.5f, -0.5f, 0.5f,    1,     1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,   1,     0.0f, 1.0f,

        // All side faces (faceId = 2)
        // Front Face
        -0.5f, -0.5f, 0.5f,   2,     0.0f, 0.0f,
        0.5f, -0.5f, 0.5f,     2,     1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,      2,     1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,    2,     0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,      2,     1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f,     2,     0.0f, 1.0f,

        // Back Face
        -0.5f, -0.5f, -0.5f,  2,     0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   2,     1.0f, 0.0f,
        0.5f, 0.5f, -0.5f,     2,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   2,     0.0f, 0.0f,
        0.5f, 0.5f, -0.5f,     2,     1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,    2,     0.0f, 1.0f,

        // Left Face
        -0.5f, -0.5f, -0.5f,  2,     0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,   2,     1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,    2,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  2,     0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f,    2,     1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f,   2,     0.0f, 1.0f,

        // Right Face
        0.5f, -0.5f, -0.5f,   2,     0.0f, 0.0f,
        0.5f, -0.5f, 0.5f,    2,     1.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     2,     1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   2,     0.0f, 0.0f,
        0.5f, 0.5f, 0.5f,     2,     1.0f, 1.0f,
        0.5f, 0.5f, -0.5f,    2,     0.0f, 1.0f
    };

    // Rest of the initCube() function remains the same...
    glGenBuffers(1, &squareVBO);
    glGenVertexArrays(1, &squareVAO);

    glBindVertexArray(squareVAO);
    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Face ID attribute
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


// send matrix to vertex shader
void initMatrixLocations() {
    modelLoc = glGetUniformLocation(shaderProgramId, "model");
    viewLoc = glGetUniformLocation(shaderProgramId, "view");
    projectionLoc = glGetUniformLocation(shaderProgramId, "projection");
}

void setMatrix() {
    view = glm::lookAt(
        camPos,
        camPos + camFront,
        up // not camUp
    );

    camRight = glm::normalize(glm::cross(camFront, up));

    projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void calcDeltaTime() {
    float curFrameTime = glfwGetTime();
    deltaTime = curFrameTime - lastFrameTime;
    lastFrameTime = curFrameTime;
}

double yawAngle;
double pitchAngle;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos; 
        firstMouse = false;
        return;
    }

    xOffset = xpos - lastX;
    yOffset = lastY - ypos; // y goes from top to bottom
    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yawAngle += xOffset;
    pitchAngle += yOffset;

    if(pitchAngle > 89.0f)
        pitchAngle = 89.0f;
    if(pitchAngle < -89.0f)
        pitchAngle = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
    direction.y = sin(glm::radians(pitchAngle));
    direction.z = sin(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
    camFront = glm::normalize(direction);

};

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state using
    calcDeltaTime();
    setMatrix();

    Chunk chunk(0, 0);
    chunk.genChunk();
}