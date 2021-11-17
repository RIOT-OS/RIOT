/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */
/**
 * @defgroup    drivers_periph_usbdev usbdev - USB Device Driver API
 * @ingroup     drivers_periph
 * @brief       This is a generic low-level USB driver interface
 * @{
 *
 * # About
 *
 * usbdev specifies a common USB device API for low level USB interfaces. The
 * interface is split in a definition for the USB interface hardware and for
 * individual USB endpoints.
 *
 * # Design goals
 *
 *  1. Support for multiple instances on a single board
 *  2. Interface optimized for MCU peripheral interfaces.
 *
 * # Details
 *
 * The driver interface is split in two separate parts. One part is a global USB
 * device interface, the other is an endpoint control API.
 *
 * The usb device driver can manage parts of the USB interface itself such as
 * the pull up resistor state or the USB speed.
 *
 * The endpoint control API manages a single endpoint. This allows for a modular
 * approach where multiple USB functions/interfaces can be multiplexed over a
 * single USB interface. Each interface can be implemented as a separate module.
 * The interface handler does not have to care about the usb device itself or
 * it's endpoint number. It can simply request an available endpoint from the
 * usb device with the @ref usbdev_new_ep function.
 *
 * Each interface handler can request multiple endpoints from the usbdev device.
 * It must supply the expected maximum endpoint transfer size and other
 * properties of the endpoint. A pointer to an usbdev endpoint is returned if
 * an endpoint fitting the requirements is available.
 *
 * Data transmission is done via the @ref usbdev_ep_xmit function. A buffer and
 * length must be supplied. Received data from the host ends up at this buffer
 * automatically by the low level drivers after reception. For reception the
 * maximum expected transfer length must be supplied, equal to the maximum
 * endpoint transfer size. The data received can be less than this.
 *
 * For transmitting data back to the host, a similar approach is used. The data
 * to be transmitted is supplied as buffer and the @ref usbdev_ep_xmit function
 * is called with the buffer and the size of the data.
 *
 * To ensure that the data buffers adhere to the restrictions of the low level
 * USB peripheral memory interface, the specific @ref usbdev_ep_buf_t data type
 * must be used. It behaves as a regular `uint8_t` byte buffer, but is
 * instantiated with the attributes to ensure that the low level DMA interface
 * can use it.
 *
 * A callback function is required for signalling events from the driver. The
 * @ref USBDEV_EVENT_ESR is special in that it indicates that the USB peripheral
 * had an interrupt that needs to be serviced in a non-interrupt context. This
 * requires the usbdev_esr function to be called. In the case that the event is
 * signalled via the @ref usbdev_t::epcb callback, the @ref usbdev_ep_esr
 * should be called to service the endpoint specific events from the
 * peripheral.
 *
 * @file
 * @brief       Definitions low-level USB driver interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_USBDEV_H
#define PERIPH_USBDEV_H

#include <stdint.h>
#include <stddef.h>

#include "assert.h"
#include "periph_cpu.h"
#include "usb.h"
#include "usb/usbopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief usbdev_t forward declaration
 */
typedef struct usbdev usbdev_t;

/**
 * @brief usbdev_ep_t forward declaration
 */
typedef struct usbdev_ep usbdev_ep_t;

/**
 * @brief USBDEV endpoint buffer CPU-specific requirements
 *
 * Can be overridden by periph_cpu if needed by the USB peripheral DMA.
 */
#ifndef USBDEV_CPU_DMA_REQUIREMENTS
#define USBDEV_CPU_DMA_REQUIREMENTS
#endif

/**
 * @brief Instantiation type for usbdev endpoint buffers
 *
 * Functions passing around pointers to these buffers can still use `uint8_t`
 * for the argument type.
 *
 * Example usage:
 *
 * ```
 * usbdev_ep_buf_t buffer[64];
 * ```
 *
 * @note This is a define and not a typedef so that the above works. With a
 * typedef it would instantiate an array of aligned uint8_t and not an aligned
 * array of uint8_t (assuming the requirement is alignment).
 */
#define usbdev_ep_buf_t USBDEV_CPU_DMA_REQUIREMENTS uint8_t

/**
 * @brief Number of USB IN and OUT endpoints allocated
 *
 * Configures the number of endpoints allocated. An equal number of IN and OUT
 * endpoints are allocated
 */
#ifndef USBDEV_NUM_ENDPOINTS
#define USBDEV_NUM_ENDPOINTS       8
#endif

/**
 * @brief   List of event types that can be send from the device driver to the
 *          upper layer
 */
typedef enum {
    /**
     * @brief Driver needs it's ESR (event service routine) handled
     */
    USBDEV_EVENT_ESR = 1,

    /**
     * @brief Host connection detected
     *
     * A host has connected to the device. Detection should happen by detecting
     * the USB power, but other detection mechanisms might be used.
     */
    USBDEV_EVENT_HOST_CONNECT,

    /**
     * @brief Host disconnected from the device
     *
     * Similar to @ref USBDEV_EVENT_HOST_CONNECT, except that the host
     * disconnected instead.
     */
    USBDEV_EVENT_HOST_DISCONNECT,

    /**
     * @brief Line reset occurred
     *
     * A line reset is a host initiated USB reset to the peripheral
     *
     * The peripheral driver must clears the following settings before
     * emitting this event:
     *  - Device address
     *  - Endpoint configuration, including stall settings
     */
    USBDEV_EVENT_RESET,

    /**
     * @brief Start of Frame received
     *
     * A Start of Frame (SoF) packet is received from the host.
     */
    USBDEV_EVENT_SOF,

    /**
     * @brief USB suspend condition active
     */
    USBDEV_EVENT_SUSPEND,

    /**
     * @brief USB suspend condition no longer active
     */
    USBDEV_EVENT_RESUME,

    /**
     * @brief Transaction completed event.
     *
     * An endpoint must emit this event after a transaction with the host
     * occurred to indicate that the data in the buffer is used or new
     * depending on the endpoint direction
     */
    USBDEV_EVENT_TR_COMPLETE,

    /**
     * @brief Transaction stall event.
     *
     * An endpoint should emit this event after a stall reply has been
     * transmitted to the host
     */
    USBDEV_EVENT_TR_STALL,

    /**
     * @brief Transaction fail event.
     *
     * @deprecated This event is deprecated as only a limited number of devices
     *             implement and doesn't offer value for upper layer code. This
     *             event will be removed after the 2022.07 release.
     *
     * An endpoint should emit this event after a nack reply to the host.
     */
    USBDEV_EVENT_TR_FAIL,
    /* expand list if required */
} usbdev_event_t;

/**
 * @brief   Event callback for signaling usbdev event to upper layers
 *
 * @param[in] usbdev        usbdev context
 * @param[in] event         type of the event
 */
typedef void (*usbdev_event_cb_t)(usbdev_t *usbdev, usbdev_event_t event);

/**
 * @brief   Event callback for signaling endpoint events to upper layers
 *
 * @param[in] ep            usbdev endpoint context
 * @param[in] event         type of the event
 */
typedef void (*usbdev_ep_event_cb_t)(usbdev_ep_t *ep,
                                     usbdev_event_t event);

/**
 * @brief usbdev device descriptor
 */
struct usbdev {
    const struct usbdev_driver *driver; /**< usbdev driver struct           */
    usbdev_event_cb_t cb;               /**< Event callback supplied by
                                          *  upper layer                    */
    usbdev_ep_event_cb_t epcb;          /**< Endpoint event callback for
                                          *  upper layer                    */
    void *context;                      /**< Ptr to the thread context      */
};

/**
 * @brief usbdev endpoint descriptor
 */
struct usbdev_ep {
    usbdev_t *dev;      /**< USB device this endpoint belongs to            */
    size_t len;         /**< Endpoint configured max transfer size in bytes */
    usb_ep_dir_t dir;   /**< Endpoint direction                             */
    usb_ep_type_t type; /**< Endpoint type                                  */
    uint8_t num;        /**< Endpoint number                                */
};

/**
 * @brief usbdev driver functions
 *
 * Helpers (such as @ref usbdev_init) are provided and should be used instead.
 * Directly calling these functions is not recommended.
 */
typedef struct usbdev_driver {

    /**
     * @brief Initialize the USB peripheral device
     *
     * This initializes the USB device but must not enable the USB pull up.
     *
     * @param[in]   dev     USB device descriptor
     */
    void (*init)(usbdev_t *usbdev);

    /**
     * @brief Retrieve an USB endpoint of the specified type
     *
     * requesting an endpoint of @ref USB_EP_TYPE_CONTROL must always return
     * endpoint 0 of the specified direction
     *
     * @param[in]   dev     USB device descriptor
     * @param[in]   type    USB endpoint type
     * @param[in]   dir     USB endpoint direction
     * @param[in]   size    USB endpoint buffer size
     *
     * @return              ptr to the new USB endpoint descriptor
     * @return              NULL on error
     */
    usbdev_ep_t *(*new_ep)(usbdev_t *dev, usb_ep_type_t type, usb_ep_dir_t dir, size_t size);

    /**
     * @brief   Get an option value from a given usb device
     *
     * @param[in]   dev     USB device descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              `< 0` on error 0
     */
    int (*get)(usbdev_t *usbdev, usbopt_t opt,
               void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given usb device
     *
     * @param[in] dev       USB device descriptor
     * @param[in] opt       option type
     * @param[in] value     value to set
     * @param[in] value_len the length of @p value
     *
     * @return              number of bytes used from @p value
     * @return              `< 0` on error
     */
    int (*set)(usbdev_t *usbdev, usbopt_t opt,
               const void *value, size_t value_len);

    /**
     * @brief a driver's user-space event service handler
     *
     * This function will be called from a USB stack's loop when being
     * notified by usbdev_event_isr.
     *
     * @param[in]   dev     USB device descriptor
     */
    void (*esr)(usbdev_t *dev);

    /**
     * @brief Initialize the USB endpoint
     *
     * This initializes the USB endpoint with the settings from the
     * @ref usbdev_ep.
     *
     * @param[in]   ep      USB endpoint descriptor
     */
    void (*ep_init)(usbdev_ep_t *ep);

    /**
     * @brief   Get an option value from a given usb device endpoint
     *
     * @param[in]   ep      USB endpoint descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximum number of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              `< 0` on error
     */
    int (*ep_get)(usbdev_ep_t *ep, usbopt_ep_t opt,
               void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given usb device endpoint
     *
     * @param[in] ep        USB endpoint descriptor
     * @param[in] opt       option type
     * @param[in] value     pointer to the value
     * @param[in] value_len the length of @p value
     *
     * @return              number of bytes used from @p value
     * @return              `< 0` on error
     */
    int (*ep_set)(usbdev_ep_t *ep, usbopt_ep_t opt,
               const void *value, size_t value_len);

    /**
     * @brief an endpoint's user-space event handler
     *
     * Must be called in response to an @ref USBDEV_EVENT_ESR event in
     * userspace context.
     *
     * @param[in] ep        USB endpoint descriptor to service
     */
    void (*ep_esr)(usbdev_ep_t *ep);

    /**
     * @brief Transmit a data buffer
     *
     * Ownership of the @p buf is transferred to the usbdev device after calling
     * this. Do not modify (or unallocate) the buffer between calling this and
     * when it is released via the @ref USBDEV_EVENT_TR_COMPLETE event.
     *
     * This clears the stall setting in the endpoint if that is enabled.
     *
     * @note The @p buf passed here must have been declared as
     * @ref usbdev_ep_buf_t before so that DMA restrictions are applied to it
     *
     * @param[in] ep        USB endpoint descriptor
     * @param[inout] buf    Buffer with the data to transfer
     * @param[in] len       (Max) Length of the data to transfer
     */
    int (*xmit)(usbdev_ep_t *ep, uint8_t *buf, size_t len);
} usbdev_driver_t;

/**
 * @brief Low level USB peripheral driver initialization
 *
 * This function prepares all usbdev peripherals available for initialization
 */
void usbdev_init_lowlevel(void);

/**
 * @brief Retrieve usbdev context from the peripheral
 *
 * @param num   usbdev peripheral number to retrieve
 *
 * @returns     the usbdev context at index @p num
 */
usbdev_t *usbdev_get_ctx(unsigned num);

/**
 * @brief Initialize the USB peripheral device
 *
 * @see @ref usbdev_driver_t::init
 *
 * @pre `(dev != NULL)`
 *
 * @param dev   usb device to initialize
 */
static inline void usbdev_init(usbdev_t *dev)
{
    assert(dev);
    dev->driver->init(dev);
}

/**
 * @brief Retrieve an USB endpoint of the specified type
 *
 * @see @ref usbdev_driver_t::new_ep
 *
 * @pre `(dev != NULL)`
 *
 * @param[in]   dev     USB device descriptor
 * @param[in]   type    USB endpoint type
 * @param[in]   dir     USB endpoint direction
 * @param[in]   size    Maximum USB endpoint buffer size used
 *
 * @return              ptr to the new USB endpoint descriptor
 * @return              NULL on error
 */
static inline usbdev_ep_t * usbdev_new_ep(usbdev_t *dev, usb_ep_type_t type,
                                          usb_ep_dir_t dir, size_t size)
{
    assert(dev);
    return dev->driver->new_ep(dev, type, dir, size);
}

/**
 * @brief   Get an option value from a given usb device
 *
 * @see @ref usbdev_driver_t::get
 *
 * @pre `(dev != NULL)`
 *
 * @param[in]   dev     USB device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              `< 0` on error 0
 */
static inline int usbdev_get(usbdev_t *dev, usbopt_t opt,
                             void *value, size_t max_len)
{
    assert(dev);
    return dev->driver->get(dev, opt, value, max_len);
}

/**
 * @brief   Set an option value for a given usb device
 *
 * @see @ref usbdev_driver_t::set
 *
 * @pre `(dev != NULL)`
 *
 * @param[in] dev       USB device descriptor
 * @param[in] opt       option type
 * @param[in] value     value to set
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              `< 0` on error
 */
static inline int usbdev_set(usbdev_t *dev, usbopt_t opt,
                             const void *value, size_t value_len)
{
    assert(dev);
    return dev->driver->set(dev, opt, value, value_len);
}

/**
 * @brief a driver's user-space event service handler
 *
 * @see @ref usbdev_driver_t::esr
 *
 * @pre `(dev != NULL)`
 *
 * @param[in]   dev     USB device descriptor
 */
static inline void usbdev_esr(usbdev_t *dev)
{
    assert(dev);
    dev->driver->esr(dev);
}

/**
 * @brief Initialize the USB endpoint
 *
 * @see @ref usbdev_driver_t::ep_init
 *
 * @pre `(ep != NULL)`
 * @pre `(ep->dev != NULL)`
 *
 * @param[in]   ep      USB endpoint descriptor
 */
static inline void usbdev_ep_init(usbdev_ep_t *ep)
{
    assert(ep);
    assert(ep->dev);
    ep->dev->driver->ep_init(ep);
}

/**
 * @brief   Get an option value from a given usb device endpoint
 *
 * @see @ref usbdev_driver_t::ep_get
 *
 * @pre `(ep != NULL)`
 * @pre `(ep->dev != NULL)`
 *
 * @param[in]   ep      USB endpoint descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximum number of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              `< 0` on error
 */
static inline int usbdev_ep_get(usbdev_ep_t *ep, usbopt_ep_t opt,
                                void *value, size_t max_len)
{
    assert(ep);
    assert(ep->dev);
    return ep->dev->driver->ep_get(ep, opt, value, max_len);
}

/**
 * @brief   Set an option value for a given usb device endpoint
 *
 * @see @ref usbdev_driver_t::ep_set
 *
 * @pre `(ep != NULL)`
 * @pre `(ep->dev != NULL)`
 *
 * @param[in] ep        USB endpoint descriptor
 * @param[in] opt       option type
 * @param[in] value     pointer to the value
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              `< 0` on error
 */
static inline int usbdev_ep_set(usbdev_ep_t *ep, usbopt_ep_t opt,
                                const void *value, size_t value_len)
{
    assert(ep);
    assert(ep->dev);
    return ep->dev->driver->ep_set(ep, opt, value, value_len);
}

/**
 * @brief an endpoint's user-space event handler
 *
 * @see @ref usbdev_driver_t::ep_esr
 *
 * @pre `(ep != NULL)`
 * @pre `(ep->dev != NULL)`
 *
 * @param[in] ep        USB endpoint descriptor to service
 */
static inline void usbdev_ep_esr(usbdev_ep_t *ep)
{
    assert(ep);
    assert(ep->dev);
    ep->dev->driver->ep_esr(ep);
}

/**
 * @brief Submit a buffer for a USB endpoint transmission
 *
 * When dealing with an OUT type endpoint, @p len must be the maximum allowed
 * transfer size for the endpoint. The host is allowed to transfer fewer bytes
 * than @p len.
 *
 * @see @ref usbdev_driver_t::xmit
 *
 * @pre `(ep != NULL)`
 * @pre `(ep->dev != NULL)`
 *
 * @param[in]    ep     USB endpoint descriptor
 * @param[inout] buf    Buffer to submit for transmission
 * @param[in]    len    length of the buffer in bytes to be transmitted or received
 */
static inline int usbdev_ep_xmit(usbdev_ep_t *ep, uint8_t *buf, size_t len)
{
    assert(ep);
    assert(ep->dev);
    return ep->dev->driver->xmit(ep, buf, len);
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_USBDEV_H */
/** @} */
