/*
 * Copyright (C) 2010-2017 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU
 * licence.
 *
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/**
 * @file mali_osk_mali.c
 * Implementation of the OS abstraction layer which is specific for the Mali kernel device driver
 */
#include "../platform/rk/custom_log.h"

#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/mali/mali_utgard.h>
#include <linux/of.h>
#include <linux/of_device.h>

#include "mali_osk_mali.h"
#include "mali_kernel_common.h" /* MALI_xxx macros */
#include "mali_osk.h"           /* kernel side OS functions */
#include "mali_kernel_linux.h"

static mali_bool mali_secure_mode_enabled = MALI_FALSE;
static mali_bool mali_secure_mode_supported = MALI_FALSE;

/* Function that init the mali gpu secure mode */
void (*mali_secure_mode_deinit)(void) = NULL;
/* Function that reset GPU and enable the mali gpu secure mode */
int (*mali_gpu_reset_and_secure_mode_enable)(void) = NULL;
/* Function that reset GPU and disable the mali gpu secure mode */
int (*mali_gpu_reset_and_secure_mode_disable)(void) = NULL;

#ifdef CONFIG_MALI_DT

#define MALI_OSK_INVALID_RESOURCE_ADDRESS 0xFFFFFFFF

/**
 * Define the max number of resource we could have.
 */
#define MALI_OSK_MAX_RESOURCE_NUMBER 27

/**
 * Define the max number of resource with interrupts, and they are
 * the first 20 elements in array mali_osk_resource_bank.
 */
#define MALI_OSK_RESOURCE_WITH_IRQ_NUMBER 20

/**
 * pp core start and end location in mali_osk_resource_bank array.
 */
#define MALI_OSK_RESOURCE_PP_LOCATION_START 2
#define MALI_OSK_RESOURCE_PP_LOCATION_END 17
#define MALI_OSK_RESOURCE_PP_CORE_NUMBER 3
#define MALI_OSK_RESOURCE_PP_CORE_NUMBER_MALI 4

/**
 * L2 cache start and end location in mali_osk_resource_bank array.
 */
#define MALI_OSK_RESOURCE_L2_LOCATION_START 20
#define MALI_OSK_RESOURCE_l2_LOCATION_END 22

/**
 * DMA unit location.
 */
#define MALI_OSK_RESOURCE_DMA_LOCATION 26

static _mali_osk_resource_t mali_osk_resource_bank[MALI_OSK_MAX_RESOURCE_NUMBER] = {
    {
        .description = "Mali_GP",
        .base = MALI_OFFSET_GP,
        .irq_name = "Mali_GP_IRQ",
    },
    {
        .description = "Mali_GP_MMU",
        .base = MALI_OFFSET_GP_MMU,
        .irq_name = "Mali_GP_MMU_IRQ",
    },
    {
        .description = "Mali_PP0",
        .base = MALI_OFFSET_PP0,
        .irq_name = "Mali_PP0_IRQ",
    },
    {
        .description = "Mali_PP0_MMU",
        .base = MALI_OFFSET_PP0_MMU,
        .irq_name = "Mali_PP0_MMU_IRQ",
    },
    {
        .description = "Mali_PP1",
        .base = MALI_OFFSET_PP1,
        .irq_name = "Mali_PP1_IRQ",
    },
    {
        .description = "Mali_PP1_MMU",
        .base = MALI_OFFSET_PP1_MMU,
        .irq_name = "Mali_PP1_MMU_IRQ",
    },

    {
        .description = "Mali_PP2",
        .base = MALI_OFFSET_PP2,
        .irq_name = "Mali_PP2_IRQ",
    },
    {
        .description = "Mali_PP2_MMU",
        .base = MALI_OFFSET_PP2_MMU,
        .irq_name = "Mali_PP2_MMU_IRQ",
    },
    {
        .description = "Mali_PP3",
        .base = MALI_OFFSET_PP3,
        .irq_name = "Mali_PP3_IRQ",
    },
    {
        .description = "Mali_PP3_MMU",
        .base = MALI_OFFSET_PP3_MMU,
        .irq_name = "Mali_PP3_MMU_IRQ",
    },

    {
        .description = "Mali_PP4",
        .base = MALI_OFFSET_PP4,
        .irq_name = "IRQPP4",
    },
    {
        .description = "Mali_PP4_MMU",
        .base = MALI_OFFSET_PP4_MMU,
        .irq_name = "IRQPPMMU4",
    },
    {
        .description = "Mali_PP5",
        .base = MALI_OFFSET_PP5,
        .irq_name = "IRQPP5",
    },
    {
        .description = "Mali_PP5_MMU",
        .base = MALI_OFFSET_PP5_MMU,
        .irq_name = "IRQPPMMU5",
    },
    {
        .description = "Mali_PP6",
        .base = MALI_OFFSET_PP6,
        .irq_name = "IRQPP6",
    },
    {
        .description = "Mali_PP6_MMU",
        .base = MALI_OFFSET_PP6_MMU,
        .irq_name = "IRQPPMMU6",
    },
    {
        .description = "Mali_PP7",
        .base = MALI_OFFSET_PP7,
        .irq_name = "IRQPP7",
    },
    {
        .description = "Mali_PP7_MMU",
        .base = MALI_OFFSET_PP7_MMU,
        .irq_name = "IRQPPMMU",
    },
    {
        .description = "Mali_PP_Broadcast",
        .base = MALI_OFFSET_PP_BCAST,
        .irq_name = "IRQPP",
    },
    {
        .description = "Mali_PMU",
        .base = MALI_OFFSET_PMU,
        .irq_name = "IRQPMU",
    },
    {
        .description = "Mali_L2",
        .base = MALI_OFFSET_L2_RESOURCE0,
    },
    {
        .description = "Mali_L2",
        .base = MALI_OFFSET_L2_RESOURCE1,
    },
    {
        .description = "Mali_L2",
        .base = MALI_OFFSET_L2_RESOURCE2,
    },
    {
        .description = "Mali_PP_MMU_Broadcast",
        .base = MALI_OFFSET_PP_BCAST_MMU,
    },
    {
        .description = "Mali_Broadcast",
        .base = MALI_OFFSET_BCAST,
    },
    {
        .description = "Mali_DLBU",
        .base = MALI_OFFSET_DLBU,
    },
    {
        .description = "Mali_DMA",
        .base = MALI_OFFSET_DMA,
    },
};

static int mali_osk_get_compatible_name(const char **out_string)
{
    struct device_node *node = mali_platform_device->dev.of_node;

    MALI_DEBUG_ASSERT(node != NULL);

    return of_property_read_string(node, "compatible", out_string);
}

mali_osk_errcode_t mali_osk_resource_initialize(void)
{
    mali_bool mali_is_450 = MALI_FALSE, mali_is_470 = MALI_FALSE;
    int i, pp_core_num = 0, l2_core_num = 0;
    struct resource *res;
    const char *compatible_name = NULL;

    if (mali_osk_get_compatible_name(&compatible_name) == 0) {
        if (strncmp(compatible_name, "arm,mali-450", strlen("arm,mali-450")) == 0) {
            mali_is_450 = MALI_TRUE;
            MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN, ("mali-450 device tree detected."));
        } else if (strncmp(compatible_name, "arm,mali-470", strlen("arm,mali-470")) == 0) {
            mali_is_470 = MALI_TRUE;
            MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN, ("mali-470 device tree detected."));
        }
    }

    for (i = 0; i < MALI_OSK_RESOURCE_WITH_IRQ_NUMBER; i++) {
        res = platform_get_resource_byname(mali_platform_device, IORESOURCE_IRQ, mali_osk_resource_bank[i].irq_name);
        if (res) {
            mali_osk_resource_bank[i].irq = res->start;
        } else {
            mali_osk_resource_bank[i].base = MALI_OSK_INVALID_RESOURCE_ADDRESS;
        }
    }

    for (i = MALI_OSK_RESOURCE_PP_LOCATION_START; i <= MALI_OSK_RESOURCE_PP_LOCATION_END; i++) {
        if (mali_osk_resource_bank[i].base != MALI_OSK_INVALID_RESOURCE_ADDRESS) {
            pp_core_num++;
        }
    }

    /* We have to divide by 2, because we caculate twice for only one pp(pp_core and pp_mmu_core). */
    if (pp_core_num % MALI_OSK_RESOURCE_PP_LOCATION_START != 0) {
        MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN, ("The value of pp core number isn't normal."));
        return MALI_OSK_ERR_FAULT;
    }

    pp_core_num /= MALI_OSK_RESOURCE_PP_LOCATION_START;

    /**
     * we can caculate the number of l2 cache core according the number of pp core number
     * and device type(mali400/mali450/mali470).
     */
    l2_core_num = 1;
    if (mali_is_450) {
        if (pp_core_num > MALI_OSK_RESOURCE_PP_CORE_NUMBER_MALI) {
            l2_core_num = MALI_OSK_RESOURCE_PP_CORE_NUMBER;
        } else if (pp_core_num <= MALI_OSK_RESOURCE_PP_CORE_NUMBER_MALI) {
            l2_core_num = MALI_OSK_RESOURCE_PP_LOCATION_START;
        }
    }

    for (i = MALI_OSK_RESOURCE_l2_LOCATION_END; i > MALI_OSK_RESOURCE_L2_LOCATION_START + l2_core_num - 1; i--) {
        mali_osk_resource_bank[i].base = MALI_OSK_INVALID_RESOURCE_ADDRESS;
    }

    /* If device is not mali-450 type, we have to remove related resource from resource bank. */
    if (!(mali_is_450 || mali_is_470)) {
        for (i = MALI_OSK_RESOURCE_l2_LOCATION_END + 1; i < MALI_OSK_MAX_RESOURCE_NUMBER; i++) {
            mali_osk_resource_bank[i].base = MALI_OSK_INVALID_RESOURCE_ADDRESS;
        }
    }

    if (mali_is_470) {
        mali_osk_resource_bank[MALI_OSK_RESOURCE_DMA_LOCATION].base = MALI_OSK_INVALID_RESOURCE_ADDRESS;
    }

    return MALI_OSK_ERR_OK;
}

mali_osk_errcode_t mali_osk_resource_find(u32 addr, _mali_osk_resource_t *res)
{
    int i;

    if (mali_platform_device == NULL) {
        return MALI_OSK_ERR_ITEM_NOT_FOUND;
    }

    /* Traverse all of resources in resources bank to find the matching one. */
    for (i = 0; i < MALI_OSK_MAX_RESOURCE_NUMBER; i++) {
        if (mali_osk_resource_bank[i].base == addr) {
            if (res != NULL) {
                res->base = addr + mali_osk_resource_base_address();
                res->description = mali_osk_resource_bank[i].description;
                res->irq = mali_osk_resource_bank[i].irq;
            }
            return MALI_OSK_ERR_OK;
        }
    }

    return MALI_OSK_ERR_ITEM_NOT_FOUND;
}

uintptr_t mali_osk_resource_base_address(void)
{
    struct resource *reg_res = NULL;
    uintptr_t ret = 0;

    reg_res = platform_get_resource(mali_platform_device, IORESOURCE_MEM, 0);
    if (reg_res != NULL) {
        ret = reg_res->start;
    }

    return ret;
}

void mali_osk_device_data_pmu_config_get(u16 *domain_config_array, int array_size)
{
    struct device_node *node = mali_platform_device->dev.of_node;
    struct property *prop;
    const __be32 *p;
    int length = 0, i = 0;
    u32 u;

    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN, ("Get pmu config from device tree configuration.\n"));

    MALI_DEBUG_ASSERT(node != NULL);

    if (!of_get_property(node, "pmu_domain_config", &length)) {
        return;
    }

    if (array_size != length / sizeof(u32)) {
        MALI_PRINT_ERROR(("Wrong pmu domain config in device tree."));
        return;
    }

    of_property_for_each_u32(node, "pmu_domain_config", prop, p, u)
    {
        domain_config_array[i] = (u16)u;
        i++;
    }

    return;
}

u32 mali_osk_get_pmu_switch_delay(void)
{
    struct device_node *node = mali_platform_device->dev.of_node;
    u32 switch_delay;

    MALI_DEBUG_ASSERT(node != NULL);

    if (of_property_read_u32(node, "pmu_switch_delay", &switch_delay) == 0) {
        return switch_delay;
    } else {
        MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN,
                         ("Couldn't find pmu_switch_delay in device tree configuration.\n"));
    }

    return 0;
}

#else /* CONFIG_MALI_DT */ /* 若未 定义 CONFIG_MALI_DT. */

mali_osk_errcode_t mali_osk_resource_find(u32 addr, _mali_osk_resource_t *res)
{
    int i;
    uintptr_t phys_addr;

    if (mali_platform_device == NULL) {
        /* Not connected to a device */
        return MALI_OSK_ERR_ITEM_NOT_FOUND;
    }

    phys_addr = addr + mali_osk_resource_base_address();
    for (i = 0; i < mali_platform_device->num_resources; i++) {
        if (IORESOURCE_MEM == resource_type(&(mali_platform_device->resource[i])) &&
            mali_platform_device->resource[i].start == phys_addr) {
            if (NULL != res) {
                res->base = phys_addr;
                res->description = mali_platform_device->resource[i].name;

                /* Any (optional) IRQ resource belonging to this resource will follow */
                if ((i + 1) < mali_platform_device->num_resources &&
                    IORESOURCE_IRQ == resource_type(&(mali_platform_device->resource[i + 1]))) {
                    res->irq = mali_platform_device->resource[i + 1].start;
                } else {
                    res->irq = -1;
                }
            }
            return MALI_OSK_ERR_OK;
        }
    }

    return MALI_OSK_ERR_ITEM_NOT_FOUND;
}

uintptr_t mali_osk_resource_base_address(void)
{
    uintptr_t lowest_addr = (uintptr_t)(0 - 1);
    uintptr_t ret = 0;

    if (NULL != mali_platform_device) {
        int i;
        for (i = 0; i < mali_platform_device->num_resources; i++) {
            if (mali_platform_device->resource[i].flags & IORESOURCE_MEM &&
                mali_platform_device->resource[i].start < lowest_addr) {
                lowest_addr = mali_platform_device->resource[i].start;
                ret = lowest_addr;
            }
        }
    }

    return ret;
}

void mali_osk_device_data_pmu_config_get(u16 *domain_config_array, int array_size)
{
    mali_osk_device_data data = {
        0,
    };

    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_INFORMATOIN, ("Get pmu config from platform device data.\n"));
    if (MALI_OSK_ERR_OK == mali_osk_device_data_get(&data)) {
        /* Copy the custom customer power domain config */
        mali_osk_memcpy(domain_config_array, data.pmu_domain_config, sizeof(data.pmu_domain_config));
    }

    return;
}

u32 mali_osk_get_pmu_switch_delay(void)
{
    mali_osk_errcode_t err;
    mali_osk_device_data data = {
        0,
    };

    err = mali_osk_device_data_get(&data);
    if (MALI_OSK_ERR_OK == err) {
        return data.pmu_switch_delay;
    }

    return 0;
}
#endif                     /* CONFIG_MALI_DT */

mali_osk_errcode_t mali_osk_device_data_get(mali_osk_device_data *data)
{
    MALI_DEBUG_ASSERT_POINTER(data);

    if (mali_platform_device != NULL) {
        struct mali_gpu_device_data *os_data = NULL;

        os_data = (struct mali_gpu_device_data *)mali_platform_device->dev.platform_data;
        if (os_data != NULL) {
            /* Copy data from OS dependant struct to Mali neutral struct (identical!) */
            BUILD_BUG_ON(sizeof(*os_data) != sizeof(*data));
            mali_osk_memcpy(data, os_data, sizeof(*os_data));

            return MALI_OSK_ERR_OK;
        }
    }

    return MALI_OSK_ERR_ITEM_NOT_FOUND;
}

u32 mali_osk_identify_gpu_resource(void)
{
    if (MALI_OSK_ERR_OK == mali_osk_resource_find(MALI_OFFSET_L2_RESOURCE1, NULL)) {
        /* Mali 450 */
        return 0x450;
    }

    if (MALI_OSK_ERR_OK == mali_osk_resource_find(MALI_OFFSET_DLBU, NULL)) {
        /* Mali 470 */
        return 0x470;
    }

    /* Mali 400 */
    return 0x400;
}

mali_bool mali_osk_shared_interrupts(void)
{
    u32 irqs[128];
    u32 i, j, irq, num_irqs_found = 0;

    MALI_DEBUG_ASSERT_POINTER(mali_platform_device);
    MALI_DEBUG_ASSERT(mali_platform_device->num_resources <= 0X80);

    for (i = 0; i < mali_platform_device->num_resources; i++) {
        if (IORESOURCE_IRQ & mali_platform_device->resource[i].flags) {
            irq = mali_platform_device->resource[i].start;

            for (j = 0; j < num_irqs_found; ++j) {
                if (irq == irqs[j]) {
                    return MALI_TRUE;
                }
            }

            irqs[num_irqs_found++] = irq;
        }
    }

    return MALI_FALSE;
}

mali_osk_errcode_t mali_osk_gpu_secure_mode_init(void)
{
    mali_osk_device_data data = {
        0,
    };

    if (MALI_OSK_ERR_OK == mali_osk_device_data_get(&data)) {
        if ((data.secure_mode_init != NULL) && (data.secure_mode_deinit !=NULL) &&
            (data.gpu_reset_and_secure_mode_enable != NULL) && (data.gpu_reset_and_secure_mode_disable != NULL)) {
            int err = data.secure_mode_init();
            if (err) {
                MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_WRANING, ("Failed to init gpu secure mode.\n"));
                return MALI_OSK_ERR_FAULT;
            }

            mali_secure_mode_deinit = data.secure_mode_deinit;
            mali_gpu_reset_and_secure_mode_enable = data.gpu_reset_and_secure_mode_enable;
            mali_gpu_reset_and_secure_mode_disable = data.gpu_reset_and_secure_mode_disable;

            mali_secure_mode_supported = MALI_TRUE;
            mali_secure_mode_enabled = MALI_FALSE;
            return MALI_OSK_ERR_OK;
        }
    }
    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_MESSAGE, ("GPU secure mode not supported.\n"));
    return _MALI_OSK_ERR_UNSUPPORTED;
}

mali_osk_errcode_t mali_osk_gpu_secure_mode_deinit(void)
{
    if (mali_secure_mode_deinit != NULL) {
        mali_secure_mode_deinit();
        mali_secure_mode_enabled = MALI_FALSE;
        mali_secure_mode_supported = MALI_FALSE;
        return MALI_OSK_ERR_OK;
    }
    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_MESSAGE, ("GPU secure mode not supported.\n"));
    return _MALI_OSK_ERR_UNSUPPORTED;
}

mali_osk_errcode_t mali_osk_gpu_reset_and_secure_mode_enable(void)
{
    /* the mali executor lock must be held before enter this function. */

    MALI_DEBUG_ASSERT(MALI_FALSE == mali_secure_mode_enabled);

    if (mali_gpu_reset_and_secure_mode_enable != NULL) {
        if (mali_gpu_reset_and_secure_mode_enable()) {
            MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_WRANING, ("Failed to reset GPU or enable gpu secure mode.\n"));
            return MALI_OSK_ERR_FAULT;
        }
        mali_secure_mode_enabled = MALI_TRUE;
        return MALI_OSK_ERR_OK;
    }
    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_WRANING, ("GPU secure mode not supported.\n"));
    return _MALI_OSK_ERR_UNSUPPORTED;
}

mali_osk_errcode_t mali_osk_gpu_reset_and_secure_mode_disable(void)
{
    /* the mali executor lock must be held before enter this function. */

    MALI_DEBUG_ASSERT(MALI_TRUE == mali_secure_mode_enabled);

    if (mali_gpu_reset_and_secure_mode_disable != NULL) {
        if (mali_gpu_reset_and_secure_mode_disable()) {
            MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_WRANING, ("Failed to reset GPU or disable gpu secure mode.\n"));
            return MALI_OSK_ERR_FAULT;
        }
        mali_secure_mode_enabled = MALI_FALSE;

        return MALI_OSK_ERR_OK;
    }
    MALI_DEBUG_PRINT(MALI_KERNEL_LEVEL_WRANING, ("GPU secure mode not supported.\n"));
    return _MALI_OSK_ERR_UNSUPPORTED;
}

mali_bool mali_osk_gpu_secure_mode_is_enabled(void)
{
    return mali_secure_mode_enabled;
}

mali_bool mali_osk_gpu_secure_mode_is_supported(void)
{
    return mali_secure_mode_supported;
}
