# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(conversion)
add_subdirectory(qaxobject)
add_subdirectory(dumpcpp)
add_subdirectory(cmake)
add_subdirectory(qbstr)

find_program(midl midl.exe)
if (midl)
    add_subdirectory(qaxobjectcom)
endif()

if(NOT GCC)
    add_subdirectory(qaxscript)
    add_subdirectory(qaxscriptmanager)
endif()
