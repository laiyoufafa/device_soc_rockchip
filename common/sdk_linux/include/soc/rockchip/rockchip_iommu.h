/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2022 Rockchip Electronics Co., Ltd
 */
#ifndef __SOC_ROCKCHIP_IOMMU_H
#define __SOC_ROCKCHIP_IOMMU_H

struct device;

#if IS_ENABLED(CONFIG_ROCKCHIP_IOMMU)
int rockchip_iommu_enable(struct device *dev);
int rockchip_iommu_disable(struct device *dev);
#else
static inline int rockchip_iommu_enable(struct device *dev)
{
    return -ENODEV;
}
static inline int rockchip_iommu_disable(struct device *dev)
{
    return -ENODEV;
}
#endif

#endif
