/*
 * Copyright (c) 2016, Nordic Semiconductor
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @ingroup         pkg_nordic-softdevice-ble
 * @brief           Public API and definitions of Nordic Softdevice BLE
 * @{
 *
 * @file
 */


#ifndef BLE_MAC_H
#define BLE_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLE_EVENT_RX_DONE = 20000,
} ble_mac_event_enum_t;

#define BLE_IFACE_ADDED             (10000)

#define BLE_SIXLOWPAN_MTU           (1280U)
#define BLE_L2_ADDR_LEN             (6U)

#ifndef IPV6_IID_FLIP_VALUE
#define IPV6_IID_FLIP_VALUE         (0x02)
#endif

/**
 * @brief   Get BLE EUI64 from EUI48
 *
 * @param[out] eui48                   The output EUI48 (big-endian,
 *                                     6 bytes long)
 * @param[in] ble_addr                 The input BLE address (little-endian,
 *                                     6 bytes long)
 * @param[in] _public                  True if public interface, false otherwise
 */
static inline void ble_eui48(uint8_t *eui48, const uint8_t *ble_addr, int _public)
{
    eui48[0] = ble_addr[5];
    eui48[1] = ble_addr[4];
    eui48[2] = ble_addr[3];
    eui48[3] = ble_addr[2];
    eui48[4] = ble_addr[1];
    eui48[5] = ble_addr[0];
    if (_public) {
        eui48[0] &= ~(IPV6_IID_FLIP_VALUE);
    }
    else {
        eui48[0] |= IPV6_IID_FLIP_VALUE;
    }
}

/**
 * @brief   Structure handling a received BLE mac packet
 */
typedef struct {
    uint8_t payload[BLE_SIXLOWPAN_MTU];  /**< Payload of the packet */
    uint8_t src[BLE_L2_ADDR_LEN];        /**< Source address of the packet */
    uint16_t len;                        /**< Length of the packet */
    int8_t rssi;                         /**< RSSI of the received packet */
} ble_mac_inbuf_t;

/**
 * @brief   Mac event callback function definition
 */
typedef void (*ble_mac_callback_t)(ble_mac_event_enum_t event, void*arg);

/**
 * @brief   Initialize the BLE mac
 *
 * @param[in] callback             The callback function triggered on mac event
 */
void ble_mac_init(ble_mac_callback_t callback);

/**
 * @brief   Send a BLE message
 *
 * @param[in] dest                 The destination address
 * @param[in] data                 The data to send
 * @param[in] len                  The length of the data to send
 *
 * @return 0 if send is successful
 * @return <0 if send failed
 */
int ble_mac_send(uint8_t dest[BLE_L2_ADDR_LEN], void *data,
                 size_t len);

extern volatile int ble_mac_busy_tx;    /**< Flag is set to 1 when the driver
                                             is busy transmitting a packet. */
extern volatile int ble_mac_busy_rx;    /**< Flag is set to 1 when there is a
                                             received packet pending. */

#ifdef __cplusplus
}
#endif

#endif /* BLE_MAC_H */
/** @} */
