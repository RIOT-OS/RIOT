/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  netdev
 * @{
 *
 * @file        netdev/base.h
 * @brief       Basic network device driver interface definitions.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef NETDEV_BASE_H_
#define NETDEV_BASE_H_

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type for @ref msg_t if device fired an event.
 */
#define NETDEV_MSG_EVENT_TYPE   (0x0100)

/**
 * @brief   Definition of device families.
 */
typedef enum {
    NETDEV_TYPE_UNKNOWN = 0,    /**< Type was not specified and may not
                                     understand this API */
    NETDEV_TYPE_BASE,           /**< Device understands this API */
    NETDEV_TYPE_802154,         /**< Device understands this API and the API
                                     defined in @ref netdev_802154 */
} netdev_type_t;

/**
 * @brief   Definition of protocol families to determine which frame types a
 *          device or protocol layer (see @ref netapi) can handle
 *
 * @note    XXX: The concrete definition of the values is necessary to work
 *          with super-flexible devices as e.g. @ref native_net. It was also
 *          decided not to use ethertype since protocols not supplied by it
 *          might be supported
 */
typedef enum {
    NETDEV_PROTO_UNKNOWN        = 0x0000,   /**< Type was not specified */

    /**
     * @brief   Radio frame protocol
     *
     * @details Sends frames as defined by radio_packet_t.
     */
    NETDEV_PROTO_RADIO          = 0x0001,

    /**
     * @brief   IEEE 802.15.4
     *
     * @details Sends frames as defined by ieee802154_frame_t
     */
    NETDEV_PROTO_802154         = 0x0002,
    NETDEV_PROTO_6LOWPAN        = 0x0003,   /**< 6LoWPAN. */
    NETDEV_PROTO_IPV6           = 0x0004,   /**< IPv6. */
    NETDEV_PROTO_UDP            = 0x0005,   /**< UDP. */
    NETDEV_PROTO_TCP            = 0x0006,   /**< TCP. */
    NETDEV_PROTO_CCNL           = 0x0007,   /**< CCN lite. */

    /**
     * @brief   CC110x frame format protocol
     *
     * @detail  Sends frames as defined by cc110x_packet_t.
     */
    NETDEV_PROTO_CC110X         = 0x0008,
} netdev_proto_t;

/**
 * @brief   Definition of basic network device options.
 * @note    Feel free to expand if your device needs/supports more.
 */
typedef enum {
    /**
     * @brief   Communication type for the device as defined by @ref netdev_proto_t
     *
     * @details If a driver does not support the type (but the setting of the
     *          option is supported) it @ref netdev_driver_t::set_option() shall result
     *          with -EPROTONOSUPPORT.
     *
     *          *value_len* for @ref netdev_driver_t::get_option() must always be at
     *          least `sizeof(netdev_proto_t)`.
     */
    NETDEV_OPT_PROTO = 0,
    NETDEV_OPT_CHANNEL,             /**< Channel for the device as unsigned value
                                         in host byte order */
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
    NETDEV_STATE_POWER_OFF = 0,         /**< Device is powered off */
    NETDEV_STATE_POWER_SLEEP,           /**< Device is sleeping */
    NETDEV_STATE_POWER_IDLE,            /**< Device is idle */
    NETDEV_STATE_RX_MODE,               /**< Device is in receive mode */
    NETDEV_STATE_PROMISCUOUS_MODE,      /**< Device is in receive mode and
                                             accepts all packets without regard
                                             for their destination */
    NETDEV_STATE_TX_BURST,              /**< Device is burst sending and
                                             does not accept packets */
} netdev_state_t;

/**
 * @brief   Circular list type to store a number of protocol headers of
 *          unspecified type to work with @ref clist.h.
 *
 * @extends clist_node_t
 */
typedef struct __attribute__((packed)) netdev_hlist_t {
    struct netdev_hlist_t *next;    /**< next element in list */
    struct netdev_hlist_t *prev;    /**< previous element in list */
    netdev_proto_t protocol;        /**< protocol of the header */
    void *header;                   /**< the header stored in here */
    size_t header_len;              /**< the length of the header in byte */
} netdev_hlist_t;

/**
 * @brief   Definition of the network device type
 *
 * @see struct netdev_t
 *
 * @note    Forward definition to use in @ref netdev_driver_t
 */
typedef struct netdev_t netdev_t;

/**
 * Receive data callback for data frames from given network device.
 *
 * @param[in] dev           the network device the frame came from.
 * @param[in] src           the (hardware) source address of the frame in host
 *                          byte order.
 * @param[in] src_len       the length of *src* in byte
 * @param[in] dest          the (hardware) destination address of the frame in
 *                          host byte order.
 * @param[in] dest_len      the length of *dest* in byte
 * @param[in] payload       the payload of the frame.
 * @param[in] payload_len   the length of *payload* in byte
 *
 * @return  the number of bytes in payload actually processed by the callback,
 *          on success
 * @return  a fitting negative errno on failure
 */
typedef int (*netdev_rcv_data_cb_t)(netdev_t *dev, void *src, size_t src_len,
                                    void *dest, size_t dest_len, void *payload,
                                    size_t payload_len);

/**
 * @brief   Network device API definition.
 *
 * @details This is a set of functions that must be implemented by any driver
 *           for a network device.
 */
typedef struct {
    /**
     * @brief Initialize a given network device.
     *
     * @param[in] dev           the device to initialize
     *
     * @return  0 on success
     * @return  -ENODEV if *dev* can not be initialized
     */
    int (*init)(netdev_t *dev);

    /**
     * @brief Send data via a given network device
     *
     * @param[in] dev               the network device
     * @param[in] dest              the (hardware) destination address for the data
     *                              in host byte order.
     * @param[in] dest_len          the length of *dest* in byte
     * @param[in] upper_layer_hdrs  header data from higher network layers from
     *                              highest to lowest layer. Must be prepended to
     *                              the data stream by the network device. May be
     *                              NULL if there are none.
     * @param[in] data              the data to send
     * @param[in] data_len          the length of *data* in byte
     *
     * @return  the number of byte actually send on success
     * @return  -EAFNOSUPPORT if address of length dest_len is not supported
     *          by the device *dev*
     * @return  -ENODEV if *dev* is not recognized
     * @return  -EMSGSIZE if the total frame size is too long to fit in a frame
     *          of the device *dev*
     * @return  a fitting negative other errno on other failure
     */
    int (*send_data)(netdev_t *dev, void *dest, size_t dest_len,
                     netdev_hlist_t *upper_layer_hdrs, void *data,
                     size_t data_len);

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
     * @param[in] state    the network device
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     * @return  -ENOTSUP, if *state* is not supported
     * @return  -ETIME, if device timed out on trying to change state
     */
    int (*set_state)(netdev_t *dev, netdev_state_t state);

    /**
     * @brief   Must be called by a controlling thread if a message of type
     *          NETDEV_MSG_EVENT_TYPE was received
     *
     * @param[in] dev           the network device that fired the event.
     * @param[in] event_type    Event type. Values are free to choose for the
     *                          driver. Must be given in the @ref msg_t::value
     *                          of the received message
     */
    void (*event)(netdev_t *dev, uint32_t event_type);
} netdev_driver_t;

/**
 * @brief   Definition of the network device type
 *
 * @note    Your driver may overload this with additional information (e.g.
 *          how the device is connected)
 */
struct netdev_t {
    netdev_type_t type;                 /**< The type of this device */
    const netdev_driver_t *driver;      /**< The driver for this device */
    void *more;                         /**< Pointer to device dependent
                                             additional information. E.g. the
                                             low-level device(s) to
                                             communiticate with this device. */
};

/**
 * @brief   Helper function to calculate the total length of the headers in
 *          *hlist*.
 *
 * @param[in]   hlist   The header list.
 *
 * @return  Length in number of bytes of all headers in *hlist*.
 */
size_t netdev_get_hlist_len(const netdev_hlist_t *hlist);

/**
 * @brief Advance the header list
 *
 * @see clist_advance
 *
 * @param[in,out] list  The list to work upon.
 */
static inline void netdev_hlist_advance(netdev_hlist_t **list)
{
    clist_advance((clist_node_t **)list);
}

/**
 * @brief Add *node* to list start of *list*
 *
 * @param[in,out] list  The list to add *node* to
 * @param[in] node      The node to add to the list
 */
static inline void netdev_hlist_add(netdev_hlist_t **list,
                                    netdev_hlist_t *node)
{
    clist_add((clist_node_t **)list, (clist_node_t *)node);
    *list = node;
}

/**
 * @brief Removes *node* from list *list*
 *
 * @see clist_remove
 *
 * @param[in,out] list  The list to remove *node* from
 * @param[in] node      The node to remove from the list
 */
static inline void netdev_hlist_remove(netdev_hlist_t **list,
                                       netdev_hlist_t *node)
{
    clist_remove((clist_node_t **)list, (clist_node_t *)node);
}

#ifdef __cplusplus
}
#endif

#endif /* NETDEV_BASE_H_ */
/**
 * @}
 */
