/*
 * SPDX-FileCopyrightText: 2015-2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_nrf5x_nrfmin NRF minimal radio driver
 * @ingroup     drivers_netdev
 * @brief       Minimal driver for the NRF51 radio
 *
 * This driver uses the nRF5x radio in a proprietary/custom way, defining our
 * own custom link layer. This custom link layer resembles some characteristics
 * of the IEEE802.15.4 link layer, but is not at all compatible to it.
 *
 * One key point is, that this custom link layer is only meant to operate
 * between nRF5x devices, which let's us make some very nice assumptions:
 *  - all communicating hosts are little-endian
 *    -> we define host byte order := network byte order
 *
 * The driver is using a Nordic proprietary physical layer, configured to a
 * bitrate of 1 Mbit/s. The maximum payload length can be freely configured, but
 * the maximal supported value is 250 byte (default is 200 byte).
 *
 * We define the nrfmin link layer to use 16-bit addresses. On the physical
 * layer we encode these addresses by putting these addresses into the 2 least
 * significant bytes of the supported 5-byte addresses, while setting the other
 * 3 bytes to 0xe7.
 *
 * For out custom link layer, we define our own proprietary link layer format
 * (all fields are in host byte order (little endian)):
 *
 *    byte0 | byte1 - byte2 | byte3 - byte4 | byte5 | byte7 - byteN
 *   ------ | ------------- | ------------- | ----- | -------------
 *   length |   src_addr    |   dst_addr    | proto |   payload...
 *
 * With:
 * - length: length of the packet, including the header -> payload len + 6
 * - src_addr: 16-bit source address
 * - dst_addr: 16-bit destination address
 * - proto: type of data transferred (similar to an Ethertype field)
 *
 * SUMMARY:
 * This driver / link layer supports:
 *   - 16-bit addressing (16-bit)
 *     -> extract default address from CPU ID
 *   - broadcast (broadcast address is ff:ff)
 *   - channels from 0 to 31 [2400MHz to 2524MHz, 4MHz per channel]
 *   - setting of TX power [+4dBm to -20dBm, in ~4dBm steps]
 *   - 8-bit packet type/proto field (to be used as seen fit)
 *   - setting device state (RX, SLEEP)
 *
 * But so far no support for:
 *   - link layer ACKs
 *   - retransmissions
 *
 * @todo        So far the driver uses only a single RX buffer that is locked
 *              until the data was read/discarded. This can potentially lead to
 *              a lot of packet loss -> using more than one buffer would help
 *              here...
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the nrfmin NRF51822 radio driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   nrfmin channel configuration
 * @{
 */
#define NRFMIN_CHAN_MIN             (0U)
#define NRFMIN_CHAN_DEFAULT         (0U)                /* 2400MHz */
#define NRFMIN_CHAN_MAX             (32)
/** @} */

/**
 * @brief   Default transmission power used
 */
#define NRFMIN_TXPOWER_DEFAULT      (0)                 /* 0dBm */

/**
 * @brief   Export the default nrfmin broadcast address
 */
#define NRFMIN_ADDR_BCAST           (0xffff)

/**
 * @brief   Default maximum payload length (must be <= 250)
 */
#ifndef NRFMIN_PAYLOAD_MAX
#define NRFMIN_PAYLOAD_MAX          (200U)
#endif

/**
 * @brief   Export some information on header and packet lengths
 * @{
 */
#define NRFMIN_HDR_LEN              (sizeof(nrfmin_hdr_t))
#define NRFMIN_PKT_MAX              (NRFMIN_HDR_LEN + NRFMIN_PAYLOAD_MAX)
/** @} */

/**
 * @brief   Header format used for our custom nrfmin link layer
 */
typedef struct __attribute__((packed)) {
    uint8_t len;            /**< packet length, including this header */
    uint16_t src_addr;      /**< source address of the packet */
    uint16_t dst_addr;      /**< destination address */
    uint8_t proto;          /**< protocol of payload */
} nrfmin_hdr_t;

/**
 * @brief   In-memory structure of a nrfmin radio packet
 */
typedef union {
    struct __attribute__((packed)) {
        nrfmin_hdr_t hdr;                       /**< the nrfmin header */
        uint8_t payload[NRFMIN_PAYLOAD_MAX];    /**< actual payload */
    } pkt;                                      /**< typed packet access */
    uint8_t raw[NRFMIN_PKT_MAX];                /**< raw packet access */
} nrfmin_pkt_t;

/**
 * @brief   Export the netdev device descriptor
 */
extern netdev_t nrfmin_dev;

/**
 * @brief   Reference to the netdev driver interface
 */
extern const netdev_driver_t nrfmin_netdev;

/**
 * @brief   Setup the device driver's data structures
 */
void nrfmin_setup(void);

/**
 * @brief   Get the currently active address

 * @return  the 16-bit node address
 */
uint16_t nrfmin_get_addr(void);

/**
 * @brief   Set the 16-bit radio address
 *
 * @param[in] addr      address to set
 */
void nrfmin_set_addr(uint16_t addr);

/**
 * @brief   Get the IID build from the 16-bit node address
 *
 * @param[out] iid      the 64-bit IID, as array of 4 * 16-bit
 */
void nrfmin_get_iid(uint16_t *iid);

/**
 * @brief   Get the current channel
 *
 * @return  currently active channel
 */
uint16_t nrfmin_get_channel(void);

/**
 * @brief   Set the active channel
 *
 * @param[in] chan      targeted channel [0-31]
 *
 * @return  sizeof(uint16_t) on success
 * @return  -EOVERFLOW if channel is not applicable
 */
int nrfmin_set_channel(uint16_t chan);

/**
 * @brief   Get the current radio state
 *
 * @return  state the radio is currently in
 */
netopt_state_t nrfmin_get_state(void);

/**
 * @brief   Put the device into the given state
 *
 * @param[in] val       target state
 *
 * @return  sizeof(netopt_state_t) on success
 * @return  -ENOTSUP if target state is not applicable
 */
int nrfmin_set_state(netopt_state_t val);

/**
 * @brief   Get the current transmit power
 *
 * @return  transmission power in [dBm]
 */
int16_t nrfmin_get_txpower(void);

/**
 * @brief   Set the used transmission power
 *
 * @param[in] power     targeted power, in [dBm]
 */
void nrfmin_set_txpower(int16_t power);

#ifdef __cplusplus
}
#endif

/** @} */
