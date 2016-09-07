#-------------------------------------------------
#
# Project created by QtCreator 2016-08-30T11:26:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt debug

TARGET = qmanager.elf
TEMPLATE = app

INCLUDEPATH += $$PWD/../../api

SOURCES += main.cpp \
        ui/QManagerWindow.cpp \
        ui/QManagerDialogAddRule.cpp \
        ui/QTableModel.cpp \
        model/QNHM.cpp \
        model/QNHMRule.cpp \
        model/QNHMWorker.cpp

HEADERS  += ui/QManagerWindow.hpp \
        ui/QManagerDialogAddRule.hpp \
        ui/QTableModel.hpp \
        model/QNHM.hpp \
        model/QNHMRule.hpp \
        model/QNHMWorker.hpp

FORMS    += ui/QManagerWindow.ui ui/QManagerDialogAddRule.ui
