/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    usb_usbus USBUS device and endpoint manager
 * @ingroup     usb
 * @brief       USBUS (Universal Serial Bus Unified Stack),  USB device
 *              management interface
 *
 * @{
 *
 * @file
 * @brief       USBUS basic interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USB_USBUS_H
#define USB_USBUS_H

#include <stdint.h>
#include <stdlib.h>
#include "usb/usbdev.h"
#include "usb.h"
#include "usb/message.h"

#include "kernel_types.h"
#include "msg.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define USBUS_MSG_TYPE_EVENT   (0x1234)
#define USBUS_MSG_TYPE_EP_EVENT   (0x1235)
#define USBUS_MSG_TYPE_SETUP_RQ   (0x1236)
#define USBUS_MSG_TYPE_TR_COMPLETE   (0x1237)

typedef enum {
    USBUS_STATE_DISCONNECT,
    USBUS_STATE_RESET,
    USBUS_STATE_ADDR,
    USBUS_STATE_CONFIGURED,
    USBUS_STATE_SUSPEND,
} usbus_state_t;

typedef enum {
    USBUS_SETUPRQ_READY,      /**< Ready for new request */
    USBUS_SETUPRQ_INDATA,     /**< Request received with expected DATA IN stage */
    USBUS_SETUPRQ_OUTACK,     /**< Expecting a ZLP ack from host */
    USBUS_SETUPRQ_OUTDATA,    /**< Data out expected */
    USBUS_SETUPRQ_INACK,      /**< ACK in request */
} usbus_setuprq_state_t;

typedef struct usbus_string usbus_string_t;
typedef struct usbus_hdr_gen usbus_hdr_gen_t;
typedef struct usbus_interface usbus_interface_t;
typedef struct usbus_endpoint usbus_endpoint_t;
typedef struct usbus_handler usbus_handler_t;


struct usbus_string {
    struct usbus_string *next;
    uint16_t idx;
    const char *str;
    size_t len;
};

struct usbus_endpoint {
    struct usbus_endpoint *next;
    usbus_hdr_gen_t *hdr_gen;
    usbdev_ep_t *ep;
    uint16_t maxpacketsize;             /**< Max packet size of this endpoint */
    uint8_t interval;                   /**< Poll interval for interrupt endpoints */
    bool active;                        /**< If the endpoint should be activated after reset */
};

struct usbus_interface {
    struct usbus_interface *next;
    struct usbus_interface_alt *alts;
    usbus_handler_t* handler;
    usbus_hdr_gen_t *hdr_gen;     /**< Additional header generators */
    usbus_endpoint_t *ep;         /**< Linked list of endpoints     */
    usbus_string_t *descr;        /**< Descriptor string            */
    uint16_t idx;
    uint8_t class;
    uint8_t subclass;
    uint8_t protocol;
};

typedef struct usbus_interface_alt {
    struct usbus_interface_alt *next;
    usbus_hdr_gen_t *hdr_gen;
    usbus_endpoint_t *ep;
} usbus_interface_alt_t;

typedef struct {
    size_t start;
    size_t cur;
    size_t len;
    size_t transfered;
    size_t reqlen;                  /**< Maximum length of request */
} usbus_controlbuilder_t;

typedef struct {
    usbus_string_t manuf;         /**< Manufacturer string                */
    usbus_string_t product;       /**< Product string                     */
    usbus_string_t config;        /**< Configuration string               */
    usbus_controlbuilder_t builder;
    usbdev_ep_t *out;               /**< EP0 out endpoint                   */
    usbdev_ep_t *in;                /**< EP0 in endpoint                    */
    usbdev_t *dev;                  /**< usb phy device of the usb manager  */
    usb_setup_t setup;              /**< Last received setup packet         */
    usbus_hdr_gen_t *hdr_gen;
    usbus_string_t *strings;      /**< List of descriptor strings         */
    usbus_interface_t *iface;     /**< List of USB interfaces             */
    usbus_handler_t *handler;
    kernel_pid_t pid;               /**< PID of the usb manager's thread    */
    uint16_t addr;                  /**< Address of the USB port            */
    usbus_state_t state;          /**< Current state                      */
    usbus_setuprq_state_t setup_state; /**< Setup request state machine   */
    uint16_t str_idx;
    mutex_t lock;                   /**< Mutex for modifying the object     */
} usbus_t;

struct usbus_hdr_gen {
    struct usbus_hdr_gen *next;
    size_t (*gen_hdr)(usbus_t *usbus, void *arg);
    size_t (*hdr_len)(usbus_t *usbus, void *arg);
    void *arg;
};

typedef struct {
    int (*init)(usbus_t *usbus, struct usbus_handler *handler);
    int (*event_handler)(usbus_t *usbus, struct usbus_handler *handler, uint16_t event, void *arg);
} usbus_handler_driver_t;

struct usbus_handler {
    struct usbus_handler *next;
    const usbus_handler_driver_t *driver;
    usbus_interface_t *iface;
};


uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface);
int usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface, usbus_endpoint_t* ep, usb_ep_type_t type, usb_ep_dir_t dir, size_t len);
void usbus_add_conf_descriptor(usbus_t *usbus, usbus_hdr_gen_t* hdr_gen);
usbus_t *usbus_get_ctx(void);
void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler);
void usbus_init(void);
void usbus_create(char *stack, int stacksize, char priority,
                   const char *name, usbdev_t *usbdev);

size_t usbus_put_bytes(usbus_t *usbus, const uint8_t *buf, size_t len);
size_t usbus_put_char(usbus_t *usbus, char c);
void usbus_ep0_ready(usbus_t *usbus);

static inline void usbus_enable_endpoint(usbus_endpoint_t *ep)
{
    ep->active = true;
}

static inline void usbus_disable_endpoint(usbus_endpoint_t *ep)
{
    ep->active = false;
}

#ifdef __cplusplus
}
#endif
#endif /* USB_USBUS_H */
/** @} */
