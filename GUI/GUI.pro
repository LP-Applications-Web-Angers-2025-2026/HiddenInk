QT += widgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    histogramwidget.h

FORMS += \
    mainwindow.ui

TARGET = HiddenInkGUI
TEMPLATE = app

# Add project sources from the main repository so the GUI can call the steganography functions directly
INCLUDEPATH += ../stenographie ../stenographie/PNG ../stenographie/BMP ../stenographie/utils ../stenographie/utils/encrypt ../stenographie/utils/external

SOURCES += \
    ../stenographie/PNG/png_hide_text.cpp \
    ../stenographie/PNG/png_hide_image.cpp \
    ../stenographie/PNG/png_extract.cpp \
    ../stenographie/BMP/bmp_convert.cpp \
    ../stenographie/BMP/bmp_Recuperation.cpp \
    ../stenographie/utils/utils_bin.cpp \
    ../stenographie/utils/encrypt/encrypt.cpp \
    ../stenographie/utils/histogramme/histogram.cpp \
    ../stenographie/utils/external/stb_impl.cpp

HEADERS += \
    ../stenographie/PNG/png_hide_text.h \
    ../stenographie/PNG/png_hide_image.h \
    ../stenographie/PNG/png_extract.h \
    ../stenographie/BMP/bmp_convert.h \
    ../stenographie/BMP/bmp_Recuperation.h \
    ../stenographie/utils/analysis/image_analysis.hpp \
    ../stenographie/utils/histogramme/histogram.h \
    ../stenographie/utils/encrypt/encrypt.h \
    ../stenographie/utils/utils_bin.h

RESOURCES += resources.qrc

QMAKE_CXXFLAGS += -Wall -Wextra
