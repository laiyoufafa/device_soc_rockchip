/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 * //* SPDX-License-Identifier: GPL-2.0 */

/*
 * (C) COPYRIGHT 2019-2020 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 */

#ifndef KBASE_CS_EXPERIMENTAL_H_
#define KBASE_CS_EXPERIMENTAL_H_

/**
 * mali_kbase_print_cs_experimental() - Print a string for every Core Services
 * experimental feature that is enabled
 */
static inline void mali_kbase_print_cs_experimental(void)
{
#if MALI_INCREMENTAL_RENDERING
    pr_info("mali_kbase: INCREMENTAL_RENDERING (experimental) enabled");
#endif /* MALI_INCREMENTAL_RENDERING */
}

#endif /* _KBASE_CS_EXPERIMENTAL_H_ */
