/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

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


#include <stdint.h>

#include "usb/hid/hid_usage.h"

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
 * @brief   Number of IN EPs required for the HID interface
 */
#define USBUS_HID_EP_IN_REQUIRED_NUMOF   1

/**
 * @brief   Number of Out EPs required for the HID interface
 */
#define USBUS_HID_EP_OUT_REQUIRED_NUMOF  1

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

/**
 * @defgroup usb_hid_descriptor USB HID descriptor helpers
 * @ingroup  usb_hid
 *
 * @brief USB HID descriptor format helpers
 * @{
 */


/**
 * @brief HID Main item type
 */
#define USB_HID_ITEM_TYPE_MAIN      0x0

/**
 * @brief HID Global item type
 */
#define USB_HID_ITEM_TYPE_GLOBAL        0x1

/**
 * @brief HID Local item type
 */
#define USB_HID_ITEM_TYPE_LOCAL     0x2

/**
 * @brief HID Input item tag
 */
#define USB_HID_ITEM_TAG_INPUT      0x8

/**
 * @brief HID Output item tag
 */
#define USB_HID_ITEM_TAG_OUTPUT     0x9

/**
 * @brief HID Collection item tag
 */
#define USB_HID_ITEM_TAG_COLLECTION     0xA

/**
 * @brief HID Feature item tag
 */
#define USB_HID_ITEM_TAG_FEATURE        0xB

/**
 * @brief HID End Collection item tag
 */
#define USB_HID_ITEM_TAG_COLLECTION_END 0xC

/**
 * @brief HID Usage Page item tag
 */
#define USB_HID_ITEM_TAG_USAGE_PAGE     0x0
/**
 * @brief HID Logical Minimum item tag
 */
#define USB_HID_ITEM_TAG_LOGICAL_MIN    0x1

/**
 * @brief HID Logical Maximum item tag
 */
#define USB_HID_ITEM_TAG_LOGICAL_MAX    0x2

/**
 * @brief HID Physical Minimum item tag
 */
#define USB_HID_ITEM_TAG_PHYSICAL_MIN   0x3

/**
 * @brief HID Physical Maximum item tag
 */
#define USB_HID_ITEM_TAG_PHYSICAL_MAX   0x4

/**
 * @brief HID Unit Exponent item tag
 */
#define USB_HID_ITEM_TAG_UNIT_EXPONENT  0x5

/**
 * @brief HID Unit item tag
 */
#define USB_HID_ITEM_TAG_UNIT       0x6

/**
 * @brief HID Report Size item tag
 */
#define USB_HID_ITEM_TAG_REPORT_SIZE    0x7

/**
 * @brief HID Report ID item tag
 */
#define USB_HID_ITEM_TAG_REPORT_ID      0x8

/**
 * @brief HID Report count item tag
 */
#define USB_HID_ITEM_TAG_REPORT_COUNT   0x9

/**
 * @brief HID Usage item tag
 */
#define USB_HID_ITEM_TAG_USAGE      0x0

/**
 * @brief HID Usage Minimum item tag
 */
#define USB_HID_ITEM_TAG_USAGE_MIN      0x1

/**
 * @brief HID Usage Maximum item tag
 */
#define USB_HID_ITEM_TAG_USAGE_MAX      0x2

/**
 * @brief Physical collection type
 */
#define USB_HID_COLLECTION_PHYSICAL     0x00

/**
 * @brief Application collection type
 */
#define USB_HID_COLLECTION_APPLICATION  0x01

/**
 * @brief Logical (interrelated data) collection type
 */
#define USB_HID_COLLECTION_LOGICAL      0x02

/**
 * @brief Report collection type
 */
#define USB_HID_COLLECTION_REPORT       0x03

/**
 * @brief Named array collection type
 */
#define USB_HID_COLLECTION_NAMED_ARRAY  0x04

/**
 * @brief Usage switch collection type
 */
#define USB_HID_COLLECTION_USAGE_SWITCH 0x05

/**
 * @brief Modifier switch collection type
 */
#define USB_HID_COLLECTION_USAGE_MOD    0x06

/**
 * @brief Defines a HID descriptor short item.
 *
 * @param tag  Item tag
 * @param type Item type
 * @param size Item data size
 */
#define USB_HID_ITEM(tag, type, size) \
    (((tag & 0xF) << 4) | ((type & 0x3) << 2) | (size & 0x3))

/**
 * @brief Defines a HID descriptor input item.
 *
 * @note data length is one byte
 *
 * @param input Input item flags
 */
#define USB_HID_INPUT(input)  \
    USB_HID_ITEM(USB_HID_ITEM_TAG_INPUT, USB_HID_ITEM_TYPE_MAIN, 1), \
    input

/**
 * @brief Defines a HID descriptor output item.
 *
 * @note data length is one byte
 *
 * @param output Output item flags
 */
#define USB_HID_OUTPUT(output) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_OUTPUT, USB_HID_ITEM_TYPE_MAIN, 1), \
    output


/**
 * @brief Defines a HID Feature item
 *
 * @note data length is one byte.
 *
 * @param feature Feature item data
 */
#define USB_HID_FEATURE(feature) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_FEATURE, USB_HID_ITEM_TYPE_MAIN, 1), \
    feature

/**
 * @brief Defines a HID Collection item
 *
 * @note data length is one byte.
 *
 * @param collection Collection item data
 */
#define USB_HID_COLLECTION(collection) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_COLLECTION, USB_HID_ITEM_TYPE_MAIN, 1), \
    collection

/**
 * @brief Defines a HID End Collection item
 */
#define USB_HID_END_COLLECTION \
    USB_HID_ITEM(USB_HID_ITEM_TAG_COLLECTION_END, USB_HID_ITEM_TYPE_MAIN, 0)

/**
 * @brief Defines a HID Usage Page item.
 *
 * @note data length is one byte.
 *
 * @param page Usage Page
 */
#define USB_HID_USAGE_PAGE(page) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_PAGE, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    page

/**
 * @brief Defines a HID Usage Page item.
 *
 * @note data length is two bytes.
 *
 * @param page      Usage page
 */
#define USB_HID_USAGE_PAGE16(page) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_PAGE, USB_HID_ITEM_TYPE_GLOBAL, 2), \
    page & 0xff, (page >> 8)

/**
 * @brief Defines a HID Logical Minimum item
 *
 * @note data length is one byte.
 *
 * @param  minimum  Minimum value in logical units
 */
#define USB_HID_LOGICAL_MIN8(minimum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MIN, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    minimum

/**
 * @brief Defines a HID Logical Minimum item
 *
 * @note data length is two bytes.
 *
 * @param  minimum  Minimum value in logical units
 */
#define USB_HID_LOGICAL_MIN16(minimum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MIN, USB_HID_ITEM_TYPE_GLOBAL, 2), \
    minimum & 0xff, (minimum >> 8)


/**
 * @brief Defines a HID Logical Minimum item
 *
 * @note data length is four bytes.
 *
 * @param  minimum  Minimum value in logical units
 */
#define USB_HID_LOGICAL_MIN32(minimum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MIN, USB_HID_ITEM_TYPE_GLOBAL, 3), \
    minimum & 0xff, \
    (minimum >> 8) & 0xff, \
    (minimum >> 16) & 0xff, \
    (minimum >> 24) & 0xff

/**
 * @brief Defines a HID Logical Maximum item
 *
 * @note data length is one byte.
 *
 * @param maximum   Maximum value in logical units
 */
#define USB_HID_LOGICAL_MAX8(maximum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MAX, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    maximum

/**
 * @brief Defines a HID Logical Maximum item
 *
 * @note data length is two bytes.
 *
 * @param maximum   Maximum value in logical units
 */
#define USB_HID_LOGICAL_MAX16(maximum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MAX, USB_HID_ITEM_TYPE_GLOBAL, 2), \
    (maximum & 0xff), (maximum >> 8)

/**
 * @brief Defines a HID Logical Maximum item
 *
 * @note data length is four bytes.
 *
 * @param maximum   Maximum value in logical units
 */
#define USB_HID_LOGICAL_MAX32(maximum) \
    USB_HID_ITEM(USB_HID_ITEM_TAG_LOGICAL_MAX, USB_HID_ITEM_TYPE_GLOBAL, 3), \
    maximum & 0xff, \
    (maximum >> 8) & 0xff, \
    (maximum >> 16) & 0xff, \
    (maximum >> 24) & 0xff

/**
 * @brief Defines a HID Report Size item
 *
 * @note data length is one byte.
 *
 * @param size Report field size in bits
 */
#define USB_HID_REPORT_SIZE(size)       \
    USB_HID_ITEM(USB_HID_ITEM_TAG_REPORT_SIZE, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    size

/**
 * @brief Defines a HID Report ID item
 *
 * @note data length is one byte.
 *
 * @param id Report ID
 */
#define USB_HID_REPORT_ID(id)       \
    USB_HID_ITEM(USB_HID_ITEM_TAG_REPORT_ID, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    id

/**
 * @brief Define HID Report Count item
 *
 * @note data length is one byte.
 *
 * @param count Number of data fields included in the report
 */
#define USB_HID_REPORT_COUNT(count)     \
    USB_HID_ITEM(USB_HID_ITEM_TAG_REPORT_COUNT, USB_HID_ITEM_TYPE_GLOBAL, 1), \
    count

/**
 * @brief Defines a HID Usage Index item
 *
 * @note data length is one byte.
 *
 * @param index Number of data fields included in the report
 */
#define USB_HID_USAGE(index)            \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE, USB_HID_ITEM_TYPE_LOCAL, 1), \
    index

/**
 * @brief Defines a HID Usage Minimum item
 *
 * @note data length is one byte.
 *
 * @param start Starting Usage
 */
#define USB_HID_USAGE_MIN8(start)       \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_MIN, USB_HID_ITEM_TYPE_LOCAL, 1), \
    start

/**
 * @brief Define HID Usage Minimum item with the data length of two bytes.
 *
 * @note data length is two bytes.
 *
 * @param start Starting Usage
 */
#define USB_HID_USAGE_MIN16(start)      \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_MIN, USB_HID_ITEM_TYPE_LOCAL, 2), \
    start & 0xff, (start >> 8) & 0xff

/**
 * @brief Defines HID Usage Maximum item
 *
 * @note data length is one byte.
 *
 * @param end Ending Usage
 */
#define USB_HID_USAGE_MAX8(end)     \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_MAX, USB_HID_ITEM_TYPE_LOCAL, 1), \
    end

/**
 * @brief Defines HID Usage Maximum item
 *
 * @note data length is one byte.
 *
 * @param end Ending Usage
 */
#define USB_HID_USAGE_MAX16(end)      \
    USB_HID_ITEM(USB_HID_ITEM_TAG_USAGE_MAX, USB_HID_ITEM_TYPE_LOCAL, 2), \
    end & 0xff, (end >> 8) & 0xff

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
