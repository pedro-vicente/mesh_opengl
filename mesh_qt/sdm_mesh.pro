QT        += core gui opengl widgets

HEADERS   = glwidget.hpp \
            sdm_mesh_file.hpp
            
SOURCES   = main.cpp \
            sdm_mesh_file.cpp \
            glwidget.cpp \

win32 {
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 LIBS += opengl32.lib
}


