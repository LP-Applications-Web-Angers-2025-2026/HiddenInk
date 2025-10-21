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
        interfaces/cli/non_interactif/fonctions_main.cpp
        stenographie/utils/histogramme/histogram.cpp
        stenographie/utils/histogramme/histogram.h
        stenographie/utils/histogramme/histogram.cpp
        stenographie/main_helpers.hpp
        stenographie/main_helpers.hpp
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        interfaces/cli/interactif/fonction_menu_interactif.cpp
        interfaces/cli/interactif/fonction_menu_interactif.h)
