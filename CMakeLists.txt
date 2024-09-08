cmake_minimum_required(VERSION 3.16)

project(EC2-Instance-Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(EC2-Instance-Manager
        MANUAL_FINALIZATION
        include/mainwindow.h
        src/CMakeLists.txt src/main.cpp src/mainwindow.cpp src/mainwindow.ui
        include/EC2Utils.h
        include/EC2InstanceManager.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET EC2-Instance-Manager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(EC2-Instance-Manager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(EC2-Instance-Manager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(EC2-Instance-Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.EC2-Instance-Manager)
endif()
set_target_properties(EC2-Instance-Manager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EC2-Instance-Manager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(EC2-Instance-Manager)
endif()

# Set include directory

target_include_directories(EC2-Instance-Manager PUBLIC "${PROJECT_SOURCE_DIR}/include")

# AWS

set(WINDOWS_BUILD ${MSVC})

set(SERVICE_COMPONENTS ec2)

if (WINDOWS_BUILD)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/AWS CPP SDK/Debug")
    elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/AWS CPP SDK/Release")
    endif ()
endif ()

find_package(AWSSDK REQUIRED COMPONENTS ${SERVICE_COMPONENTS})

if (WINDOWS_BUILD AND AWSSDK_INSTALL_AS_SHARED_LIBS)
    AWSSDK_CPY_DYN_LIBS(SERVICE_COMPONENTS "" ${CMAKE_CURRENT_BINARY_DIR})
endif ()

add_subdirectory("src")

# Link libraries

target_link_libraries(EC2-Utils PUBLIC ${AWSSDK_LINK_LIBRARIES})

target_link_libraries(EC2-Instance-Manager PUBLIC EC2-Utils)

target_link_libraries(EC2-Utils PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
