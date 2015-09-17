/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  usbdev_stack
 * @{
 *
 * @file
 * @brief       USB device stack configuration.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef USBDEV_CONFIG_H
#define USBDEV_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define USBDEV_IDVENDOR                 0xDEAD
#define USBDEV_IDPRODUCT                0xBEEF
#define USBDEV_BCDDEVICE                0x0100
#define USBDEV_STRDESC_LANGID           0x0409

#define USBDEV_MAX_USER_ENDPOINTS       4

#define USBDEV_MAX_ENDPOINTS            (USBDEV_MAX_USER_ENDPOINTS + 1)

#define USBDEV_MAX_ENDPOINT_IN          (USBDEV_MAX_ENDPOINTS)
#define USBDEV_MAX_ENDPOINT_OUT         (USBDEV_MAX_ENDPOINTS)
#define USBDEV_MAX_ENDPOINT_NUM         (USBDEV_MAX_ENDPOINT_IN + USBDEV_MAX_ENDPOINT_OUT)

#define USBDEV_MAX_INTERFACES           8

#define USBDEV_EP0_MAX_PACKET           8

#define USBDEV_DESCRIPTOR_IDX_MAX       16

#ifdef __cplusplus
}
#endif

#endif
/**
 * @}
 */
