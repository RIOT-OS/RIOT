/*
 * Copyright (C) 2018 Koen Zandberg
 * Copyright (C) 2022 Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lpm013m126
 * @{
 *
 * @file
 * @brief       Device driver implementation for the lpm013m126 display controller
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * @}
 */

#ifndef LPM013M126_INTERNAL_H
#define LPM013M126_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LPM013M126 commands
 *
 * Not exhaustive, please extend when required
 * @{
 */

// COLUMNS must always be %2=0 -> 4bpp
#define LPM013M126_COLUMNS 176
#define LPM013M126_LINES 176

#define LPM013M126_CMD_UPDATE            (0x90) // Update Mode (4bit Data Mode)
#define LPM013M126_CMD_ALL_CLEAR         (0x20) // All Clear Mode
#define LPM013M126_CMD_NO_UPDATE         (0x00) // No Update Mode
#define LPM013M126_CMD_BLINKING_WHITE    (0x18) // Display Blinking Color Mode (White)
#define LPM013M126_CMD_BLINKING_BLACK    (0x10) // Display Blinking Color Mode (Black)
#define LPM013M126_CMD_INVERSION         (0x14) // Display Inversion Mode

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* LPM013M126_INTERNAL_H */
