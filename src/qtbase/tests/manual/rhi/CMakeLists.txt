# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(hellominimalcrossgfxtriangle)
add_subdirectory(compressedtexture_bc1)
add_subdirectory(compressedtexture_bc1_subupload)
add_subdirectory(texuploads)
add_subdirectory(msaatexture)
add_subdirectory(msaarenderbuffer)
add_subdirectory(cubemap)
add_subdirectory(cubemap_scissor)
add_subdirectory(cubemap_render)
add_subdirectory(multiwindow)
add_subdirectory(multiwindow_threaded)
add_subdirectory(triquadcube)
add_subdirectory(offscreen)
add_subdirectory(floattexture)
add_subdirectory(float16texture_with_compute)
add_subdirectory(mrt)
add_subdirectory(shadowmap)
add_subdirectory(computebuffer)
add_subdirectory(computeimage)
add_subdirectory(instancing)
add_subdirectory(noninstanced)
add_subdirectory(tex3d)
add_subdirectory(texturearray)
add_subdirectory(polygonmode)
add_subdirectory(tessellation)
add_subdirectory(geometryshader)
add_subdirectory(stenciloutline)
add_subdirectory(stereo)
add_subdirectory(tex1d)
add_subdirectory(displacement)
add_subdirectory(imguirenderer)
add_subdirectory(multiview)
add_subdirectory(msaatextureresolve)
if(QT_FEATURE_widgets)
    add_subdirectory(rhiwidgetproto)
endif()
