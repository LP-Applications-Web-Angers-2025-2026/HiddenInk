cmake_minimum_required(VERSION 4.0)
project(HiddenInk)

set(CMAKE_CXX_STANDARD 20)

add_executable(HiddenInk main.cpp
        stenographie/BMP/bmp_convert.cpp
        stenographie/BMP/bmp_Recuperation.cpp
        stenographie/BMP/bmp_Recuperation.h
        stenographie/utils/utils_bin.cpp
        stenographie/utils/utils_bin.h
        stenographie/utils/external/stb_impl.cpp
        stenographie/PNG/fonction_menu.cpp
        stenographie/PNG/fonction_menu.h
        stenographie/utils/external/stb_image_write.h
        stenographie/utils/encrypt/encrypt.h
        stenographie/utils/encrypt/encrypt.cpp)
