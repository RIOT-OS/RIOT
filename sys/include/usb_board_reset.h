/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_usb_board_reset Board reset via USB CDC ACM
 * @ingroup     sys
 * @brief       Trigger a board reset via USB CDC ACM
 *
 * @{
 *
 * @file
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Trigger a simple reset, back to the application
 */
void usb_board_reset_in_application(void);

/**
 * @brief   Trigger a bootloader reset, start the bootloader after reset
 */
void usb_board_reset_in_bootloader(void);

#ifdef __cplusplus
}
#endif

/** @} */
