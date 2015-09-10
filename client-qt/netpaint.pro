QT += widgets core gui network
TARGET = netpaint
TEMPLATE = app
SOURCES += main.cpp \
 mainwindow.cpp \
 canvas.cpp \
 line.cpp \
 client.cpp \
    rect.cpp \
    freehand.cpp \
    text.cpp \
    ellips.cpp \
    server.cpp \
    setupdialog.cpp \
    eraser.cpp \
    fill.cpp \
    colorpanel.cpp \
    colorwidget.cpp \
    paste.cpp
HEADERS += mainwindow.h \
 server.h \
 client.h \
 tool.h \
 state.h \
 canvas.h \
 line.h \
    rect.h \
    freehand.h \
    text.h \
    ellips.h \
    setupdialog.h \
    eraser.h \
    fill.h \
    colorpanel.h \
    colorwidget.h \
    paste.h
FORMS += mainwindow.ui \
    setupdialog.ui
