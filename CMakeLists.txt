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
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    get_target_property(qmake_executable Qt::qmake IMPORTED_LOCATION)
    get_filename_component(qt_bin_directory "${qmake_executable}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${qt_bin_directory}" REQUIRED)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}"
            ARGS ${PROJECT_NAME}.app
            COMMENT "Run macdeployqt to create a macOS bundle"
    )
elseif(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS ${PROJECT_NAME}
        BUNDLE  DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    NO_TRANSLATIONS
)

install(SCRIPT ${deploy_script})