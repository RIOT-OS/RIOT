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

#include "board.h"

/**
 * @defgroup usb_conf USB peripheral compile time configurations
 * @ingroup config
 * @{
 */

/* These can be overridden by boards that should come up with their board
 * supplier VID/PID pair. Boards should only override this if the RIOT built-in
 * peripherals are compatible with whatever is usually shipped with that pair
 * */
#ifndef INTERNAL_PERIPHERAL_VID
/** Reserved for RIOT standard peripherals as per http://pid.codes/1209/7D00/ */
#define INTERNAL_PERIPHERAL_VID (0x1209)
#endif
#ifndef INTERNAL_PERIPHERAL_PID
/** Reserved for RIOT standard peripherals as per http://pid.codes/1209/7D00/ */
#define INTERNAL_PERIPHERAL_PID (0x7D00)
#endif

#if !(defined(CONFIG_USB_VID) && defined(CONFIG_USB_PID))
#ifdef USB_H_USER_IS_RIOT_INTERNAL
#define CONFIG_USB_VID INTERNAL_PERIPHERAL_VID
#define CONFIG_USB_PID INTERNAL_PERIPHERAL_PID
#else
#error Please configure your vendor and product IDs. For development, you may \
    set USB_VID=${USB_VID_TESTING} USB_PID=${USB_PID_TESTING}.
#endif
#else
#if CONFIG_USB_VID == INTERNAL_PERIPHERAL_VID && \
    CONFIG_USB_PID == INTERNAL_PERIPHERAL_PID
#error Please configure your vendor and product IDs differently than the \
    INTERNAL_PERIPHERAL_* settings. For development, you may set \
    USB_VID=${USB_VID_TESTING} \
    USB_PID=${USB_PID_TESTING}.
#endif
#endif /* !(defined(CONFIG_USB_VID) && defined(CONFIG_USB_PID)) */

/**
 * @brief USB peripheral device vendor ID
 *
 * @note You must provide your own VID/PID combination when manufacturing a
 * device with USB.
 */
#ifdef DOXYGEN
#define CONFIG_USB_VID
#endif

/**
 * @brief USB peripheral device product ID
 *
 * @note You must provide your own VID/PID combination when manufacturing a
 * device with USB.
 */
#ifdef DOXYGEN
#define CONFIG_USB_PID
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
#define CONFIG_USB_PRODUCT_STR  RIOT_BOARD
#endif

/**
 * @brief USB peripheral configuration string
 */
#ifndef CONFIG_USB_CONFIGURATION_STR
#define CONFIG_USB_CONFIGURATION_STR    "USB config"
#endif

/**
 * @brief USB peripheral serial string
 *
 * Compile-time value to override the serial string with. An LUID-based hex
 * string is generated when this value is not used.
 *
 * This string does not have to be a number, but it must be unique between
 * devices with identical VID:PID combination.
 */
#ifdef DOXYGEN
#define CONFIG_USB_SERIAL_STR           "RIOT-12345"
#endif

/**
 * @brief USB peripheral serial string length
 *
 * Maximum value is 63. Sensible values are between 8 to 32 depending on the
 * number of expected devices.
 *
 * The length here is in bytes. The generated hex string is twice as many chars
 * in length due to conversion from bytes to hex chars.
 */
#if !defined(CONFIG_USB_SERIAL_STR) && !defined(CONFIG_USB_SERIAL_BYTE_LENGTH)
#define CONFIG_USB_SERIAL_BYTE_LENGTH 8
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
 * @brief USB peripheral setting to indicate remote wakeup capability.
 */
#ifndef CONFIG_USB_REM_WAKEUP
#define CONFIG_USB_REM_WAKEUP   (0)
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
 * @brief RIOT-internal USB peripheral clearance indicator
 *
 * This define must only be set in compilation units that are RIOT internal,
 * and only when they implement peripherals that can be considered default RIOT
 * peripherals.
 *
 * When this is defined in all uses of `usb.h`, the board can use the
 * 0x1209/0x7D00 VID/PID pair unless explicit configuration using @ref
 * CONFIG_USB_VID and @ref CONFIG_USB_PID say otherwise.
 *
 * There is no sharp characterization of what consititutes an internal
 * peripheral; a good check is this: If an application can, just by switching
 * between boards, can have a feature provided by either RIOT's USB stack or a
 * different mechanism, the USB version is a default RIOT peripheral.
 *
 * Examples are stdio access (is provided by most boards using a UART and an
 * external USB UART adapter), Ethernet (is provided by other boards using
 * ethos) and firmware upload and reset (is provided by other boards using an
 * on-board programmer).
 *
 * See http://pid.codes/1209/7D00/ for the allocation of that code.
 * @{
 */
#ifdef DOXYGEN
#define USB_H_USER_IS_RIOT_INTERNAL
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

/**
 * @brief Maximum transfer size for interrupt endpoints at full speed
 */
#define USB_ENDPOINT_INTERRUPT_FS_MAX_SIZE  (64)

/**
 * @brief Maximum transfer size for interrupt endpoints at high speed
 */
#define USB_ENDPOINT_INTERRUPT_HS_MAX_SIZE  (1024)

/**
 * @brief Maximum transfer size for bulk endpoints at full speed
 */
#define USB_ENDPOINT_BULK_FS_MAX_SIZE  (64)

/**
 * @brief Maximum transfer size for bulk endpoints at high speed
 */
#define USB_ENDPOINT_BULK_HS_MAX_SIZE  (512)

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USB_H */
