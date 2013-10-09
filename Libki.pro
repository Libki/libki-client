TEMPLATE = app
TARGET = libkiclient
DEPENDPATH += .
INCLUDEPATH += .

QT += core
QT += gui
QT += network
QT += script
QT += webkitwidgets

#CONFIG += console

# Input
HEADERS += loginwindow.h networkclient.h timerwindow.h
FORMS += loginwindow.ui timerwindow.ui
RESOURCES += libki.qrc
RC_FILE += libki.rc
SOURCES += loginwindow.cpp \
           main.cpp \
           networkclient.cpp \
           timerwindow.cpp
TRANSLATIONS = languages/libkiclient_fr_ca.ts

OTHER_FILES +=


