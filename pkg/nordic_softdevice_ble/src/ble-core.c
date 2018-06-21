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

#include "nrf_sdm.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_advdata.h"
#include "ble_srv_common.h"
#include "ble_ipsp.h"
#include "app_error.h"
#include "iot_defines.h"
#include "ble-core.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * Time for which the device must be advertising in non-connectable
 * mode (in seconds). 0 disables timeout.
 */
#define APP_ADV_DURATION                0

/**
 * The advertising interval. This value can vary between 100ms to 10.24s).
 */
#define APP_ADV_INTERVAL                MSEC_TO_UNITS(333, UNIT_0_625_MS)

/**
 * Identifies the L2CAP configuration used with SoftDevice.
 */
#define BLE_IPSP_TAG                        35

/**
 * Minimum acceptable connection interval (0.5 seconds).
 */
#define GAP_MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)

/**
 * Maximum acceptable connection interval (1 second).
 */
#define GAP_MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)

/**
 * Slave latency.
 */
#define GAP_SLAVE_LATENCY                   0

/**
 * Connection supervisory time-out (4 seconds).
 */
#define GAP_CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)

/**
 * Advertising handle used to identify an advertising set.
 */
static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;

static void
ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);

/*---------------------------------------------------------------------------*/

// Register with the SoftDevice handler module for BLE events.
NRF_SDH_BLE_OBSERVER(m_ble_observer, BLE_IPV6_MEDIUM_BLE_OBSERVER_PRIO, ble_evt_handler, NULL/*p_context*/);

/**
 * @brief Initialize and enable the BLE stack.
 */
void
ble_stack_init(void)
{
  uint32_t err_code;
  uint32_t app_ram_start = 0;

  // Add configuration the BLE stack using the default settings
  // Fetch the start address of the application RAM.
  err_code = nrf_sdh_ble_default_cfg_set(BLE_IPSP_TAG, &app_ram_start);
  APP_ERROR_CHECK(err_code);

  // Set L2CAP channel configuration for 6LowPan
  const ble_cfg_t ble_cfg = {
    .conn_cfg = {
      .conn_cfg_tag = BLE_IPSP_TAG,
      .params = {
        .l2cap_conn_cfg = {
          .rx_mps        = BLE_IPSP_RX_MPS,
          .rx_queue_size = BLE_IPSP_RX_BUFFER_COUNT,
          .tx_mps        = BLE_IPSP_TX_MPS,
          .tx_queue_size = 1,
          .ch_count      = BLE_IPSP_MAX_CHANNELS,
        },
      },
    },
  };
  err_code = sd_ble_cfg_set(BLE_CONN_CFG_L2CAP, &ble_cfg, app_ram_start);
  APP_ERROR_CHECK(err_code);

  DEBUG("ble_stack_init: app_ram_start[%d]=%lx\n", BLE_IPSP_TAG, app_ram_start);

  // Enable BLE stack.
  err_code = nrf_sdh_ble_enable(&app_ram_start); // Issues warnings/debug about RAM start
  if (NRF_ERROR_NO_MEM == err_code) {
    core_panic(PANIC_GENERAL_ERROR, "ble-core: BLE initialisation failed: SoftDevice RAM too small");
  }
  APP_ERROR_CHECK(err_code);

  // Setup address
  ble_gap_addr_t ble_addr;
  err_code = sd_ble_gap_addr_get(&ble_addr);
  APP_ERROR_CHECK(err_code);

  ble_addr.addr[5] = 0x00;
  ble_addr.addr_type = BLE_GAP_ADDR_TYPE_PUBLIC;

  err_code = sd_ble_gap_addr_set(&ble_addr);
  APP_ERROR_CHECK(err_code);
}

/*---------------------------------------------------------------------------*/

/**
 * @brief Return device EUI64 MAC address
 * @param addr pointer to a buffer to store the address
 */
void
ble_get_mac(uint8_t addr[8])
{
  uint32_t err_code;
  ble_gap_addr_t ble_addr;

  err_code = sd_ble_gap_addr_get(&ble_addr);
  APP_ERROR_CHECK(err_code);

  IPV6_EUI64_CREATE_FROM_EUI48(addr, ble_addr.addr, ble_addr.addr_type);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Initialize BLE advertising data.
 * @param name Human readable device name that will be advertised
 */
void
ble_advertising_init(const char *name)
{
  /**
   * Buffers for storing an encoded advertising set.
   * The ble_gap_adv_data_t buffer must be statically allocated for the SoftDevice to use.
   * See nRF5_SDK_15.0.0_a53641a/components/softdevice/s132/headers/ble_gap.h#L836
   */
  static uint8_t enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
  static ble_gap_adv_data_t adv_data = {
    .adv_data =      { .p_data = enc_advdata, .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX },
    .scan_rsp_data = { .p_data = NULL,        .len    = 0                             },
  };

  uint32_t err_code;
  ble_advdata_t advdata;
  ble_gap_adv_params_t adv_params;
  const uint8_t flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)name, strlen(name));
  APP_ERROR_CHECK(err_code);

  ble_gap_conn_params_t gap_conn_params;
  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = GAP_MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = GAP_MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = GAP_SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = GAP_CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);

  ble_uuid_t adv_uuids[] = {{BLE_UUID_IPSP_SERVICE, BLE_UUID_TYPE_BLE}};

  // Build and set advertising data.
  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type = BLE_ADVDATA_FULL_NAME;
  advdata.flags = flags;
  advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
  advdata.uuids_complete.p_uuids = adv_uuids;

  err_code = ble_advdata_encode(&advdata, adv_data.adv_data.p_data, &adv_data.adv_data.len);
  APP_ERROR_CHECK(err_code);

  // Initialize advertising parameters (used when starting advertising).
  memset(&adv_params, 0, sizeof(adv_params));

  adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;
  adv_params.duration        = APP_ADV_DURATION;
  adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
  adv_params.p_peer_addr     = NULL; // Undirected advertisement.
  adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
  adv_params.interval        = APP_ADV_INTERVAL;

  err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &adv_data, &adv_params);
  APP_ERROR_CHECK(err_code);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Start BLE advertising.
 */
void
ble_advertising_start(void)
{
  uint32_t err_code;

  err_code = sd_ble_gap_adv_start(m_adv_handle, BLE_IPSP_TAG);
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
  DEBUG(" (%d)", addr->addr_type);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief Function for handling the Application's BLE Stack events.
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void
on_ble_evt(ble_evt_t const *p_ble_evt)
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

  case BLE_L2CAP_EVT_CH_SETUP_REFUSED:
    // Check for insufficient local resources
    if (   BLE_L2CAP_CH_SETUP_REFUSED_SRC_LOCAL
             == p_ble_evt->evt.l2cap_evt.params.ch_setup_refused.source
        && BLE_L2CAP_CH_STATUS_CODE_NO_RESOURCES
             == p_ble_evt->evt.l2cap_evt.params.ch_setup_refused.status) {
      DEBUG("ble-core: too few L2CAP SD channels: increase BLE_IPSP_MAX_CHANNELS\n");
    }
    break;

  default:
    break;
  }

  ble_ipsp_evt_handler(p_ble_evt);
}
/*---------------------------------------------------------------------------*/
/**
 * @brief SoftDevice BLE event callback.
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void
ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
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
