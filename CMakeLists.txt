cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 14)

# Include headers
include_directories(includes)

# Path to static libs
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# 🛠️ Force linker to use `main()` instead of `WinMain`
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-subsystem,console")

# Add executable
add_executable(minecraft
    src/main.cpp
    src/glad.c
)

# Not a Windows GUI app (uses main)
set_target_properties(minecraft PROPERTIES WIN32_EXECUTABLE OFF)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Link everything needed
target_link_libraries(minecraft
    ${OPENGL_gl_LIBRARY}
    ${LIB_DIR}/libSDL2.a           # ✅ Now included
    ${LIB_DIR}/libSDL2_mixer.a
    ${LIB_DIR}/libglfw3.a
    gdi32
    user32
    shell32
    kernel32
    setupapi
    winmm
    imm32
    version
)
