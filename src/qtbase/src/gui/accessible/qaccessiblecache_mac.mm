// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qaccessiblecache_p.h"

// qcocoaaccessibilityelement.h in platform plugin
@interface QT_MANGLE_NAMESPACE(QMacAccessibilityElement)
- (void)invalidate;
@end

QT_BEGIN_NAMESPACE

void QAccessibleCache::insertElement(QAccessible::Id axid, QT_MANGLE_NAMESPACE(QMacAccessibilityElement) *element) const
{
    accessibleElements[axid] = element;
}

void QAccessibleCache::removeAccessibleElement(QAccessible::Id axid)
{
    QT_MANGLE_NAMESPACE(QMacAccessibilityElement) *element = elementForId(axid);
    [element invalidate];
    accessibleElements.remove(axid);
}

QT_MANGLE_NAMESPACE(QMacAccessibilityElement) *QAccessibleCache::elementForId(QAccessible::Id axid) const
{
    return accessibleElements.value(axid);
}

QT_END_NAMESPACE
