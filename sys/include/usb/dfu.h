/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    usb_dfu   DFU - USB Device Firmware Upgrade
 * @ingroup     usb
 * @brief       Generic USB DFU defines and helpers
 *
 * @{
 *
 * @file
 * @brief       Definition for USB DFU interfaces
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include <stdint.h>

#include "usb.h"
#include "usb/descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_IF_DESCRIPTOR_DFU           0x21   /**< USB DFU type descriptor*/
#define USB_DFU_VERSION_BCD             0x0110 /**< USB DFU version in BCD */

/**
 * @name    Default USB detach timeout for DFU descriptor
 * @{
 */
#ifndef USB_DFU_DETACH_TIMEOUT_MS
#define USB_DFU_DETACH_TIMEOUT_MS 255
#endif
/** @} */

/**
 * @name USB DFU interface attributes
 * @{
 */
#define USB_DFU_CAN_DOWNLOAD            0x01    /**< DFU Download attribute */
#define USB_DFU_CAN_UPLOAD              0x02    /**< DFU Upload attribute */
#define USB_DFU_MANIFEST_TOLERANT       0x04    /**< DFU Manifest tolerant attribute */
#define USB_DFU_WILL_DETACH             0x08    /**< DFU Detach capability attribute */
/** @} */

/**
 * @name USB DFU interface type
 * @{
 */
#define USB_DFU_INTERFACE               0xFE /** Application Specific Interface */
/** @} */

/**
 * @name USB DFU subclass types
 * @anchor usb_dfu_subtype
 * @{
 */
#define USB_DFU_SUBCLASS_DFU            0x01 /**< DFU subclass */
/** @} */

/**
 * @name USB DFU protocol types
 * @{
 */
#define USB_DFU_PROTOCOL_RUNTIME_MODE   0x01 /**< Runtime mode */
#define USB_DFU_PROTOCOL_DFU_MODE       0x02 /**< DFU mode */
/** @} */

/**
 * @name USB DFU setup request
 * @{
 */
#define DFU_DETACH      0x00        /**< DFU Detach request */
#define DFU_DOWNLOAD    0x01        /**< DFU Download request */
#define DFU_UPLOAD      0x02        /**< DFU Upload request */
#define DFU_GET_STATUS  0x03        /**< DFU Get Status request */
#define DFU_CLR_STATUS  0x04        /**< DFU Clear Status request */
#define DFU_GET_STATE   0x05        /**< DFU Get State request */
#define DFU_ABORT       0x06        /**< DFU Abort request */
/** @} */

/**
 * @brief USBUS DFU internal state
 */
typedef enum {
    USB_DFU_STATE_APP_IDLE,               /**< DFU application idle */
    USB_DFU_STATE_APP_DETACH,             /**< DFU application detach (reboot to DFU mode) */
    USB_DFU_STATE_DFU_IDLE,               /**< DFU runtime mode idle */
    USB_DFU_STATE_DFU_DL_SYNC,            /**< DFU download synchronization */
    USB_DFU_STATE_DFU_DL_BUSY,            /**< DFU download busy */
    USB_DFU_STATE_DFU_DL_IDLE,            /**< DFU download idle */
    USB_DFU_STATE_DFU_MANIFEST_SYNC,      /**< DFU manifest synchronization */
    USB_DFU_STATE_DFU_MANIFEST,           /**< DFU manifest mode */
    USB_DFU_STATE_DFU_MANIFEST_WAIT_RST,  /**< DFU manifest wait for CPU reset */
    USB_DFU_STATE_DFU_UP_IDLE,            /**< DFU upload idle */
    USB_DFU_STATE_DFU_ERROR               /**< DFU internal error */
} usb_dfu_state_t;

/**
 * @brief USB DFU interface descriptor
 */
typedef struct __attribute__((packed)) {
    uint8_t length;              /**< Descriptor length */
    uint8_t type;                /**< Descriptor type */
    uint8_t attribute;           /**< Descriptor attributes flags */
    uint16_t detach_timeout;     /**< Descriptor detach timeout (ms) */
    uint16_t xfer_size;          /**< Descriptor transaction size */
    uint16_t bcd_dfu;            /**< Descriptor bcd version */
} usb_desc_if_dfu_t;

/**
 * @brief USB DFU get_status control request packet
 */
typedef struct __attribute__((packed))  {
    uint8_t status;             /**< DFU status response */
    uint32_t timeout : 24;      /**< DFU timeout (ms) response */
    uint8_t state;              /**< DFU internal state machine */
    uint8_t string;             /**< DFU string */
} dfu_get_status_pkt_t;

#ifdef __cplusplus
}
#endif

/** @} */
