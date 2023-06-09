/* SPDX-License-Identifier: (GPL-2.0+ OR MIT) */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd.
 * Author: Sandy Huang <hjc@rock-chips.com>
 */

#ifndef PANEL_SIMPLE_H
#define PANEL_SIMPLE_H
#include <drm/drm_panel.h>

#if IS_ENABLED(CONFIG_DRM_PANEL_SIMPLE) || IS_ENABLED(CONFIG_DRIVERS_HDF_DISP)
int panel_simple_loader_protect(struct drm_panel *panel);
#else
static inline int panel_simple_loader_protect(struct drm_panel *panel)
{
    return 0;
}
#endif
#endif
