cmake_minimum_required(VERSION 4.2)
project (libki-client)
set(CAME_CXX_STANDARD 03)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Webkit REQUIRED)
find_package(Qt5Widgets REQUIRED)

add_executable(libkiclient
        main.cpp
        loginwindow.h
        loginwindow.cpp
        logutils.h
        logutils.cpp
        networkclient.h
        networkclient.cpp
        printprotocol.h
        printsubmissionserver.h
        printsubmissionserver.cpp
        sessionlockedwindow.h
        sessionlockedwindow.cpp
        syslog.h
        syslog.cpp
        timerwindow.h
        timerwindow.cpp
        timesplash.h
        timesplash.cpp
        utils.h
        utils.cpp
        log.cpp
        log.h
        logsdata.cpp
        logsdata.h
)

target_link_libraries(libkiclient Qt5::Core Qt5::Gui Qt5::Network Qt5::Script Qt5::Widgets Qt5::Widgets)
