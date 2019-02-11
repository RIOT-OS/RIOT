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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef USBUS_STACKSIZE
#define USBUS_STACKSIZE             (THREAD_STACKSIZE_DEFAULT)
#endif

#ifndef USBUS_PRIO
#define USBUS_PRIO                  (THREAD_PRIORITY_MAIN - 6)
#endif

#define USBUS_TNAME                 "usbus"

#ifndef USBUS_AUTO_ATTACH
#define USBUS_AUTO_ATTACH           (1)
#endif

#ifndef USBUS_EP0_SIZE
#define USBUS_EP0_SIZE              64
#endif

/**
 * @name USBUS message types
 */
#define USBUS_MSG_TYPE_EVENT        (0x0100)
#define USBUS_MSG_TYPE_EP_EVENT     (0x0200)
#define USBUS_MSG_TYPE_SETUP_RQ     (0x0300)
#define USBUS_MSG_TYPE_TR_COMPLETE  (0x0400)
#define USBUS_MSG_TYPE_HANDLER      (0x0500)

/**
 * @name USBUS global state machine states
 */
typedef enum {
    USBUS_STATE_DISCONNECT, /**< Device is disconnected from the host */
    USBUS_STATE_RESET,      /**< Reset condition received */
    USBUS_STATE_ADDR,       /**< Address configured */
    USBUS_STATE_CONFIGURED, /**< Peripheral is configured */
    USBUS_STATE_SUSPEND,    /**< Peripheral is suspended by the host */
} usbus_state_t;

/**
 * @name USBUS setup request state machine
 */
typedef enum {
    USBUS_SETUPRQ_READY,      /**< Ready for new request */
    USBUS_SETUPRQ_INDATA,     /**< Request received with expected DATA IN stage */
    USBUS_SETUPRQ_OUTACK,     /**< Expecting a ZLP ack from host */
    USBUS_SETUPRQ_OUTDATA,    /**< Data out expected */
    USBUS_SETUPRQ_INACK,      /**< ACK in request */
} usbus_setuprq_state_t;

typedef struct usbus_string {
    struct usbus_string *next;
    const char *str;
    uint16_t idx;
} usbus_string_t;

typedef struct usbus usbus_t;
typedef struct usbus_handler usbus_handler_t;

typedef size_t (*gen_hdr)(usbus_t *usbus, void *arg);
typedef size_t (*hdr_len)(usbus_t *usbus, void *arg);

typedef struct {
    gen_hdr get_header;         /**< function ptr to retrieve the header */
    hdr_len get_header_len;     /**< function ptr to retrieve the header len */
} usbus_hdr_gen_funcs_t;

/**
 * @brief USBUS header generator function pointers
 */
typedef struct usbus_hdr_gen {
    struct usbus_hdr_gen *next;        /**< ptr to the next header generator */
    const usbus_hdr_gen_funcs_t *funcs; /**< Function pointers */
    void *arg;                         /**< Extra argument fot the headers */
} usbus_hdr_gen_t;

typedef struct usbus_endpoint {
    struct usbus_endpoint *next;
    usbus_hdr_gen_t *hdr_gen;
    usbdev_ep_t *ep;
    uint16_t maxpacketsize;             /**< Max packet size of this endpoint */
    uint8_t interval;                   /**< Poll interval for interrupt endpoints */
    bool active;                        /**< If the endpoint should be activated after reset */
} usbus_endpoint_t;

typedef struct usbus_interface_alt {
    struct usbus_interface_alt *next;
    usbus_hdr_gen_t *hdr_gen;
    usbus_endpoint_t *ep;
} usbus_interface_alt_t;

typedef struct usbus_interface {
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
} usbus_interface_t;

typedef struct usbus_handler_driver{
    int (*init)(usbus_t *usbus, struct usbus_handler *handler);
    int (*event_handler)(usbus_t *usbus, struct usbus_handler *handler, uint16_t event, void *arg);
} usbus_handler_driver_t;

struct usbus_handler {
    struct usbus_handler *next;
    const usbus_handler_driver_t *driver;
    usbus_interface_t *iface;
};

typedef struct {
    size_t start;
    size_t cur;
    size_t len;
    size_t transfered;              /**< Number of bytes transfered    */
    size_t reqlen;                  /**< Maximum length of the request */
} usbus_controlslicer_t;

struct usbus {
    usbus_string_t manuf;         /**< Manufacturer string                   */
    usbus_string_t product;       /**< Product string                        */
    usbus_string_t config;        /**< Configuration string                  */
    usbus_controlslicer_t slicer; /**< Slicer for multipart control messages */
    usbdev_ep_t *out;             /**< EP0 out endpoint                      */
    usbdev_ep_t *in;              /**< EP0 in endpoint                       */
    usbdev_t *dev;                /**< usb phy device of the usb manager     */
    usb_setup_t setup;            /**< Last received setup packet            */
    usbus_hdr_gen_t *hdr_gen;
    usbus_string_t *strings;      /**< List of descriptor strings            */
    usbus_interface_t *iface;     /**< List of USB interfaces                */
    usbus_handler_t *handler;     /**< */
    kernel_pid_t pid;             /**< PID of the usb manager's thread       */
    uint16_t addr;                /**< Address of the USB port               */
    usbus_state_t state;          /**< Current state                         */
    usbus_setuprq_state_t setup_state; /**< Setup request state machine      */
    uint16_t str_idx;
};

uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface);
int usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface, usbus_endpoint_t* ep, usb_ep_type_t type, usb_ep_dir_t dir, size_t len);
void usbus_add_conf_descriptor(usbus_t *usbus, usbus_hdr_gen_t* hdr_gen);
usbus_t *usbus_get_ctx(void);
void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler);
void usbus_init(usbus_t *usbus, usbdev_t *usbdev);
void usbus_create(char *stack, int stacksize, char priority,
                   const char *name, usbus_t *usbus);

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
