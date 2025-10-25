QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Disable console window on Windows in release mode
CONFIG(release, debug|release): CONFIG += windows

TARGET = HiddenInk_GUI
TEMPLATE = app

# Source files
SOURCES += \
    main_gui.cpp \
    mainwindow.cpp \
    ../stenographie/PNG/png_hide_text.cpp \
    ../stenographie/PNG/png_hide_image.cpp \
    ../stenographie/PNG/png_extract.cpp \
    ../stenographie/utils/external/stb_impl.cpp \
    ../stenographie/utils/histogramme/histogram.cpp

# Header files
HEADERS += \
    mainwindow.h \
    ../stenographie/PNG/png_hide_text.h \
    ../stenographie/PNG/png_hide_image.h \
    ../stenographie/PNG/png_extract.h \
    ../stenographie/utils/stegano/stegano_text.hpp \
    ../stenographie/utils/stegano/stegano_imageinimage.hpp \
    ../stenographie/utils/analysis/image_analysis.hpp \
    ../stenographie/utils/external/stb_image.h \
    ../stenographie/utils/external/stb_image_write.h \
    ../stenographie/utils/histogramme/histogram.h

# Include paths
INCLUDEPATH += \
    ../stenographie \
    ../stenographie/utils \
    ../stenographie/utils/external

# Compiler flags
QMAKE_CXXFLAGS += -std=c++17

# Windows specific
# win32 {
#     RC_ICONS = icon.ico
# }

# Output directories
DESTDIR = ../build
OBJECTS_DIR = ../build/obj
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
