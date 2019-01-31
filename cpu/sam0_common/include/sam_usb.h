/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_uuid RFC 4122 compliant UUID's
 * @ingroup     sys
 * @brief       Provides RFC 4122 compliant UUID's
 *
 * This module provides RFC 4122 compliant UUID generation. The UUID stored in
 * @ref uuid_t struct is stored in network byte order.
 *
 * @{
 *
 * @file
 * @brief       [RFC 4122](https://tools.ietf.org/html/rfc4122) UUID functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SAM_USB_H
#define SAM_USB_H

#include <stdint.h>
#include <stdlib.h>
#include "usb/usbdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * USB endpoint buffer space
 *
 * @todo global configurable?
 */
#define SAM_USB_BUF_SPACE   USBDEV_EP_BUF_SPACE

/**
 * Number of USB IN and OUT endpoints
 */
#define SAM_USB_NUM_EP      USBDEV_NUM_ENDPOINTS

typedef struct {
    usbdev_t usbdev;
    size_t used;                        /**< Number of bytes from the
                                             buffer that are used */
    uint8_t buffer[SAM_USB_BUF_SPACE];  /**< Buffer space */
} sam0_common_usb_t;

void usb_init(void);

void usb_attach(void);
void usb_detach(void);
#ifdef __cplusplus
}
#endif
#endif /* SAM_USB_H */
/** @} */
