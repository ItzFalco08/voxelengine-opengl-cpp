cmake_minimum_required(VERSION 3.10)
project(Learngl)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Add executable
add_executable(main 
    src/main.cpp 
    src/glad.c
    # Don't add stb_image.h here - it's a header-only library
)

# Include directories
target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

# Link libraries
target_link_libraries(main PRIVATE 
    OpenGL::GL 
    glfw
)