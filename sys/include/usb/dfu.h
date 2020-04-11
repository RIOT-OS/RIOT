/*
 * Copyright (C) 2020 Dylan Laduranty
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

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

#ifndef USB_DFU_H
#define USB_DFU_H

#include <stdint.h>

#include "usb.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_IF_DESCRIPTOR_DFU           0x21   /**< USB DFU type descriptor */
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
 * @name    Default USB transfer size for DFU descriptor
 * @{
 */
#ifndef USB_DFU_TRANSFER_SIZE
#define USB_DFU_TRANSFER_SIZE FLASHPAGE_SIZE
#endif
/** @} */

/**
 * @name USB DFU interface attribute bit flags
 * @{
 */
#define USB_DFU_CAN_DOWNLOAD            0x01    /**<  USB DFU download capability flag */
#define USB_DFU_CAN_UPLOAD              0x02    /**<  USB DFU upload capability flag */
#define USB_DFU_MANIFEST_TOLERANT       0x04    /**<  USB DFU manifest tolerant capability flag */
#define USB_DFU_WILL_DETACH             0x08    /**<  USB DFU detach capability flag */
/** @} */

/**
 * @name USB DFU interface type
 * @{
 */
#define USB_DFU_INTERFACE               0xFE /**< Application Specific Interface */
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
#define DFU_DETACH      0x00        /**< DFU detach request */
#define DFU_DOWNLOAD    0x01        /**< DFU download request */
#define DFU_UPLOAD      0x02        /**< DFU upload request */
#define DFU_GET_STATUS  0x03        /**< DFU get status request */
#define DFU_CLR_STATUS  0x04        /**< DFU clear status request */
#define DFU_GET_STATE   0x05        /**< DFU get state request */
#define DFU_ABORT       0x06        /**< DFU abort operation request */
/** @} */

/**
 * @name USBUS DFU startup mode to be used with @ref usbus_dfu_init
 * @{
 */
typedef enum {
    USBUS_APP_MODE,         /**< USBUS DFU application mode selection */
    USBUS_DFU_MODE          /**< USBUS DFU runtime mode selection */
} usbus_dfu_startup_mode_t;
/** @} */

/**
 * @name USBUS DFU internal state machine
 * @{
 */
typedef enum {
    USBUS_DFU_STATE_APP_IDLE,               /**< DFU application idle */
    USBUS_DFU_STATE_APP_DETACH,             /**< DFU application detach (reboot to DFU mode) */
    USBUS_DFU_STATE_DFU_IDLE,               /**< DFU runtime mode idle */
    USBUS_DFU_STATE_DFU_DL_SYNC,            /**< DFU download synchronization */
    USBUS_DFU_STATE_DFU_DL_BUSY,            /**< DFU download busy */
    USBUS_DFU_STATE_DFU_DL_IDLE,            /**< DFU download idle */
    USBUS_DFU_STATE_DFU_MANIFEST_SYNC,      /**< DFU manifest synchronization */
    USBUS_DFU_STATE_DFU_MANIFEST,           /**< DFU manifest mode */
    USBUS_DFU_STATE_DFU_MANIFEST_WAIT_RST,  /**< DFU manifest wait for CPU reset */
    USBUS_DFU_STATE_DFU_UP_IDLE,            /**< DFU upload idle */
    USBUS_DFU_STATE_DFU_ERROR               /**< DFU internal error */
} usbus_dfu_state_t;
/** @} */
/**
 * @brief USB DFU interface descriptor
 */
typedef struct __attribute__((packed)) {
    uint8_t length;              /**< Descriptor length */
    uint8_t type;                /**< Descriptor type */
    uint8_t attribute;           /**< Descriptor attributes flags */
    uint16_t detach_timeout;     /**< Descriptor detach timeout (ms) */
    uint16_t xfer_size;          /**< Descriptor transfer size */
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

/**
 * @brief USBUS DFU device interface context
 */
typedef struct usbus_dfu_device {
    usbus_handler_t handler_ctrl;           /**< Control interface handler */
    usbus_interface_t iface;                /**< Control interface */
    usbus_descr_gen_t dfu_descr;            /**< DFU descriptor generator */
    usbus_t *usbus;                         /**< Ptr to the USBUS context */
    unsigned mode;                          /**< 0 - APP mode, 1 DFU mode */
} usbus_dfu_device_t;

/**
 * @brief DFU initialization function
 *
 * @param   usbus   USBUS thread to use
 * @param   handler DFU device struct
 * @param   mode    DFU start mode (0 runtime mode / 1 dfu mode)
 */
void usbus_dfu_init(usbus_t *usbus, usbus_dfu_device_t *handler, usbus_dfu_startup_mode_t mode);

/**
 * @brief DFU control request handler
 *
 * @param   usbus   USBUS thread to use
 * @param   handler DFU device struct
 */
void dfu_control_req(usbus_t *usbus, usbus_control_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* USB_DFU_H */
/** @} */
