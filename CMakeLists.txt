cmake_minimum_required(VERSION 3.20)
project(HiddenInk VERSION 1.0.0 LANGUAGES CXX)

# Configuration du C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de compilation
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Répertoires d'inclusion
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/stegano
    ${PROJECT_SOURCE_DIR}/include/analysis
    ${PROJECT_SOURCE_DIR}/include/external
)

# Fichiers source
set(SOURCES
    src/main.cpp
    src/stb_impl.cpp
)

# Création de l'exécutable
add_executable(HiddenInk ${SOURCES})

# Propriétés de l'exécutable
set_target_properties(HiddenInk PROPERTIES
    OUTPUT_NAME "hiddenink"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Message de confirmation
message(STATUS "HiddenInk v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
