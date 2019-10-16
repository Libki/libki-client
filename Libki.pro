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
TRANSLATIONS = languages/libkiclient_fr.ts \
        languages/libkiclient_sv.ts \
        languages/libkiclient_es.ts
OTHER_FILES +=
