/*
 * Copyright (C) 2018 Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef USB_CDC_H
#define USB_CDC_H

/**
 * @defgroup    usb_cdc   CDC - USB communications device class
 * @ingroup     usb
 * @brief       Generic USB CDC defines and helpers
 *
 * @{
 *
 * @file
 * @brief       Definition for USB CDC interfaces
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>

#include "usb.h"
#include "usb/descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USB_TYPE_DESCRIPTOR_CDC      0x24   /**< USB CDC type descriptor*/
#define USB_CDC_VERSION_BCD          0x0120 /**< USB CDC version in BCD */

/**
 * @name USB CDC subclass types
 * @anchor usb_cdc_subtype
 * @{
 */
#define USB_CDC_SUBCLASS_NONE          0x00 /**< No subclass */
#define USB_CDC_SUBCLASS_DLCM          0x01 /**< Direct Line Control Model */
#define USB_CDC_SUBCLASS_ACM           0x02 /**< Abstract Control Model */
#define USB_CDC_SUBCLASS_TCM           0x03 /**< Telephone Control Model */
#define USB_CDC_SUBCLASS_MCCM          0x04 /**< Multi-Channel Control Model */
#define USB_CDC_SUBCLASS_CCM           0x05 /**< CAPI Control Mode */
#define USB_CDC_SUBCLASS_ENCM          0x06 /**< Eth Networking Control Model */
#define USB_CDC_SUBCLASS_ANCM          0x07 /**< ATM Networking Control Model */
#define USB_CDC_SUBCLASS_WHCM          0x08 /**< Wireless Handset Control Model */
#define USB_CDC_SUBCLASS_DM            0x09 /**< Device Management */
#define USB_CDC_SUBCLASS_MDLM          0x0A /**< Mobile Direct Line Model */
#define USB_CDC_SUBCLASS_OBEX          0x0B /**< OBEX */
#define USB_CDC_SUBCLASS_EEM           0x0C /**< Ethernet Emulation Model */
#define USB_CDC_SUBCLASS_NCM           0x0D /**< Network Control Model */
/** @} */

/**
 * @name USB CDC protocol types
 * @{
 */
#define USB_CDC_PROTOCOL_NONE          0x00 /**< No protocol required */
#define USB_CDC_PROTOCOL_ITU           0x01 /**< AT Commands: V.250 etc */
#define USB_CDC_PROTOCOL_PCCA          0x02 /**< AT Commands defined by PCCA-101 */
#define USB_CDC_PROTOCOL_PCCA_A        0x03 /**< AT Commands defined by PCCA-101 & Annex O */
#define USB_CDC_PROTOCOL_GSM           0x04 /**< AT Commands defined by GSM 07.07 */
#define USB_CDC_PROTOCOL_3GPP          0x05 /**< AT Commands defined by 3GPP 27.007 */
#define USB_CDC_PROTOCOL_CS            0x06 /**< AT Commands defined by TIA for CDMA */
#define USB_CDC_PROTOCOL_EEM           0x07 /**< Ethernet Emulation Model */
#define USB_CDC_PROTOCOL_EXT           0xFE /**< External Protocol */
#define USB_CDC_PROTOCOL_VENDOR        0xFF /**< Vendor-specific */
/** @} */

/**
 * @name USB CDC descriptor subtypes
 * @{
 */
#define USB_CDC_DESCR_SUBTYPE_FUNCTIONAL    0x00 /**< functional
                                                   *  descriptor */
#define USB_CDC_DESCR_SUBTYPE_CALL_MGMT     0x01 /**< Call management
                                                      descriptor */
#define USB_CDC_DESCR_SUBTYPE_ACM           0x02 /**< Abstract control
                                                      management descriptor */
#define USB_CDC_DESCR_SUBTYPE_UNION         0x06 /**< Union descriptor */
#define USB_CDC_DESCR_SUBTYPE_ETH_NET       0x0f /**< Ethernet descriptor */
/** @} */

/**
 * @name USB CDC management requests
 * @{
 */

/**
 * @brief Set line character formatting properties
 */
#define USB_CDC_MGNT_REQUEST_SET_LINE_CODING            (0x20)

/**
 * @brief Request the currently configured line coding
 */
#define USB_CDC_MGNT_REQUEST_GET_LINE_CODING            (0x21)

/**
 * @brief Set the control line state
 */
#define USB_CDC_MGNT_REQUEST_SET_CONTROL_LINE_STATE     (0x22)

/**
 * @brief Set ethernet multicast filter request
 */
#define USB_CDC_MGNT_REQUEST_SET_ETH_MULTICAST_FILTER   0x40

/**
 * @brief Set ethernet power management pattern filter
 */
#define USB_CDC_MGNT_REQUEST_SET_ETH_PM_PATTERN_FILTER  0x41

/**
 * @brief Get ethernet power management pattern filter
 */
#define USB_CDC_MGNT_REQUEST_GET_ETH_PM_PATTERN_FILTER  0x42

/**
 * @brief Set ethernet packet filter
 */
#define USB_CDC_MGNT_REQUEST_SET_ETH_PACKET_FILTER      0x43

/**
 * @brief Get ethernet statistics
 */
#define USB_CDC_MGNT_REQUEST_GET_ETH_STATISTICS         0x44
/** @} */

/**
 * @name USB CDC ACM control line state flags
 * @{
 */

/**
 * @brief DTE (e.g. a PC) is present and listening
 */
#define USB_CDC_ACM_CONTROL_LINE_DTE            (0x01)

/**
 * @brief Activate carrier control for half duplex modems
 */
#define USB_CDC_ACM_CONTROL_LINE_CARRIER        (0x02)
/** @} */

/**
 * @name USB CDC management notifications
 * @{
 */

/**
 * @brief Network connection status notification
 */
#define USB_CDC_MGNT_NOTIF_NETWORK_CONNECTION   0x00

/**
 * @brief Response available notification
 */
#define USB_CDC_MGNT_NOTIF_RESPONSE_AVAILABLE   0x01

/**
 * @brief Hook on the auxiliary phone changed notification
 */
#define USB_CDC_MGNT_NOTIF_AUX_JACK_HOOK_STATE  0x08

/**
 * @brief Ring voltage on the POTS line interface notification
 */
#define USB_CDC_MGNT_NOTIF_RING_DETECT          0x09

/**
 * @brief Asynchronous UART status notification
 */
#define USB_CDC_MGNT_NOTIF_SERIAL_STATE         0x20

/**
 * @brief Call state change notification
 */
#define USB_CDC_MGNT_NOTIF_CALL_STATE_CHANGE    0x28

/**
 * @brief Line state change notification
 */
#define USB_CDC_MGNT_NOTIF_LINE_STATE_CHANGE    0x29

/**
 * @brief Throughput change notification
 */
#define USB_CDC_MGNT_NOTIF_CONN_SPEED_CHANGE    0x2A

/** @} */

/**
 * @brief USB CDC ECM descriptor
 *
 * @see USB CDC 1.2 ECM spec table 3
 */
typedef struct __attribute__((packed)) {
    uint8_t length;                 /**< Size of this descriptor */
    uint8_t type;                   /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CDC) */
    uint8_t subtype;                /**< Descriptor subtype (@ref USB_CDC_DESCR_SUBTYPE_ETH_NET) */
    uint8_t macaddress;             /**< Index of the string containing the ethernet MAC address */
    uint32_t ethernetstatistics;    /**< Bitmap indicating the statistics caps */
    uint16_t maxsegmentsize;        /**< Maximum segment size of the interface */
    uint16_t numbermcfilters;       /**< Number of configurable multicast filters */
    uint8_t numberpowerfilters;     /**< Number of pattern filters for host wake-up */
} usb_desc_ecm_t;

/**
 * @brief USB CDC ACM descriptor
 *
 * @see USB CDC 1.2 PTSN spec table 4
 */
typedef struct __attribute__((packed)) {
    uint8_t length;         /**< Size of this descriptor */
    uint8_t type;           /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CDC) */
    uint8_t subtype;        /**< Descriptor subtype (@ref USB_CDC_DESCR_SUBTYPE_ACM) */
    uint8_t capabilities;   /**< Bitmap indicating the capabilities */
} usb_desc_acm_t;

/**
 * @brief Generic USB CDC descriptor
 *
 * @see USB CDC 1.2 spec table 15
 */
typedef struct __attribute__((packed)) {
    uint8_t length;     /**< Size of this descriptor */
    uint8_t type;       /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CDC) */
    uint8_t subtype;    /**< Descriptor subtype (@ref usb_cdc_subtype) */
    uint16_t bcd_cdc;   /**< CDC release number in bcd (@ref USB_CDC_VERSION_BCD) */
} usb_desc_cdc_t;

/**
 * @brief USB union descriptor
 */
typedef struct __attribute__((packed)) {
    uint8_t length;     /**< Size of this descriptor */
    uint8_t type;       /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CDC) */
    uint8_t subtype;    /**< Descriptor subtype (@ref USB_CDC_DESCR_SUBTYPE_UNION) */
    uint8_t master_if;  /**< Master/controlling interface number */
    uint8_t slave_if;   /**< Slave/subordinate interface number */
} usb_desc_union_t;

/**
 * @brief USB CDC call management functional descriptor
 *
 * @see USB CDC 1.2 PSTN spec table 13
 */
typedef struct __attribute__((packed)) {
    uint8_t length;     /**< Size of this descriptor */
    uint8_t type;       /**< Descriptor type (@ref USB_TYPE_DESCRIPTOR_CDC) */
    uint8_t subtype;    /**< Descriptor subtype (@ref USB_CDC_DESCR_SUBTYPE_CALL_MGMT) */
    uint8_t capabilities; /**< Supported capabilities */
    uint8_t data_if;      /**< Interface number used for the call management */
} usb_desc_call_mngt_t;

/**
 * @brief USB CDC ECM connection speed change notification
 *
 * @see USB CDC 1.2 spec section 6.3.3
 */
typedef struct __attribute__((packed)) {
    usb_setup_t setup;  /**< Setup request header for the notification */
    uint32_t down;      /**< Downlink bit rate */
    uint32_t up;        /**< Uplink bit rate */
} usb_desc_cdcecm_speed_t;

/**
 * @name USB CDC ACM line coding setup defines
 * @{
 */

/**
 * @brief USB CDC ACM line coding setup content
 * @see USB CDC 1.2 PSTN subclass spec section 6.3.11
 */
typedef struct __attribute__((packed)) {
    uint32_t baud;      /**< Requested baud rate                    */
    uint8_t format;     /**< Stop bits settings                     */
    uint8_t parity;     /**< Parity settings                        */
    uint8_t databits;   /**< Number of data bits (5, 6, 7, 8 or 16) */
} usb_req_cdcacm_coding_t;

#define USB_CDC_ACM_CODING_STOP_BITS_1      0 /**< 1 stop bit    */
#define USB_CDC_ACM_CODING_STOP_BITS_1_5    1 /**< 1.5 stop bits */
#define USB_CDC_ACM_CODING_STOP_BITS_2      2 /**< 2 stop bits   */

#define USB_CDC_ACM_CODING_PARITY_NONE      0 /**< No parity bit */
#define USB_CDC_ACM_CODING_PARITY_ODD       1 /**< Odd parity    */
#define USB_CDC_ACM_CODING_PARITY_EVEN      2 /**< Even parity   */
#define USB_CDC_ACM_CODING_PARITY_MARK      3 /**< Mark parity   */
#define USB_CDC_ACM_CODING_PARITY_SPACE     4 /**< Space parity  */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* USB_CDC_H */
