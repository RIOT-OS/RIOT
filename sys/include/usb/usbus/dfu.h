/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef USB_USBUS_DFU_H
#define USB_USBUS_DFU_H

/**
 * @defgroup    usbus_dfu   USBUS DFU - USB Device Firmware Upgrade
 * @ingroup     usb
 * @brief       Specific USB DFU defines and helpers for USBUS
 *
 * @{
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include "usb/dfu.h"
#include "riotboot/flashwrite.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USBUS DFU device interface context
 */
typedef struct usbus_dfu_device {
    usbus_handler_t handler_ctrl;           /**< Control interface handler */
    usbus_interface_t iface;                /**< Control interface */
    usbus_descr_gen_t dfu_descr;            /**< DFU descriptor generator */
    usbus_string_t slot0_str;               /**< Descriptor string for Slot 0 */
#ifdef MODULE_RIOTBOOT_USB_DFU
    usbus_interface_alt_t iface_alt_slot1;  /**< Alt interface for secondary slot */
    usbus_string_t slot1_str;               /**< Descriptor string for Slot 1 */
    riotboot_flashwrite_t writer;           /**< DFU firmware update state structure */
#endif
    bool skip_signature;                    /**< Skip RIOTBOOT signature status */
    usbus_t *usbus;                         /**< Ptr to the USBUS context */
    unsigned mode;                          /**< 0 - APP mode, 1 DFU mode */
    unsigned selected_slot;                 /**< Slot used for upgrade */
    usb_dfu_state_t dfu_state;              /**< Internal DFU state machine */
} usbus_dfu_device_t;

/**
 * @brief DFU initialization function
 *
 * @param   usbus   USBUS thread to use
 * @param   handler DFU device struct
 * @param   mode    DFU start mode (0 runtime mode / 1 dfu mode)
 */
void usbus_dfu_init(usbus_t *usbus, usbus_dfu_device_t *handler, unsigned mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USB_USBUS_DFU_H */
