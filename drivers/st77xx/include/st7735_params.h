/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

#if !DOXYGEN    /* hide from documentation */
/**
 * @ingroup     drivers_st7735
 *
 * @{
 * @file
 * @brief       Header file for ST7735 configuration for backward compatibility
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * If a board definition already used the ST7735 driver, the `st7735_params.h`
 * header file and `ST7735_*` macros are used. This file is kept for
 * backward compatibility.
 */


#include "board.h"
#include "lcd.h"
#include "st7735.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* DOXYGEN */
