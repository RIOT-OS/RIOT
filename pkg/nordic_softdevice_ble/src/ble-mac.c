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

#include <stdint.h>
#include <string.h>

#define DONT_OVERRIDE_NVIC

#include "ble-core.h"
#include "ble_ipsp.h"
#include "ble_gap.h"

#include "ble-mac.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if defined(MODULE_OD) && ENABLE_DEBUG
#include "od.h"
#endif

#ifndef BLE_MAC_MAX_INTERFACE_NUM
#define BLE_MAC_MAX_INTERFACE_NUM (1U)  /**< Maximum number of interfaces, i.e.,
                                             connection to master devices */
#endif

typedef struct {
  uint8_t peer_addr[BLE_L2_ADDR_LEN];
  ble_ipsp_handle_t handle;
} ble_mac_interface_t;

static ble_mac_interface_t interfaces[BLE_MAC_MAX_INTERFACE_NUM];

volatile int ble_mac_busy_tx;
volatile int ble_mac_busy_rx;

static ble_mac_inbuf_t inbuf;
static ble_mac_callback_t _callback;

/**
 * @brief Lookup interface by IPSP connection.
 *
 * @param[in] handle    a pointer to IPSP handle.
 * @return a pointer to interface structure
 * @return NULL if no interface has been found for a given handle
 */
static ble_mac_interface_t *ble_mac_interface_lookup(ble_ipsp_handle_t *handle)
{
    if (handle == NULL) {
        return NULL;
    }
    for (int i = 0; i < BLE_MAC_MAX_INTERFACE_NUM; i++) {
        if (interfaces[i].handle.conn_handle == handle->conn_handle &&
            interfaces[i].handle.cid == handle->cid) {
            return &interfaces[i];
        }
    }
    return NULL;
}

/**
 * @brief Add IPSP connection to the interface table.
 *
 * This function binds IPSP connection with peer address.
 *
 * @param[in] peer      a pointer to eui64 address
 * @param[in] handle    a pointer to IPSP handle
 *
 * @return  a pointer to an interface structure on success
 * @return  NULL if interface table is full
 */
static ble_mac_interface_t *ble_mac_interface_add(uint8_t peer[BLE_L2_ADDR_LEN],
                                                  ble_ipsp_handle_t *handle)
{
    DEBUG("ble_mac_interface_add()\n");
    for (int i = 0; i < BLE_MAC_MAX_INTERFACE_NUM; i++) {
        if (interfaces[i].handle.conn_handle == 0 && interfaces[i].handle.cid == 0) {
            memcpy(&interfaces[i].handle, handle, sizeof(ble_ipsp_handle_t));
            memcpy(&interfaces[i].peer_addr, peer, BLE_L2_ADDR_LEN);

            /* notify handler thread */
            /* msg_t m = { .type = BLE_IFACE_ADDED, .content.ptr = &interfaces[i] }; */
            /* msg_send(&m, gnrc_nordic_ble_6lowpan_pid); */

            return &interfaces[i];
        }
    }
    return NULL;
}

/**
 * @brief       Remove interface from the interface table.
 * @param[in]   interface a pointer to interface
 */
static void ble_mac_interface_delete(ble_mac_interface_t *interface)
{
    DEBUG("ble_mac_interface_delete()\n");
    memset(interface, 0, sizeof(ble_mac_interface_t));
}

/**
 * @brief Lookup IPSP handle by peer address.
 *
 * @param[in] addr a pointer to eui64 address.
 * @retval a pointer to IPSP handle on success
 * @retval NULL if an IPSP handle for given address haven't been found
 */
static ble_ipsp_handle_t *_find_handle(const uint8_t *addr)
{
    for (int i = 0; i < BLE_MAC_MAX_INTERFACE_NUM; i++) {
        if (memcmp(interfaces[i].peer_addr, addr, BLE_L2_ADDR_LEN) == 0) {
            return &interfaces[i].handle;
        }
    }
    return NULL;
}

/**
 * @brief Send packet on a given IPSP handle.
 *
 * @param[in] handle a pointer to IPSP handle.
 * @return 1 on success, 0 otherwise
 */
static int _send_to_peer(ble_ipsp_handle_t *handle, void *data, size_t len)
{
  DEBUG("ble-mac: sending packet[GAP handle:%d CID:0x%04X]\n",
          handle->conn_handle, handle->cid);
  return ble_ipsp_send(handle, data, len);
}

static int _is_broadcast(uint8_t dest[BLE_L2_ADDR_LEN])
{
    uint32_t *_dest = (uint32_t*)dest;
    for (int i = 0; i < 2; i++) {
        if (_dest[i]) {
            return 0;
        }
    }
    return 1;
}

int ble_mac_send(uint8_t dest[BLE_L2_ADDR_LEN], void *data, size_t len)
{
    DEBUG("ble_mac_send(): sending pkt with len %u\n", (unsigned)len);

#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(dest, BLE_L2_ADDR_LEN, OD_WIDTH_DEFAULT);
    od_hex_dump(data, len, OD_WIDTH_DEFAULT);
#endif

    ble_ipsp_handle_t *handle;
    int ret = -1;

    if ((!dest) || _is_broadcast(dest)) {
        DEBUG("broadcast\n");
        for (int i = 0; i < BLE_MAC_MAX_INTERFACE_NUM; i++) {
            if (interfaces[i].handle.cid != 0 && interfaces[i].handle.conn_handle != 0) {
                ret = _send_to_peer(&interfaces[i].handle, data, len);
                DEBUG("ret=%i\n", ret);
            }
        }
    }
    else if ((handle = _find_handle(dest)) != NULL) {
        DEBUG("unicast\n");
        ret = _send_to_peer(handle, data, len);
    }
    else {
        DEBUG("ble-mac: no connection found for peer\n");
    }

    if (ret == NRF_SUCCESS) {
        ble_mac_busy_tx = 1;
        return 0;
    }
    else {
        DEBUG("ble-mac: send error: %i\n", ret);
        return -1;
    }
}

static uint32_t ble_mac_ipsp_evt_handler_irq(ble_ipsp_handle_t *p_handle, ble_ipsp_evt_t *p_evt)
{
    uint32_t retval = NRF_SUCCESS;

    ble_mac_interface_t *p_instance = ble_mac_interface_lookup(p_handle);

    if (p_handle) {
        DEBUG("ble-mac: IPSP event [handle:%d CID 0x%04X]\n", p_handle->conn_handle, p_handle->cid);
    }

    switch (p_evt->evt_id) {
        case BLE_IPSP_EVT_CHANNEL_CONNECTED: {
            uint8_t peer_addr[BLE_L2_ADDR_LEN];

            DEBUG("ble-mac: channel connected\n");
            ble_eui48(peer_addr,
                      p_evt->evt_param->params.ch_conn_request.peer_addr.addr,
                      p_evt->evt_param->params.ch_conn_request.peer_addr.addr_type ==
                              BLE_GAP_ADDR_TYPE_PUBLIC);
            p_instance = ble_mac_interface_add(peer_addr, p_handle);

            if (p_instance != NULL) {
                DEBUG("ble-mac: added new IPSP interface\n");
            }
            else {
                DEBUG("ble-mac: cannot add new interface. Table is full\n");
                ble_ipsp_disconnect(p_handle);
            }
            break;
        }

        case BLE_IPSP_EVT_CHANNEL_DISCONNECTED: {
            DEBUG("ble-mac: channel disconnected\n");
            if (p_instance != NULL) {
                DEBUG("ble-mac: removed IPSP interface\n");
                ble_mac_interface_delete(p_instance);
            }
            break;
        }

        case BLE_IPSP_EVT_CHANNEL_DATA_RX: {
            DEBUG("ble-mac: data received\n");
            if (p_instance != NULL) {
                if (ble_mac_busy_rx) {
                    DEBUG("ble-mac: packet dropped as input buffer is busy\n");
                    break;
                }

                if (p_evt->evt_param->params.ch_rx.len > BLE_SIXLOWPAN_MTU) {
                    DEBUG("ble-mac: packet buffer is too small!\n");
                    break;
                }

                ble_mac_busy_rx = 1;

                inbuf.len = p_evt->evt_param->params.ch_rx.len;
                memcpy(inbuf.payload, p_evt->evt_param->params.ch_rx.p_data, inbuf.len);
                memcpy(inbuf.src, p_instance->peer_addr, BLE_L2_ADDR_LEN);
                sd_ble_gap_rssi_get(p_handle->conn_handle, &inbuf.rssi);

                _callback(BLE_EVENT_RX_DONE, &inbuf);
            }
            else {
                DEBUG("ble-mac: got data to unknown interface!\n");
            }
            break;
        }

        case BLE_IPSP_EVT_CHANNEL_DATA_TX_COMPLETE: {
            DEBUG("ble-mac: data transmitted\n");
            ble_mac_busy_tx = 0;
            //_callback(BLE_EVENT_TX_DONE, NULL);
            break;
        }
    }

    if (sched_context_switch_request) {
        NVIC_SetPendingIRQ( SWI0_EGU0_IRQn );
    }

    return retval;
}

void ble_mac_init(ble_mac_callback_t callback)
{
    assert(callback);

    uint32_t res;
    ble_ipsp_init_t ipsp_init_params = {
        .evt_handler = ble_mac_ipsp_evt_handler_irq
    };

    _callback = callback;

    res = ble_ipsp_init(&ipsp_init_params);
    DEBUG("ble_ipsp_init() res = %" PRIu32 "\n", res);
    (void)res;
}
