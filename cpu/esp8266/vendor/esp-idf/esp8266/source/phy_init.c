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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "rom/ets_sys.h"

#include "esp_err.h"
#include "esp_phy_init.h"
#include "esp_system.h"
#include "esp_log.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "phy_init_data.h"
#include "phy.h"

static const char *TAG = "phy_init";

static uint8_t phy_check_calibration_data(uint8_t *rf_cal_data)
{
#define CHECK_NUM   26
#define CHIP_ID_L   24
#define CHIP_ID_H   25

    uint8_t i;
    uint32_t *cal_data_word = (uint32_t *)rf_cal_data;
    uint32_t check_sum = 0;

    /* L: flag_1[79:76], version[59:56], mac_map[55:48], mac_l[47:24] */
    uint32_t chip_id_l = ((REG_READ(0x3FF00058) & 0xF000) << 16) |
            (REG_READ(0x3ff00054) & 0xFFFFFFF);
    /* H: mac_l[31:24], mac_h[119:96] */
    uint32_t chip_id_h = (REG_READ(0x3FF00050) & 0xFF000000) |
            (REG_READ(0x3ff0005C) & 0xFFFFFF);

    cal_data_word[CHIP_ID_L] = chip_id_l;
    cal_data_word[CHIP_ID_H] = chip_id_h;

    for (i = 0; i < CHECK_NUM; i++) {
        check_sum += cal_data_word[i];
    }

    return (cal_data_word[CHECK_NUM] != ~check_sum);
}

/* temporary put rx_gain_dc_table in memory */
/* ToDo: use rx_gain_dc_table in nvs, need to modify internal libraries */
uint32_t rx_gain_dc_table[125];

esp_err_t esp_phy_rf_init(const esp_phy_init_data_t *init_data, esp_phy_calibration_mode_t mode,
                          esp_phy_calibration_data_t *calibration_data, phy_rf_module_t module)
{
    esp_err_t status = ESP_OK;
    uint8_t sta_mac[6];
    uint8_t *local_init_data = calloc(1, 256);
#ifdef CONFIG_CONSOLE_UART_BAUDRATE
    const uint32_t uart_baudrate = CONFIG_CONSOLE_UART_BAUDRATE;
#else
    const uint32_t uart_baudrate = 74880; // ROM default baudrate
#endif

    memcpy(local_init_data, init_data->params, 128);

    extern uint32_t *phy_rx_gain_dc_table;
    phy_rx_gain_dc_table = calibration_data->rx_gain_dc_table;
    uint8_t cal_data_check = phy_check_calibration_data(calibration_data->rf_cal_data) ||
            phy_check_data_table(phy_rx_gain_dc_table, 125, 1);

    phy_afterwake_set_rfoption(1);

    if (!cal_data_check) {
        write_data_to_rtc(calibration_data->rf_cal_data);
    }

    esp_efuse_mac_get_default(sta_mac);
    chip_init(local_init_data, sta_mac, uart_baudrate);
    get_data_from_rtc((uint8_t *)calibration_data);

    memcpy(rx_gain_dc_table, calibration_data->rx_gain_dc_table, 4 * 125);
    phy_rx_gain_dc_table = rx_gain_dc_table;

    free(local_init_data);

    if (cal_data_check == ESP_CAL_DATA_CHECK_FAIL) {
#ifdef CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE
        ESP_LOGW(TAG, "saving new calibration data because of checksum failure, mode(%d)", mode);
        if (mode != PHY_RF_CAL_FULL) {
            esp_phy_store_cal_data_to_nvs(calibration_data);
        }
#endif
    }

    return status;
}

esp_err_t esp_phy_rf_deinit(phy_rf_module_t module)
{
    esp_err_t status = ESP_OK;

    return status;
}

// PHY init data handling functions
#if CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION
#include "esp_partition.h"

const esp_phy_init_data_t *esp_phy_get_init_data(void)
{
    const esp_partition_t *partition = esp_partition_find_first(
                                           ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_PHY, NULL);

    if (partition == NULL) {
        ESP_LOGE(TAG, "PHY data partition not found");
        return NULL;
    }

    ESP_LOGD(TAG, "loading PHY init data from partition at offset 0x%x", partition->address);
    size_t init_data_store_length = sizeof(phy_init_magic_pre) +
                                    sizeof(esp_phy_init_data_t) + sizeof(phy_init_magic_post);
    uint8_t *init_data_store = (uint8_t *) malloc(init_data_store_length);

    if (init_data_store == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for PHY init data");
        return NULL;
    }

    esp_err_t err = esp_partition_read(partition, 0, init_data_store, init_data_store_length);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to read PHY data partition (0x%x)", err);
        return NULL;
    }

    if (memcmp(init_data_store, PHY_INIT_MAGIC, sizeof(phy_init_magic_pre)) != 0 ||
            memcmp(init_data_store + init_data_store_length - sizeof(phy_init_magic_post),
                   PHY_INIT_MAGIC, sizeof(phy_init_magic_post)) != 0) {
        ESP_LOGE(TAG, "failed to validate PHY data partition");
        return NULL;
    }

    ESP_LOGD(TAG, "PHY data partition validated");
    return (const esp_phy_init_data_t *)(init_data_store + sizeof(phy_init_magic_pre));
}

void esp_phy_release_init_data(const esp_phy_init_data_t *init_data)
{
    free((uint8_t *) init_data - sizeof(phy_init_magic_pre));
}

#else // CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION

// phy_init_data.h will declare static 'phy_init_data' variable initialized with default init data

const esp_phy_init_data_t *esp_phy_get_init_data(void)
{
    ESP_LOGD(TAG, "loading PHY init data from application binary");
    return &phy_init_data;
}

void esp_phy_release_init_data(const esp_phy_init_data_t *init_data)
{
    // no-op
}
#endif // CONFIG_ESP_PHY_INIT_DATA_IN_PARTITION


// PHY calibration data handling functions
static const char *PHY_NAMESPACE = "phy";
static const char *PHY_CAL_DATA_KEY = "cal_data";
static const char *PHY_RX_GAIN_DC_TABLE_KEY = "dc_table";

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle handle,
        esp_phy_calibration_data_t *out_cal_data);

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle handle,
        const esp_phy_calibration_data_t *cal_data);

esp_err_t esp_phy_load_cal_data_from_nvs(esp_phy_calibration_data_t *out_cal_data)
{
    nvs_handle handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READONLY, &handle);

    if (err == ESP_ERR_NVS_NOT_INITIALIZED) {
        ESP_LOGE(TAG, "%s: NVS has not been initialized. "
                 "Call nvs_flash_init before starting WiFi/BT.", __func__);
    } else if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return err;
    }

    err = load_cal_data_from_nvs_handle(handle, out_cal_data);
    nvs_close(handle);
    return err;
}

esp_err_t esp_phy_store_cal_data_to_nvs(const esp_phy_calibration_data_t *cal_data)
{
    nvs_handle handle;
    esp_err_t err = nvs_open(PHY_NAMESPACE, NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: failed to open NVS namespace (0x%x)", __func__, err);
        return err;
    } else {
        err = store_cal_data_to_nvs_handle(handle, cal_data);
        nvs_close(handle);
        return err;
    }
}

static esp_err_t load_cal_data_from_nvs_handle(nvs_handle handle,
        esp_phy_calibration_data_t *out_cal_data)
{
    esp_err_t err;

    size_t length = sizeof(out_cal_data->rf_cal_data);

    err = nvs_get_blob(handle, PHY_CAL_DATA_KEY, out_cal_data->rf_cal_data, &length);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed to get cal_data(0x%x)", __func__, err);
        return err;
    }

    if (length != sizeof(out_cal_data->rf_cal_data)) {
        ESP_LOGD(TAG, "%s: invalid length of cal_data (%d)", __func__, length);
        return ESP_ERR_INVALID_SIZE;
    }

    length = sizeof(out_cal_data->rx_gain_dc_table);
    err = nvs_get_blob(handle, PHY_RX_GAIN_DC_TABLE_KEY, out_cal_data->rx_gain_dc_table, &length);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: failed to get rx_gain_dc_table(0x%x)", __func__, err);
        return err;
    }

    if (length != sizeof(out_cal_data->rx_gain_dc_table)) {
        ESP_LOGD(TAG, "%s: invalid length of rx_gain_dc_table (%d)", __func__, length);
        return ESP_ERR_INVALID_SIZE;
    }

    return ESP_OK;
}

static esp_err_t store_cal_data_to_nvs_handle(nvs_handle handle,
        const esp_phy_calibration_data_t *cal_data)
{
    esp_err_t err;

    err = nvs_set_blob(handle, PHY_CAL_DATA_KEY, cal_data->rf_cal_data, sizeof(cal_data->rf_cal_data));

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration data failed(0x%x)\n", __func__, err);
        return err;
    }

    err = nvs_set_blob(handle, PHY_RX_GAIN_DC_TABLE_KEY, cal_data->rx_gain_dc_table, sizeof(cal_data->rx_gain_dc_table));

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store rx gain dc table failed(0x%x)\n", __func__, err);
        return err;
    }

    err = nvs_commit(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: store calibration nvs commit failed(0x%x)\n", __func__, err);
    }

    return err;
}

void esp_phy_load_cal_and_init(phy_rf_module_t module)
{
    esp_phy_calibration_data_t *cal_data =
        (esp_phy_calibration_data_t *) calloc(sizeof(esp_phy_calibration_data_t), 1);

    if (cal_data == NULL) {
        ESP_LOGE(TAG, "failed to allocate memory for RF calibration data");
        abort();
    }

    const esp_phy_init_data_t *init_data = esp_phy_get_init_data();

    if (init_data == NULL) {
        ESP_LOGE(TAG, "failed to obtain PHY init data");
        abort();
    }

#ifdef CONFIG_ESP_PHY_CALIBRATION_AND_DATA_STORAGE
    esp_phy_calibration_mode_t calibration_mode = PHY_RF_CAL_PARTIAL;

//    if (rtc_get_reset_reason(0) == DEEPSLEEP_RESET) {
//        calibration_mode = PHY_RF_CAL_NONE;
//    }

    esp_err_t err = esp_phy_load_cal_data_from_nvs(cal_data);

    if (err != ESP_OK) {
        ESP_LOGW(TAG, "failed to load RF calibration data (0x%x), falling back to full calibration", err);
        calibration_mode = PHY_RF_CAL_FULL;
    }

    esp_phy_rf_init(init_data, calibration_mode, cal_data, module);

    if (calibration_mode != PHY_RF_CAL_NONE && err != ESP_OK) {
        err = esp_phy_store_cal_data_to_nvs(cal_data);
    } else {
        err = ESP_OK;
    }

#else
    esp_phy_rf_init(init_data, PHY_RF_CAL_FULL, cal_data, module);
#endif

    esp_phy_release_init_data(init_data);

    free(cal_data); // PHY maintains a copy of calibration data, so we can free this
}
