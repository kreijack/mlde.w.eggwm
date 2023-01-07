QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

TEMPLATE     = app
TARGET       = eggwm
QT          += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4) { 
QT          += x11extras network
}
OBJECTS_DIR += build
MOC_DIR     += build
LIBS        += -lX11 -lxcb
//DEFINES      = QT_NO_DEBUG_OUTPUT
INCLUDEPATH += $$PWD/src

target.path  = /usr/bin
config.path  = /usr/share/eggwm
config.files = data/*
INSTALLS += target config


include(src/eggwm.pri)
