/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */
/**
 * @defgroup    usb   USB
 * @ingroup     sys
 * @brief       Configuration defines for USB peripheral devices.
 * @{
 *
 * @file
 * @brief       Definition of global compile time configuration options
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_H
#define USB_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup usb_conf USB peripheral compile time configurations
 * @ingroup config
 * @{
 */
/**
 * @brief USB peripheral device vendor ID
 *
 * @note You must provide your own VID/PID combination when manufacturing a
 * device with USB.
 */
#ifndef CONFIG_USB_VID
#ifdef DOXYGEN
#define CONFIG_USB_VID
#else
#error  Please supply your vendor ID by setting CONFIG_USB_VID
#endif
#endif

/**
 * @brief USB peripheral device product ID
 *
 * @note You must provide your own VID/PID combination when manufacturing a
 * device with USB.
 */
#ifndef CONFIG_USB_PID
#ifdef DOXYGEN
#define CONFIG_USB_PID
#else
#error  Please supply your vendor ID by setting CONFIG_USB_PID
#endif
#endif

/**
 * @brief USB peripheral manufacturer string
 */
#ifndef CONFIG_USB_MANUF_STR
#define CONFIG_USB_MANUF_STR   "RIOT-os.org"
#endif

/**
 * @brief USB peripheral product string
 */
#ifndef CONFIG_USB_PRODUCT_STR
#define CONFIG_USB_PRODUCT_STR  "USB device"
#endif

/**
 * @brief USB peripheral configuration string
 */
#ifndef CONFIG_USB_CONFIGURATION_STR
#define CONFIG_USB_CONFIGURATION_STR    "USB config"
#endif

/**
 * @brief USB peripheral device version
 *
 * This is the version number of this peripheral
 * @note Not to be be confused with the USB version number
 */
#ifndef CONFIG_USB_PRODUCT_BCDVERSION
#define CONFIG_USB_PRODUCT_BCDVERSION   0x0100
#endif

/**
 * @brief USB specification version
 */
#ifndef CONFIG_USB_SPEC_BCDVERSION
#if defined(CONFIG_USB_SPEC_BCDVERSION_1_1)
#define CONFIG_USB_SPEC_BCDVERSION      0x0110
#elif defined(CONFIG_USB_SPEC_BCDVERSION_2_0)
#define CONFIG_USB_SPEC_BCDVERSION      0x0200
#else
#define CONFIG_USB_SPEC_BCDVERSION      0x0200
#endif
#endif

/**
 * @brief USB peripheral setting to indicate self powered devices.
 */
#ifndef CONFIG_USB_SELF_POWERED
#define CONFIG_USB_SELF_POWERED   (0)
#endif

/**
 * @brief USB device max power draw in mA, between 0 and 500mA
 */
#ifndef CONFIG_USB_MAX_POWER
#define CONFIG_USB_MAX_POWER   (100)
#endif

/**
 * @brief Default LANG ID reported to the host
 */
#ifndef CONFIG_USB_DEFAULT_LANGID
#define CONFIG_USB_DEFAULT_LANGID   0x0409 /* EN-US */
#endif
/** @} */

/**
 * @brief USB version definitions
 */
typedef enum {
    USB_VERSION_1x,     /* USB 1.0 or 1.1 device    */
    USB_VERSION_20,     /* USB 2.0 device           */
} usb_version_t;

/**
 * @brief USB speed definitions
 */
typedef enum {
    USB_SPEED_LOW,      /* Low speed (1.5Mbit/s)    */
    USB_SPEED_FULL,     /* Full speed (12Mbit/s)    */
    USB_SPEED_HIGH,     /* High speed (480Mbit/s)   */
} usb_speed_t;

/**
 * @brief USB endpoint types
 */
typedef enum {
    USB_EP_TYPE_NONE = 0,       /**< Unused endpoint    */
    USB_EP_TYPE_CONTROL,        /**< Control type       */
    USB_EP_TYPE_INTERRUPT,      /**< Interrupt type     */
    USB_EP_TYPE_BULK,           /**< Bulk type          */
    USB_EP_TYPE_ISOCHRONOUS,    /**< Isochronous type   */
} usb_ep_type_t;

/**
 * @brief USB endpoint directions
 */
typedef enum {
    USB_EP_DIR_OUT, /**< Host out, device in */
    USB_EP_DIR_IN,  /**< Host in, device out */
} usb_ep_dir_t;

#ifdef __cplusplus
}
#endif

#endif /* USB_H */
/** @} */
