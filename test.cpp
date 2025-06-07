// Multithreading example
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

class Chunk {
private:
    std::atomic<bool> isVertexLoaded = false;

    int blocks[1];
    std::vector <float> vertices;
 
    void genChunk() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        blocks[0] = 1;
    }

    void buildVertices() {
        // only work if there is blocks
        if(blocks[0]) {
            vertices = {1.0f};
            isVertexLoaded = true;
        } else {
            std::cout << "Vertices generation failed: no blocks data";
        }
    }

public:

    void prepairVertices() {
        genChunk();
        buildVertices();
    }

    Chunk() {
        std::thread vertGen(&Chunk::prepairVertices, this);
        vertGen.detach();
    }

    void renderChunk() {
        if (isVertexLoaded) {
            std::cout << "chunk rendered" << std::endl;
        } else {
            std::cout << "chunk is loading...." << std::endl;
        }
    }
};

std::vector <std::unique_ptr<Chunk>> chunks;

void createChunks();
void renderChunks();

int main() {
    createChunks();
    renderChunks();
    return 1;
}

void createChunks() {
    chunks.push_back(std::make_unique<Chunk>());
    chunks.push_back(std::make_unique<Chunk>());
    chunks.push_back(std::make_unique<Chunk>());
}

void renderChunks() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (auto itr = chunks.begin(); itr != chunks.end(); ++itr) {
            (*itr)->renderChunk();
        }
    }
}
