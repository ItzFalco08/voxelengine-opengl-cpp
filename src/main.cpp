#include "main.hpp"

// basic stuff
#include <map>
#include <vector>

// multithreading
#include <atomic>
#include <thread>
#include <mutex>

// this code runs background musics
bool playBgm()
{
    std::cout << "playBgm called" << std::endl;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("SDL_Init AUDIO failed: %s\n", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return false;
    }

    // Figure out absolute path at runtime
    char *basePath = SDL_GetBasePath(); // e.g. "/home/user/mygame/bin/"
    std::string musicPath = std::string(basePath) + "../src/sounds/bgm.mp3";
    SDL_free(basePath);

    bgm = Mix_LoadMUS(musicPath.c_str());
    if (!bgm)
    {
        printf("Mix_LoadMUS failed at '%s': %s\n", musicPath.c_str(), Mix_GetError());
        return false;
    }

    // Start the music (loop forever)
    Mix_PlayMusic(bgm, -1);
    return true;
}

// blocktypes
enum BlockType
{
    AIR,     // 0
    GRASS,   // 1
    DIRT,    // 2
    STONE,   // 3
    BEDROCK, // 4
    LOG,     // 5
    LEAVES,  // 6
};

// shader class
Shader *shader;

// some variables
// Chunk size
const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 32; // 16 until we add caves via 3D noise
int renderDistance = 5;

float noise_scale = 0.05f;
static std::mutex coutMutex;
int maxHeight = 20;

// Chunk Class
// this si the core code, it generates the chunk and shows in the screen
class Chunk {
private:
    int initialX;
    int initialZ;
    int finalX;
    int finalZ;

    // all the blocks generated int eh chunk are stored in this 3d array
    int blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];

    std::atomic<bool> verticesLoaded{false};
    bool verticesUploaded = false;

    std::vector<float> vertices; // use array[CHUNKWIDTH*CHUNKWIDTH*CHUNKHEIGHT*36] for better memory management

    unsigned int VAO, VBO;


    enum FaceDirection
    {
        TOP,
        BOTTOM,
        FRONT,
        BACK,
        LEFT,
        RIGHT,
    };

    void setVertices()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // buffer data
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // vao attibutes

        // position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)0);

        // uv
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(3 * sizeof(float)));

        // face
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(5 * sizeof(float)));

        // blocktype
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(6 * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    int genRandomInt(int x, int z, int maxInt)
    {
        // randomize the seed with prime multiplication and bit manipulation
        int seed = x * 374761393 + z * 668265263;
        seed = (seed ^ (seed >> 13)) * 1274126177;
        seed = seed ^ (seed >> 16);

        // return the range (0 to maxInt(100))
        return abs(seed % maxInt + 1);
    }

    void genLeaves(int x, int y, int z)
    {
        if (y >= CHUNK_HEIGHT) return;

        for (int leafX = (x - 2); leafX <= (x + 2); leafX++) {
            if (leafX < 0 || leafX >= CHUNK_WIDTH) continue;
            blocks[leafX][y][z] = LEAVES;

            // std::lock_guard<std::mutex> lock(coutMutex);
            // std::cout << "Set leaf at: " << leafX << ", " << y << ", " << z << ", x: " << x << std::endl;
            // std::cout << "existance in blocks: " << (blocks[leafX][y][z] ? "true" : "false") << std::endl;
        }
    }


    void genTree(int x, int y, int z)
    {
        // make a tree at x,  + 1, z

        int trunkHeight = genRandomInt(x, z, 3) + 4;

        int localX = x - initialX;
        int localZ = z - initialZ;

        for (int treeY = 0; treeY <= trunkHeight; treeY++)
        {
            int targetY = y + treeY;

            if (targetY < CHUNK_HEIGHT)
            {
                blocks[localX][targetY][localZ] = LOG;
            }

            if (treeY == trunkHeight)
            {
                // gen leaves at x,y,z
                genLeaves(localX, targetY, localZ);
            }
        }
    }

    void genFeatures(int x, int z, int y)
    {
        // generate random int based on x,z
        int randomInt = genRandomInt(x, z, 1000); // return integer between 0 - 100

        if (randomInt < 6)
        {
            // generate tree 1% chance
            genTree(x, y, z);
        }
    }

    void genChunk()
    {
        for (int x = initialX; x <= finalX; x++)
        {
            float nx = x * noise_scale;
            for (int z = initialZ; z <= finalZ; z++)
            {
                float nz = z * noise_scale;

                // generate noise
                float noise = stb_perlin_noise3(nx, nz, 0.0f, 0, 0, 0); // -1 -> +1

                float normalized = (noise + 1) / 2.0f;      // 0 -> 1
                int height = (int)(normalized * maxHeight); // 0 -> maxHeight(10)

                int localX = x - initialX;
                int localZ = z - initialZ;

                int terrainY = height + (CHUNK_HEIGHT - maxHeight);

                for (int y = 0; y < CHUNK_HEIGHT; y++)
                {

                    // top layer (y = terrainY) is grass
                    if (y == terrainY)
                    {
                        blocks[localX][y][localZ] = GRASS;
                    }
                    // then dirt
                    else if (y < terrainY && y >= terrainY - 3)
                    {
                        blocks[localX][y][localZ] = DIRT;
                    }
                    else if (y < terrainY - 3 && y > 0)
                    {
                        blocks[localX][y][localZ] = STONE;
                    }
                    else if (y == 0)
                    {
                        blocks[localX][y][localZ] = BEDROCK;
                    }
                    else
                    {
                        blocks[localX][y][localZ] = AIR;
                    }
                }

                // NOTE: calling it after terrrain generation because terrain generation will set rest of the blocks to air
                genFeatures(x, z, terrainY);
            }
        }
    }

    bool isAir(int x, int y, int z)
    {
        // first cheack if x,y,z is valid
        if (x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT)
        {
            return true;
        }
        // if the block inside chunk, cheack if its assigned as air or not
        if (blocks[x][y][z] == AIR)
        {
            return true;
        }

        return false;
    }

    // x, y, z, u, v, face, type
    float localPos[6][6][3] = {
        {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}, // TOP
        {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // BOTTOM
        {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}, // FRONT
        {{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // BACK
        {{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}, // LEFT
        {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}  // RIGHT
    };

    // for vertex in each face
    float localUv[6][2] = {
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };

    void addFace(FaceDirection face, int x, int y, int z, int type)
    {
        // Debug: Print when adding leaf faces
        // if (type == LEAVES) {
        //     std::lock_guard<std::mutex> lock(coutMutex);
        //     std::cout << "Adding leaf face: " << x << ", " << y << ", " << z << ", face: " << face << std::endl;
        // }
        
        // add vace vertex to verticies vector
        for (int vertex = 0; vertex < 6; vertex++)
        {
            float* pos = localPos[face][vertex];
            float* uv = localUv[vertex];

            // Convert local coordinates to world coordinates
            vertices.push_back(pos[0] + x + initialX);
            vertices.push_back(pos[1] + y);
            vertices.push_back(pos[2] + z + initialZ);

            vertices.push_back(uv[0]);
            vertices.push_back(uv[1]);

            vertices.push_back(static_cast<float>(face)); // for face of block texture
            vertices.push_back(static_cast<float>(type)); // for block texture
        }
    }

    void buildMesh()
    {
        vertices.clear();

        for (int x = 0; x < CHUNK_WIDTH; x++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                for (int z = 0; z < CHUNK_WIDTH; z++)
                {
                    int type = blocks[x][y][z];
                    if (type == AIR)
                        continue;

                    // add each exposed face if block is not air
                    if (isAir(x, y + 1, z))
                        addFace(TOP, x, y, z, type); // add top face
                    if (isAir(x, y - 1, z))
                        addFace(BOTTOM, x, y, z, type); // add bottom face
                    if (isAir(x + 1, y, z))
                        addFace(RIGHT, x, y, z, type); // add right face
                    if (isAir(x - 1, y, z))
                        addFace(LEFT, x, y, z, type); // add left face
                    if (isAir(x, y, z + 1))
                        addFace(FRONT, x, y, z, type); // add front face
                    if (isAir(x, y, z - 1))
                        addFace(BACK, x, y, z, type); // add back face
                }
            }
        }

        // print(logCount);
    }

    void buildVertices()
    {
        genChunk();  // set blocks
        buildMesh(); // build vertices array from blocks
        verticesLoaded = true;
    }

    void uploadToGpu()
    {
        if (verticesLoaded && !verticesUploaded)
        {
            setVertices();
            verticesUploaded = true;
        }
    }


public:
    ~Chunk()
    {
        glDeleteBuffers(1, &VBO);
    }

    Chunk(int x, int z)
    {
        // generate form (16x, 16z) to (16x + 15, 16z + 15)
        initialX = x * 16;
        initialZ = z * 16;
        finalX = initialX + 15;
        finalZ = initialZ + 15;

        std::thread chunkGen(&Chunk::buildVertices, this);
        chunkGen.detach();
    }

    void renderChunk()
    {
        uploadToGpu(); // uploads vertices to GPU if its loaded
        if (verticesUploaded)
        {
            // render all vertex from vertices
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 7);
        }
    }
};

std::map<std::pair<int, int>, std::unique_ptr<Chunk>> chunks;

void initChunks()
{
    int fromX = -renderDistance;
    int fromZ = -renderDistance;
    int toX = renderDistance;
    int toZ = renderDistance;

    for (int x = fromX; x <= toX; x++)
    {
        for (int z = fromZ; z <= toZ; z++)
        {
            chunks.emplace(std::make_pair(x, z), std::make_unique<Chunk>(x, z));
        }
    }
}

void renderChunks()
{
    // render chunks map
    for (auto itr = chunks.begin(); itr != chunks.end(); itr++)
    {
        (itr->second)->renderChunk();
    }
}

// nuklear context
struct nk_context *ctx;

int initNuklear(GLFWwindow *window)
{
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    if (!ctx)
    {
        std::cerr << "Failed to initialize Nuklear context" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    if (!atlas)
    {
        std::cerr << "Failed to begin font stash" << std::endl;
        nk_glfw3_shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    nk_glfw3_font_stash_end();
    return 0;
}

void drawDebugMenu()
{
    int nuklear_window = nk_begin(ctx, "Debug Menu", nk_rect(10, 10, 220, 200), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MINIMIZABLE);

    if (nuklear_window)
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "Camera Position:", NK_TEXT_LEFT);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "X: %.2f Y: %.2f Z: %.2f", camPos.x, camPos.y, camPos.z);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        nk_label(ctx, "player Chunk Pos:", NK_TEXT_LEFT);
        snprintf(buffer, sizeof(buffer), "X: %.2f Z: %.2f", playerChunkPos.x, playerChunkPos.y);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        nk_label(ctx, "FPS:", NK_TEXT_LEFT);
        snprintf(buffer, sizeof(buffer), "%.2f", 1.0f / deltaTime);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        float noiseMin = 0.0f;
        float noiseMax = 1.0f;
        float noiseStep = 0.05f;

        snprintf(buffer, sizeof(buffer), "Noise Scale: %.2f", noise_scale);

        nk_label(ctx, buffer, NK_TEXT_LEFT);
        nk_slider_float(ctx, noiseMin, &noise_scale, noiseMax, noiseStep);

        int minMaxH = 5;
        int maxMaxH = 50;
        int stepMaxH = 1;

        snprintf(buffer, sizeof(buffer), "Max Height: %i", maxHeight);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        nk_slider_int(ctx, minMaxH, &maxHeight, maxMaxH, stepMaxH);

        int minRenderDistance = 1;
        int maxRenderDistance = 30;
        int stepRenderDistance = 1;

        snprintf(buffer, sizeof(buffer), "Render Distance: %i", renderDistance);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        nk_slider_int(ctx, minRenderDistance, &renderDistance, maxRenderDistance, stepRenderDistance);

        if (nk_button_label(ctx, "Reload Chunks"))
        {
            chunks.clear();
            initChunks();
        }
    }

    nk_end(ctx);
}

void initialization(GLFWwindow *window)
{
    shader->use();

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

    // enables alpha to work (currently not working even with this code)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set the texture to fragment shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId1);
    shader->setInt("curTexture", 0);
    glBindVertexArray(cubeVAO);

    playBgm();
    initNuklear(window);
    initChunks();
}

int main()
{
        // all this code is start loop
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Minecraft C++", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);

    // ✅ GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    shader = new Shader("../shaders/shader.vert", "../shaders/shader.frag");

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    initialization(window);

    // like unity, update loop
    // game loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glfwPollEvents();
        display();
        glfwSwapBuffers(window);
    }

    nk_glfw3_shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

bool cursorLocked = true;
bool escPressedLastFrame = false; // define this globally or persistently

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camPos += camFront * deltaTime * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camPos -= camFront * deltaTime * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camPos += camRight * deltaTime * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camPos -= camRight * deltaTime * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camPos += up * deltaTime * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camPos -= up * deltaTime * camSpeed;
    }

    bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (!escPressedLastFrame && escPressed)
    {
        if (cursorLocked)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursorPosCallback(window, 0);
            cursorLocked = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(window, mouse_callback);
            cursorLocked = true;
        }
    }

    escPressedLastFrame = escPressed;
}

unsigned int loadTexture(const char *texLocation)
{
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
    unsigned char *data = stbi_load(texLocation, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load texture: " << texLocation << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}

void initTexture()
{
    stbi_set_flip_vertically_on_load(true); // Flip image

    // Load textures without specifying texture units
    textureId1 = loadTexture("../src/textures/atlas.png");

    if (!textureId1)
    {
        std::cerr << "Failed to load one or more textures!" << std::endl;
    }
}

// send matrix to vertex shader
void initMatrixLocations()
{
    modelLoc = glGetUniformLocation(shader->programId, "model");
    viewLoc = glGetUniformLocation(shader->programId, "view");
    projectionLoc = glGetUniformLocation(shader->programId, "projection");
}

void setMatrix()
{
    view = glm::lookAt(
        camPos,
        camPos + camFront,
        up // not camUp
    );

    camRight = glm::normalize(glm::cross(camFront, up));

    projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void calcDeltaTime()
{
    float curFrameTime = glfwGetTime();
    deltaTime = curFrameTime - lastFrameTime;
    lastFrameTime = curFrameTime;
}

double yawAngle;
double pitchAngle;
bool firstMouse = true;

// playercirduates -> translate value of view matrix

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
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

    if (pitchAngle > 89.0f)
        pitchAngle = 89.0f;
    if (pitchAngle < -89.0f)
        pitchAngle = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
    direction.y = sin(glm::radians(pitchAngle));
    direction.z = sin(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
    camFront = glm::normalize(direction);
};

void handleChunks()
{
    int fromX = playerChunkPos.x - renderDistance;
    int fromZ = playerChunkPos.y - renderDistance;
    int toX = playerChunkPos.x + renderDistance;
    int toZ = playerChunkPos.y + renderDistance;

    // 1. loop through range to load chunks
    for (int x = fromX; x <= toX; x++)
    {
        for (int z = fromZ; z <= toZ; z++)
        {
            std::pair<int, int> pos = {x, z};

            if (chunks.find(pos) == chunks.end())
            {
                // it do not exists
                chunks.emplace(std::make_pair(x, z), std::make_unique<Chunk>(x, z));
            }
        }
    }

    // 2. loop through map to unload chunks
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        int x = it->first.first;
        int z = it->first.second;

        if (x < fromX || x > toX || z < fromZ || z > toZ)
        {
            auto nextIt = std::next(it);
            chunks.erase(it); // it makes it null so it++ dont works
            it = nextIt;
        }
        else
        {
            it++;
        }
    }
}

void updatePlayerChunkPos()
{
    glm::vec2 newPos = glm::floor(glm::vec2(camPos.x, camPos.z) / 16.0f);

    if (playerChunkPos == newPos)
    {
        return;
    }
    else
    {
        // user entered new chunk
        playerChunkPos = newPos;
        // recalculate chunks map
        handleChunks();
    }
}

void restoreState()
{
    // Restore critical OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBindVertexArray(cubeVAO);
    shader->use();
    glm::mat4 model = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Restore texture binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId1);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state using
    calcDeltaTime();
    setMatrix();

    updatePlayerChunkPos();
    renderChunks();

    nk_glfw3_new_frame();
    drawDebugMenu();
    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
    // 512 * 1024   // max vertex buffer size = 512 KB
    // 128 * 1024   // max element buffer size = 128 KB

    restoreState();
}