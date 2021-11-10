set(MY_NAME VirtualRobot)
set(MY_SOURCE main.cpp VirtualRobot.qrc util.h
        vrmainwindow.ui vrmainwindow.h vrmainwindow.cpp
        ../settingsdialog.ui ../settingsdialog.h ../settingsdialog.cpp)

if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
    set(MY_SOURCE WIN32 ${MY_SOURCE})
endif ()

add_executable(${MY_NAME} ${MY_SOURCE})
target_link_libraries(${MY_NAME} PRIVATE ${REQUIRED_LIBS_QUALIFIED})
