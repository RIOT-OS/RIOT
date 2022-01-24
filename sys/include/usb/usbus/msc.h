/*
 * Copyright (C) 2019-2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 *
 *
 */

/**
 * @ingroup     usbus_msc
 *
 * @{
 *
 * @file
 * @brief       USBUS Mass Storage Class functions definitions
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef USB_USBUS_MSC_H
#define USB_USBUS_MSC_H

#include <stdint.h>
#include "usb/usbus.h"
#include "usb/usbus/msc/scsi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USBUS_MSC_BUFFER_SIZE
#define USBUS_MSC_BUFFER_SIZE 512
#endif /* USBUS_MSC_BUFFER_SIZE */

typedef enum {
    WAITING,
    WAIT_FOR_TRANSFER,
    DATA_TRANSFER_IN,
    DATA_TRANSFER_OUT,
    DATA_TRANSFER_LAST,
    GEN_CSW
} usbus_msc_state_t;

/**
 * @brief USBUS MSC device interface context
 */
typedef struct usbus_msc_device {
    usbus_handler_t handler_ctrl;   /**< Control interface handler */
    usbus_interface_t iface;        /**< MSC interface */
    usbus_endpoint_t *ep_in;        /**< Data endpoint in */
    usbus_endpoint_t *ep_out;       /**< Data endpoint out */
    uint8_t* out_buf;               /**< Pointer to internal out endpoint buffer */
    uint8_t* in_buf;                /**< Pointer to internal in endpoint buffer */
    usbus_descr_gen_t msc_descr;    /**< MSC descriptor generator */
    usbus_t *usbus;                 /**< Pointer to the USBUS context */
    cbw_info_t cmd;                 /**< Command Block Wrapper information */
    event_t rx_event;               /**< Transmit ready event */
    usbus_msc_state_t state;        /**< Internal state machine for msc */
    uint8_t *buffer;                /**< Pointer to the current data transfer buffer */
    uint32_t block;                 /**< First block to transfer data from/to */
    uint16_t block_nb;              /**< Number of block to transfer for READ and
                                         WRITE operations */
    uint16_t block_offset;          /**< Internal offset for endpoint size chunk transfer */
    uint8_t  pages_per_vpage;       /**< Number of pages to fill host expected page size */
} usbus_msc_device_t;

/**
 * @brief MSC initialization function
 *
 * @param   usbus   USBUS thread to use
 * @param   handler MSC device struct
 */

int usbus_msc_init(usbus_t *usbus, usbus_msc_device_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* USB_USBUS_MSC_H */
/** @} */
