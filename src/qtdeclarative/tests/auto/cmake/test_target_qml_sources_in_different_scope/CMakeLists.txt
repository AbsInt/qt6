# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.19)
project(test_target_qml_sources_in_different_scope LANGUAGES CXX)

if(NOT CMAKE_GENERATOR MATCHES "^Ninja")
    return()
endif()

find_package(Qt6 COMPONENTS REQUIRED Quick)
qt_standard_project_setup()

qt_add_library(MainModule STATIC)
qt_add_qml_module(MainModule
    URI main_module
    VERSION 1.0
    QML_FILES
        Main.qml
)

add_subdirectory(subdir)
