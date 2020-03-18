/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef USB_BOARD_RESET_H
#define USB_BOARD_RESET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#include "usb/usbus/cdc/acm.h"

/**
 * @brief   USB coding callback used to trigger the board reset
 *
 * @param[in] cdcacm    Pointer to the cdcacm device
 * @param[in] baud      Baudrate used by the client. Only 1200 baud is taken into account
 * @param[in] bits      Number of bit mode used by the client
 * @param[in] parity    Parity mode used by the client
 * @param[in] stop      Stop bit mode used by the client
 *
 * @return  Always return 0
 */
int usb_board_reset_coding_cb(usbus_cdcacm_device_t *cdcacm,
                              uint32_t baud, uint8_t bits,
                              uint8_t parity, uint8_t stop);

/**
 * @brief   Trigger a simple reset, back to the application
 */
void usb_board_reset_in_application(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_BOARD_RESET_H */
/** @} */
