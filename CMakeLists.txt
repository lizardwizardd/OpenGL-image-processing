cmake_minimum_required(VERSION 3.5)

project(OpenGL-image-processing VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        glwidget.cpp
        glwidget.h
        shadermanager.cpp
        shadermanager.h
        eventfilter.cpp
        eventfilter.h

        shaders/base.vert
        shaders/base.frag
        shaders/correction.frag
        shaders/correction.vert
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenGL-image-processing
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(OpenGL-image-processing
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(OpenGL-image-processing PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

set_target_properties(OpenGL-image-processing PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS OpenGL-image-processing
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OpenGL-image-processing)
endif()
