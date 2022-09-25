// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qxcbcursor.h"
#include "qxcbconnection.h"
#include "qxcbwindow.h"
#include "qxcbimage.h"
#include "qxcbxsettings.h"

#if QT_CONFIG(library)
#include <QtCore/QLibrary>
#endif
#include <QtGui/QWindow>
#include <QtGui/QBitmap>
#include <QtGui/private/qguiapplication_p.h>
#include <X11/cursorfont.h>
#include <xcb/xfixes.h>
#include <xcb/xcb_image.h>

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

typedef int (*PtrXcursorLibraryLoadCursor)(void *, const char *);
typedef char *(*PtrXcursorLibraryGetTheme)(void *);
typedef int (*PtrXcursorLibrarySetTheme)(void *, const char *);
typedef int (*PtrXcursorLibraryGetDefaultSize)(void *);

#if QT_CONFIG(xcb_xlib) && QT_CONFIG(library)
#include <X11/Xlib.h>
enum {
    XCursorShape = CursorShape
};
#undef CursorShape

static PtrXcursorLibraryLoadCursor ptrXcursorLibraryLoadCursor = nullptr;
static PtrXcursorLibraryGetTheme ptrXcursorLibraryGetTheme = nullptr;
static PtrXcursorLibrarySetTheme ptrXcursorLibrarySetTheme = nullptr;
static PtrXcursorLibraryGetDefaultSize ptrXcursorLibraryGetDefaultSize = nullptr;
#endif

static xcb_font_t cursorFont = 0;
static int cursorCount = 0;

#ifndef QT_NO_CURSOR

static uint8_t cur_blank_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static const uint8_t cur_ver_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xc0, 0x03, 0xe0, 0x07, 0xf0, 0x0f,
    0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0xf0, 0x0f,
    0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00 };
static const uint8_t mcur_ver_bits[] = {
    0x00, 0x00, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x3f,
    0xfc, 0x7f, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xfc, 0x7f, 0xf8, 0x3f,
    0xf0, 0x1f, 0xe0, 0x0f, 0xc0, 0x07, 0x80, 0x03 };
static const uint8_t cur_hor_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x30, 0x18,
    0x38, 0x38, 0xfc, 0x7f, 0xfc, 0x7f, 0x38, 0x38, 0x30, 0x18, 0x20, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t mcur_hor_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x40, 0x04, 0x60, 0x0c, 0x70, 0x1c, 0x78, 0x3c,
    0xfc, 0x7f, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfc, 0x7f, 0x78, 0x3c,
    0x70, 0x1c, 0x60, 0x0c, 0x40, 0x04, 0x00, 0x00 };
static const uint8_t cur_bdiag_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x3e, 0x00, 0x3c, 0x00, 0x3e,
    0x00, 0x37, 0x88, 0x23, 0xd8, 0x01, 0xf8, 0x00, 0x78, 0x00, 0xf8, 0x00,
    0xf8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t mcur_bdiag_bits[] = {
    0x00, 0x00, 0xc0, 0x7f, 0x80, 0x7f, 0x00, 0x7f, 0x00, 0x7e, 0x04, 0x7f,
    0x8c, 0x7f, 0xdc, 0x77, 0xfc, 0x63, 0xfc, 0x41, 0xfc, 0x00, 0xfc, 0x01,
    0xfc, 0x03, 0xfc, 0x07, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t cur_fdiag_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xf8, 0x00, 0x78, 0x00,
    0xf8, 0x00, 0xd8, 0x01, 0x88, 0x23, 0x00, 0x37, 0x00, 0x3e, 0x00, 0x3c,
    0x00, 0x3e, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t mcur_fdiag_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0xfc, 0x07, 0xfc, 0x03, 0xfc, 0x01, 0xfc, 0x00,
    0xfc, 0x41, 0xfc, 0x63, 0xdc, 0x77, 0x8c, 0x7f, 0x04, 0x7f, 0x00, 0x7e,
    0x00, 0x7f, 0x80, 0x7f, 0xc0, 0x7f, 0x00, 0x00 };
static const uint8_t *cursor_bits16[] = {
    cur_ver_bits, mcur_ver_bits, cur_hor_bits, mcur_hor_bits,
    cur_bdiag_bits, mcur_bdiag_bits, cur_fdiag_bits, mcur_fdiag_bits,
    nullptr, nullptr, cur_blank_bits, cur_blank_bits };

static const uint8_t vsplit_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00,
    0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t vsplitm_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00,
    0x00, 0xf8, 0x0f, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
    0x00, 0xc0, 0x01, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
    0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
    0x80, 0xff, 0xff, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
    0x00, 0xc0, 0x01, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0xf0, 0x07, 0x00,
    0x00, 0xe0, 0x03, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t hsplit_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
    0x00, 0x41, 0x82, 0x00, 0x80, 0x41, 0x82, 0x01, 0xc0, 0x7f, 0xfe, 0x03,
    0x80, 0x41, 0x82, 0x01, 0x00, 0x41, 0x82, 0x00, 0x00, 0x40, 0x02, 0x00,
    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
    0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t hsplitm_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe2, 0x47, 0x00, 0x00, 0xe3, 0xc7, 0x00,
    0x80, 0xe3, 0xc7, 0x01, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07,
    0xc0, 0xff, 0xff, 0x03, 0x80, 0xe3, 0xc7, 0x01, 0x00, 0xe3, 0xc7, 0x00,
    0x00, 0xe2, 0x47, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
    0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t whatsthis_bits[] = {
    0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0xf0, 0x07, 0x00,
    0x09, 0x18, 0x0e, 0x00, 0x11, 0x1c, 0x0e, 0x00, 0x21, 0x1c, 0x0e, 0x00,
    0x41, 0x1c, 0x0e, 0x00, 0x81, 0x1c, 0x0e, 0x00, 0x01, 0x01, 0x07, 0x00,
    0x01, 0x82, 0x03, 0x00, 0xc1, 0xc7, 0x01, 0x00, 0x49, 0xc0, 0x01, 0x00,
    0x95, 0xc0, 0x01, 0x00, 0x93, 0xc0, 0x01, 0x00, 0x21, 0x01, 0x00, 0x00,
    0x20, 0xc1, 0x01, 0x00, 0x40, 0xc2, 0x01, 0x00, 0x40, 0x02, 0x00, 0x00,
    0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
static const uint8_t whatsthism_bits[] = {
    0x01, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x07, 0x00, 0x07, 0xf8, 0x0f, 0x00,
    0x0f, 0xfc, 0x1f, 0x00, 0x1f, 0x3e, 0x1f, 0x00, 0x3f, 0x3e, 0x1f, 0x00,
    0x7f, 0x3e, 0x1f, 0x00, 0xff, 0x3e, 0x1f, 0x00, 0xff, 0x9d, 0x0f, 0x00,
    0xff, 0xc3, 0x07, 0x00, 0xff, 0xe7, 0x03, 0x00, 0x7f, 0xe0, 0x03, 0x00,
    0xf7, 0xe0, 0x03, 0x00, 0xf3, 0xe0, 0x03, 0x00, 0xe1, 0xe1, 0x03, 0x00,
    0xe0, 0xe1, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00,
    0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
static const uint8_t busy_bits[] = {
    0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,
    0x41, 0xe0, 0xff, 0x00, 0x81, 0x20, 0x80, 0x00, 0x01, 0xe1, 0xff, 0x00,
    0x01, 0x42, 0x40, 0x00, 0xc1, 0x47, 0x40, 0x00, 0x49, 0x40, 0x55, 0x00,
    0x95, 0x80, 0x2a, 0x00, 0x93, 0x00, 0x15, 0x00, 0x21, 0x01, 0x0a, 0x00,
    0x20, 0x01, 0x11, 0x00, 0x40, 0x82, 0x20, 0x00, 0x40, 0x42, 0x44, 0x00,
    0x80, 0x41, 0x4a, 0x00, 0x00, 0x40, 0x55, 0x00, 0x00, 0xe0, 0xff, 0x00,
    0x00, 0x20, 0x80, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t busym_bits[] = {
    0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
    0x0f, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
    0x7f, 0xe0, 0xff, 0x00, 0xff, 0xe0, 0xff, 0x00, 0xff, 0xe1, 0xff, 0x00,
    0xff, 0xc3, 0x7f, 0x00, 0xff, 0xc7, 0x7f, 0x00, 0x7f, 0xc0, 0x7f, 0x00,
    0xf7, 0x80, 0x3f, 0x00, 0xf3, 0x00, 0x1f, 0x00, 0xe1, 0x01, 0x0e, 0x00,
    0xe0, 0x01, 0x1f, 0x00, 0xc0, 0x83, 0x3f, 0x00, 0xc0, 0xc3, 0x7f, 0x00,
    0x80, 0xc1, 0x7f, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00, 0xe0, 0xff, 0x00,
    0x00, 0xe0, 0xff, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t * const cursor_bits32[] = {
    vsplit_bits, vsplitm_bits, hsplit_bits, hsplitm_bits,
    nullptr, nullptr, nullptr, nullptr, whatsthis_bits, whatsthism_bits, busy_bits, busym_bits
};

static const uint8_t forbidden_bits[] = {
    0x00,0x00,0x00,0x80,0x1f,0x00,0xe0,0x7f,0x00,0xf0,0xf0,0x00,0x38,0xc0,0x01,
    0x7c,0x80,0x03,0xec,0x00,0x03,0xce,0x01,0x07,0x86,0x03,0x06,0x06,0x07,0x06,
    0x06,0x0e,0x06,0x06,0x1c,0x06,0x0e,0x38,0x07,0x0c,0x70,0x03,0x1c,0xe0,0x03,
    0x38,0xc0,0x01,0xf0,0xe0,0x00,0xe0,0x7f,0x00,0x80,0x1f,0x00,0x00,0x00,0x00 };

static const uint8_t forbiddenm_bits[] = {
    0x80,0x1f,0x00,0xe0,0x7f,0x00,0xf0,0xff,0x00,0xf8,0xff,0x01,0xfc,0xf0,0x03,
    0xfe,0xc0,0x07,0xfe,0x81,0x07,0xff,0x83,0x0f,0xcf,0x07,0x0f,0x8f,0x0f,0x0f,
    0x0f,0x1f,0x0f,0x0f,0x3e,0x0f,0x1f,0xfc,0x0f,0x1e,0xf8,0x07,0x3e,0xf0,0x07,
    0xfc,0xe0,0x03,0xf8,0xff,0x01,0xf0,0xff,0x00,0xe0,0x7f,0x00,0x80,0x1f,0x00};

static const uint8_t openhand_bits[] = {
    0x80,0x01,0x58,0x0e,0x64,0x12,0x64,0x52,0x48,0xb2,0x48,0x92,
    0x16,0x90,0x19,0x80,0x11,0x40,0x02,0x40,0x04,0x40,0x04,0x20,
    0x08,0x20,0x10,0x10,0x20,0x10,0x00,0x00};
static const uint8_t openhandm_bits[] = {
    0x80,0x01,0xd8,0x0f,0xfc,0x1f,0xfc,0x5f,0xf8,0xff,0xf8,0xff,
    0xf6,0xff,0xff,0xff,0xff,0x7f,0xfe,0x7f,0xfc,0x7f,0xfc,0x3f,
    0xf8,0x3f,0xf0,0x1f,0xe0,0x1f,0x00,0x00};
static const uint8_t closedhand_bits[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0xb0,0x0d,0x48,0x32,0x08,0x50,
    0x10,0x40,0x18,0x40,0x04,0x40,0x04,0x20,0x08,0x20,0x10,0x10,
    0x20,0x10,0x20,0x10,0x00,0x00,0x00,0x00};
static const uint8_t closedhandm_bits[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0xb0,0x0d,0xf8,0x3f,0xf8,0x7f,
    0xf0,0x7f,0xf8,0x7f,0xfc,0x7f,0xfc,0x3f,0xf8,0x3f,0xf0,0x1f,
    0xe0,0x1f,0xe0,0x1f,0x00,0x00,0x00,0x00};

static const uint8_t * const cursor_bits20[] = {
    forbidden_bits, forbiddenm_bits
};

// ### FIXME This mapping is incomplete - QTBUG-71423
static const std::vector<const char *> cursorNames[] = {
    { "left_ptr", "default", "top_left_arrow", "left_arrow" },
    { "up_arrow" },
    { "cross" },
    { "wait", "watch", "0426c94ea35c87780ff01dc239897213" },
    { "ibeam", "text", "xterm" },
    { "size_ver", "ns-resize", "v_double_arrow", "00008160000006810000408080010102" },
    { "size_hor", "ew-resize", "h_double_arrow", "028006030e0e7ebffc7f7070c0600140" },
    { "size_bdiag", "nesw-resize", "50585d75b494802d0151028115016902", "fcf1c3c7cd4491d801f1e1c78f100000" },
    { "size_fdiag", "nwse-resize", "38c5dff7c7b8962045400281044508d2", "c7088f0f3e6c8088236ef8e1e3e70000" },
    { "size_all" },
    { "blank" },
    { "split_v", "row-resize", "sb_v_double_arrow", "2870a09082c103050810ffdffffe0204", "c07385c7190e701020ff7ffffd08103c" },
    { "split_h", "col-resize", "sb_h_double_arrow", "043a9f68147c53184671403ffa811cc5", "14fef782d02440884392942c11205230" },
    { "pointing_hand", "pointer", "hand1", "e29285e634086352946a0e7090d73106" },
    { "forbidden", "not-allowed", "crossed_circle", "circle", "03b6e0fcb3499374a867c041f52298f0" },
    { "whats_this", "help", "question_arrow", "5c6cd98b3f3ebcb1f9c7f1c204630408", "d9ce0ab605698f320427677b458ad60b" },
    { "left_ptr_watch", "half-busy", "progress", "00000000000000020006000e7e9ffc3f", "08e8e1c95fe2fc01f976f1e063a24ccd" },
    { "openhand", "grab", "fleur", "5aca4d189052212118709018842178c0", "9d800788f1b08800ae810202380a0822" },
    { "closedhand", "grabbing", "208530c400c041818281048008011002" },
    { "dnd-copy", "copy" },
    { "dnd-move", "move" },
    { "dnd-link", "link" }
};

QXcbCursorCacheKey::QXcbCursorCacheKey(const QCursor &c)
    : shape(c.shape()), bitmapCacheKey(0), maskCacheKey(0)
{
    if (shape == Qt::BitmapCursor) {
        const qint64 pixmapCacheKey = c.pixmap().cacheKey();
        if (pixmapCacheKey) {
            bitmapCacheKey = pixmapCacheKey;
        } else {
            Q_ASSERT(!c.bitmap().isNull());
            Q_ASSERT(!c.mask().isNull());
            bitmapCacheKey = c.bitmap().cacheKey();
            maskCacheKey = c.mask().cacheKey();
        }
    }
}

#endif // !QT_NO_CURSOR

QXcbCursor::QXcbCursor(QXcbConnection *conn, QXcbScreen *screen)
    : QXcbObject(conn), m_screen(screen), m_gtkCursorThemeInitialized(false)
{
#if QT_CONFIG(cursor)
    // see NUM_BITMAPS in libXcursor/src/xcursorint.h
    m_bitmapCache.setMaxCost(8);
#endif

    if (cursorCount++)
        return;

    cursorFont = xcb_generate_id(xcb_connection());
    const char *cursorStr = "cursor";
    xcb_open_font(xcb_connection(), cursorFont, strlen(cursorStr), cursorStr);

#if QT_CONFIG(xcb_xlib) && QT_CONFIG(library)
    static bool function_ptrs_not_initialized = true;
    if (function_ptrs_not_initialized) {
        QLibrary xcursorLib("Xcursor"_L1, 1);
        bool xcursorFound = xcursorLib.load();
        if (!xcursorFound) { // try without the version number
            xcursorLib.setFileName("Xcursor"_L1);
            xcursorFound = xcursorLib.load();
        }
        if (xcursorFound) {
            ptrXcursorLibraryLoadCursor =
                (PtrXcursorLibraryLoadCursor) xcursorLib.resolve("XcursorLibraryLoadCursor");
            ptrXcursorLibraryGetTheme =
                    (PtrXcursorLibraryGetTheme) xcursorLib.resolve("XcursorGetTheme");
            ptrXcursorLibrarySetTheme =
                    (PtrXcursorLibrarySetTheme) xcursorLib.resolve("XcursorSetTheme");
            ptrXcursorLibraryGetDefaultSize =
                    (PtrXcursorLibraryGetDefaultSize) xcursorLib.resolve("XcursorGetDefaultSize");
        }
        function_ptrs_not_initialized = false;
    }

#endif
}

QXcbCursor::~QXcbCursor()
{
    xcb_connection_t *conn = xcb_connection();

    if (m_gtkCursorThemeInitialized) {
        m_screen->xSettings()->removeCallbackForHandle(this);
    }

    if (!--cursorCount)
        xcb_close_font(conn, cursorFont);

#ifndef QT_NO_CURSOR
    for (xcb_cursor_t cursor : qAsConst(m_cursorHash))
        xcb_free_cursor(conn, cursor);
#endif
}

#ifndef QT_NO_CURSOR
void QXcbCursor::changeCursor(QCursor *cursor, QWindow *window)
{
    if (!window || !window->handle())
        return;

    xcb_cursor_t c = XCB_CURSOR_NONE;
    if (cursor) {
        const QXcbCursorCacheKey key(*cursor);
        const Qt::CursorShape shape = cursor->shape();

        if (shape == Qt::BitmapCursor) {
            auto *bitmap = m_bitmapCache.object(key);
            if (bitmap) {
                c = bitmap->cursor;
            } else {
                c = createBitmapCursor(cursor);
                m_bitmapCache.insert(key, new CachedCursor(xcb_connection(), c));
            }
        } else {
            auto it = m_cursorHash.find(key);
            if (it == m_cursorHash.end()) {
                c = createFontCursor(shape);
                m_cursorHash.insert(key, c);
            } else {
                c = it.value();
            }
        }
    }

    auto *w = static_cast<QXcbWindow *>(window->handle());
    xcb_change_window_attributes(xcb_connection(), w->xcb_window(), XCB_CW_CURSOR, &c);
    xcb_flush(xcb_connection());
}

static int cursorIdForShape(int cshape)
{
    int cursorId = 0;
    switch (cshape) {
    case Qt::ArrowCursor:
        cursorId = XC_left_ptr;
        break;
    case Qt::UpArrowCursor:
        cursorId = XC_center_ptr;
        break;
    case Qt::CrossCursor:
        cursorId = XC_crosshair;
        break;
    case Qt::WaitCursor:
        cursorId = XC_watch;
        break;
    case Qt::IBeamCursor:
        cursorId = XC_xterm;
        break;
    case Qt::SizeAllCursor:
        cursorId = XC_fleur;
        break;
    case Qt::PointingHandCursor:
        cursorId = XC_hand2;
        break;
    case Qt::SizeBDiagCursor:
        cursorId = XC_top_right_corner;
        break;
    case Qt::SizeFDiagCursor:
        cursorId = XC_bottom_right_corner;
        break;
    case Qt::SizeVerCursor:
    case Qt::SplitVCursor:
        cursorId = XC_sb_v_double_arrow;
        break;
    case Qt::SizeHorCursor:
    case Qt::SplitHCursor:
        cursorId = XC_sb_h_double_arrow;
        break;
    case Qt::WhatsThisCursor:
        cursorId = XC_question_arrow;
        break;
    case Qt::ForbiddenCursor:
        cursorId = XC_circle;
        break;
    case Qt::BusyCursor:
        cursorId = XC_watch;
        break;
    default:
        break;
    }
    return cursorId;
}

xcb_cursor_t QXcbCursor::createNonStandardCursor(int cshape)
{
    xcb_cursor_t cursor = 0;
    xcb_connection_t *conn = xcb_connection();

    if (cshape == Qt::BlankCursor) {
        xcb_pixmap_t cp = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(), cur_blank_bits, 16, 16,
                                                             1, 0, 0, nullptr);
        xcb_pixmap_t mp = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(), cur_blank_bits, 16, 16,
                                                             1, 0, 0, nullptr);
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, cp, mp, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 8, 8);
    } else if (cshape >= Qt::SizeVerCursor && cshape < Qt::SizeAllCursor) {
        int i = (cshape - Qt::SizeVerCursor) * 2;
        xcb_pixmap_t pm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                             const_cast<uint8_t*>(cursor_bits16[i]),
                                                             16, 16, 1, 0, 0, nullptr);
        xcb_pixmap_t pmm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                              const_cast<uint8_t*>(cursor_bits16[i + 1]),
                                                              16, 16, 1, 0, 0, nullptr);
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, pm, pmm, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 8, 8);
    } else if ((cshape >= Qt::SplitVCursor && cshape <= Qt::SplitHCursor)
               || cshape == Qt::WhatsThisCursor || cshape == Qt::BusyCursor) {
        int i = (cshape - Qt::SplitVCursor) * 2;
        xcb_pixmap_t pm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                             const_cast<uint8_t*>(cursor_bits32[i]),
                                                             32, 32, 1, 0, 0, nullptr);
        xcb_pixmap_t pmm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                              const_cast<uint8_t*>(cursor_bits32[i + 1]),
                                                              32, 32, 1, 0, 0, nullptr);
        int hs = (cshape == Qt::PointingHandCursor || cshape == Qt::WhatsThisCursor
                  || cshape == Qt::BusyCursor) ? 0 : 16;
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, pm, pmm, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, hs, hs);
    } else if (cshape == Qt::ForbiddenCursor) {
        int i = (cshape - Qt::ForbiddenCursor) * 2;
        xcb_pixmap_t pm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                             const_cast<uint8_t*>(cursor_bits20[i]),
                                                             20, 20, 1, 0, 0, nullptr);
        xcb_pixmap_t pmm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                              const_cast<uint8_t*>(cursor_bits20[i + 1]),
                                                              20, 20, 1, 0, 0, nullptr);
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, pm, pmm, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 10, 10);
    } else if (cshape == Qt::OpenHandCursor || cshape == Qt::ClosedHandCursor) {
        bool open = cshape == Qt::OpenHandCursor;
        xcb_pixmap_t pm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                             const_cast<uint8_t*>(open ? openhand_bits : closedhand_bits),
                                                             16, 16, 1, 0, 0, nullptr);
        xcb_pixmap_t pmm = xcb_create_pixmap_from_bitmap_data(conn, m_screen->root(),
                                                             const_cast<uint8_t*>(open ? openhandm_bits : closedhandm_bits),
                                                             16, 16, 1, 0, 0, nullptr);
        cursor = xcb_generate_id(conn);
        xcb_create_cursor(conn, cursor, pm, pmm, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 8, 8);
    } else if (cshape == Qt::DragCopyCursor || cshape == Qt::DragMoveCursor
               || cshape == Qt::DragLinkCursor) {
        QImage image = QGuiApplicationPrivate::instance()->getPixmapCursor(static_cast<Qt::CursorShape>(cshape)).toImage();
        if (!image.isNull()) {
            xcb_pixmap_t pm = qt_xcb_XPixmapFromBitmap(m_screen, image);
            xcb_pixmap_t pmm = qt_xcb_XPixmapFromBitmap(m_screen, image.createAlphaMask());
            cursor = xcb_generate_id(conn);
            xcb_create_cursor(conn, cursor, pm, pmm, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF, 8, 8);
            xcb_free_pixmap(conn, pm);
            xcb_free_pixmap(conn, pmm);
        }
    }

    return cursor;
}

#if QT_CONFIG(xcb_xlib) && QT_CONFIG(library)
bool updateCursorTheme(void *dpy, const QByteArray &theme) {
    if (!ptrXcursorLibraryGetTheme
            || !ptrXcursorLibrarySetTheme)
        return false;
    QByteArray oldTheme = ptrXcursorLibraryGetTheme(dpy);
    if (oldTheme == theme)
        return false;

    int setTheme = ptrXcursorLibrarySetTheme(dpy,theme.constData());
    return setTheme;
}

 void QXcbCursor::cursorThemePropertyChanged(QXcbVirtualDesktop *screen, const QByteArray &name, const QVariant &property, void *handle)
{
    Q_UNUSED(screen);
    Q_UNUSED(name);
    QXcbCursor *self = static_cast<QXcbCursor *>(handle);
    self->m_cursorHash.clear();

    updateCursorTheme(self->connection()->xlib_display(),property.toByteArray());
}

static xcb_cursor_t loadCursor(void *dpy, int cshape)
{
    xcb_cursor_t cursor = XCB_NONE;
    if (!ptrXcursorLibraryLoadCursor || !dpy)
        return cursor;

    for (const char *cursorName: cursorNames[cshape]) {
        cursor = ptrXcursorLibraryLoadCursor(dpy, cursorName);
        if (cursor != XCB_NONE)
            break;
    }

    return cursor;
}
#endif // QT_CONFIG(xcb_xlib) / QT_CONFIG(library)

xcb_cursor_t QXcbCursor::createFontCursor(int cshape)
{
    xcb_connection_t *conn = xcb_connection();
    int cursorId = cursorIdForShape(cshape);
    xcb_cursor_t cursor = XCB_NONE;

#if QT_CONFIG(xcb_xlib) && QT_CONFIG(library)
    if (m_screen->xSettings()->initialized())
        m_screen->xSettings()->registerCallbackForProperty("Gtk/CursorThemeName",cursorThemePropertyChanged,this);

    // Try Xcursor first
    if (cshape >= 0 && cshape <= Qt::LastCursor) {
        void *dpy = connection()->xlib_display();
        cursor = loadCursor(dpy, cshape);
        if (!cursor && !m_gtkCursorThemeInitialized && m_screen->xSettings()->initialized()) {
            QByteArray gtkCursorTheme = m_screen->xSettings()->setting("Gtk/CursorThemeName").toByteArray();
            if (updateCursorTheme(dpy,gtkCursorTheme)) {
                cursor = loadCursor(dpy, cshape);
            }
            m_gtkCursorThemeInitialized = true;
        }
    }
    if (cursor)
        return cursor;
    if (!cursor && cursorId) {
        cursor = XCreateFontCursor(static_cast<Display *>(connection()->xlib_display()), cursorId);
        if (cursor)
            return cursor;
    }

#endif

    // Non-standard X11 cursors are created from bitmaps
    cursor = createNonStandardCursor(cshape);

    // Create a glyph cursor if everything else failed
    if (!cursor && cursorId) {
        cursor = xcb_generate_id(conn);
        xcb_create_glyph_cursor(conn, cursor, cursorFont, cursorFont,
                                cursorId, cursorId + 1,
                                0xFFFF, 0xFFFF, 0xFFFF, 0, 0, 0);
    }

    if (cursor && cshape >= 0 && cshape < Qt::LastCursor && connection()->hasXFixes()) {
        const char *name = cursorNames[cshape].front();
        xcb_xfixes_set_cursor_name(conn, cursor, strlen(name), name);
    }

    return cursor;
}

xcb_cursor_t QXcbCursor::createBitmapCursor(QCursor *cursor)
{
    QPoint spot = cursor->hotSpot();
    xcb_cursor_t c = XCB_NONE;
    if (cursor->pixmap().depth() > 1) {
        if (connection()->hasXRender(0, 5))
            c = qt_xcb_createCursorXRender(m_screen, cursor->pixmap().toImage(), spot);
        else
            qCWarning(lcQpaXcb, "xrender >= 0.5 required to create pixmap cursors");
    } else {
        xcb_connection_t *conn = xcb_connection();
        xcb_pixmap_t cp = qt_xcb_XPixmapFromBitmap(m_screen, cursor->bitmap().toImage());
        xcb_pixmap_t mp = qt_xcb_XPixmapFromBitmap(m_screen, cursor->mask().toImage());
        c = xcb_generate_id(conn);
        xcb_create_cursor(conn, c, cp, mp, 0, 0, 0, 0xFFFF, 0xFFFF, 0xFFFF,
                          spot.x(), spot.y());
        xcb_free_pixmap(conn, cp);
        xcb_free_pixmap(conn, mp);
    }
    return c;
}
#endif

/*! \internal

    Note that the logical state of a device (as seen by means of the protocol) may
    lag the physical state if device event processing is frozen. See QueryPointer
    in X11 protocol specification.
*/
void QXcbCursor::queryPointer(QXcbConnection *c, QXcbVirtualDesktop **virtualDesktop, QPoint *pos, int *keybMask)
{
    if (pos)
        *pos = QPoint();

    xcb_window_t root = c->primaryVirtualDesktop()->root();

    auto reply = Q_XCB_REPLY(xcb_query_pointer, c->xcb_connection(), root);
    if (reply) {
        if (virtualDesktop) {
            const auto virtualDesktops = c->virtualDesktops();
            for (QXcbVirtualDesktop *vd : virtualDesktops) {
                if (vd->root() == reply->root) {
                    *virtualDesktop = vd;
                    break;
                }
            }
        }
        if (pos)
            *pos = QPoint(reply->root_x, reply->root_y);
        if (keybMask)
            *keybMask = reply->mask;
        return;
    }
}

QPoint QXcbCursor::pos() const
{
    QPoint p;
    queryPointer(connection(), nullptr, &p);
    return p;
}

void QXcbCursor::setPos(const QPoint &pos)
{
    QXcbVirtualDesktop *virtualDesktop = nullptr;
    queryPointer(connection(), &virtualDesktop, nullptr);
    if (virtualDesktop)
        xcb_warp_pointer(xcb_connection(), XCB_NONE, virtualDesktop->root(), 0, 0, 0, 0, pos.x(), pos.y());
    xcb_flush(xcb_connection());
}

QT_END_NAMESPACE
