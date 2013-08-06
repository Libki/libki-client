TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

QT += core
QT += gui
QT += network
QT += script
QT += webkit

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

#win32 {
#    LIBS += -luser32 -lshell32 -lgdi32
#}
