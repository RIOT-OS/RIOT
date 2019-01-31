/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef USB_MESSAGE_H
#define USB_MESSAGE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "c" {
#endif

/**
 * @brief USB standard device request codes (USB 2.0 spec table 9-4)
 * @{
 */
#define USB_SETUP_REQ_GET_STATUS            0x00 /**< Status request        */
#define USB_SETUP_REQ_CLEAR_FEATURE         0x01 /**< Clear feature         */
#define USB_SETUP_REQ_SET_FEATURE           0x03 /**< Set feature           */
#define USB_SETUP_REQ_SET_ADDRESS           0x05 /**< Set address           */
#define USB_SETUP_REQ_GET_DESCRIPTOR        0x06 /**< Get descriptor        */
#define USB_SETUP_REQ_SET_DESCRIPTOR        0x07 /**< Set descriptor        */
#define USB_SETUP_REQ_GET_CONFIGURATION     0x08 /**< Get configuration     */
#define USB_SETUP_REQ_SET_CONFIGURATION     0x09 /**< Set configuration     */
/** @} */

/**
 * @brief USB descriptor types (USB 2.0 spec table 9-5)
 * @{
 */
#define USB_TYPE_DESCRIPTOR_DEVICE          0x01 /**< Device descriptor         */
#define USB_TYPE_DESCRIPTOR_CONFIGURATION   0x02 /**< Configuration Descriptor  */
#define USB_TYPE_DESCRIPTOR_STRING          0x03 /**< String descriptor         */
#define USB_TYPE_DESCRIPTOR_INTERFACE       0x04 /**< Interface descriptor      */
#define USB_TYPE_DESCRIPTOR_ENDPOINT        0x05 /**< Endpoint descriptor       */
#define USB_TYPE_DESCRIPTOR_DEV_QUALIFIER   0x06 /**< Device qualifier          */
#define USB_TYPE_DESCRIPTOR_SPEED_CONFIG    0x07 /**< Other speed configuration */
#define USB_TYPE_DESCRIPTOR_IFACE_POWER     0x08 /**< Interface power           */
#define USB_TYPE_DESCRIPTOR_INTERFACE_ASSOC 0x0B /**< Interface association
                                                   *  descriptor                */
/** @} */

/**
 * @brief USB configuration attributes
 * @{
 */
#define USB_CONF_ATTR_RESERVED              0x80
#define USB_CONF_ATTR_SELF_POWERED          0x40
#define USB_CONF_ATTR_REM_WAKEUP            0x20
/** @} */

#define USB_SETUP_REQUEST_DEVICE2HOST           0x80

#define USB_SETUP_REQUEST_RECIPIENT_MASK        0x1f
#define USB_SETUP_REQUEST_RECIPIENT_DEVICE      0x00
#define USB_SETUP_REQUEST_RECIPIENT_INTERFACE   0x01

#define USB_SETUP_REQUEST_TYPE_MASK             0x60
#define USB_SETUP_REQUEST_TYPE_STANDARD         0x00
#define USB_SETUP_REQUEST_TYPE_CLASS            0x20
#define USB_SETUP_REQUEST_TYPE_VENDOR           0x40
#define USB_SETUP_REQUEST_TYPE_RESERVED         0x60

/**
 * @brief USB isochronous endpoint interval
 */
#define USB_ENDPOINT_DESCRIPTOR_INTERVAL_ISOCHRONOUS    1

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

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint16_t total_length;
    uint8_t num_interfaces;
    uint8_t val;
    uint8_t idx;
    uint8_t attributes;
    uint8_t max_power;
} usb_descriptor_configuration_t;

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint8_t interface_num;
    uint8_t alternate_setting;
    uint8_t num_endpoints;
    uint8_t class;
    uint8_t subclass;
    uint8_t protocol;
    uint8_t idx;
} usb_descriptor_interface_t;

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint8_t address;
    uint8_t attributes;
    uint16_t max_packet_size;
    uint8_t interval;
} usb_descriptor_endpoint_t;


typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
} usb_descriptor_string_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t request;
    uint16_t value;
    uint16_t index;
    uint16_t length;
} usb_setup_t;

typedef struct __attribute__((packed)) {
    uint8_t length;
    uint8_t type;
    uint8_t first_interface;
    uint8_t interface_count;
    uint8_t class;
    uint8_t subclass;
    uint8_t protocol;
    uint8_t idx;
} usb_descriptor_interface_association_t;

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

static inline bool usb_setup_is_type_standard(usb_setup_t *pkt)
{
    return (pkt->type & USB_SETUP_REQUEST_TYPE_MASK) ==
        USB_SETUP_REQUEST_TYPE_STANDARD;
}

static inline bool usb_setup_is_type_class(usb_setup_t *pkt)
{
    return (pkt->type & USB_SETUP_REQUEST_TYPE_MASK) ==
        USB_SETUP_REQUEST_TYPE_CLASS;
}

#ifdef __cplusplus
}
#endif

#endif /* USB_MESSAGE_H */
/** @} */
