// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_libc.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "internal/esp_wifi_internal.h"
#include "esp_socket.h"
#include "net/sockio.h"
#include "phy.h"

const size_t _g_esp_wifi_ppt_task_stk_size = CONFIG_WIFI_PPT_TASKSTACK_SIZE;

esp_err_t esp_wifi_init_internal(const wifi_init_config_t *config);

/**
  * @brief  Init WiFi
  *         Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
  *         WiFi NVS structure etc, this WiFi also start WiFi task
  *
  * @attention 1. This API must be called before all other WiFi API can be called
  * @attention 2. Always use WIFI_INIT_CONFIG_DEFAULT macro to init the config to default values, this can
  *               guarantee all the fields got correct value when more fields are added into wifi_init_config_t
  *               in future release. If you want to set your owner initial values, overwrite the default values
  *               which are set by WIFI_INIT_CONFIG_DEFAULT, please be notified that the field 'magic' of 
  *               wifi_init_config_t should always be WIFI_INIT_CONFIG_MAGIC!
  *
  * @param  config pointer to WiFi init configuration structure; can point to a temporary variable.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: out of memory
  *    - others: refer to error code esp_err.h
  */
esp_err_t esp_wifi_init(const wifi_init_config_t *config)
{
    esp_event_set_default_wifi_handlers();
    return esp_wifi_init_internal(config);
}

void esp_deep_sleep_set_rf_option(uint8_t option)
{
    phy_afterwake_set_rfoption(option);
}

size_t __attribute__((weak)) esp_wifi_scan_get_ap_num_max(void)
{
    return CONFIG_SCAN_AP_MAX;
}

bool IRAM_ATTR esp_wifi_try_rate_from_high(void) {
#if CONFIG_WIFI_TX_RATE_SEQUENCE_FROM_HIGH
    int8_t rssi;
    rssi = esp_wifi_get_ap_rssi();
    wifi_mode_t mode;
    esp_wifi_get_mode( &mode );
    if (rssi < -26 && mode == WIFI_MODE_STA) {
        return true;
    }
#endif
    return false;
}
