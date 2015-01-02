#-------------------------------------------------
#
# Project created by QtCreator 2014-11-01T15:04:20
#
#-------------------------------------------------

QT       += core gui xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ACGTagGenerator
TEMPLATE = app


SOURCES += main.cpp\
        acgtaggenerator.cpp \
    databasedialog.cpp

HEADERS  += acgtaggenerator.h \
    databasedialog.h

FORMS    += acgtaggenerator.ui \
    databasedialog.ui

QTPLUGIN += qsqlite
