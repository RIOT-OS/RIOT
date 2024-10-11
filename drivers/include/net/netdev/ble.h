/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_netdev_ble netdev BLE mode
 * @ingroup     drivers_netdev_api
 * @brief       BLE adaption of netdev
 * @{
 *
 * @warning     This API is experimental and in an early state - expect
 *              significant changes!
 *
 * # About
 *
 * BLE defines a very specific environment for the used radio, both in terms of
 * communication sequences and in terms of timings. BLE communication is
 * structured in so called events, where each event is a sequence of request and
 * reply packets send between two peers. A radio context (frequency, CRC
 * initializer, and access address) is used throughout such an event and
 * typically changed for the next one. In addition, the timing of the packets
 * sent in a sequence is fixed to an inter-frame-spacing of exactly 150us.
 *
 * To cater with these specific attributes of BLE, this interface tailors the
 * generic netdev interface to be used for BLE radios.
 *
 *
 * # Interface Adaption / Netdev Interpretation
 *
 * ## Transmission Sequence Based Approach
 *
 * To be able to handle the exact inter-packet-spacing if 150us seconds, this
 * interface expects the device driver to stay in a continuous alternating
 * RX-TX sequence, until it is manually aborted. While in this sequence, the
 * radio driver needs to take care of switching to RX mode 150us after sending
 * the last packet, and to send the next packet 150us after the last packet was
 * received.
 *
 * Such a transmission sequence is started by calling either the radio's send
 * or receive function while the radio is in idle/standby mode.
 *
 * Once a transmission sequence is in progress, the next packet to be send, or
 * the next reception buffer to be used is specified also using the send/recv
 * functions. They should be called in the `event_callback` right after the
 * last transmission (RX or TX) was finished.
 *
 * The transmission sequence is aborted by calling `netdev_ble_stop(dev)`
 * (`netdev->set(dev, NETOPT_BLE_CTX, NULL, 0)`). This will put the radio back
 * into idle/standby mode.
 *
 * ## Radio Context
 *
 * As BLE uses time sliced channel hopping, the used channel needs to be set
 * regularly. Additionally, also the used access address and the CRC initializer
 * need to be set regularly, as they differ for different BLE connections. To
 * make setting these values more efficient, this interface combines these three
 * values in to a so called `radio context` and adds a `netopt` option to set
 * all three values at once using `netdev_ble_set_ctx(dev, ctx)`
 * (`netdev->set(dev, NETOPT_BLE_CTX, ctx, sizeof(netdev_ble_ctx_t))`).
 *
 *
 * # Implementation Status and Limitations
 * - This interface works for memory mapped radios only (no support for
 *   bus-connected devices). This is mainly for timing reasons.
 * - No support for LE Data Length Extension (bigger packet size), yet
 *
 * @file
 * @brief       BLE specific adaption for the Netdev API
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_NETDEV_BLE_H
#define NET_NETDEV_BLE_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum payload length of a standard BLE packet
 */
#define NETDEV_BLE_PDU_MAXLEN       (37U)

/**
 * @brief   Mask for the actual (3 byte) CRC data in the context's CRC field
 */
#define NETDEV_BLE_CRC_MASK         (0x00ffffff)

/**
 * @brief   Flag for marking a correct CRC on packet reception
 */
#define NETDEV_BLE_CRC_OK           (0x80000000)

/**
 * @brief   BLE packet structure (as defined by the BLE standard)
 */
typedef struct __attribute__((packed)) {
    uint8_t flags;                          /**< header flags */
    uint8_t len;                            /**< actual length of PDU */
    uint8_t pdu[NETDEV_BLE_PDU_MAXLEN];     /**< protocol data unit (PDU) */
} netdev_ble_pkt_t;

/**
 * @brief   Radio context
 */
typedef struct {
    union {
        uint8_t raw[4];                     /**< byte-wise access */
        uint32_t u32;                       /**< compact access */
    } aa;                                   /**< access address */
    uint32_t crc;                           /**< CRC: 3 LSB for CRC, most
                                             *   significant bit for RX state*/
    uint8_t chan;                           /**< channel to use/used */
} netdev_ble_ctx_t;

/**
 * @brief   Send the given packet on the next occasion
 *
 * If a transmission sequence is in progress, the given packet will be send
 * after 150us after receptions of the last packet. If no sequence is currently
 * active, the packet will be send immediately and a new transmission sequence
 * is started.
 *
 * @note    Call this function only to start a new transmission sequence (radio
 *          is currently idle), or right after a packet was received. If called
 *          at any other point in time, the behavior is undefined.
 *
 * @param[in] dev   radio to use for sending
 * @param[in] pkt   data to send
 *
 * @return  0 on success
 * @return `< 0` on error
 */
static inline int netdev_ble_send(netdev_t *dev, netdev_ble_pkt_t *pkt)
{
    struct iolist data = { NULL, pkt, sizeof(netdev_ble_pkt_t) };
    return dev->driver->send(dev, &data);
}

/**
 * @brief   Start listening for an incoming packet and write it into @p pkt
 *
 * If a transmission sequence is in progress, the radio will use the given
 * buffer for reception when it goes in to RX mode 150us after sending the last
 * packet. If no sequence is in progress, the radio will go into RX mode
 * immediately (using the given RX buffer), and a new transmission sequence is
 * started.
 *
 * @note    Call this function only to start a new transmission sequence (radio
 *          is currently idle), or right after a packet was sent. If called
 *          at any other point in time, the behavior is undefined.
 *
 * @param[in]  dev  radio to use for receiving
 * @param[out] pkt  buffer to write new packet to
 *
 * @return  0 on success
 * @return `< 0` on error
 */
static inline int netdev_ble_recv(netdev_t *dev, netdev_ble_pkt_t *pkt)
{
    return dev->driver->recv(dev, pkt, sizeof(netdev_ble_pkt_t), NULL);
}

/**
 * @brief   Set the radio context for the given radio device
 *
 * @param[in] dev   target radio device
 * @param[in] ctx   new radio context (CRC, channel, access address)
 */
static inline void netdev_ble_set_ctx(netdev_t *dev, netdev_ble_ctx_t *ctx)
{
    dev->driver->set(dev, NETOPT_BLE_CTX, ctx, sizeof(netdev_ble_ctx_t));
}

/**
 * @brief   Stop the ongoing RX/TX sequence
 *
 * @note    This function has not effect if the radio is in the middle of a
 *          data transfer
 *
 * @param[in] dev   target radio device
 */
static inline void netdev_ble_stop(netdev_t *dev)
{
    dev->driver->set(dev, NETOPT_BLE_CTX, NULL, 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETDEV_BLE_H */
