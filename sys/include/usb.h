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
extern "c" {
#endif

/**
 * @brief USB peripheral device vendor ID
 */
#ifndef USB_CONFIG_VID
#define USB_CONFIG_VID          (0x046d)
#endif

/**
 * @brief USB peripheral device product ID
 */
#ifndef USB_CONFIG_PID
#define USB_CONFIG_PID          (0x5678)
#endif

/**
 * @brief USB peripheral manufacturer string
 */
#ifndef USB_CONFIG_MANUF_STR
#define USB_CONFIG_MANUF_STR   "RIOT-os.org"
#endif

/**
 * @brief USB peripheral product string
 */
#ifndef USB_CONFIG_PRODUCT_STR
#define USB_CONFIG_PRODUCT_STR  "USB device"
#endif

/**
 * @brief USB peripheral configuration string
 */
#ifndef USB_CONFIG_CONFIGURATION_STR
#define USB_CONFIG_CONFIGURATION_STR    "USB config"
#endif

/**
 * @brief USB peripheral device version
 *
 * Should not be confused with the USB version
 */
#ifndef USB_CONFIG_PRODUCT_BCDVERSION
#define USB_CONFIG_PRODUCT_BCDVERSION   "0x0100"
#endif

/**
 * @brief USB peripheral setting to indicate self powered devices.
 */
#ifndef USB_CONFIG_SELF_POWERED
#define USB_CONFIG_SELF_POWERED   (0)
#endif

/**
 * @brief USB device max power draw in mA, between 0 and 500mA
 */
#ifndef USB_CONFIG_MAX_POWER
#define USB_CONFIG_MAX_POWER   (100)
#endif

/**
 * @brief Default LANG ID reported to the host
 */
#ifndef USB_CONFIG_DEFAULT_LANGID
#define USB_CONFIG_DEFAULT_LANGID   0x0409 /* EN-US */
#endif

/**
 * @brief USB device class numbers
 * @{
 */
#define USB_CLASS_AUDIO             0x01    /**< Audio device               */
#define USB_CLASS_CDC_CONTROL       0x02    /**< CDC control interface      */
#define USB_CLASS_HID               0x03    /**< Human Interface device     */
#define USB_CLASS_PHYSICAL          0x05    /**< Physical device class      */
#define USB_CLASS_IMAGE             0x06    /**< Image device class         */
#define USB_CLASS_PRINTER           0x07    /**< Printer device class       */
#define USB_CLASS_MASS_STORAGE      0x08    /**< Mass storage device class  */
#define USB_CLASS_CDC_DATA          0x0a    /**< CDC data specification     */
#define USB_CLASS_VENDOR            0xff    /**< Vendor specific class      */
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
