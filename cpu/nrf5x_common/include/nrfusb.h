/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_nrf52_nrfusb NRF usb peripheral implementation
 * @ingroup     cpu_nrf52
 * @brief       Minimal driver for the NRF52840 usb peripheral
 *
 * @{
 *
 * @file
 * @brief       USB interface functions for the nrf52840 class devices
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef NRFUSB_H
#define NRFUSB_H

#include <stdint.h>
#include <stddef.h>
#include "periph/usbdev.h"
#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Number of USB peripherals on the MCU
 */
#define NRF_USB_NUM_PERIPH 1

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
    nrfusb_setup_state_t sstate;            /**< Setup request state machine    */
} nrfusb_t;

#ifdef __cplusplus
}
#endif
#endif /* NRFUSB_H */
/** @} */
