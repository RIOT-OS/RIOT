/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     usbdev_stack
 * @{
 *
 * @file
 * @brief       Data types for the USB device stack
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef USB_DATATYPES_H
#define USB_DATATYPES_H

#include <stdint.h>

/* --- USB Common Descriptors and Codes --- */

/**
 * @brief USB Device Requests
 * Request data transfer direction
 * @note usb_20.pdf, 9.3, Table 9-2
 */
#define REQUEST_HOST_TO_DEVICE          0
#define REQUEST_DEVICE_TO_HOST          1
#define REQUEST_DIR_SHIFT               7
#define REQUEST_DIR_MASK                1
#define REQUEST_DIR(request)            (((request) >> REQUEST_DIR_SHIFT) & REQUEST_DIR_MASK)

/**
 * @brief USB Device Requests
 * Request Type
 * @note usb_20.pdf, 9.3, Table 9-2
 */
#define REQUEST_STANDARD                0
#define REQUEST_CLASS                   1
#define REQUEST_VENDOR                  2
#define REQUEST_RESERVED                3
#define REQUEST_TYPE_SHIFT              5
#define REQUEST_TYPE_MASK               3
#define REQUEST_TYPE(request)           (((request) >> REQUEST_TYPE_SHIFT) & REQUEST_TYPE_MASK)

/**
 * @brief USB Device Requests
 * Request Recipient
 * @note usb_20.pdf, 9.3, Table 9-2
 */
#define REQUEST_TO_DEVICE               0
#define REQUEST_TO_INTERFACE            1
#define REQUEST_TO_ENDPOINT             2
#define REQUEST_TO_OTHER                3
#define REQUEST_RECIPIENT_SHIFT         0
#define REQUEST_RECIPIENT_MASK          0x1f
#define REQUEST_RECIPIENT(request)      ((request) & REQUEST_RECIPIENT_MASK)

/**
 * @brief Standard Request Codes
 * @note usb_20.pdf, 9.3, Table 9-4
 */
#define REQUEST_GET_STATUS              0
#define REQUEST_CLEAR_FEATURE           1
#define REQUEST_RESERVED_2              2
#define REQUEST_SET_FEATURE             3
#define REQUEST_RESERVED_4              4
#define REQUEST_SET_ADDRESS             5
#define REQUEST_GET_DESCRIPTOR          6
#define REQUEST_SET_DESCRIPTOR          7
#define REQUEST_GET_CONFIGURATION       8
#define REQUEST_SET_CONFIGURATION       9
#define REQUEST_GET_INTERFACE           10
#define REQUEST_SET_INTERFACE           11
#define REQUEST_SYNC_FRAME              12
#define REQUEST_RESERVED_13             13
#define REQUEST_RESERVED_14             14
#define REQUEST_RESERVED_15             15

/**
 * @brief Information Returned by a GetStatus Request
 * @note usb_20.pdf, 9.4.4, Figure 9-4, Figure 9-6
 */
/* USB GET_STATUS Bit Values */
#define DEVICE_STATUS_SELF_POWERED      1
#define DEVICE_STATUS_REMOTE_WAKEUP     2
#define ENDPOINT_STATUS_ENDPOINT_HALT   1

/**
 * @brief Standard Feature Selectors
 * @note usb_20.pdf, 9.4.1, Table 9-6
 */
#define FEATURE_ENDPOINT_HALT           0
#define FEATURE_DEVICE_REMOTE_WAKEUP    1
#define FEATURE_TEST_MODE               2

#define REQUEST_TO_HOST(type, recipient)    (REQUEST_DIR_MASK | \
                                            (((type) & REQUEST_DIR_MASK) << REQUEST_TYPE_SHIFT) | \
                                            ((recipient) & REQUEST_RECIPIENT_MASK))

/**
 * @brief USB Device Requests
 * Format of Setup Data
 * @note usb_20.pdf, 9.3, Table 9-2
 */
typedef struct __attribute__((packed))
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    __attribute__((packed)) union {
        uint16_t wValue;
        __attribute__((packed)) struct {
            uint8_t wValueL;
            uint8_t wValueH;
        };
    };
   __attribute__((packed)) union {
        uint16_t wIndex;
        __attribute__((packed)) struct {
            uint8_t wIndexL;
            uint8_t wIndexH;
        };
    };
    uint16_t wLength;
} usb_device_request_t;


/**
 * @brief Descriptor Types
 * @note usb_20.pdf, 9.4, Table 9-5
 */
#define DESCRIPTOR_TYPE_DEVICE                  1
#define DESCRIPTOR_TYPE_CONFIGURATION           2
#define DESCRIPTOR_TYPE_STRING                  3
#define DESCRIPTOR_TYPE_INTERFACE               4
#define DESCRIPTOR_TYPE_ENDPOINT                5
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER        6
#define DESCRIPTOR_TYPE_OTHER_SPEED_CONFIG      7
#define DESCRIPTOR_TYPE_INTERFACE_POWER         8
#define DESCRIPTOR_TYPE_OTG                     9
#define DESCRIPTOR_TYPE_DEBUG                   10

/**
 * @brief Generic Device Descriptor
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
}
usb_generic_descriptor_t;

/**
 * @brief Standard Device Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-8
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
}
usb_device_descriptor_t;

/**
 * @brief Standard Configuration Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-10
 * Configuration characteristics
 */
#define CONFIG_BUS_POWERED                      (1 << 7)
#define CONFIG_SELF_POWERED                     (1 << 7 | 1 << 6)
#define CONFIG_SELF_POWERED_MASK                (1 << 6)
#define CONFIG_REMOTE_WAKEUP                    (1 << 7 | 1 << 5)
#define CONFIG_REMOTE_WAKEUP_MASK               (1 << 5)

/**
 * @brief Standard Configuration Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-10
 * Maximum power consumption
 * Expressed in 2 mA units (e.g. 50 = 100 mA).
 */
#define CONFIG_POWER_MA(p)                      ((p)/2)

/**
 * @brief Standard Configuration Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-10
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
}
usb_cfg_descriptor_t;

/**
 * @brief Standard Interface Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-12
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
}
usb_if_descriptor_t;

/**
 * @brief Standard Endpoint Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-13
 * bEndpointAddress description
 */
#define ENDPOINT_ADDR_DIRECTION_MASK            (1 << 7)
#define ENDPOINT_IN_MASK                        (1 << 7)
#define ENDPOINT_NUMBER_MASK                    0xf

#define ENDPOINT_ADDR_OUT(num)                  ((num) & ENDPOINT_NUMBER_MASK)
#define ENDPOINT_ADDR_IN(num)                   (((num) & ENDPOINT_NUMBER_MASK) \
                                                | ENDPOINT_ADDR_DIRECTION_MASK)
#define ENDPOINT_ADDR(n)                        ((n) & (ENDPOINT_ADDR_DIRECTION_MASK \
                                                | ENDPOINT_NUMBER_MASK))
#define ENDPOINT_NUM(addr)                      ((addr) & (ENDPOINT_NUMBER_MASK))

/**
 * @brief Standard Endpoint Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-13
 * bmAttributes description
 */
#define ENDPOINT_TRANSFER_TYPE_CONTROL          (0 << 0)
#define ENDPOINT_TRANSFER_TYPE_ISOCHRONOUS      (1 << 0)
#define ENDPOINT_TRANSFER_TYPE_BULK             (2 << 0)
#define ENDPOINT_TRANSFER_TYPE_INTERRUPT        (3 << 0)
#define ENDPOINT_SYNC_TYPE_NO_SYNCHRONIZATION   (0 << 2)
#define ENDPOINT_SYNC_TYPE_ASYNCHRONOUS         (1 << 2)
#define ENDPOINT_SYNC_TYPE_ADAPTIVE             (2 << 2)
#define ENDPOINT_SYNC_TYPE_SYNCHRONOUS          (3 << 2)
#define ENDPOINT_USAGE_TYPE_DATA                (0 << 4)
#define ENDPOINT_USAGE_TYPE_FEEDBACK            (1 << 4)
#define ENDPOINT_USAGE_TYPE_IMPLICIT_FEEDBACK   (2 << 4)

/**
 * @brief Standard Endpoint Descriptor
 * @note PSTN120.pdf, 9.6, Table 9-13
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
}
usb_ep_descriptor_t;

/**
 * @brief UNICODE String Descriptor
 * @note PSTN120.pdf, 9.7, Table 9-16
 */
typedef struct __attribute__((packed))
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bString;
}
usb_string_descriptor_t;



/* --- CDC Functional Descriptors and Codes --- */

/**
 * @brief Communications Devices Specification release number in BCD format.
 */
#define CDC_SRN_1_20                    0x0120

/**
 * @brief Communications Device Class Code
 * @note CDC120-20101103-track.pdf, 4.2, Table 2
 */
#define COMMUNICATION_DEVICE_CLASS      0x02

/**
 * @brief Communications Interface Class Code
 * @note CDC120-20101103-track.pdf, 4.2, Table 3
 */
#define COMMUNICATION_INTERFACE_CLASS   0x02

/**
 * @brief Communications Class Subclass Codes
 * @note CDC120-20101103-track.pdf, 4.2, Table 4
 */
#define DIRECT_LINE_CONTROL_MODEL       0x01
#define ABSTRACT_CONTROL_MODEL          0x02
#define TELEPHONE_CONTROL_MODEL         0x03

/**
 * @brief Values for the bDescriptorType Field
 * @note CDC120-20101103-track.pdf, 5.2.3, Table 12
 */
#define CS_INTERFACE                    0x24
#define CS_ENDPOINT                     0x25

/**
 * @brief bDescriptor SubType in Communications Class Functional Descriptors
 * @note CDC120-20101103-track.pdf, 5.2.3, Table 13
 */
#define HEADER_FUNC_DESCRIPTOR                      0x00
#define CALL_MANAGEMENT_FUNC_DESCRIPTOR             0x01
#define ABSTRACT_CONTROL_MANAGEMENT_FUNC_DESCRIPTOR 0x02
#define UNION_FUNC_DESCRIPTOR                       0x06

/**
 * @brief Data Class Interface Codes
 * @note PSTN120.pdf, 4.5, Table 6
 */
#define DATA_INTERFACE_CLASS            0x0A

/**
 * @brief Header Functional Descriptor
 * CDC120-20101103-track.pdf, 5.2.3.1
 */
typedef struct __attribute__((packed))
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdCDC;
}
cdc_header_descriptor_t;

/**
 * @brief Union Interface Functional Descriptor
 * @note CDC120-20101103-track.pdf, 5.2.3.2
 */
typedef struct __attribute__((packed))
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;            /**< CS_INTERFACE */
    uint8_t bDescriptorSubtype;
    uint8_t bControlInterface;
    uint8_t bSubordinateInterface0;
}
cdc_union_descriptor_t;

/**
 * @brief Call Management Functional Descriptor
 * @note PSTN120.pdf, 5.3.1
 */
typedef struct __attribute__((packed))
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;            /**< CS_INTERFACE */
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
    uint8_t bDataInterface;
}
cdc_cm_descriptor_t;

/**
 * @brief Abstract Control Management Functional Descriptor
 * @note PSTN120.pdf, 5.3.2
 */
typedef struct __attribute__((packed))
{
    uint8_t bFunctionLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bmCapabilities;
}
cdc_acm_descriptor_t;

/**
 * @brief PSTN Subclass Specific Requests
 * Class-Specific Request Codes
 * @note PSTN120.pdf, 6.3, Table 13
 */
#define SET_COMM_FEATURE                0x02
#define GET_COMM_FEATURE                0x03
#define CLEAR_COMM_FEATURE              0x04
#define SET_AUX_LINE_STATE              0x10
#define SET_HOOK_STATE                  0x11
#define PULSE_SETUP                     0x12
#define SEND_PULSE                      0x13
#define SET_PULSE_TIME                  0x14
#define RING_AUX_JACK                   0x15
#define SET_LINE_CODING                 0x20
#define GET_LINE_CODING                 0x21
#define SET_CONTROL_LINE_STATE          0x22
#define SEND_BREAK                      0x23
#define SET_RINGER_PARMS                0x30
#define GET_RINGER_PARMS                0x31
#define SET_OPERATION_PARMS             0x32
#define GET_OPERATION_PARMS             0x33
#define SET_LINE_PARMS                  0x34
#define GET_LINE_PARMS                  0x35
#define DIAL_DIGITS                     0x36

/**
 * @brief PSTN Subclass Specific Request
 * @note PSTN120.pdf, 6.3
 */
typedef struct __attribute__((packed))
{
    uint8_t bmRequestType;
    uint8_t bRequestCode;               /**< Class-Specific Request Code */
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint16_t Data;                      /**< e.g. State or Line Coding Structure */
}
usb_cdc_request_t;

/**
 * @brief Line Coding Structure
 * @note PSTN120.pdf, 6.3.10, Set/Get Line Coding
 */
typedef struct __attribute__((packed))
{
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
}
cdc_line_coding_t;

/**
 * @brief PSTN Subclass Specific Notifications
 * Class-Specific Notification Codes
 * @note PSTN120.pdf, 6.5, Table 30
 */
#define NETWORK_CONNECTION              0x00
#define RESPONSE_AVAILABLE              0x01
#define AUX_JACK_HOOK_STATE             0x08
#define RING_DETECT                     0x09
#define SERIAL_STATE                    0x20
#define LINE_STATE_CHANGE               0x23
#define CALL_STATE_CHANGE               0x28

/**
 * @brief PSTN Subclass Specific Notifications
 * UART State Bitmap Values
 * @note PSTN120.pdf, 6.5.4, Table 31
 */
#define SERIAL_STATE_OVERRUN            6
#define SERIAL_STATE_OVERRUN_MASK       (1 << SERIAL_STATE_OVERRUN)
#define SERIAL_STATE_PARITY             5
#define SERIAL_STATE_PARITY_MASK        (1 << SERIAL_STATE_PARITY)
#define SERIAL_STATE_FRAMING            4
#define SERIAL_STATE_FRAMING_MASK       (1 << SERIAL_STATE_FRAMING)
#define SERIAL_STATE_RING               3
#define SERIAL_STATE_RING_MASK          (1 << SERIAL_STATE_RING)
#define SERIAL_STATE_BREAK              2
#define SERIAL_STATE_BREAK_MASK         (1 << SERIAL_STATE_BREAK)
#define SERIAL_STATE_TX_CARRIER         1
#define SERIAL_STATE_TX_CARRIER_MASK    (1 << SERIAL_STATE_TX_CARRIER)
#define SERIAL_STATE_RX_CARRIER         0
#define SERIAL_STATE_RX_CARRIER_MASK    (1 << SERIAL_STATE_RX_CARRIER)

/**
 * @brief PSTN Subclass Specific Notification
 * @note PSTN120.pdf, 6.5.4
 */
typedef struct __attribute__((packed))
{
    uint8_t bmRequestType;
    uint8_t bNotification;              /**< Class-Specific Notification Code */
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
    uint16_t Data;                      /**< UART State Bitmap Value */
}
usb_cdc_notification_t;

#endif
/** @} */
