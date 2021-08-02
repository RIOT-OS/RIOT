/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    usb_hid   HID - USB communications device class
 * @ingroup     usb
 * @brief       Generic USB HID defines and helpers
 *
 * @{
 *
 * @file
 * @brief       Definition for USB HID interfaces
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#ifndef USB_HID_H
#define USB_HID_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USB HID type descriptor
 */
#define USB_TYPE_DESCRIPTOR_HID      0x21

/**
 * @brief USB HID version in BCD
 */
#define USB_HID_VERSION_BCD          0x0110

/**
 * @name USB HID subclass types
 * @{
 */
#define USB_HID_SUBCLASS_NONE 0x0
#define USB_HID_SUBCLASS_BOOT 0x1
/** @} */

/**
 * @name USB HID protocol types
 * @{
 */
#define USB_HID_PROTOCOL_NONE       0x0
#define USB_HID_PROTOCOL_KEYBOARD   0x1
#define USB_HID_PROTOCOL_MOUSE      0x2
/** @} */

/**
 * @name USB HID descriptor types
 * @{
 */
#define USB_HID_DESCR_HID       0x21
#define USB_HID_DESCR_REPORT    0x22
#define USB_HID_DESCR_PHYSICAL  0x23
/** @} */

/**
 * @brief  USB HID country codes
 */
#define USB_HID_COUNTRY_CODE_NOTSUPPORTED 0x00

/**
 * @name USB HID class specific control requests
 * @{
 */
#define USB_HID_REQUEST_GET_REPORT    0x01
#define USB_HID_REQUEST_GET_IDLE      0x02
#define USB_HID_REQUEST_GET_PROTOCOL  0x03
#define USB_HID_REQUEST_SET_REPORT    0x09
#define USB_HID_REQUEST_SET_IDLE      0x0a
#define USB_HID_REQUEST_SET_PROTOCOL  0x0b
/** @} */

/**
 * @brief USB HID descriptor struct
 *
 * @see USB HID 1.11 spec section 6.2.1
 */
typedef struct __attribute__((packed)){
    uint8_t length;         /**< Numeric expression that is the total size of
                                 the HID descriptor */
    uint8_t desc_type;      /**< Constant name specifying type of HID
                                 descriptor.*/

    uint16_t bcd_hid;       /**< Numeric expression identifying the HID Class
                                 Specification release */
    uint8_t country_code;   /**< Numeric expression identifying country code of
                                 the localized hardware.  */
    uint8_t num_descrs;     /**< Numeric expression specifying the number of
                                 class descriptors */

    uint8_t report_type;    /**< Type of HID class report. */
    uint16_t report_length; /**< the total size of the Report descriptor. */
} usb_desc_hid_t;

#ifdef __cplusplus
}
#endif

#endif /* USB_HID_H */
/** @} */
