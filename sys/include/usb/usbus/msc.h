/*
 * Copyright (C) 2019-2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 *
 *
 */

#ifndef USB_USBUS_MSC_H
#define USB_USBUS_MSC_H

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

#include <stdint.h>
#include "usb/usbus.h"
#include "usb/usbus/msc/scsi.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USBUS MSC bulk data endpoint size.
 */
#if defined(MODULE_PERIPH_USBDEV_HS_UTMI) || defined(MODULE_PERIPH_USBDEV_HS_ULPI)
#define USBUS_MSC_EP_DATA_SIZE  512
#else
#define USBUS_MSC_EP_DATA_SIZE  64
#endif

/**
 * @brief Number of IN EPs required for the MSC interface
 */
#define USBUS_MSC_EP_IN_REQUIRED_NUMOF   1

/**
 * @brief Number of Out EPs required for the MSC interface
 */
#define USBUS_MSC_EP_OUT_REQUIRED_NUMOF  1

/**
 * @brief USBUS MSC internal state machine enum
 */
typedef enum {
    WAITING,            /**< Initial state, wait for USB setup */
    WAIT_FOR_TRANSFER,  /**< Wait for a single packet transfer before sending CSW */
    DATA_TRANSFER_IN,   /**< Ongoing transfer on USB MSC IN endpoint */
    DATA_TRANSFER_OUT,  /**< Ongoing transfer on USB MSC OUT endpoint */
    GEN_CSW             /**< Generate CSW response to host w/ the current transfer status */
} usbus_msc_state_t;

/**
 * @brief USBUS MSC Logical Unit descriptor
 */
typedef struct usbus_msc_lun {
    mtd_dev_t *mtd;         /**< Pointer to the current mtd device used */
    uint32_t block_nb;      /**< Reported USB block number */
    uint32_t block_size;    /**< Reported USB block size */
} usbus_msc_lun_t;

/**
 * @brief USBUS MSC device interface context
 */
typedef struct usbus_msc_device {
    usbus_handler_t handler_ctrl;    /**< Control interface handler */
    usbus_interface_t iface;         /**< MSC interface */
    usbus_endpoint_t *ep_in;         /**< Data endpoint in */
    usbus_endpoint_t *ep_out;        /**< Data endpoint out */
    uint8_t *out_buf;                /**< Pointer to internal out endpoint buffer */
    uint8_t *in_buf;                 /**< Pointer to internal in endpoint buffer */
    usbus_descr_gen_t msc_descr;     /**< MSC descriptor generator */
    usbus_t *usbus;                  /**< Pointer to the USBUS context */
    cbw_info_t cmd;                  /**< Command Block Wrapper information */
    event_t rx_event;                /**< Transmit ready event */
    usbus_msc_state_t state;         /**< Internal state machine for msc */
    uint8_t *buffer;                 /**< Pointer to the current data transfer buffer */
    uint32_t buffer_size;            /**< Size of the internal buffer used for data transfer */
    uint32_t block;                  /**< First block to transfer data from/to */
    uint16_t block_nb;               /**< Number of block to transfer for READ and
                                          WRITE operations */
    uint16_t block_offset;           /**< Internal offset for endpoint size chunk transfer */
    usbus_msc_lun_t *lun_dev;        /**< Array holding exported logical unit descriptor */
} usbus_msc_device_t;

/**
 * @brief MSC initialization function
 *
 * @param   usbus   USBUS thread to use
 * @param   handler MSC device struct
 */

int usbus_msc_init(usbus_t *usbus, usbus_msc_device_t *handler);

/**
 * @brief Register a MTD device as a MSC LUN (Logical Unit Number)
 *
 * @param[in]   usbus   USBUS context
 * @param[in]   dev     pointer to the MTD device to export
 *
 * @return      0 on success
 *              -ENOMEM, if the selected MTD device doesn't have enough memory
 *              -EAGAIN, if no more MTD devices can be exported
 *              -EBUSY, if the MTD device is already exported
 */
int usbus_msc_add_lun(usbus_t *usbus, mtd_dev_t *dev);

/**
 * @brief Unregister a MTD device as a MSC LUN
 *
 * @param[in]   usbus   USBUS context
 * @param[in]   dev     pointer to the MTD device to export
 *
 * @return      0 on success
 *              -ENODEV, if no matching MTD device was found
 */
int usbus_msc_remove_lun(usbus_t *usbus, mtd_dev_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USB_USBUS_MSC_H */
