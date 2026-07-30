
cmake_minimum_required(VERSION 3.31)

#set...
set(CMAKE_TOOLCHAIN_FILE "d:/VS/Product/VC/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

project(main)

#include...
include(GenerateExportHeader)

#find_package...
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

#add_executable...
add_library(veil SHARED
    src/lib/window.cpp
    src/lib/shader.cpp
    src/lib/glcamera.cpp
    src/lib/mesh.cpp
    src/lib/model.cpp
    src/lib/cachemgr.cpp
    src/lib/logmgr.cpp
    src/lib/glrenderer.cpp
)
add_executable(main WIN32 
    src/app/main.cpp
)

#target_link_libraries...
target_link_libraries(veil PUBLIC 
    glad::glad 
    glfw
    glm::glm
    assimp::assimp
)
target_link_libraries(main PRIVATE
    veil
)

generate_export_header(veil
    BASE_NAME veil
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/include/veil_export.h
)

target_link_options(main PRIVATE 
    "/ENTRY:mainCRTStartup"
    "/SUBSYSTEM:WINDOWS"
)
target_include_directories(veil PUBLIC 
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${Stb_INCLUDE_DIR}
)