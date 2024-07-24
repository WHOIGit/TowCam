cmake_minimum_required(VERSION 3.25)

project(towcam
        VERSION 1.2
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

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE TOWCAM_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
            TOWCAM_VERSION_MINOR="${PROJECT_VERSION_MINOR}")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Widgets Qt::Gui Qt::Network Qt::PrintSupport)

if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
    NO_TRANSLATIONS
    DEPLOY_TOOL_OPTIONS ${DEPLOY_TOOL_OPTIONS}
    # Fedora installs Qt libraries into /lib64, which is excluded due to QT_DEPLOY_IGNORED_LIB_DIRS
    # POST_INCLUDE_REGEXES here will make sure at at least the Qt libraries are grabbed.
    POST_INCLUDE_REGEXES "libQt6.*"
)


install(SCRIPT ${deploy_script})

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
string(TOLOWER "${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}" CPACK_PACKAGE_FILE_NAME)
set(CPACK_VERBATIM_VARIABLES ON)
if(NOT APPLE)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/${PROJECT_NAME}")
endif()
include(CPack)
