cmake_minimum_required(VERSION 3.16)
project(HiddenInk_GUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# GUI Application
add_executable(HiddenInk_GUI
    main_gui.cpp
    mainwindow.cpp
    mainwindow.h
)

# Link Qt libraries
target_link_libraries(HiddenInk_GUI
    Qt6::Core
    Qt6::Widgets
)

# Set Windows subsystem for release builds (no console window)
if(WIN32)
    set_target_properties(HiddenInk_GUI PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Installation rules
install(TARGETS HiddenInk_GUI
    RUNTIME DESTINATION bin
)
