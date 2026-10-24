
## A wrapper for GLFW, OpenGL and Assimp with caching.

![Language](https://img.shields.io/badge/language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Linux-27272a?logo=linux&logoColor=white)
![Build](https://img.shields.io/badge/build-passing-brightgreen)
![License](https://img.shields.io/badge/license-Unlicense-blue)

> [!IMPORTANT]
> Development and testing currently target Linux exclusively. Other operating systems (Windows, BSD, macOS, etc.) are untested and unsupported at this time — compatibility is not guaranteed

### Building from source

#### Prerequisites
- CMake

#### Dependencies
- GLFW3
- GLM
- Assimp

```sh
# Installing dependencies on Arch Linux
sudo pacman -S glfw glm assimp
```

#### Compilation steps
Execute the following commands from the project root directory:
```sh
# Generate the build configuration
cmake -B build -S . 

# Compile the binaries in Release mode
cmake --build build --config Release  

# (Optional) Install headers and binaries to a target directory
sudo cmake --install build
```

### Integration and usage
Once installed, you can consume the library in downstream projects via CMake's `find_package` architecture. The snippet below demonstrates it:

```cmake
# Locate veil package
find_package(veil CONFIG REQUIRED)

# Link against veil
target_link_libraries(main 
    PRIVATE
    veil::veil
)
```
