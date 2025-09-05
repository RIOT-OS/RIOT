/*
 * SPDX-FileCopyrightText: 2018 Koen Zandberg
 * SPDX-FileCopyrightText: 2021 Francisco Molina
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#if !DOXYGEN    /* hide from documentation */

/**
 * @defgroup    drivers_st7735 ST7735 display driver head files for backward compatibility
 * @ingroup     drivers_display
 *
 * @brief       Header file for the ST7735 display controller for backward compatibility
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * If a board definition already used the ST7735 driver, the `st7735.h`
 * header file and `ST7735_*` macros are used. This file is kept for
 * backward compatibility.
 */

#include "lcd.h"
#include "st77xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ST7735 display rotation modes
 * @{
 */
#define ST7735_ROTATION_VERT        ST77XX_ROTATION_VERT       /**< Vertical mode */
#define ST7735_ROTATION_VERT_FLIP   ST77XX_ROTATION_VERT_FLIP  /**< Flipped vertical */
#define ST7735_ROTATION_HORZ        ST77XX_ROTATION_HORZ       /**< Horizontal mode */
#define ST7735_ROTATION_HORZ_FLIP   ST77XX_ROTATION_HORZ_FLIP  /**< Horizontal flipped */
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* !DOXYGEN */
