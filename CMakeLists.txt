cmake_minimum_required(VERSION 4.0)
project(HiddenInk)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HiddenInk main.cpp
        stenographie/BMP/bmp_hide.cpp
        stenographie/utils/utils_bin.cpp
        stenographie/utils/utils_bin.h
        stenographie/utils/external/stb_impl.cpp
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        interfaces/cli/interactif/fonction_menu_interactif.h
        stenographie/PNG/png_hide_text.cpp
        stenographie/PNG/png_hide_text.h
        stenographie/PNG/png_hide_image.cpp
        stenographie/PNG/png_hide_image.h
        stenographie/PNG/png_extract.cpp
        stenographie/PNG/png_extract.h
        stenographie/utils/external/stb_image_write.h
        stenographie/utils/encrypt/encrypt.h
        stenographie/utils/encrypt/encrypt.cpp
        interfaces/cli/non_interactif/fonction_menu_non_interactif.cpp
        stenographie/utils/histogramme/histogram.cpp
        stenographie/utils/histogramme/histogram.h
        stenographie/utils/histogramme/histogram.cpp
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        interfaces/cli/interactif/fonction_menu_interactif.h
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        stenographie/BMP/bmp_hide.cpp
        stenographie/BMP/bmp_extract.cpp
        stenographie/BMP/bmp_hide.cpp)

if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif ()
