/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @defgroup    usb_descriptor   USB descriptors
 * @ingroup     usb
 * @brief       Provides USB protocol descriptors and defines for USB frameworks.
 * @{
 *
 * @file
 * @brief       Definitions for USB protocol messages
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name USB standard device request codes (USB 2.0 spec table 9-4)
 * @{
 */
#define USB_SETUP_REQ_GET_STATUS            0x00    /**< Status request        */
#define USB_SETUP_REQ_CLEAR_FEATURE         0x01    /**< Clear feature         */
#define USB_SETUP_REQ_SET_FEATURE           0x03    /**< Set feature           */
#define USB_SETUP_REQ_SET_ADDRESS           0x05    /**< Set address           */
#define USB_SETUP_REQ_GET_DESCRIPTOR        0x06    /**< Get descriptor        */
#define USB_SETUP_REQ_SET_DESCRIPTOR        0x07    /**< Set descriptor        */
#define USB_SETUP_REQ_GET_CONFIGURATION     0x08    /**< Get configuration     */
#define USB_SETUP_REQ_SET_CONFIGURATION     0x09    /**< Set configuration     */
#define USB_SETUP_REQ_GET_INTERFACE         0x0a    /**< Get interface         */
#define USB_SETUP_REQ_SET_INTERFACE         0x0b    /**< Set interface         */
#define USB_SETUP_REQ_SYNCH_FRAME           0x0c    /**< Synch frame           */
/** @} */

/**
 * @name USB descriptor types (USB 2.0 spec table 9-5)
 * @{
 */
#define USB_TYPE_DESCRIPTOR_DEVICE          0x01    /**< Device descriptor         */
#define USB_TYPE_DESCRIPTOR_CONFIGURATION   0x02    /**< Configuration Descriptor  */
#define USB_TYPE_DESCRIPTOR_STRING          0x03    /**< String descriptor         */
#define USB_TYPE_DESCRIPTOR_INTERFACE       0x04    /**< Interface descriptor      */
#define USB_TYPE_DESCRIPTOR_ENDPOINT        0x05    /**< Endpoint descriptor       */
#define USB_TYPE_DESCRIPTOR_DEV_QUALIFIER   0x06    /**< Device qualifier          */
#define USB_TYPE_DESCRIPTOR_SPEED_CONFIG    0x07    /**< Other speed configuration */
#define USB_TYPE_DESCRIPTOR_IFACE_POWER     0x08    /**< Interface power           */
#define USB_TYPE_DESCRIPTOR_INTERFACE_ASSOC 0x0b    /**< Interface association     */
/** @} */

/**
 * @name USB standard feature selectors
 * @{
 */
#define USB_FEATURE_ENDPOINT_HALT           0x00    /**< Endpoint halt        */
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP    0x01    /**< Device remote wakeup */
#define USB_FEATURE_TEST_MODE               0x02    /**< Test mode feature    */
/** @} */

/**
 * @name USB configuration attributes
 * @anchor USB_CONF_ATTR
 * @{
 */
#define USB_CONF_ATTR_RESERVED              0x80    /**< Reserved bit (always 1)   */
#define USB_CONF_ATTR_SELF_POWERED          0x40    /**< Self powered device flag  */
#define USB_CONF_ATTR_REM_WAKEUP            0x20    /**< Remote wake-up flag       */
/** @} */

/**
 * @name USB setup packet requests types
 * @{
 */
#define USB_SETUP_REQUEST_DEVICE2HOST           0x80    /**< Request direction
                                                             From device to host if
                                                             the bit is set          */

#define USB_SETUP_REQUEST_RECIPIENT_MASK        0x1f    /**< Recipient mask          */
#define USB_SETUP_REQUEST_RECIPIENT_DEVICE      0x00    /**< Device request          */
#define USB_SETUP_REQUEST_RECIPIENT_INTERFACE   0x01    /**< Interface request       */
#define USB_SETUP_REQUEST_RECIPIENT_ENDPOINT    0x02    /**< Endpoint request        */
#define USB_SETUP_REQUEST_RECIPIENT_OTHER       0x03    /**< Other type request      */

#define USB_SETUP_REQUEST_TYPE_MASK             0x60    /**< Mask to select the type */
#define USB_SETUP_REQUEST_TYPE_STANDARD         0x00    /**< Standard request        */
#define USB_SETUP_REQUEST_TYPE_CLASS            0x20    /**< Class request           */
#define USB_SETUP_REQUEST_TYPE_VENDOR           0x40    /**< Vendor specific request */
/** @} */

/**
 * @brief USB isochronous endpoint interval
 */
#define USB_ENDPOINT_DESCRIPTOR_INTERVAL_ISOCHRONOUS    1

/**
 * @name USB device class numbers
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
 * @brief USB device descriptor (USB 2.0 spec table 9-8)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor */
    uint8_t type;               /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_DEVICE)  */
    uint16_t bcd_usb;           /**< Binary-coded decimal USB release specification     */
    uint8_t class;              /**< Device class code                                  */
    uint8_t subclass;           /**< Device subclass code                               */
    uint8_t protocol;           /**< Device protocol code                               */
    uint8_t max_packet_size;    /**< EP0 max packet size (8, 16, 32 or 64 bytes)        */
    uint16_t vendor_id;         /**< Vendor ID (as assigned by the USB-IF)              */
    uint16_t product_id;        /**< Product ID                                         */
    uint16_t bcd_device;        /**< Binary-coded decimal device release                */
    uint8_t manufacturer_idx;   /**< Manufacturer string index number                   */
    uint8_t product_idx;        /**< Product string index number                        */
    uint8_t serial_idx;         /**< Device serial number string index number           */
    uint8_t num_configurations; /**< Number of possible configurations                  */
} usb_descriptor_device_t;

/**
 * @brief USB configuration descriptor (USB 2.0 spec table 9-10)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor                                  */
    uint8_t type;               /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CONFIGURATION) */
    uint16_t total_length;      /**< Total length of the configuration                        */
    uint8_t num_interfaces;     /**< Number of interfaces supported by this configuration     */
    uint8_t val;                /**< Value to select this configuration by                    */
    uint8_t idx;                /**< Configuration descriptor string index                    */
    uint8_t attributes;         /**< Configuration attributes (@ref USB_CONF_ATTR)            */
    uint8_t max_power;          /**< Maximum power consumption in 2mA steps                   */
} usb_descriptor_configuration_t;

/**
 * @brief USB interface descriptor (USB 2.0 spec table 9-12)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor                              */
    uint8_t type;               /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_INTERFACE) */
    uint8_t interface_num;      /**< Interface number                                     */
    uint8_t alternate_setting;  /**< Alternate setting index                              */
    uint8_t num_endpoints;      /**< Number of endpoints used by this interface           */
    uint8_t class;              /**< Interface class code as assigned by the USB-IF       */
    uint8_t subclass;           /**< Subclass code as assigned by the USB-IF              */
    uint8_t protocol;           /**< Protocol code                                        */
    uint8_t idx;                /**< Interface descriptor string index                    */
} usb_descriptor_interface_t;

/**
 * @brief USB endpoint descriptor (USB 2.0 spec table 9-13)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor                             */
    uint8_t type;               /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_ENDPOINT) */
    uint8_t address;            /**< Address of the endpoint                             */
    uint8_t attributes;         /**< Attributes of the endpoint                          */
    uint16_t max_packet_size;   /**< Maximum packet size of the endpoint                 */
    uint8_t interval;           /**< Polling interval for the endpoint                   */
} usb_descriptor_endpoint_t;

/**
 * @brief USB string descriptor (USB 2.0 spec table 9-16)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor */
    uint8_t type;               /**< String descriptor type  */
} usb_descriptor_string_t;

/**
 * @brief USB interface association descriptor (Interface Association
 * Descriptors table 9-Z)
 */
typedef struct __attribute__((packed)) {
    uint8_t length;             /**< Size of this descriptor */
    uint8_t type;               /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_INTERFACE_ASSOC) */
    uint8_t first_interface;    /**< Index of the first associated interface */
    uint8_t interface_count;    /**< Number of associated interfaces */
    uint8_t class;              /**< Non-zero class code */
    uint8_t subclass;           /**< Subclass code */
    uint8_t protocol;           /**< Protocol code */
    uint8_t idx;                /**< Descriptor string index */
} usb_descriptor_interface_association_t;

/**
 * @brief USB setup packet (USB 2.0 spec table 9-2)
 */
typedef struct __attribute__((packed)) {
    uint8_t type;       /**< Request type             */
    uint8_t request;    /**< Specific request         */
    uint16_t value;     /**< Value word               */
    uint16_t index;     /**< Index/offset word        */
    uint16_t length;    /**< Length of the data stage */
} usb_setup_t;

/**
 * @brief getter for setup packet direction
 *
 * @param[in]   pkt setup packet
 *
 * @return          nonzero if it is a read request
 * @return          zero if it is a write request
 */
static inline bool usb_setup_is_read(usb_setup_t *pkt)
{
    return pkt->type & USB_SETUP_REQUEST_DEVICE2HOST;
}

#ifdef __cplusplus
}
#endif

/** @} */
