cmake_minimum_required(VERSION 3.10)

# Configure by yourself
project(RobotCommander VERSION "0.2.0")

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_PREFIX_PATH "your/cmake/prefix/path")
set(QT_VERSION 6)
configure_file(RobotCommanderConfig.h.in robotcommanderconfig.h)

# Required Qt Libraries
set(REQUIRED_LIBS Core Gui Widgets SerialPort Network)
foreach (R_LIB ${REQUIRED_LIBS})
    set(REQUIRED_LIBS_QUALIFIED ${REQUIRED_LIBS_QUALIFIED} Qt${QT_VERSION}::${R_LIB})
endforeach (R_LIB)

# Target directory
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find package
find_package(Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Project sources
set(PROJECT_SOURCES RobotCommander.qrc main.cpp protocol.h
        mainwindow.ui mainwindow.cpp mainwindow.h
        settings.cpp settings.h
        settingsdialog.ui settingsdialog.cpp settingsdialog.h
        updater.cpp updater.h
        mapwidget.cpp mapwidget.h
        compositor.cpp compositor.h
        command/command.h
        command/positioncommand.cpp command/positioncommand.h
        command/anycommand.cpp command/anycommand.h
        info/info.h
        info/positioninfo.cpp info/positioninfo.h
        info/anyinfo.cpp info/anyinfo.h
        data/position.cpp data/position.h
        data/hexdisplayer.cpp data/hexdisplayer.h
        util/autorefreshcombobox.cpp util/autorefreshcombobox.h
        panel/senderpanel.cpp panel/senderpanel.h panel/senderpanel.ui
        panel/consolepanel.cpp panel/consolepanel.h panel/consolepanel.ui
        panel/panelbase.h)

if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    set(PROJECT_SOURCES WIN32 ${PROJECT_SOURCES})
endif ()

if (CMAKE_HOST_WIN32)
    set(PROJECT_SOURCES ${PROJECT_SOURCES} version.rc)
endif ()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")