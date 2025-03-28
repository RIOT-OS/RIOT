/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#ifndef CAN_CANDEV_H
#define CAN_CANDEV_H

/**
 * @defgroup    drivers_candev CAN device driver interface
 * @ingroup     drivers_can
 * @brief       Definitions for low-level CAN driver interface
 * @{
 *
 * This is the CAN controller generic driver interface
 *
 * @file
 * @brief       Definitions for low-level CAN driver interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "can/can.h"
#include "can/common.h"
#include "mutex.h"

/**
 * @brief   Possible event types that are sent from the device driver to the
 *          upper layer
 */
typedef enum {
    CANDEV_EVENT_NOEVENT,          /**< no event, used internally */
    CANDEV_EVENT_ISR,              /**< driver needs its ISR handled */
    CANDEV_EVENT_WAKE_UP,          /**< driver has been woken up by bus */
    CANDEV_EVENT_TX_CONFIRMATION,  /**< a packet has been sent */
    CANDEV_EVENT_TIMEOUT_TX_CONF,  /**< tx conf timeout received */
    CANDEV_EVENT_RX_INDICATION,    /**< a packet has been received */
    CANDEV_EVENT_TX_ERROR,         /**< there was an error when transmitting */
    CANDEV_EVENT_RX_ERROR,         /**< there was an error when receiving */
    CANDEV_EVENT_BUS_OFF,          /**< bus-off detected */
    CANDEV_EVENT_ERROR_PASSIVE,    /**< driver switched in error passive */
    CANDEV_EVENT_ERROR_WARNING,    /**< driver reached error warning */
    /* expand this list if needed */
} candev_event_t;

/**
 * @brief   Forward declaration for candev struct
 */
typedef struct candev candev_t;

/**
 * @brief   Event callback for signaling event to upper layers
 *
 * @param[in] dev           CAN device descriptor
 * @param[in] event         type of the event
 * @param[in] arg           event argument
 */
typedef void (*candev_event_cb_t)(candev_t *dev, candev_event_t event, void *arg);

/**
 * @brief Structure to hold driver state
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 */
struct candev {
    const struct candev_driver *driver;    /**< ptr to that driver's interface. */
    candev_event_cb_t event_callback;      /**< callback for device events */
    void *isr_arg;                         /**< argument to pass on isr event */
    struct can_bittiming bittiming;        /**< device bittimings */
    enum can_state state;                  /**< device state */
};

/**
 * @brief Structure to hold driver interface -> function mapping
 */
typedef struct candev_driver {
    /**
     * @brief Send packet
     *
     * @param[in] dev       CAN device descriptor
     * @param[in] frame     CAN frame to send
     *
     * @return < 0 on error
     * @return mailbox id >= 0 if OK
     */
    int (*send)(candev_t *dev, const struct can_frame *frame);

    /**
     * @brief Abort a packet sending
     *
     * @param[in] dev       CAN device descriptor
     * @param[in] frame     CAN frame to abort
     *
     * @return < 0 on error
     * @return 0 on OK
     */
    int (*abort)(candev_t *dev, const struct can_frame *frame);

    /**
     * @brief the driver's initialization function
     *
     * @param[in] dev       CAN device descriptor
     *
     * @return < 0 on error, 0 on success
     */
    int (*init)(candev_t *dev);

    /**
     * @brief a driver's user-space ISR handler
     *
     * @param[in] dev       CAN device descriptor
     */
    void (*isr)(candev_t *dev);

    /**
     * @brief   Get an option value from a given CAN device
     *
     * @param[in]   dev     CAN device descriptor
     * @param[in]   opt     option type
     * @param[out]  value   pointer to store the option's value in
     * @param[in]   max_len maximal amount of byte that fit into @p value
     *
     * @return              number of bytes written to @p value
     * @return              <0 on error
     */
    int (*get)(candev_t *dev, canopt_t opt, void *value, size_t max_len);

    /**
     * @brief   Set an option value for a given CAN device
     *
     * @param[in] dev       CAN device descriptor
     * @param[in] opt       option type
     * @param[in] value     value to set
     * @param[in] value_len the length of @p value
     *
     * @return              number of bytes used from @p value
     * @return              <0 on error
     */
    int (*set)(candev_t *dev, canopt_t opt, void *value, size_t value_len);

    /**
     * @brief   Set a receive @p filter
     *
     * @param[in] dev       CAN device descriptor
     * @param[in] filter    filter to set
     *
     * @return              a positive filter number
     * @return              <0 on error
     */
    int (*set_filter)(candev_t *dev, const struct can_filter *filter);

    /**
     * @brief  Remove a @p filter
     *
     * @param[in] dev       CAN device descriptor
     * @param[in] filter    filter to remove
     *
     * @return              0 on success
     * @return              <0 on error
     */
    int (*remove_filter)(candev_t *dev, const struct can_filter *filter);
} candev_driver_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CAN_CANDEV_H */
