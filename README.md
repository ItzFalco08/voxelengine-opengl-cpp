## How to Run the Game?

1. **Clone the repository**
   ```bash
   git clone repository_url
   ```

2. **Install OpenGL dependencies**

   ### 🔧 Linux (Debian/Ubuntu)
   ```bash
   sudo apt install libglfw3-dev
   ```

   ### 🪟 Windows (with vcpkg)
   ```bash
   vcpkg install glfw3
   ```

   ### 🍎 macOS (with Homebrew)
   ```bash
   brew install glfw
   ```

3. **Install `g++` and `cmake`**  
   Make sure you have both installed. You can install them via:

   #### Linux
   ```bash
   sudo apt install g++ cmake
   ```

   #### macOS
   ```bash
   brew install gcc cmake
   ```

   #### Windows  
   Use [MinGW](https://www.mingw-w64.org/) or install via Chocolatey:
   ```bash
   choco install mingw cmake
   ```

4. **Build and run the game**
   ```bash
   cd build
   cmake ..
   make
   ./main
   ```

