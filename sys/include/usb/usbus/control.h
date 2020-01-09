/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     usb_usbus
 * @brief       USBUS control endpoint module
 *
 * @{
 *
 * @file
 * @brief       USBUS control endpoint module interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBUS_CONTROL_H
#define USB_USBUS_CONTROL_H

#include "usb/usbus.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief helper struct to divide control messages in multiple parts
 */
typedef struct {
    size_t start;                   /**< Start offset of the current part */
    size_t cur;                     /**< Current position in the message  */
    size_t len;                     /**< Length of the full message       */
    size_t transferred;             /**< Number of bytes transferred      */
    size_t reqlen;                  /**< Maximum length of the request    */
} usbus_control_slicer_t;

/**
 * @brief Endpoint zero event handler
 */
typedef struct {

    /**
     * @brief inherited generic handler
     */
    usbus_handler_t handler;

    /**
     * @brief Last received setup packet
     */
    usb_setup_t setup;

    /**
     * @brief Control request state machine state
     */
    usbus_control_request_state_t control_request_state;

    /**
     * @brief Slicer state for multipart control request messages
     */
    usbus_control_slicer_t slicer;

    /**
     * @brief Received bytes for set requests
     */
    size_t received_len;

    /**
     * @brief EP0 OUT endpoint
     */
    usbdev_ep_t *out;

    /**
     * @brief EP0 IN endpoint
     */
    usbdev_ep_t *in;
} usbus_control_handler_t;

/**
 * @brief Initialize the control endpoint handler
 *
 * @param[in] usbus     USBUS context
 * @param[in] handler   control handler to initialize
 */
void usbus_control_init(usbus_t *usbus, usbus_control_handler_t *handler);

/**
 * @brief Helper function for adding bytes to the current control message part
 *
 * @param[in] usbus     USBUS context
 * @param[in] buf       Buffer to add bytes from
 * @param[in] len       Length of @p buf
 *
 * @return              Actual number of bytes written
 */
size_t usbus_control_slicer_put_bytes(usbus_t *usbus, const uint8_t *buf,
                                      size_t len);

/**
 * @brief Helper function for adding single bytes to the current control
 * message part
 *
 * @param[in] usbus     USBUS context
 * @param[in] c         byte to add
 *
 * @return              Actual number of bytes written
 */
size_t usbus_control_slicer_put_char(usbus_t *usbus, char c);

/**
 * @brief Helper function to signal the end of the control message
 *
 * @param[in] usbus     USBUS context
 */
void usbus_control_slicer_ready(usbus_t *usbus);

/**
 * @brief Initialize the next slice of the control message
 *
 * @param[in] usbus     USBUS context
 *
 * @return              1 when there is a next slice
 * @return              0 when the data is fully transferred
 */
int usbus_control_slicer_nextslice(usbus_t *usbus);

/**
 * @brief Retrieve the data from the OUT pipe of the control endpoint
 *
 * @pre usbus->state == USBUS_CONTROL_REQUEST_STATE_OUTDATA
 *
 * @param[in]   usbus       USBUS context
 * @param[out]  len         Length of the data part
 *
 * @return                  pointer to the data buffer
 */
uint8_t *usbus_control_get_out_data(usbus_t *usbus, size_t *len);

#ifdef __cplusplus
}
#endif
#endif /* USB_USBUS_CONTROL_H */
/** @} */
