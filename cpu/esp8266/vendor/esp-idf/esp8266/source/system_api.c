// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <stdlib.h>

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"

#include "crc.h"

#include "esp8266/eagle_soc.h"
#include "esp8266/efuse_register.h"

#include "FreeRTOS.h"

#include "nvs.h"
#include "nvs_flash.h"

static const char* TAG = "system_api";

static uint8_t base_mac_addr[6] = { 0 };

esp_err_t esp_base_mac_addr_set(uint8_t *mac)
{
    if (mac == NULL) {
        ESP_LOGE(TAG, "Base MAC address is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(base_mac_addr, mac, 6);

    return ESP_OK;
}

esp_err_t esp_base_mac_addr_get(uint8_t *mac)
{
    uint8_t null_mac[6] = {0};

    if (memcmp(base_mac_addr, null_mac, 6) == 0) {
        ESP_LOGI(TAG, "Base MAC address is not set, read default base MAC address from BLK0 of EFUSE");
        return ESP_ERR_INVALID_MAC;
    }

    memcpy(mac, base_mac_addr, 6);

    return ESP_OK;
}

esp_err_t esp_efuse_mac_get_default(uint8_t* mac)
{
    uint32_t efuse[4];

    uint8_t efuse_crc = 0;
    uint8_t calc_crc = 0;
    uint8_t version;
    uint8_t use_default = 1;

    efuse[0] = REG_READ(EFUSE_DATA0_REG);
    efuse[1] = REG_READ(EFUSE_DATA1_REG);
    efuse[2] = REG_READ(EFUSE_DATA2_REG);
    efuse[3] = REG_READ(EFUSE_DATA3_REG);

    mac[3] = efuse[1] >> 8;
    mac[4] = efuse[1];
    mac[5] = efuse[0] >> 24;

    if (efuse[2] & EFUSE_IS_48BITS_MAC) {
        uint8_t tmp_mac[4];

        mac[0] = efuse[3] >> 16;
        mac[1] = efuse[3] >> 8;
        mac[2] = efuse[3];

        use_default = 0;

        tmp_mac[0] = mac[2];
        tmp_mac[1] = mac[1];
        tmp_mac[2] = mac[0];

        efuse_crc = efuse[2] >> 24;
        calc_crc = esp_crc8(tmp_mac, 3);

        if (efuse_crc != calc_crc)
            use_default = 1;

        if (!use_default) {
            version = (efuse[1] >> EFUSE_VERSION_S) & EFUSE_VERSION_V;

            if (version == EFUSE_VERSION_1 || version == EFUSE_VERSION_2) {
                tmp_mac[0] = mac[5];
                tmp_mac[1] = mac[4];
                tmp_mac[2] = mac[3];
                tmp_mac[3] = efuse[1] >> 16;

                efuse_crc = efuse[0] >> 16;
                calc_crc = esp_crc8(tmp_mac, 4);

                if (efuse_crc != calc_crc)
                    use_default = 1;
            }
        }
    }

    if (use_default) {
        mac[0] = 0x18;
        mac[1] = 0xFE;
        mac[2] = 0x34;
    }

    return ESP_OK;
}

static const char *BACKUP_MAC_NAMESPACE = "backup_mac";
static const char *BACKUP_MAC_DATA_KEY = "backup_mac_data";
#define MAC_DATA_LEN_WITH_CRC (4*4)

static esp_err_t load_backup_mac_data(uint8_t *mac)
{
    esp_err_t err;
    nvs_handle handle;
    uint32_t efuse[4];
    uint8_t efuse_crc = 0;
    uint8_t calc_crc = 0;
    uint8_t version;

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    err = nvs_open(BACKUP_MAC_NAMESPACE, NVS_READONLY, &handle);

    if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_LOGE(TAG, "%s: NVS has not been initialized. "
                 "Call nvs_flash_init before starting WiFi/BT.", __func__);
        return ESP_ERR_INVALID_MAC;
    } else if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return ESP_ERR_INVALID_MAC;
    }

    size_t length = MAC_DATA_LEN_WITH_CRC;
    err = nvs_get_blob(handle, BACKUP_MAC_DATA_KEY, efuse, &length);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to get backup mac (0x%x)", __func__, err);
        return ESP_ERR_INVALID_MAC;
    }
    if (length != MAC_DATA_LEN_WITH_CRC) {
        ESP_LOGD(TAG, "%s: invalid length of backup mac (%d)", __func__, length);
        return ESP_ERR_INVALID_MAC;
    }
    nvs_close(handle);

    mac[3] = efuse[1] >> 8;
    mac[4] = efuse[1];
    mac[5] = efuse[0] >> 24;

    if (efuse[2] & EFUSE_IS_48BITS_MAC) {
        uint8_t tmp_mac[4];

        mac[0] = efuse[3] >> 16;
        mac[1] = efuse[3] >> 8;
        mac[2] = efuse[3];

        tmp_mac[0] = mac[2];
        tmp_mac[1] = mac[1];
        tmp_mac[2] = mac[0];

        efuse_crc = efuse[2] >> 24;
        calc_crc = esp_crc8(tmp_mac, 3);

        if (efuse_crc != calc_crc) {
            ESP_LOGE(TAG, "High MAC CRC error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
            return ESP_ERR_INVALID_MAC;
        }

        version = (efuse[1] >> EFUSE_VERSION_S) & EFUSE_VERSION_V;

        if (version == EFUSE_VERSION_1 || version == EFUSE_VERSION_2) {
            tmp_mac[0] = mac[5];
            tmp_mac[1] = mac[4];
            tmp_mac[2] = mac[3];
            tmp_mac[3] = efuse[1] >> 16;

            efuse_crc = efuse[0] >> 16;
            calc_crc = esp_crc8(tmp_mac, 4);

            if (efuse_crc != calc_crc) {
                ESP_LOGE(TAG, "CRC8 error, efuse_crc = 0x%02x; calc_crc = 0x%02x", efuse_crc, calc_crc);
                return ESP_ERR_INVALID_MAC;
            }
        }
    } else {
        mac[0] = 0x18;
        mac[1] = 0xFE;
        mac[2] = 0x34;
    }

    return ESP_OK;
}

static esp_err_t store_backup_mac_data(void)
{
    esp_err_t err;
    nvs_handle handle;
    uint32_t efuse[4];
    efuse[0] = REG_READ(EFUSE_DATA0_REG);
    efuse[1] = REG_READ(EFUSE_DATA1_REG);
    efuse[2] = REG_READ(EFUSE_DATA2_REG);
    efuse[3] = REG_READ(EFUSE_DATA3_REG);

    err = nvs_open(BACKUP_MAC_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return err;
    }

    err = nvs_set_blob(handle, BACKUP_MAC_DATA_KEY, efuse, MAC_DATA_LEN_WITH_CRC);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store backup mac data failed(0x%x)\n", __func__, err);
        return err;
    }

    err = nvs_commit(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store backup mac data failed(0x%x)\n", __func__, err);
    }

    return err;
}

esp_err_t esp_derive_mac(uint8_t* local_mac, const uint8_t* universal_mac)
{
    uint8_t idx;

    if (local_mac == NULL || universal_mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    memcpy(local_mac, universal_mac, 6);
    for (idx = 0; idx < 64; idx++) {
        local_mac[0] = universal_mac[0] | 0x02;
        local_mac[0] ^= idx << 2;

        if (memcmp(local_mac, universal_mac, 6)) {
            break;
        }
    }

    return ESP_OK;
}

esp_err_t esp_read_mac(uint8_t* mac, esp_mac_type_t type)
{
    uint8_t efuse_mac[6];

    if (mac == NULL) {
        ESP_LOGE(TAG, "mac address param is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (type < ESP_MAC_WIFI_STA || type > ESP_MAC_WIFI_SOFTAP) {
        ESP_LOGE(TAG, "mac type is incorrect");
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_base_mac_addr_get(efuse_mac) != ESP_OK) {
        if (load_backup_mac_data(efuse_mac) != ESP_OK) {
            if (esp_efuse_mac_get_default(efuse_mac) != ESP_OK) {
                ESP_LOGE(TAG, "Get mac address error");
                abort();
            } else {
                store_backup_mac_data();
            }
        }
    }

    switch (type) {
    case ESP_MAC_WIFI_STA:
        memcpy(mac, efuse_mac, 6);
        break;
    case ESP_MAC_WIFI_SOFTAP:
        esp_derive_mac(mac, efuse_mac);
        break;
    default:
        ESP_LOGW(TAG, "incorrect mac type");
        break;
    }

    return ESP_OK;
}

/**
 * Get IDF version
 */
const char* esp_get_idf_version(void)
{
    return IDF_VER;
}

/**
 * @brief Fill an esp_chip_info_t structure with information about the ESP8266 chip
 */
static void get_chip_info_esp8266(esp_chip_info_t* out_info)
{
    memset(out_info, 0, sizeof(*out_info));
    
    out_info->model = CHIP_ESP8266;
    out_info->revision = 1;
    out_info->cores = 1;
    out_info->features = CHIP_FEATURE_WIFI_BGN;
}

/**
 * @brief Fill an esp_chip_info_t structure with information about the chip
 */
void esp_chip_info(esp_chip_info_t* out_info)
{
    // Only ESP8266 is supported now, in the future call one of the
    // chip-specific functions based on sdkconfig choice
    return get_chip_info_esp8266(out_info);
}

#ifdef MODULE_ESP_IDF_HEAP
/**
  * @brief  Get the size of available heap.
  */
uint32_t esp_get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

/**
  * @brief Get the minimum heap that has ever been available
  */
uint32_t esp_get_minimum_free_heap_size(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT);
}
#endif /* MODULE_ESP_IDF_HEAP */
