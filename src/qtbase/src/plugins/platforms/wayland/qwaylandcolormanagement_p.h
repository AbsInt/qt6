// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QWAYLANDCOLORMANAGEMENT_H
#define QWAYLANDCOLORMANAGEMENT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QPointF>
#include <QColorSpace>
#include <QList>

#include "qwayland-xx-color-management-v4.h"

QT_BEGIN_NAMESPACE

namespace QtWaylandClient {

class ImageDescription;

class ColorManager : public QObject, public QtWayland::xx_color_manager_v4
{
    Q_OBJECT
public:
    enum class Feature {
        ICC = 1 << 0,
        Parametric = 1 << 1,
        SetPrimaries = 1 << 2,
        PowerTransferFunction = 1 << 3,
        SetLuminances = 1 << 4,
        SetMasteringDisplayPrimaries = 1 << 5,
        ExtendedTargetVolume = 1 << 6,
    };
    Q_ENUM(Feature);
    Q_DECLARE_FLAGS(Features, Feature);

    explicit ColorManager(struct ::wl_registry *registry, uint32_t id, int version);
    ~ColorManager() override;

    Features supportedFeatures() const;
    bool supportsNamedPrimary(QtWayland::xx_color_manager_v4::primaries primaries) const;
    bool supportsTransferFunction(QtWayland::xx_color_manager_v4::transfer_function transferFunction) const;

    std::unique_ptr<ImageDescription> createImageDescription(const QColorSpace &colorspace);

private:
    void xx_color_manager_v4_supported_feature(uint32_t feature) override;
    void xx_color_manager_v4_supported_primaries_named(uint32_t primaries) override;
    void xx_color_manager_v4_supported_tf_named(uint32_t transferFunction) override;

    Features mFeatures;
    QList<QtWayland::xx_color_manager_v4::primaries> mPrimaries;
    QList<QtWayland::xx_color_manager_v4::transfer_function> mTransferFunctions;
};

class ImageDescriptionInfo : public QObject, public QtWayland::xx_image_description_info_v4
{
    Q_OBJECT
public:
    explicit ImageDescriptionInfo(ImageDescription *descr);
    ~ImageDescriptionInfo();

    Q_SIGNAL void done();

    uint32_t mTransferFunction = 0;
    QPointF mContainerRed;
    QPointF mContainerGreen;
    QPointF mContainerBlue;
    QPointF mContainerWhite;
    QPointF mTargetRed;
    QPointF mTargetGreen;
    QPointF mTargetBlue;
    QPointF mTargetWhite;
    double mMinLuminance;
    double mMaxLuminance;
    double mReferenceLuminance;
    double mTargetMinLuminance;
    double mTargetMaxLuminance;

private:
    void xx_image_description_info_v4_done() override;
    void xx_image_description_info_v4_icc_file(int32_t icc, uint32_t icc_size) override;
    void xx_image_description_info_v4_primaries(int32_t r_x, int32_t r_y, int32_t g_x, int32_t g_y, int32_t b_x, int32_t b_y, int32_t w_x, int32_t w_y) override;
    void xx_image_description_info_v4_tf_named(uint32_t transferFunction) override;
    void xx_image_description_info_v4_luminances(uint32_t min_lum, uint32_t max_lum, uint32_t reference_lum) override;
    void xx_image_description_info_v4_target_primaries(int32_t r_x, int32_t r_y, int32_t g_x, int32_t g_y, int32_t b_x, int32_t b_y, int32_t w_x, int32_t w_y) override;
    void xx_image_description_info_v4_target_luminance(uint32_t min_lum, uint32_t max_lum) override;
};

class ImageDescription : public QObject, public QtWayland::xx_image_description_v4
{
    Q_OBJECT
public:
    explicit ImageDescription(::xx_image_description_v4 *descr);
    ~ImageDescription();

    Q_SIGNAL void ready();

private:
    void xx_image_description_v4_failed(uint32_t cause, const QString &msg) override;
    void xx_image_description_v4_ready(uint32_t identity) override;
};

class ColorManagementFeedback : public QObject, public QtWayland::xx_color_management_feedback_surface_v4
{
    Q_OBJECT
public:
    explicit ColorManagementFeedback(::xx_color_management_feedback_surface_v4 *obj);
    ~ColorManagementFeedback();

    Q_SIGNAL void preferredChanged();

    std::unique_ptr<ImageDescriptionInfo> mPreferredInfo;

private:
    void xx_color_management_feedback_surface_v4_preferred_changed() override;
    void handlePreferredDone();

    std::unique_ptr<ImageDescription> mPreferred;
    std::unique_ptr<ImageDescriptionInfo> mPendingPreferredInfo;

};

class ColorManagementSurface : public QObject, public QtWayland::xx_color_management_surface_v4
{
    Q_OBJECT
public:
    explicit ColorManagementSurface(::xx_color_management_surface_v4 *obj);
    ~ColorManagementSurface();

    void setImageDescription(ImageDescription *descr);
};

}

QT_END_NAMESPACE

#endif
