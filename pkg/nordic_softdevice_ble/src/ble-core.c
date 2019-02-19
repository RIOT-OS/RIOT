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
 * @addtogroup cpu
 * @{
 *
 * @addtogroup nrf52832
 * @{
 *
 * @addtogroup nrf52832-ble Bluetooth Low Energy drivers
 * @{
 *
 * @file
 *         Basic BLE functions.
 * @author
 *         Wojciech Bober <wojciech.bober@nordicsemi.no>
 *         Kaspar Schleiser <kaspar@schleiser.de>
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//#include "boards.h"
//#include "nordic_common.h"
//#include "nrf_delay.h"
#include "nrf_sdm.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"
#include "ble_ipsp.h"
#include "softdevice_handler.h"
#include "app_error.h"
#include "iot_defines.h"
#include "ble-core.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define IS_SRVC_CHANGED_CHARACT_PRESENT 1
#define APP_ADV_TIMEOUT                 0                                  /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define APP_ADV_ADV_INTERVAL            MSEC_TO_UNITS(333, UNIT_0_625_MS)  /**< The advertising interval. This value can vary between 100ms to 10.24s). */

static ble_gap_adv_params_t m_adv_params; /**< Parameters to be passed to the stack when starting advertising. */

static void
ble_evt_dispatch(ble_evt_t * p_ble_evt);
/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize and enable the BLE stack.
 */
void
ble_stack_init(void)
{
  uint32_t err_code;

  // Enable BLE stack.
  ble_enable_params_t ble_enable_params;
  memset(&ble_enable_params, 0, sizeof(ble_enable_params));
  ble_enable_params.gatts_enable_params.attr_tab_size =
  BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
  ble_enable_params.gatts_enable_params.service_changed =
  IS_SRVC_CHANGED_CHARACT_PRESENT;
  err_code = sd_ble_enable(&ble_enable_params);
  APP_ERROR_CHECK(err_code);

  // Register with the SoftDevice handler module for BLE events.
  err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
  APP_ERROR_CHECK(err_code);

  // Setup address
  ble_gap_addr_t ble_addr;
  err_code = sd_ble_gap_address_get(&ble_addr);
  APP_ERROR_CHECK(err_code);

  ble_addr.addr[5] = 0x00;
  ble_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

  err_code = sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &ble_addr);
  APP_ERROR_CHECK(err_code);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Return device EUI64 MAC address
 * @param addr pointer to a buffer to store the address
 */
#include "ble-mac.h"
void
ble_get_mac(uint8_t addr[6])
{
  uint32_t err_code;
  ble_gap_addr_t ble_addr;

  err_code = sd_ble_gap_address_get(&ble_addr);
  APP_ERROR_CHECK(err_code);

  ble_eui48(addr, ble_addr.addr,
            ble_addr.addr_type == BLE_GAP_ADDR_TYPE_PUBLIC);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize BLE advertising data.
 * @param name Human readable device name that will be advertised
 */
void
ble_advertising_init(const char *name)
{
  uint32_t err_code;
  ble_advdata_t advdata;
  uint8_t flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)name,
                                        strlen(name));
  APP_ERROR_CHECK(err_code);

  ble_uuid_t adv_uuids[] = {{BLE_UUID_IPSP_SERVICE, BLE_UUID_TYPE_BLE}};

  // Build and set advertising data.
  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type = BLE_ADVDATA_FULL_NAME;
  advdata.flags = flags;
  advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
  advdata.uuids_complete.p_uuids = adv_uuids;

  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);

  // Initialize advertising parameters (used when starting advertising).
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type = BLE_GAP_ADV_TYPE_ADV_IND;
  m_adv_params.p_peer_addr = NULL; // Undirected advertisement.
  m_adv_params.fp = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval = APP_ADV_ADV_INTERVAL;
  m_adv_params.timeout = APP_ADV_TIMEOUT;
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Start BLE advertising.
 */
void
ble_advertising_start(void)
{
  uint32_t err_code;

  err_code = sd_ble_gap_adv_start(&m_adv_params);
  APP_ERROR_CHECK(err_code);

  DEBUG("ble-core: advertising started\n");
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Print GAP address.
 * @param addr a pointer to address
 */
void
ble_gap_addr_print(const ble_gap_addr_t *addr)
{
    unsigned int i;
    for(i = 0; i < sizeof(addr->addr); i++) {
        if (i > 0) {
            DEBUG(":");
        }
        DEBUG("%02x", addr->addr[i]);
    }
    DEBUG(" (%d)\n", addr->addr_type);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Function for handling the Application's BLE Stack events.
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void
on_ble_evt(ble_evt_t *p_ble_evt)
{
  switch(p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      DEBUG("ble-core: connected [handle:%d, peer: ", p_ble_evt->evt.gap_evt.conn_handle);
      ble_gap_addr_print(&(p_ble_evt->evt.gap_evt.params.connected.peer_addr));
      DEBUG("]\n");
      sd_ble_gap_rssi_start(p_ble_evt->evt.gap_evt.conn_handle,
                            BLE_GAP_RSSI_THRESHOLD_INVALID,
                            0);
      break;

    case BLE_GAP_EVT_DISCONNECTED:
      DEBUG("ble-core: disconnected [handle:%d]\n", p_ble_evt->evt.gap_evt.conn_handle);
      ble_advertising_start();
      break;
    default:
      break;
  }
}
/*---------------------------------------------------------------------------*/
/**
 * @brief SoftDevice BLE event callback.
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void
ble_evt_dispatch(ble_evt_t *p_ble_evt)
{
  ble_ipsp_evt_handler(p_ble_evt);
  on_ble_evt(p_ble_evt);
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
