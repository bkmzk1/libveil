
## A wrapper for GLFW, OpenGL and Assimp with caching.

![Language](https://img.shields.io/badge/language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Fedora%20Linux-51A2DA?logo=fedora&logoColor=white)
![License](https://img.shields.io/badge/license-Unlicense-blue)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

> [!IMPORTANT]
> Development and testing currently target Fedora 44 exclusively. Other operating systems (Windows, BSD, macOS, etc.) are untested and unsupported at this time — compatibility is not guaranteed

### Building from source

#### Prerequisites
- CMake
- GLFW3
- GLM
- Assimp

#### Compilation steps
Execute the following commands from the project root directory:
```bash
# Generate the build configuration
cmake -B build -S . 

# Compile the binaries in Release mode
cmake --build build --config Release  

# (Optional) Install headers and binaries to a target directory
cmake --install build
```

### Integration and usage
Once installed, you can consume the library in downstream projects via CMake's `find_package` architecture. The snippet below demonstrates how to link the library and propagate its dependency graph.

```cmake
find_package(veil CONFIG REQUIRED)

# Link against veil
target_link_libraries(main 
    PRIVATE
    veil::veil
)
```
