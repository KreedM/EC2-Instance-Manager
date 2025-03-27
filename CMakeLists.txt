# This file is part of EC2-Instance-Manager
# Copyright (C) 2025  Derek Meng

# EC2-Instance-Manager is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# EC2-Instance-Manager is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with EC2-Instance-Manager.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.16)

project(EC2-Instance-Manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    "src/mainwindow.ui"
    "src/mainwindow.cpp"
    "src/main.cpp"
)

add_executable(EC2-Instance-Manager
    ${PROJECT_SOURCES}
)

target_link_libraries(EC2-Instance-Manager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(EC2-Instance-Manager PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS EC2-Instance-Manager
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

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

target_link_libraries(EC2-Utils PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_link_libraries(EC2-Instance-Manager PRIVATE EC2-Utils)
