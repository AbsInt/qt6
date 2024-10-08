// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsgsoftwarepainternode_p.h"
#include "qsgsoftwarepixmaptexture_p.h"
#include <qmath.h>

QT_BEGIN_NAMESPACE

QSGSoftwarePainterNode::QSGSoftwarePainterNode(QQuickPaintedItem *item)
    : QSGPainterNode()
    , m_preferredRenderTarget(QQuickPaintedItem::Image)
    , m_item(item)
    , m_texture(nullptr)
    , m_dirtyContents(false)
    , m_opaquePainting(false)
    , m_linear_filtering(false)
    , m_mipmapping(false)
    , m_smoothPainting(false)
    , m_fastFBOResizing(false)
    , m_fillColor(Qt::transparent)
    , m_contentsScale(1.0)
    , m_dirtyGeometry(false)
{
    setMaterial((QSGMaterial*)1);
    setGeometry((QSGGeometry*)1);
}

QSGSoftwarePainterNode::~QSGSoftwarePainterNode()
{
    delete m_texture;
}

void QSGSoftwarePainterNode::setPreferredRenderTarget(QQuickPaintedItem::RenderTarget target)
{
    if (m_preferredRenderTarget == target)
        return;

    m_preferredRenderTarget = target;
}

void QSGSoftwarePainterNode::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;

    m_dirtyGeometry = true;
}

void QSGSoftwarePainterNode::setDirty(const QRect &dirtyRect)
{
    m_dirtyContents = true;
    m_dirtyRect = dirtyRect;
    markDirty(DirtyMaterial);
}

void QSGSoftwarePainterNode::setOpaquePainting(bool opaque)
{
    if (opaque == m_opaquePainting)
        return;

    m_opaquePainting = opaque;
}

void QSGSoftwarePainterNode::setLinearFiltering(bool linearFiltering)
{
    if (linearFiltering == m_linear_filtering)
        return;

    m_linear_filtering = linearFiltering;
}

void QSGSoftwarePainterNode::setMipmapping(bool mipmapping)
{
    if (mipmapping == m_mipmapping)
        return;

    m_mipmapping = mipmapping;
}

void QSGSoftwarePainterNode::setSmoothPainting(bool s)
{
    if (s == m_smoothPainting)
        return;

    m_smoothPainting = s;
}

void QSGSoftwarePainterNode::setFillColor(const QColor &c)
{
    if (c == m_fillColor)
        return;

    m_fillColor = c;
    markDirty(DirtyMaterial);
}

void QSGSoftwarePainterNode::setContentsScale(qreal s)
{
    if (s == m_contentsScale)
        return;

    m_contentsScale = s;
    markDirty(DirtyMaterial);
}

void QSGSoftwarePainterNode::setFastFBOResizing(bool dynamic)
{
    m_fastFBOResizing = dynamic;
}

QImage QSGSoftwarePainterNode::toImage() const
{
    return m_pixmap.toImage();
}

void QSGSoftwarePainterNode::update()
{
    if (m_dirtyGeometry) {
        m_pixmap = QPixmap(m_textureSize);
        if (!m_opaquePainting)
            m_pixmap.fill(Qt::transparent);

        if (m_texture)
            delete m_texture;
        m_texture = new QSGSoftwarePixmapTexture(m_pixmap);
    }

    if (m_dirtyContents)
        paint();

    m_dirtyGeometry = false;
    m_dirtyContents = false;
}

void QSGSoftwarePainterNode::paint(QPainter *painter)
{
    bool before = painter->testRenderHint(QPainter::SmoothPixmapTransform);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, m_linear_filtering);
    painter->drawPixmap(0, 0, m_size.width(), m_size.height(), m_pixmap);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, before);
}

void QSGSoftwarePainterNode::paint()
{
    QRect dirtyRect = m_dirtyRect.isNull() ? QRect(0, 0, m_size.width(), m_size.height()) : m_dirtyRect;

    QPainter painter;

    painter.begin(&m_pixmap);
    if (m_smoothPainting) {
        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    }

    QRect clipRect;

    if (m_contentsScale == 1) {
        qreal scaleX = m_textureSize.width() / (qreal) m_size.width();
        qreal scaleY = m_textureSize.height() / (qreal) m_size.height();
        painter.scale(scaleX, scaleY);
        clipRect = dirtyRect;
    } else {
        painter.scale(m_contentsScale, m_contentsScale);

        QRect sclip(qFloor(dirtyRect.x()/m_contentsScale),
                    qFloor(dirtyRect.y()/m_contentsScale),
                    qCeil(dirtyRect.width()/m_contentsScale+dirtyRect.x()/m_contentsScale-qFloor(dirtyRect.x()/m_contentsScale)),
                    qCeil(dirtyRect.height()/m_contentsScale+dirtyRect.y()/m_contentsScale-qFloor(dirtyRect.y()/m_contentsScale)));

        clipRect = sclip;
    }

    if (!m_dirtyRect.isNull())
        painter.setClipRect(clipRect);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(clipRect, m_fillColor);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    m_item->paint(&painter);
    painter.end();

    m_dirtyRect = QRect();
}


void QSGSoftwarePainterNode::setTextureSize(const QSize &size)
{
    if (size == m_textureSize)
        return;

    m_textureSize = size;
    m_dirtyGeometry = true;
}

QT_END_NAMESPACE
