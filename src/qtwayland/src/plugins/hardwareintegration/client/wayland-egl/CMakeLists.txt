# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from wayland-egl.pro.

#####################################################################
## QWaylandEglClientBufferPlugin Plugin:
#####################################################################
qt_find_package(EGL) # special case

qt_internal_add_plugin(QWaylandEglClientBufferPlugin
    OUTPUT_NAME qt-plugin-wayland-egl
    PLUGIN_TYPE wayland-graphics-integration-client
    SOURCES
        main.cpp
    LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::WaylandClientPrivate
        Qt::WaylandEglClientHwIntegrationPrivate
    QT_LICENSE_ID QT_COMMERCIAL_OR_LGPL3
)

#### Keys ignored in scope 1:.:.:wayland-egl.pro:<TRUE>:
# OTHER_FILES = "wayland-egl.json"
