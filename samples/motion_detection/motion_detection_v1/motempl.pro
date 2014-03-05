TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += -L/usr/local/lib -lopencv_core \
    -lopencv_highgui \
    -lopencv_video \
    -lopencv_imgproc \
    -lopencv_core

SOURCES += \
    motempl.cpp

