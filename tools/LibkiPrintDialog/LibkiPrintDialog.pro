QT += core
QT += gui
QT += widgets
QT += network

CONFIG += c++11

TARGET = LibkiPrintDialog

TEMPLATE = app

HEADERS += \
    ../../printprotocol.h \
    printdialog.h \
    printsubmissionclient.h

SOURCES += \
    main.cpp \
    printdialog.cpp \
    printsubmissionclient.cpp

# Windows builds
win32 {
    RC_FILE = ../../libki.rc
}

# Install target (optional)
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
