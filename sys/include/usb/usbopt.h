/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */
/**
 * @defgroup    usb_usbopt   usbopt - Configuration options for USB APIs
 * @ingroup     usb
 * @brief       List of available configuration options for the
 *              @ref drivers_periph_usbdev
 * @{
 *
 * @file
 * @brief       Definition of global USB peripheral and USB peripheral endpoint
 *              configuration options.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBOPT_H
#define USB_USBOPT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   List of configuration settings for USB peripherals
 *
 * The data type specified in parentheses for each individual option is the
 * data type to use for the argument when getting/setting the value of the option.
 */
typedef enum {

    /**
     * @brief   (uint8_t) USB device address, limited to 7 bit by the protocol
     */
    USBOPT_ADDRESS,

    /**
     * @brief   (usbopt_enable_t) Attach/detach USB peripheral to host
     *
     * In practice this happens by enabling or disabling the pull-up resistor
     * on one of the data lines.
     */
    USBOPT_ATTACH,

    /**
     * @brief (usbopt_enable_t) Whether a USB host connection is detected
     *
     * Setting this option must return -ENOTSUP
     */
    USBOPT_CONNECTED,

    /**
     * @brief   (usb_version_t) Highest USB version supported by peripheral
     *
     * Setting this option must return -ENOTSUP
     */
    USBOPT_MAX_VERSION,

    /**
     * @brief   (usb_version_t) Highest USB speed supported by peripheral
     *
     * Setting this option must return -ENOTSUP
     */
    USBOPT_MAX_SPEED,

    /**
     * @brief   (usb_version_t) Speed at which the usb peripheral got enumerated by the host
     *
     * Should be equal or slower than @ref USBOPT_MAX_SPEED
     *
     * Setting this option must return -ENOTSUP
     */
    USBOPT_ENUMERATED_SPEED,

    /* expand list if required */
} usbopt_t;

/**
 * @brief   List of configuration settings for USB peripheral endpoints
 *
 * The data type specified in parentheses for each individual option is the
 * data type to use for the argument when getting/setting the value of the option.
 */
typedef enum {

    /**
     * @brief   (usbopt_enable_t) Enable or disable the endpoint
     */
    USBOPT_EP_ENABLE,

    /**
     * @brief   (usbopt_enable_t) Enable or disable stall replies for endpoint
     */
    USBOPT_EP_STALL,

    /**
     * @brief   (size_t) Retrieve number of bytes available on endpoint.
     */
    USBOPT_EP_AVAILABLE,

    /* expand list if required */
} usbopt_ep_t;

/**
 * @brief   Binary parameter for enabling and disabling options
 */
typedef enum {
    USBOPT_DISABLE = 0,         /**< disable a given option */
    USBOPT_ENABLE = 1,          /**< enable a given option */
} usbopt_enable_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USB_USBOPT_H */
