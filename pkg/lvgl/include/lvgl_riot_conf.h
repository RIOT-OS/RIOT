/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_lvgl
 * @{
 *
 * @file
 * @brief       Definitions specific to RIOT for the LVGL engine
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "kernel_defines.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximal horizontal resolution to support by the library.*/
#ifndef LV_HOR_RES_MAX
#ifdef LCD_SCREEN_WIDTH
#define LV_HOR_RES_MAX          (LCD_SCREEN_WIDTH)
#else
#define LV_HOR_RES_MAX          (320)
#endif
#endif

/** Maximal vertical resolution to support by the library.*/
#ifndef LV_VER_RES_MAX
#ifdef LCD_SCREEN_HEIGHT
#define LV_VER_RES_MAX          (LCD_SCREEN_HEIGHT)
#else
#define LV_VER_RES_MAX          (240)
#endif
#endif

/** Added to memories with alignment requirement.
 *
 * With -Os data might not be aligned to a boundary by default.
 */
#define LV_ATTRIBUTE_MEM_ALIGN  __attribute__((aligned(4)))

#ifdef __cplusplus
}
#endif

/** @} */
