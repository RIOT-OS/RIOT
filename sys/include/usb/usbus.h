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

#include "clist.h"
#include "event.h"
#include "sched.h"
#include "kernel_defines.h"
#include "msg.h"
#include "thread.h"

#include "usb.h"
#include "periph/usbdev.h"
#include "usb/descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup usb_usbus_conf    USBUS compile time configurations
 * @ingroup usb_conf
 * @{
 */
/**
 * @brief USBUS thread stack size
 */
#ifndef USBUS_STACKSIZE
#define USBUS_STACKSIZE             (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief USBUS thread priority
 */
#ifndef USBUS_PRIO
#define USBUS_PRIO                  (THREAD_PRIORITY_MAIN - 6)
#endif

/**
 * @brief USBUS auto attach setting
 *
 * When set to 1, the USBUS thread will automatically enable the USB pull-up
 * resistor after initializing the thread. This will signal to the host
 * that the USB peripheral is ready for use.
 */
#ifndef CONFIG_USBUS_AUTO_ATTACH
#if !IS_ACTIVE(KCONFIG_MODULE_USBUS)
#define CONFIG_USBUS_AUTO_ATTACH            1
#endif
#endif

/**
 * @brief USBUS endpoint 0 buffer size
 *
 * This configures the buffer size of the control endpoint. Unless you transfer
 * large amount of data often over the control endpoint, a minimal size should
 * be sufficient
 */
#if IS_ACTIVE(CONFIG_USBUS_EP0_SIZE_8)
#define CONFIG_USBUS_EP0_SIZE               8
#elif IS_ACTIVE(CONFIG_USBUS_EP0_SIZE_16)
#define CONFIG_USBUS_EP0_SIZE              16
#elif IS_ACTIVE(CONFIG_USBUS_EP0_SIZE_32)
#define CONFIG_USBUS_EP0_SIZE              32
#elif IS_ACTIVE(CONFIG_USBUS_EP0_SIZE_64)
#define CONFIG_USBUS_EP0_SIZE              64
#endif

#ifndef CONFIG_USBUS_EP0_SIZE
#define CONFIG_USBUS_EP0_SIZE              64
#endif
/** @} */

/**
 * @brief USBUS thread name
 */
#define USBUS_TNAME                 "usbus"

/**
 * @name USBUS thread flags
 *
 * Thread flags used by the USBUS thread. @ref THREAD_FLAG_EVENT is also used,
 * but defined elsewhere
 * @{
 */
#define USBUS_THREAD_FLAG_USBDEV    (0x02)  /**< usbdev esr needs handling */
#define USBUS_THREAD_FLAG_USBDEV_EP (0x04)  /**< One or more endpoints requires
                                                 servicing */
/** @} */

/**
 * @name USBUS handler subscription flags
 *
 * @{
 */
#define USBUS_HANDLER_FLAG_RESET    (0x0001)    /**< Report reset event */
#define USBUS_HANDLER_FLAG_SOF      (0x0002)    /**< Report SOF events */
#define USBUS_HANDLER_FLAG_SUSPEND  (0x0004)    /**< Report suspend events */
#define USBUS_HANDLER_FLAG_RESUME   (0x0008)    /**< Report resume from suspend */
#define USBUS_HANDLER_FLAG_TR_FAIL  (0x0010)    /**< Report transfer fail */
#define USBUS_HANDLER_FLAG_TR_STALL (0x0020)    /**< Report transfer stall complete */
/** @} */

/**
 * @brief USB handler events
 */
typedef enum {
    USBUS_EVENT_USB_RESET,      /**< USB reset event                */
    USBUS_EVENT_USB_SOF,        /**< USB start of frame received    */
    USBUS_EVENT_USB_SUSPEND,    /**< USB suspend condition detected */
    USBUS_EVENT_USB_RESUME,     /**< USB resume condition detected  */
} usbus_event_usb_t;

/**
 * @brief USB endpoint transfer status events
 */
typedef enum {
    USBUS_EVENT_TRANSFER_COMPLETE,  /**< Transfer successfully completed */
    USBUS_EVENT_TRANSFER_FAIL,      /**< Transfer nack replied by peripheral */
    USBUS_EVENT_TRANSFER_STALL,     /**< Transfer stall replied by peripheral */
} usbus_event_transfer_t;

/**
 * @brief state machine states for the global USBUS thread
 */
typedef enum {
    USBUS_STATE_DISCONNECT, /**< Device is disconnected from the host */
    USBUS_STATE_RESET,      /**< Reset condition received */
    USBUS_STATE_ADDR,       /**< Address configured */
    USBUS_STATE_CONFIGURED, /**< Peripheral is configured */
    USBUS_STATE_SUSPEND,    /**< Peripheral is suspended by the host */
} usbus_state_t;

/**
 * @brief USBUS control request state machine
 */
typedef enum {
    USBUS_CONTROL_REQUEST_STATE_READY,    /**< Ready for new control request */
    USBUS_CONTROL_REQUEST_STATE_INDATA,   /**< Request received with expected
                                               DATA IN stage */
    USBUS_CONTROL_REQUEST_STATE_OUTACK,   /**< Expecting a zero-length ack OUT
                                               request from the host */
    USBUS_CONTROL_REQUEST_STATE_OUTDATA,  /**< Data OUT expected */
    USBUS_CONTROL_REQUEST_STATE_INACK,    /**< Expecting a zero-length ack IN
                                               request from the host */
} usbus_control_request_state_t;

/**
 * @brief USBUS string type
 */
typedef struct usbus_string {
    struct usbus_string *next;  /**< Ptr to the next registered string */
    const char *str;            /**< C string to use as content        */
    uint16_t idx;               /**< USB string index                  */
} usbus_string_t;

/**
 * @brief USBUS context forward declaration
 */
typedef struct usbus usbus_t;

/**
 * @brief USBUS event handler forward declaration
 */
typedef struct usbus_handler usbus_handler_t;

/**
 * @brief descriptor length types for USB descriptor generators
 */
typedef enum {
    USBUS_DESCR_LEN_FIXED,  /**< Descriptor always generates a fixed length */
    USBUS_DESCR_LEN_FUNC,   /**< Descriptor length is calculated by a function */
} usbus_descr_len_type_t;

/**
 * @brief USBUS descriptor generator function pointers
 */
typedef struct {
    /**
     * @brief function pointer to format the descriptor content of this
     * descriptor generator. The content of this descriptor is prefixes before
     * the descriptor of the object it is part of.
     *
     * @param usbus The usbus context
     * @param arg   Additional argument for the descriptor generator
     *
     * @return      Length of the generated descriptor
     */
    size_t (*fmt_pre_descriptor)(usbus_t *usbus, void *arg);

    /**
     * @brief function pointer to format the descriptor content of this
     * descriptor generator. The content of this descriptor is appended after
     * the descriptor of the object it is part of.
     *
     * @param usbus The usbus context
     * @param arg   Additional argument for the descriptor generator
     *
     * @return      Length of the generated descriptor
     */
    size_t (*fmt_post_descriptor)(usbus_t *usbus, void *arg);
    union {
        /**
         * @brief USBUS generic descriptor generator generated length
         *
         * Must return the total length of the descriptors that will be
         * generated by @ref fmt_pre_descriptor and @ref fmt_post_descriptor
         * This function is used when @ref len_type is set to
         * @ref USBUS_DESCR_LEN_FUNC.
         *
         * @param usbus The usbus context
         * @param arg   Additional argument for the descriptor generators
         *
         * @return      Length of the generated descriptors
         */
        size_t (*get_descriptor_len)(usbus_t *usbus, void *arg);
        /**
         * @brief Fixed total length of the generated descriptors
         *
         * Must return the total length of the descriptors that will be
         * generated by the @ref fmt_pre_descriptor and @ref fmt_post_descriptor.
         * This value is used when @ref len_type is set to
         * @ref USBUS_DESCR_LEN_FIXED.
         */
        size_t fixed_len;
    } len;                            /**< Fixed or generated length of the descriptor */
    usbus_descr_len_type_t len_type;  /**< Either USBUS_DESCR_LEN_FIXED or USBUS_DESCR_LEN_FUNC */
} usbus_descr_gen_funcs_t;

/**
 * @brief USBUS descriptor generator
 *
 * The functions are called to allow custom modules to define their own
 * descriptors in addition to the USB descriptor. The top level (@ref usbus_t), the
 * interface (@ref usbus_interface_t), interface alternative settings
 * (@ref usbus_interface_alt_t) and endpoints (@ref usbus_endpoint_t) allow for
 * generating additional descriptors
 */
typedef struct usbus_descr_gen {
    struct usbus_descr_gen *next;         /**< ptr to the next descriptor generator */
    const usbus_descr_gen_funcs_t *funcs; /**< Function pointers */
    void *arg;                            /**< Extra context argument for the
                                               descriptor functions */
} usbus_descr_gen_t;

/**
 * @brief USBUS endpoint context
 */
typedef struct usbus_endpoint {
    struct usbus_endpoint *next;    /**< Next endpoint in the
                                         @ref usbus_interface_t list of
                                         endpoints */
    usbus_descr_gen_t *descr_gen;   /**< Linked list of optional additional
                                         descriptor generators */
    usbdev_ep_t *ep;                /**< ptr to the matching usbdev endpoint */
    uint16_t maxpacketsize;         /**< Max packet size of this endpoint */
    uint8_t interval;               /**< Poll interval for interrupt endpoints */
    bool active;                    /**< If the endpoint should be activated after
                                         reset */
} usbus_endpoint_t;

/**
 * @brief USBUS interface alternative setting
 *
 * Used for specifying alternative interfaces for an @ref usbus_interface_t
 */
typedef struct usbus_interface_alt {
    struct usbus_interface_alt *next;   /**< Next alternative setting */
    usbus_descr_gen_t *descr_gen;       /**< Linked list of optional additional
                                             descriptor generators */
    usbus_endpoint_t *ep;               /**< List of associated endpoints for
                                             this alternative setting */
    usbus_string_t *descr;              /**< Descriptor string */
} usbus_interface_alt_t;

/**
 * @brief USBUS interface
 */
typedef struct usbus_interface {
    struct usbus_interface *next;       /**< Next interface (set by USBUS during
                                             registration)                     */
    usbus_descr_gen_t *descr_gen;       /**< Linked list of optional additional
                                             descriptor generators */
    usbus_endpoint_t *ep;               /**< Linked list of endpoints belonging
                                             to this interface                 */
    struct usbus_interface_alt *alts;   /**< List of alt settings              */
    usbus_handler_t *handler;           /**< Handlers for this interface       */
    usbus_string_t *descr;              /**< Descriptor string                 */
    uint16_t idx;                       /**< Interface index, (set by USBUS
                                             during registration               */
    uint8_t class;                      /**< USB interface class               */
    uint8_t subclass;                   /**< USB interface subclass            */
    uint8_t protocol;                   /**< USB interface protocol            */
} usbus_interface_t;

/**
 * @brief USBUS event handler function pointers
 */
typedef struct usbus_handler_driver {

    /**
     * @brief Initialize the event handler
     *
     * This function is called in the USBUS thread context to initialize the
     * event handler
     *
     * @param usbus     USBUS context
     * @param handler   handler context
     */
    void (*init)(usbus_t * usbus, struct usbus_handler *handler);

    /**
     * @brief event handler function
     *
     * This function is passed USBUS events
     *
     * @param usbus     USBUS context
     * @param handler   handler context
     * @param event     @ref usbus_event_usb_t event to handle
     */
    void (*event_handler)(usbus_t * usbus, struct usbus_handler *handler,
                          usbus_event_usb_t event);

    /**
     * @brief transfer handler function
     *
     * This function receives transfer based events
     *
     * @param usbus     USBUS context
     * @param handler   handler context
     * @param ep        usbdev endpoint that triggered the event
     * @param event     @ref usbus_event_transfer_t event
     */
    void (*transfer_handler)(usbus_t * usbus, struct usbus_handler *handler,
                             usbdev_ep_t *ep, usbus_event_transfer_t event);

    /**
     * @brief control request handler function
     *
     * This function receives USB control requests from the USBUS stack.
     *
     * @param usbus     USBUS context
     * @param handler   handler context
     * @param state     control request state
     * @param setup     setup packet
     *
     * @return          Size of the returned data when the request is handled
     * @return          negative to have the stack return an USB stall to the
     *                  host
     * @return          zero when the request is not handled by this handler
     */
    int (*control_handler)(usbus_t * usbus, struct usbus_handler *handler,
                           usbus_control_request_state_t state,
                           usb_setup_t *request);
} usbus_handler_driver_t;

/**
 * @brief USBUS handler struct
 *
 * Inherit from this struct for custom USB functionality
 */
struct usbus_handler {
    struct usbus_handler *next;             /**< List of handlers (to be used by
                                                 @ref usbus_t) */
    const usbus_handler_driver_t *driver;   /**< driver for this handler */
    usbus_interface_t *iface;               /**< Interface this handler belongs
                                                 to */
    uint32_t flags;                         /**< Report flags */
};

/**
 * @brief USBUS context struct
 */
struct usbus {
    usbus_string_t manuf;                           /**< Manufacturer string                   */
    usbus_string_t product;                         /**< Product string                        */
    usbus_string_t config;                          /**< Configuration string                  */
    usbus_string_t serial;                          /**< serial string                  */
    usbus_endpoint_t ep_out[USBDEV_NUM_ENDPOINTS];  /**< USBUS OUT endpoints                   */
    usbus_endpoint_t ep_in[USBDEV_NUM_ENDPOINTS];   /**< USBUS IN endpoints                    */
    event_queue_t queue;                            /**< Event queue                           */
    usbdev_t *dev;                                  /**< usb phy device of the usb manager     */
    usbus_handler_t *control;                       /**< Ptr to the control endpoint handler   */
    usbus_descr_gen_t *descr_gen;                   /**< Linked list of top level descriptor
                                                         generators                            */
    usbus_string_t *strings;                        /**< List of descriptor strings            */
    usbus_interface_t *iface;                       /**< List of USB interfaces                */
    usbus_handler_t *handlers;                      /**< List of event callback handlers       */
    uint32_t ep_events;                             /**< bitflags with endpoint event state    */
    kernel_pid_t pid;                               /**< PID of the usb manager's thread       */
    uint16_t str_idx;                               /**< Number of strings registered          */
    usbus_state_t state;                            /**< Current state                         */
    usbus_state_t pstate;                           /**< state to recover to from suspend      */
    uint8_t addr;                                   /**< Address of the USB peripheral         */
#ifndef CONFIG_USB_SERIAL_STR
    /**
     * @brief Hex representation of the device serial number
     */
    char serial_str[2 * CONFIG_USB_SERIAL_BYTE_LENGTH + 1];
#endif
};

/**
 * @brief Submit an event to the usbus thread
 *
 * @param   usbus   USBUS context
 * @param   event   event to post
 */
static inline void usbus_event_post(usbus_t *usbus, event_t *event)
{
    event_post(&usbus->queue, event);
}

/**
 * @brief Add a string descriptor to the USBUS thread context
 *
 * @param[in] usbus   USBUS context
 * @param[in] desc    string descriptor context
 * @param[in] str     C string to use
 *
 * @return            Index of the string descriptor
 */
uint16_t usbus_add_string_descriptor(usbus_t *usbus, usbus_string_t *desc,
                                     const char *str);

/**
 * @brief Add an interface to the USBUS thread context
 *
 * @param[in] usbus   USBUS context
 * @param[in] iface   USB interface to add
 *
 * @return          interface index
 */
uint16_t usbus_add_interface(usbus_t *usbus, usbus_interface_t *iface);

/**
 * @brief Add alternate settings to a given interface
 *
 * @param[in] iface   USB interface
 * @param[in] alt     alternate settings interface to add
 */
void usbus_add_interface_alt(usbus_interface_t *iface,
                                 usbus_interface_alt_t *alt);

/**
 * @brief Find an endpoint from an interface based on the endpoint properties
 *
 * This iterates over the endpoints in an interface and will return the first
 * endpoint from the interface matching the @p type and @p dir. It will return
 * NULL when no matching endpoint is found.
 *
 * @param[in] interface interface to look in
 * @param[in] type      endpoint type to match
 * @param[in] dir       endpoint direction to match
 *
 * @return              ptr to the first matching endpoint
 * @return              NULL when no endpoint is found
 */
usbus_endpoint_t *usbus_interface_find_endpoint(usbus_interface_t *interface,
                                                usb_ep_type_t type,
                                                usb_ep_dir_t dir);

/**
 * @brief Add an endpoint to the specified interface
 *
 * An @ref usbdev_ep_t is requested from the low level peripheral matching the
 * type, direction and buffer length.
 *
 * @param[in] usbus USBUS context
 * @param[in] iface USB interface to add the endpoint to
 * @param[in] type  USB endpoint type
 * @param[in] dir   USB endpoint direction
 * @param[in] len   Buffer space for the endpoint to allocate
 *
 * @return           Pointer to the endpoint struct
 * @return           NULL when no endpoint available
 */
usbus_endpoint_t *usbus_add_endpoint(usbus_t *usbus, usbus_interface_t *iface,
                                     usb_ep_type_t type, usb_ep_dir_t dir,
                                     size_t len);

/**
 * @brief Add a generator for generating additional top level USB descriptor
 * content
 *
 * @param[in] usbus         USBUS context
 * @param[in] descr_gen     descriptor generator to add
 */
void usbus_add_conf_descriptor(usbus_t *usbus, usbus_descr_gen_t *descr_gen);

/**
 * @brief Add an event handler to the USBUS context
 *
 * The handler must also belong to an interface
 * (@ref usbus_interface_t::handler must point to @p handler) for
 * transfer event callbacks to work.
 *
 * @param[in] usbus     USBUS context
 * @param[in] handler   event handler to register
 */
void usbus_register_event_handler(usbus_t *usbus, usbus_handler_t *handler);

/**
 * @brief Initialize an USBUS context.
 *
 * @param[in] usbus   context to initialize
 * @param[in] usbdev  usbdev peripheral to use by USBUS
 */
void usbus_init(usbus_t *usbus, usbdev_t *usbdev);

/**
 * @brief Create and start the USBUS thread
 *
 * @param[in] stack     The stack for the USBUS thread.
 * @param[in] stacksize Size of @p stack.
 * @param[in] priority  Priority for the USBUS thread.
 * @param[in] name      Name for the USBUS thread May be NULL.
 * @param[in] usbus     context to start the thread for
 */
void usbus_create(char *stack, int stacksize, char priority,
                  const char *name, usbus_t *usbus);

/**
 * @brief Enable an endpoint
 *
 * @note must only be used before the usb peripheral is attached to the host
 *
 * @param[in] ep  endpoint to enable
 */
static inline void usbus_enable_endpoint(usbus_endpoint_t *ep)
{
    ep->active = true;
}

/**
 * @brief Disable an endpoint
 *
 * @note must only be used before the usb peripheral is attached to the host
 *
 * @param[in]   ep  endpoint to disable
 */
static inline void usbus_disable_endpoint(usbus_endpoint_t *ep)
{
    ep->active = false;
}

/**
 * @brief enable a specific handler flag
 *
 * @param[in]   handler handler to enable the flag for
 * @param[in]   flag    flag to enable
 */
static inline void usbus_handler_set_flag(usbus_handler_t *handler,
                                          uint32_t flag)
{
    handler->flags |= flag;
}

/**
 * @brief disable a specific handler flag
 *
 * @param[in]   handler handler to disable the flag for
 * @param[in]   flag    flag to disable
 */
static inline void usbus_handler_remove_flag(usbus_handler_t *handler,
                                             uint32_t flag)
{
    handler->flags &= ~flag;
}

/**
 * @brief check if a specific handler flag is set
 *
 * @param[in]   handler handler to check for flag
 * @param[in]   flag    flag to check
 *
 * @return          true if the flag is set for this handler
 */
static inline bool usbus_handler_isset_flag(usbus_handler_t *handler,
                                            uint32_t flag)
{
    return handler->flags & flag;
}

#ifdef __cplusplus
}
#endif
#endif /* USB_USBUS_H */
/** @} */
