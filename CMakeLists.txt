cmake_minimum_required(VERSION 3.21)

# Configure by yourself
project(RobotCommander VERSION "0.0.1")

# Your config.cmake should contain at least the following properties:
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_PREFIX_PATH "your/cmake/prefix/path")
#set(QT_VERSION 5)
include(config.cmake)

# Required Qt Libraries
set(REQUIRED_LIBS Core Gui Widgets SerialPort)
set(REQUIRED_LIBS_QUALIFIED Qt${QT_VERSION}::Core Qt${QT_VERSION}::Gui Qt${QT_VERSION}::Widgets Qt${QT_VERSION}::SerialPort)

# Target directory
if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_DIR}/build/debug)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_DIR}/build/debug)
else ()
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_DIR}/build/release)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_DIR}/build/release)
endif ()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find package
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
include_directories(.)

# Project sources
set(PROJECT_SOURCES main.cpp RobotCommander.qrc
        mainwindow.ui mainwindow.cpp mainwindow.h
        mapwidget.cpp mapwidget.h
        settingsdialog.ui settingsdialog.cpp settingsdialog.h
        command/command.h
        info/info.cpp info/info.h
        compositor.cpp compositor.h
        command/positioncommand.cpp command/positioncommand.h settings.h)

if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    set(PROJECT_SOURCES WIN32 ${PROJECT_SOURCES})
endif ()

if (CMAKE_HOST_WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} version.rc)
endif ()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})

add_subdirectory(virtualrobot)
