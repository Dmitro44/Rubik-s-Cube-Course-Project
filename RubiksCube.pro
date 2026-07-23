QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/cubegeometry.cpp \
    src/history.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/openglwidget.cpp \
    src/rubikscube.cpp \
    src/solcubdialog.cpp

HEADERS += \
    src/cubegeometry.h \
    src/history.h \
    src/mainwindow.h \
    src/openglwidget.h \
    src/rubikscube.h \
    src/solcubdialog.h

FORMS += \
    forms/history.ui \
    forms/mainwindow.ui \
    forms/solcubdialog.ui

RESOURCES += \
    shaders.qrc