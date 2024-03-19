/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    usbus_cdc_ecm USBUS CDC ECM - USBUS CDC ethernet control model
 * @ingroup     usb
 * @brief       USBUS CDC ECM interface module
 *
 * @{
 *
 * @file
 * @brief       Interface and definitions for USB CDC ECM type interfaces
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBUS_CDC_ECM_H
#define USB_USBUS_CDC_ECM_H

#include <stdint.h>
#include <stdlib.h>
#include "net/ethernet.h"
#include "net/ethernet/hdr.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"
#include "macros/math.h"
#include "net/netdev.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Link throughput as reported by the peripheral
 *
 * This defines a common up and down link throughput in bits/second. The USB
 * peripheral will report this to the host. This doesn't affect the actual
 * throughput, only what the peripheral reports to the host.
 */
#ifndef CONFIG_USBUS_CDC_ECM_CONFIG_SPEED
#define CONFIG_USBUS_CDC_ECM_CONFIG_SPEED  1000000
#endif

/**
 * @brief Link download speed as reported by the peripheral
 */
#ifndef CONFIG_USBUS_CDC_ECM_CONFIG_SPEED_DOWNSTREAM
#define CONFIG_USBUS_CDC_ECM_CONFIG_SPEED_DOWNSTREAM CONFIG_USBUS_CDC_ECM_CONFIG_SPEED
#endif

/**
 * @brief Link upload speed as reported by the peripheral
 */
#ifndef CONFIG_USBUS_CDC_ECM_CONFIG_SPEED_UPSTREAM
#define CONFIG_USBUS_CDC_ECM_CONFIG_SPEED_UPSTREAM   CONFIG_USBUS_CDC_ECM_CONFIG_SPEED
#endif

/**
 * @brief CDC ECM interrupt endpoint size.
 *
 * Used by the device to report events to the host.
 *
 * @note Must be at least 16B to allow for reporting the link throughput
 */
#define USBUS_CDCECM_EP_CTRL_SIZE  16

/**
 * @brief CDC ECM bulk data endpoint size.
 *
 * Used for the transfer of network frames.
 */
#ifndef MODULE_PERIPH_USBDEV_HS
#define USBUS_CDCECM_EP_DATA_SIZE  64
#else
#define USBUS_CDCECM_EP_DATA_SIZE  512
#endif

/**
 * @brief Full ethernet frame rounded up to a whole number of transfers
 */
#define USBUS_ETHERNET_FRAME_BUF   MATH_ALIGN(ETHERNET_FRAME_LEN, USBUS_CDCECM_EP_DATA_SIZE)

/**
 * @brief Number of IN EPs required for the CDC ECM interface
 */
#define USBUS_CDC_ECM_EP_IN_REQUIRED_NUMOF   2

/**
 * @brief Number of Out EPs required for the CDC ECM interface
 */
#define USBUS_CDC_ECM_EP_OUT_REQUIRED_NUMOF  1

/**
 * @brief notification state, used to track which information must be send to
 * the host
 */
typedef enum {
    USBUS_CDCECM_NOTIF_NONE,    /**< Nothing notified so far */
    USBUS_CDCECM_NOTIF_LINK_UP, /**< Link status is notified */
    USBUS_CDCECM_NOTIF_SPEED,   /**< Link speed is notified */
} usbus_cdcecm_notif_t;

/**
 * @brief USBUS CDC ECM device interface context
 */
typedef struct usbus_cdcecm_device {
    usbus_handler_t handler_ctrl;           /**< Control interface handler */
    usbus_interface_t iface_data;           /**< Data interface */
    usbus_interface_t iface_ctrl;           /**< Control interface */
    usbus_interface_alt_t iface_data_alt;   /**< Data alternative (active) interface */
    usbus_endpoint_t *ep_in;                /**< Data endpoint in */
    usbus_endpoint_t *ep_out;               /**< Data endpoint out */
    usbus_endpoint_t *ep_ctrl;              /**< Control endpoint */
    usbus_descr_gen_t ecm_descr;            /**< ECM descriptor generator */
    event_t rx_flush;                       /**< Receive flush event */
    event_t tx_xmit;                        /**< Transmit ready event */
    netdev_t netdev;                        /**< Netdev context struct */
    uint8_t mac_netdev[ETHERNET_ADDR_LEN];  /**< this device's MAC address */
    char mac_host[13];                      /**< host side's MAC address as string */
    usbus_string_t mac_str;                 /**< String context for the host side mac address */
    usbus_t *usbus;                         /**< Ptr to the USBUS context */
    mutex_t out_lock;                       /**< mutex used for locking netif/USBUS send */
    size_t tx_len;                          /**< Length of the current tx frame */
    usbus_cdcecm_notif_t notif;             /**< Startup message notification tracker */
    unsigned active_iface;                  /**< Current active data interface */

    /**
     * @brief Buffer for received frames from the host
     */
    usbdev_ep_buf_t data_out[USBUS_ETHERNET_FRAME_BUF];

    /**
     * @brief Host in device out data buffer
     */
    usbdev_ep_buf_t data_in[USBUS_CDCECM_EP_DATA_SIZE];

    /**
     * @brief Host out device in control buffer
     */
    usbdev_ep_buf_t control_in[USBUS_CDCECM_EP_CTRL_SIZE];
    /**
     * @brief Host out device in reception URB
     */
    usbus_urb_t out_urb;
} usbus_cdcecm_device_t;

/**
 * @brief CDC ECM initialization function
 *
 * @param   usbus   USBUS thread to use
 * @param   handler CDCECM device struct
 */
void usbus_cdcecm_init(usbus_t *usbus, usbus_cdcecm_device_t *handler);

#ifdef __cplusplus
}
#endif

#endif /* USB_USBUS_CDC_ECM_H */
/** @} */
