/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_nrf5x_nrfusb NRF usb peripheral implementation
 * @ingroup     drivers_usb
 * @brief       Minimal driver for the NRF52840 usb peripheral
 *
 * @{
 *
 * @file
 * @brief       USB interface functions for the nrf52 class devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef NRF_USB_H
#define NRF_USB_H

#include <stdint.h>
#include <stddef.h>
#include "usb/usbdev.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * USB endpoint buffer space
 *
 * @todo global configurable?
 */
#define NRF_USB_BUF_SPACE   USBDEV_EP_BUF_SPACE

/**
 * Number of USB IN and OUT endpoints
 */
#define NRF_USB_NUM_EP      USBDEV_NUM_ENDPOINTS

/**
 * @brief nrfusb setup packet state tracker for endpoint 0 handling
 */
typedef enum {
    NRFUSB_SETUP_READY,     /**< Ready for a new setup request        */
    NRFUSB_SETUP_READ,      /**< Read request received                */
    NRFUSB_SETUP_WRITE,     /**< Write request received               */
    NRFUSB_SETUP_ACKOUT,    /**< Expecting an ACK on the out endpoint */
    NRFUSB_SETUP_ACKIN,     /**< Expecting an ACK on the in endpoint  */
} nrfusb_setup_state_t;

/**
 * @brief nrf usb peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                        /**< Inherited usbdev struct        */
    usbdev_ep_t ep_ins[NRF_USB_NUM_EP];     /**< IN type endpoints              */
    usbdev_ep_t ep_outs[ NRF_USB_NUM_EP];   /**< OUT type endpoints             */
    NRF_USBD_Type *device;                  /**< Ptr to the device registers    */
    size_t used;                            /**< Number of bytes from the
                                                 buffer that are used           */
    uint8_t buffer[NRF_USB_BUF_SPACE];      /**< Buffer space for endpoint data */
    nrfusb_setup_state_t sstate;            /**< Setup request state machine    */
} nrfusb_t;

/**
 * @brief nrf52 usb device state setup function
 *
 * @param usbdev    usb device context to initialize
 */
void nrf_usbdev_setup(nrfusb_t *usbdev);

#ifdef __cplusplus
}
#endif
#endif /* SAM_USB_H */
/** @} */
