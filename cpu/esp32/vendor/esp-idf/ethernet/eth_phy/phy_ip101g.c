// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "esp_attr.h"
#include "esp_log.h"
#include "esp_eth.h"

#include "eth_phy/phy_ip101g.h"
#include "eth_phy/phy_reg.h"

/* Value of MII_PHY_IDENTIFIER_REGs for IC Plus IP101G
 * (Except for bottom 4 bits of ID2, used for model revision)
 */
#define IP101G_PHY_ID1 0x0243
#define IP101G_PHY_ID2 0x0C50
#define IP101G_PHY_ID2_MASK 0xFFF0

/* IP101G-specific registers */
#define PAGE_CONTROL_REG (0x14)

#define POWER_DOWN                         BIT(11)

#define PHY_SPECIAL_CONTROL_STATUS_REG    (0x12)
#define AUTO_NEGOTIATION_DONE              BIT(11)

#define PHY_CTRL_AND_SPECIFIC_STATUS_REG  (0x1e)
#define SPEED_DUPLEX_INDICATION_MASK      (0x7)
#define SPEED_DUPLEX_INDICATION_10T_HALF   0x01
#define SPEED_DUPLEX_INDICATION_10T_FULL   0x05
#define SPEED_DUPLEX_INDICATION_100T_HALF  0x02
#define SPEED_DUPLEX_INDICATION_100T_FULL  0x06

static const char *TAG = "ip101";

static void phy_ip101g_page_select(uint8_t page)
{
    ESP_LOGD(TAG, "phy_ip101g_page_select(%u)", page);
    esp_eth_smi_write(PAGE_CONTROL_REG, page);
}

void phy_ip101g_check_phy_init(void)
{
    phy_ip101g_dump_registers();

    esp_eth_smi_wait_set(MII_BASIC_MODE_STATUS_REG, MII_AUTO_NEGOTIATION_COMPLETE, 0);
    phy_ip101g_page_select(16);
    esp_eth_smi_wait_set(PHY_SPECIAL_CONTROL_STATUS_REG, AUTO_NEGOTIATION_DONE, 0);
}

eth_speed_mode_t phy_ip101g_get_speed_mode(void)
{
    phy_ip101g_page_select(16);
    uint16_t speed = esp_eth_smi_read(PHY_CTRL_AND_SPECIFIC_STATUS_REG) & SPEED_DUPLEX_INDICATION_MASK;
    if (speed == SPEED_DUPLEX_INDICATION_100T_HALF || speed == SPEED_DUPLEX_INDICATION_100T_FULL) {
        ESP_LOGD(TAG, "phy_ip101g_get_speed_mode(100)");
        return ETH_SPEED_MODE_100M;
    }
    ESP_LOGD(TAG, "phy_ip101g_get_speed_mode(10)");
    return ETH_SPEED_MODE_10M;
}

eth_duplex_mode_t phy_ip101g_get_duplex_mode(void)
{
    phy_ip101g_page_select(16);
    uint16_t speed = esp_eth_smi_read(PHY_CTRL_AND_SPECIFIC_STATUS_REG) & SPEED_DUPLEX_INDICATION_MASK;
    if (speed == SPEED_DUPLEX_INDICATION_10T_FULL || speed == SPEED_DUPLEX_INDICATION_100T_FULL) {
        ESP_LOGD(TAG, "phy_ip101g_get_duplex_mode(FULL)");
        return ETH_MODE_FULLDUPLEX;
    }
    ESP_LOGD(TAG, "phy_ip101g_get_duplex_mode(HALF)");
    return ETH_MODE_HALFDUPLEX;
}

void phy_ip101g_power_enable(bool enable)
{
    ESP_LOGD(TAG, "phy_ip101g_power_enable(%d)", enable);
    uint16_t cfg = esp_eth_smi_read(MII_BASIC_MODE_CONTROL_REG);
    if (enable) {
        cfg &= (UINT16_MAX ^ POWER_DOWN);
    } else {
        cfg |= POWER_DOWN;
    }
    esp_eth_smi_write(MII_BASIC_MODE_CONTROL_REG, cfg);
    // TODO: only enable if config.flow_ctrl_enable == true
    phy_mii_enable_flow_ctrl();
}

void phy_ip101g_init(void)
{
    ESP_LOGD(TAG, "phy_ip101g_init()");
    phy_ip101g_dump_registers();

    esp_eth_smi_write(MII_BASIC_MODE_CONTROL_REG, MII_SOFTWARE_RESET);

    esp_err_t res1, res2;
    do {
        // Call esp_eth_smi_wait_value() with a timeout so it prints an error periodically
        res1 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_1_REG, IP101G_PHY_ID1, UINT16_MAX, 1000);
        res2 = esp_eth_smi_wait_value(MII_PHY_IDENTIFIER_2_REG, IP101G_PHY_ID2, IP101G_PHY_ID2_MASK, 1000);
    } while(res1 != ESP_OK || res2 != ESP_OK);

    ets_delay_us(300);

    // TODO: only enable if config.flow_ctrl_enable == true
    phy_mii_enable_flow_ctrl();
}

const eth_config_t phy_ip101g_default_ethernet_config = {
    // By default, the PHY address is 0 or 1 based on PHYAD0
    // pin. Can also be overriden in software. See datasheet
    // for defaults.
    .phy_addr = 1,
    .mac_mode = ETH_MODE_RMII,
    .clock_mode = ETH_CLOCK_GPIO0_IN,
    //Only FULLDUPLEX mode support flow ctrl now!
    .flow_ctrl_enable = true,
    .phy_init = phy_ip101g_init,
    .phy_check_init = phy_ip101g_check_phy_init,
    .phy_power_enable = phy_ip101g_power_enable,
    .phy_check_link = phy_mii_check_link_status,
    .phy_get_speed_mode = phy_ip101g_get_speed_mode,
    .phy_get_duplex_mode = phy_ip101g_get_duplex_mode,
    .phy_get_partner_pause_enable = phy_mii_get_partner_pause_enable,
};

void phy_ip101g_dump_registers(void)
{
    ESP_LOGD(TAG, "IP101G Registers:");
    ESP_LOGD(TAG, "BCR    0x%04x", esp_eth_smi_read(0x0));
    ESP_LOGD(TAG, "BSR    0x%04x", esp_eth_smi_read(0x1));
    ESP_LOGD(TAG, "PHY1   0x%04x", esp_eth_smi_read(0x2));
    ESP_LOGD(TAG, "PHY2   0x%04x", esp_eth_smi_read(0x3));
    ESP_LOGD(TAG, "ANAR   0x%04x", esp_eth_smi_read(0x4));
    ESP_LOGD(TAG, "ANLPAR 0x%04x", esp_eth_smi_read(0x5));
    ESP_LOGD(TAG, "ANER   0x%04x", esp_eth_smi_read(0x6));
    phy_ip101g_page_select(16);
    ESP_LOGD(TAG, "PSCR   0x%04x", esp_eth_smi_read(0x10));
    ESP_LOGD(TAG, "PSMR   0x%04x", esp_eth_smi_read(0x18));
    ESP_LOGD(TAG, "PMCSSR 0x%04x", esp_eth_smi_read(0x1e));
}
