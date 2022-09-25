// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGraphicsWidget>

class Window : public QGraphicsWidget
{
    Q_OBJECT
public:
    Window(QGraphicsItem *parent = nullptr);
};
