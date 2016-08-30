#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T11:26:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qmanager.elf
TEMPLATE = app

INCLUDEPATH += $$PWD/../../api

SOURCES += main.cpp \
        QManagerWindow.cpp \
        QManagerDialogAddRule.cpp

HEADERS  += QManagerWindow.hpp QManagerDialogAddRule.hpp 

FORMS    += QManagerWindow.ui QManagerDialogAddRule.ui
