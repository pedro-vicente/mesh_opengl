# template type is application
TEMPLATE = app
# name
TARGET = mesh

CONFIG -= qt
CONFIG += debug
CONFIG += console 

win32 {
DEFINES += _CRT_SECURE_NO_WARNINGS
DEFINES += _CRT_NONSTDC_NO_DEPRECATE
INCLUDEPATH += ../glut-3.7.6/include
LIBS += -L../glut-3.7.6/lib/glut/Debug
LIBS += glut32.lib
}

# HEADERS
HEADERS   = 

# SOURCES
SOURCES   = ./mesh.cpp
