cmake_minimum_required(VERSION 3.10)
project(Learngl)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Add executable
add_executable(main 
    src/main.cpp 
    src/glad.c
)

# Include directories
target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Link libraries
target_link_libraries(main PRIVATE 
    OpenGL::GL 
    glfw
    SDL2::SDL2
    SDL2_mixer::SDL2_mixer
)
