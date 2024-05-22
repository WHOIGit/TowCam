cmake_minimum_required(VERSION 3.25)

project(towcam
        VERSION 0.1.1
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Network Core PrintSupport)
qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        main.cpp
        ini_file.h
        ini_file.cpp
        qcustomplot.h
        qcustomplot.cpp
        TowCam.h
        TowCam.cpp
        TowcamSocketThread.h
        TowcamSocketThread.cpp
        compilation.h
        HCLog.h
        HCLog.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Gui Qt::Network Qt::PrintSupport)



install(TARGETS ${PROJECT_NAME}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})