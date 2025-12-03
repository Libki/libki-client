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
HEADERS += loginwindow.h networkclient.h timerwindow.h \
    sessionlockedwindow.h \
    logutils.h \
    timesplash.h \
    utils.h
FORMS += loginwindow.ui timerwindow.ui \
    sessionlockedwindow.ui
RESOURCES += libki.qrc
RC_FILE += libki.rc
SOURCES += loginwindow.cpp \
           main.cpp \
           networkclient.cpp \
           timerwindow.cpp \
           utils.cpp \
    sessionlockedwindow.cpp \
    logutils.cpp \
    timesplash.cpp
TRANSLATIONS = languages/libkiclient_fr.ts \
        languages/libkiclient_sv.ts \
        languages/libkiclient_es.ts \
        languages/libkiclient_nb_NO.ts \
        languages/libkiclient_pt.ts \
        languages/libkiclient_hu.ts
OTHER_FILES +=

DISTFILES += \
    images/icons/libki.ico
