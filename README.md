
## A wrapper for GLFW, OpenGL and Assimp with caching.

![Language](https://img.shields.io/badge/language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Windows%2010%20x64-0078D6?logo=windows&logoColor=white)
![License](https://img.shields.io/badge/license-Unlicense-blue)
![Build](https://img.shields.io/badge/build-passing-brightgreen)

> [!IMPORTANT]
> Development and testing currently target Windows 10 x64 exclusively. Other operating systems (Linux, BSD, macOS, etc.) are untested and unsupported at this time — compatibility is not guaranteed

### Building from source

#### Prerequisites
- CMake
- vcpkg package manager
By default, the build system generates a dynamic library (.dll) with all third-party dependencies monolithically linked into the binary.

#### Compilation steps
Execute the following commands from the project root directory:
```bash
# Generate the build configuration via CMake using the vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="<path/to/vcpkg>/scripts/buildsystems/vcpkg.cmake"

# Compile the binaries in Release mode
cmake --build build --config Release  

# (Optional) Install headers and binaries to a target directory
cmake --install build --config Release --prefix "<path/to/destination>"
```

### Integration and usage
Once installed, you can consume the library in downstream projects via CMake's `find_package` architecture. The snippet below demonstrates how to link the library and propagate its dependency graph.

```cmake
# Configure the package lookup paths
set(CMAKE_PREFIX_PATH "<path/of/installation>")
set(CMAKE_TOOLCHAIN_FILE "<path/to/vcpkg.cmake>")

# Resolve veil's required headers
find_package(glad   CONFIG REQUIRED)
find_package(glad   CONFIG REQUIRED)
find_package(glfw3  CONFIG REQUIRED)
find_package(glm    CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(veil   CONFIG REQUIRED)

# Fetch the headers
target_include_directories(main PRIVATE 
    $<TARGET_PROPERTY:glad::glad,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:assimp::assimp,INTERFACE_INCLUDE_DIRECTORIES>
)

# Link against veil
target_link_libraries(main 
    PRIVATE
    veil::veil
)

# Post-build step: Copy the dynamic library to the executable directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:veil::veil>
    $<TARGET_FILE_DIR:myapp>
)
```