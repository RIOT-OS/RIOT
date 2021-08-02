/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    usbus_hid_io USBUS HID IO
 * @ingroup     usb_hid
 * @brief       USBUS HID IO interface module
 *
 * @{
 *
 * @file
 * @brief       USB HID callback and read/write functions
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#ifndef USB_USBUS_HID_IO_H
#define USB_USBUS_HID_IO_H

#include <string.h>
#include <stdint.h>

#include "usb/usbus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USBUS HID IO RX callback function
 */
typedef void (*usb_hid_io_cb_t)(void*);

/**
 * @brief Set USBUS HID IO RX callback
 *
 * @param[in]   cb  callback function to use
 * @param[in]   arg argument that will be passed to @p cb
 */
void usb_hid_io_set_rx_cb(usb_hid_io_cb_t cb, void *arg);

/**
 * @brief Write data to USB HID IN endpoint buffer
 *
 * @pre `buffer != NULL`
 *
 * @param[in]   buffer  buffer containing data to write
 * @param[in]   len     length of buffer
 */
void usb_hid_io_write(const void *buffer, size_t len);

/**
 * @brief Read data from USB HID OUT endpoint (blocking)
 *
 * Wrapper for @ref isrpipe_read
 *
 * @pre `buffer != NULL`
 *
 * @param[out]      buffer  buffer to read data into
 * @param[in]       len     length of buffer
 *
 * @return Number of bytes read
 */
int usb_hid_io_read(void *buffer, size_t len);

/**
 * @brief Read data from USB HID OUT endpoint (with timeout, blocking)
 *
 * Wrapper for @ref isrpipe_read_timeout
 *
 * @pre `buffer != NULL`
 *
 * @param[out]      buffer  buffer to read data into
 * @param[in]       len     length of buffer
 * @param[in]       timeout timeout in microseconds
 *
 * @return Number of bytes read
 */
int usb_hid_io_read_timeout(void *buffer, size_t len, uint32_t timeout);

/**
 * @brief Initialize an USB HID IO interface
 *
 * @param[in]   usbus               USBUS context
 * @param[in]   report_desc         USB_HID report descriptor
 * @param[in]   report_desc_size    size of USB_HID report descriptor
 */
void usb_hid_io_init(usbus_t *usbus, const uint8_t *report_desc, size_t report_desc_size);

#ifdef __cplusplus
}
#endif

#endif /* USB_USBUS_HID_IO_H */
/** @} */
