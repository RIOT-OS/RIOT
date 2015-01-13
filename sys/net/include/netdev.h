/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netdev Network device driver interface
 * @addtogroup  net
 * @{
 *
 * @file
 * @brief       Network device driver interface definition.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __NETDEV_H_
#define __NETDEV_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type for @ref msg_t if device fired an event.
 */
#define NETDEV_MSG_EVENT_TYPE   (0x0100)

/**
 * @brief   Definition of basic network device options.
 * @note    Feel free to expand if your device needs/supports more.
 */
typedef enum {
    NETDEV_OPT_CHANNEL,             /**< Channel for the device as unsigned value
                                         in host byte order */
    NETDEV_OPT_IS_CHANNEL_CLR,      /**< Check if channel is clear */
    NETDEV_OPT_ADDRESS,             /**< Hardware address for the device as
                                         unsigned value in host byte order */
    NETDEV_OPT_NID,                 /**< Network ID (e.g. PAN ID in IEEE 802.15.4)
                                         for the device as unsigned value in
                                         host byte order */
    NETDEV_OPT_ADDRESS_LONG,        /**< Longer hardware address for the device
                                         (e.g. EUI-64) for the device as
                                         unsigned value in host byte order */
    NETDEV_OPT_TX_POWER,            /**< The output of the device in dB as
                                         signed value in host byte order */
    NETDEV_OPT_MAX_PACKET_SIZE,     /**< Maximum packet size the device supports
                                         unsigned value in host byte order */
    NETDEV_OPT_SRC_LEN,             /**< Default mode the source address is
                                         set to as value of `size_t`. (e.g.
                                         either PAN-centric 16-bit address or
                                         EUI-64 in IEEE 802.15.4) */
    NETDEV_OPT_EN_PRELOADING,       /**< Enable pre-loading of data, transfer
                                         data to device using send_data(), send
                                         by calling set_state(NETDEV_STATE_TX) */
    NETDEV_OPT_EN_PROMISCUOUSMODE   /**< Enable promiscuous mode */
    NETDEV_OPT_EN_AUTOACK,          /**< Automatically send link-layer ACKs */
    NETDEV_OPT_RSSI,                /**< Read the RSSI value from the last transfer */
    NETDEV_OPT_LQI,                 /**< Read the link quality indicator */

    /**
     * @brief   Last value for @ref netdev_opt_t defined here
     *
     * @details Specific devices or modules like @ref netapi that utilize these
     *          values to may define higher values, but they must be greater
     *          or equal to @ref NETDEV_OPT_LAST.
     */
    NETDEV_OPT_LAST,
} netdev_opt_t;

/**
 * @brief   Definition of basic network device.
 * @note    Feel free to expand if your device needs/supports more
 */
typedef enum {
    NETDEV_ACTION_POWEROFF = 0,
    NETDEV_ACTION_SLEEP,
    NETDEV_ACTION_IDLE,
    NETDEV_ACTION_RX,
    NETDEV_ACTION_TX,               /**< only needed if pre-loading is enabled */
} netdev_action_t;

/**
 * @brief   Definition of basic network device.
 * @note    Feel free to expand if your device needs/supports more
 */
typedef enum {
    NETDEV_STATE_OFF = 0,           /**< Device is powered off */
    NETDEV_STATE_SLEEP,             /**< Device is sleeping */
    NETDEV_STATE_IDLE,              /**< Device is idle */
    NETDEV_STATE_RX,                /**< Device is in receive mode */
    NETDEV_STATE_TX,                /**< Device is transmit mode */
    NETDEV_STATE_PROMISCUOUS,       /**< Device is in receive mode and
                                         accepts all packets without regard
                                         for their destination */
    NETDEV_STATE_TX_BURST,          /**< Device is burst sending and
                                         does not accept packets */
} netdev_state_t;

/**
 * @brief   The netdev data-structure holds the minimum information needed for
 *          interaction with MAC layers
 *
 * The netdev structure is the parent for all network device driver descriptors.
 */
typedef struct netdev_t netdev_t;

/**
 * @brief   Receive data callback for data frames from given network device.
 *
 * @param[in] data          pointer to the received data in the packet buffer
 */
typedef void (*netdev_rcv_data_cb_t)(pkt_t *data);

/**
 * @brief   Network device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a network device.
 */
typedef struct {
    /**
     * @brief Send data via a given network device
     *
     * @param[in] dev               the network device
     * @param[in] dest              the (hardware) destination address for the data
     *                              in host byte order.
     * @param[in] dest_len          the length of *dest* in byte
     * @param[in] data              pointer to the data to sent
     *
     * @return  the number of byte actually send on success
     * @return  -EAFNOSUPPORT if address of length dest_len is not supported
     *          by the device *dev*
     * @return  -ENODEV if *dev* is not recognized
     * @return  -EMSGSIZE if the total frame size is too long to fit in a frame
     *          of the device *dev*
     * @return  a fitting negative other errno on other failure
     */
    int (*send_data)(netdev_t *dev, pkt_t *data);

    /**
     * @brief   Registers a receive data callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOBUFS, if maximum number of registrable callbacks is exceeded
     */
    int (*add_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @brief   Unregisters a receive data callback to a given network device.
     *
     * @param[in] dev   the network device.
     * @param[in] cb    the callback.
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (*rem_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @brief   Get an option value from a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[out] value        pointer to store the gotten value in
     * @param[in,out] value_len the length of *value*. Must be initialized to the
     *                          available space in *value* on call.
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if available space in *value* given in *value_len*
     *          is not big enough to store the option value.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*get_option)(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t *value_len);

    /**
     * @brief   Set an option value for a given network device.
     *
     * @param[in] dev           the network device
     * @param[in] opt           the option type
     * @param[in] value         the value to set
     * @param[in] value_len     the length of *value*
     *
     * @return  0, on success
     * @return  -EINVAL, if *value* is not in a required format
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *opt* is not supported for the device with this
     *          operation
     * @return  -EOVERFLOW, if *value_len* is larger then the device expects.
     * @return  -EPROTONOSUPPORT, if *opt* was NETDEV_OPT_PROTO and type is
     *          not supported.
     * @return  any other fitting negative errno if the ones stated above
     *          are not sufficient
     */
    int (*set_option)(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t value_len);

    /**
     * @brief   Get state from a given network device.
     *
     * @param[in] dev       the network device
     * @param[out] state    the network device
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ETIME, if device timed out on trying to acquire state
     */
    int (*get_state)(netdev_t *dev, netdev_state_t *state);

    /**
     * @brief   Set state from a given network device.
     *
     * @param[in] dev      the network device
     * @param[in] action   action to trigger
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *state* is not supported
     * @return  -ETIME, if device timed out on trying to change state
     */
    int (*trigger)(netdev_t *dev, netdev_action_t action);

    /**
     * @brief   Must be called by a controlling thread if a message of type
     *          NETDEV_MSG_EVENT_TYPE was received
     *
     * @param[in] dev           the network device that fired the event.
     * @param[in] event_type    Event type. Values are free to choose for the
     *                          driver. Must be given in the @ref msg_t::value
     *                          of the received message
     */
    void (*event)(netdev_t *dev, uint16_t event_type);
} netdev_driver_t;

struct netdev_t {
    kernel_pid_t pid;           /**< the driver's thread's PID */
    netdev_driver_t *driver;    /**< pointer to the devices interface */
};

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_H_ */
/** @} */
