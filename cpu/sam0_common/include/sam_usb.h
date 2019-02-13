/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_sam0_common_usb sam0 USB peripheral
 * @ingroup     cpu_sam0_common
 * @brief       USB interface functions for the sam0 class devices
 *
 * @{
 *
 * @file
 * @brief       USB interface functions for the sam0 class devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef SAM_USB_H
#define SAM_USB_H

#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"
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

/**
 * @brief sam0 usb peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                             /**< Inherited usbdev struct */
    UsbDeviceDescBank banks[2 * SAM_USB_NUM_EP]; /**< Device descriptor banks */
    usbdev_ep_t endpoints[2 * SAM_USB_NUM_EP];   /**< Endpoints */
    UsbDevice *device;                           /**< Ptr to sam0 usb device
                                                      registers */
    size_t used;                                 /**< Number of bytes from the
                                                      buffer that are used */
    uint8_t buffer[SAM_USB_BUF_SPACE];           /**< Buffer space */
} sam0_common_usb_t;

/**
 * @brief sam0 common device state setup function
 *
 * @param usbdev    usb device context to initialize
 */
void sam_usbdev_setup(sam0_common_usb_t *usbdev);

#ifdef __cplusplus
}
#endif
#endif /* SAM_USB_H */
/** @} */
