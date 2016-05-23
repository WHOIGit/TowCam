#-------------------------------------------------
#
# Project created by QtCreator 2012-08-31T10:56:43
#
#-------------------------------------------------

QT       += core gui network widgets printsupport


ios {
    QMAKE_INFO_PLIST = ios/Info.plist
}


TARGET = towcam
TEMPLATE = app


SOURCES += main.cpp\
        TowCam.cpp \
    ini_file.cpp \
    HCLog.cpp \
    qcustomplot.cpp \
    TowcamSocketThread.cpp

HEADERS  += TowCam.h \
    ini_file.h \
    compilation.h \
    HCLog.h \
    qcustomplot.h \
    TowcamSocketThread.h
