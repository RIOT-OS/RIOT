/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_zep  Zigbee Encapsulation Protocol
 * @ingroup     net_gnrc
 * @brief       Transports IEEE 802.15.4 frames over UDP (can be parsed by
 *              Wireshark)
 * @note        Stability uncertain
 * @see         <a href="https://www.wireshark.org/docs/dfref/z/zep.html">
 *                  ZigBee Encapsulation Protocol in the Wireshark docs
 *              </a>
 * @see         <a href="https://code.wireshark.org/review/gitweb?p=wireshark.git;a=blob;f=epan/dissectors/packet-zep.c;h=3422eb7876f973f191da98cc4c02aecbd0dbaaeb;hb=HEAD">
 *                  ZEP packet definition in the Wireshark code base.
 *              </a>
 * @{
 *
 * @file
 * @brief       ZEP definitions
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef GNRC_ZEP_H_
#define GNRC_ZEP_H_

#include <inttypes.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define GNRC_ZEP_MAX_PKT_LENGTH (116)

/**
 * @brief   Default stack size to use for the ZEP thread
 */
#ifndef GNRC_ZEP_STACK_SIZE
#define GNRC_ZEP_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the ZEP thread
 */
#ifndef GNRC_ZEP_PRIO
#define GNRC_ZEP_PRIO           (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Default message queue size to use for the ZEP thread.
 */
#ifndef GNRC_ZEP_MSG_QUEUE_SIZE
#define GNRC_ZEP_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief   Default addresses if the CPUID module is not present
 * @{
 */
#define GNRC_ZEP_DEFAULT_ADDR_SHORT (0x98b0)
#define GNRC_ZEP_DEFAULT_ADDR_LONG  (0x4a8a6377552b4249)
/**
 * @}
 */

/**
 * @brief   Channel configuration
 * @{
 */
#define GNRC_ZEP_MIN_CHANNEL        (IEEE802154_CHANNEL_MIN)
#define GNRC_ZEP_MAX_CHANNEL        (IEEE802154_CHANNEL_MAX)
#define GNRC_ZEP_DEFAULT_CHANNEL    (IEEE802154_DEFAULT_CHANNEL)
/**
 * @}
 */

/**
 * @brief   Default PAN ID
 *
 * @todo Read some global network stack specific configuration value
 */
#define GNRC_ZEP_DEFAULT_PANID  (IEEE802154_DEFAULT_PANID)

/**
 * @brief   Option flags for the ZEP device
 * @{
 */
#define GNRC_ZEP_FLAGS_AUTOACK          (0x0001)    /**< auto ACKS active */
#define GNRC_ZEP_FLAGS_SRC_ADDR_LONG    (0x0002)    /**< send data using long source address */
#define GNRC_ZEP_FLAGS_DST_ADDR_LONG    (0x0004)    /**< send data using long destination address */
#define GNRC_ZEP_FLAGS_USE_SRC_PAN      (0x0008)    /**< do not compress source PAN ID */
/**
 * @}
 */

/**
 * @brief   Default UDP port for ZEP
 */
#define GNRC_ZEP_DEFAULT_PORT   (17754)

/**
 * @brief   Type == Data for ZEPv2 header
 */
#define GNRC_ZEP_V2_TYPE_DATA   (1)

/**
 * @brief   Type == Ack for ZEPv2 header
 */
#define GNRC_ZEP_V2_TYPE_ACK    (2)

/**
 * @brief   Mask for length field
 */
#define ZEP_LENGTH_MASK         (0x7f)

/**
 * @brief   ZEP header definition
 */
typedef struct __attribute__((packed)) {
    char preamble[2];       /**< Preamble code (must be "EX") */
    uint8_t version;        /**< Protocol Version (must be 1 or 2) */
} gnrc_zep_hdr_t;

/**
 * @brief   ZEPv1 header definition
 * @extends gnrc_zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    char preamble[2];       /**< preamble code (must be "EX") */
    uint8_t version;        /**< protocol Version (must be 1) */
    uint8_t chan;           /**< channel ID */
    network_uint16_t dev;   /**< device ID */
    uint8_t lqi_mode;       /**< CRC/LQI Mode */
    uint8_t lqi_val;        /**< LQI value */
    uint8_t resv[7];        /**< reserved field, must always be 0 */
    uint8_t length;         /**< length of the frame */
} gnrc_zep_v1_hdr_t;

/**
 * @brief   ZEPv2 header definition (type == Data)
 * @extends gnrc_zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    char preamble[2];       /**< preamble code (must be "EX") */
    uint8_t version;        /**< protocol Version (must be 2) */
    uint8_t type;           /**< type (must be 1/Data) */
    uint8_t chan;           /**< channel ID */
    network_uint16_t dev;   /**< device ID */
    uint8_t lqi_mode;       /**< CRC/LQI Mode */
    uint8_t lqi_val;        /**< LQI value */
    network_uint64_t time;  /**< NTP timestamp */
    network_uint32_t seq;   /**< Sequence number */
    uint8_t resv[10];       /**< reserved field, must always be 0 */
    uint8_t length;         /**< length of the frame */
} gnrc_zep_v2_data_hdr_t;

/**
 * @brief   ZEPv2 header definition (type == Ack)
 * @extends gnrc_zep_hdr_t
 */
typedef struct __attribute__((packed)) {
    char preamble[2];       /**< preamble code (must be "EX") */
    uint8_t version;        /**< protocol Version (must be 2) */
    uint8_t type;           /**< type (must be 2/Ack) */
    network_uint32_t seq;   /**< Sequence number */
} gnrc_zep_v2_ack_hdr_t;

/**
 * @brief   ZEP device descriptor.
 *
 * @extends gnrc_netdev_t
 */
typedef struct {
    gnrc_netdev_driver_t *driver;   /**< pointer to the device's interface */
    gnrc_netdev_event_cb_t event_cb;/**< netdev event callback */
    kernel_pid_t mac_pid;           /**< the driver's thread's PID */
    /**
     * @brief @ref gnrc_zep_t specific members
     * @{
     */
    le_uint16_t addr;               /**< the device's short address */
    le_uint64_t eui64;              /**< the device's EUI-64 */
    le_uint16_t pan;                /**< the device's PAN ID */
    uint16_t flags;                 /**< the device's option flags */
    uint32_t seq;                   /**< the current sequence number for frames */
    ipv6_addr_t dst;                /**< destination IPv6 address */
    uint16_t src_port;              /**< source UDP port */
    uint16_t dst_port;              /**< destination UDP port */
    gnrc_nettype_t proto;           /**< the target protocol for received packets */
    uint8_t chan;                   /**< the device's channel */
    uint8_t version;                /**< ZEP version to use (default 2) */
    uint8_t lqi_mode;               /**< LQI mode for send packets (default 1) */
    /**
     * @}
     */
} gnrc_zep_t;

/**
 * @brief   Initializion of the ZEP thread and device.
 *
 * @param[in] dev       Network device, will be initialized.
 * @param[in] src_port  Source port to use in UDP datagrams. Also the port
 *                      @ref net_gnrc_zep registers to in @ref net_gnrc_netreg.
 * @param[in] dst       Destination address to use in IPv6 packets.
 * @param[in] dst_port  Destination port to use in UDP datagrams.
 *
 * @return  PID of the ZEP thread on success.
 * @return  -EADDRINUSE, if @p src_port is already ready registered to
 *          @ref net_gnrc_netreg.
 * @return  -EEXIST, if ZEP thread was already created.
 * @return  -EINVAL, if @ref GNRC_ZEP_PRIO is greater than or equal to
 *          @ref SCHED_PRIO_LEVELS
 * @return  -ENODEV, if @p dev is NULL.
 * @return  -ENOTSUP, if @p dst is NULL or unspecified address (::).
 * @return  -EOVERFLOW, if there are too many threads running already
 */
kernel_pid_t gnrc_zep_init(gnrc_zep_t *dev, uint16_t src_port, ipv6_addr_t *dst,
                           uint16_t dst_port);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_ZEP_H_ */
/**
 * @}
 */
