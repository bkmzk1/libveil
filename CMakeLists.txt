cmake_minimum_required(VERSION 3.31)
project(libveil VERSION 1.4.1 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BINARY_DIR_BIN ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(glfw3  CONFIG REQUIRED)
find_package(glm    CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_library(
    glad 
    STATIC vendor/glad/src/glad.c
)
set_target_properties(
    glad 
    PROPERTIES 
        POSITION_INDEPENDENT_CODE ON
)
target_include_directories(
    glad 
    PUBLIC vendor/glad/include
)

add_library(
    stb 
    INTERFACE
)
target_include_directories(
    stb 
    INTERFACE vendor/stb
)

add_library(
    veil
    SHARED src/lib/window.cpp
           src/lib/shader.cpp
           src/lib/glcamera.cpp 
           src/lib/model.cpp
           src/lib/cache.cpp
           src/lib/log.cpp
           src/lib/renderer.cpp
           src/lib/drawable.cpp
)

target_compile_definitions(
    veil
    PRIVATE VEIL_EXPORTS
)

target_link_libraries(
    veil
    PUBLIC  glm::glm
    PRIVATE glad stb glfw assimp::assimp
)

generate_export_header(veil BASE_NAME veil)

target_include_directories(
    veil PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS veil 
    EXPORT  veilTargets
)
install(
    DIRECTORY   include/ vendor/stb/ vendor/glad/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    FILES       ${CMAKE_CURRENT_BINARY_DIR}/veil_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT      veilTargets
    FILE        veilTargets.cmake
    NAMESPACE   veil::
    DESTINATION ${CMAKE_INSTALL_DATADIR}/veil
)

configure_package_config_file(
    cmake/veilConfig.cmake.in 
    veilConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/veil
)
install(
    FILES       ${CMAKE_CURRENT_BINARY_DIR}/veilConfig.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/veil
)