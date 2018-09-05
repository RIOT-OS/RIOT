/* Routines to allow custom access to the Internal Espressif
   SDK PHY datastructures.

   Matches espressif/phy_internal.h

   Part of esp-open-rtos. Copyright (C) 2016 Angus Gratton,
   BSD Licensed as described in the file LICENSE.
 */

/*
Copyright (C) 2016 Angus Gratton
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MODULE_ESP_SDK
#ifdef RIOT_VERSION
#include <stdio.h>
#include "c_types.h"
#include "esp/common_macros.h"
#include "esp/flashchip.h"
#include "esp/phy_info.h"
#include "spi_flash.h"
#include "sdk/rom.h"

#define sdk_spi_flash_read   spi_flash_read
#define sdk_spi_flash_write  spi_flash_write

#else
#include <espressif/phy_info.h>
#include <espressif/esp_common.h>
#include <common_macros.h>
#endif
#include <string.h>

static const sdk_phy_info_t IROM default_phy_info = {
    ._reserved00 = { 0x05, 0x00, 0x04, 0x02, 0x05 },
    .version = 5,
    ._reserved06 = { 0x05, 0x02, 0x05, 0x00, 0x04, 0x05, 0x05, 0x04,
                     0x05, 0x05, 0x04,-0x02,-0x03,-0x01,-0x10,-0x10,
                     -0x10,-0x20,-0x20, -0x20},
    .spur_freq_primary = 225,
    .spur_freq_divisor = 10,
    .spur_freq_en_h = 0xFF,
    .spur_freq_en_l = 0xFF,

    ._reserved1e = { 0xf8, 0, 0xf8, 0xf8 },

    .target_power = { 82, 78, 74, 68, 64, 56 },
    .target_power_index_mcs = { 0, 0, 1, 1, 2, 3, 4, 5 },

    .crystal_freq = CRYSTAL_FREQ_26M,

    .sdio_config = SDIO_CONFIG_AUTO,

    .bt_coexist_config = BT_COEXIST_CONFIG_NONE,
    .bt_coexist_protocol = BT_COEXIST_PROTOCOL_WIFI_ONLY,

    .dual_ant_config = DUAL_ANT_CONFIG_NONE,

    ._reserved34 = 0x02,

    .crystal_sleep = CRYSTAL_SLEEP_OFF,

    .spur_freq_2_primary = 225,
    .spur_freq_2_divisor = 10,
    .spur_freq_2_en_h = 0x00,
    .spur_freq_2_en_l = 0x00,
    .spur_freq_cfg_msb = 0x00,
    .spur_freq_2_cfg_msb = 0x00,
    .spur_freq_3_cfg = 0x0000,
    .spur_freq_4_cfg = 0x0000,

    ._reserved4a = { 0x01, 0x93, 0x43, 0x00 },

    .low_power_en = false,
    .lp_atten_stage01 = LP_ATTEN_STAGE01_23DB,
    .lp_atten_bb = 0,

    .pwr_ind_11b_en = false,
    .pwr_ind_11b_0 = 0,
    .pwr_ind_11b_1 = 0,

    /* Nominal 3.3V VCC. NOTE: This value is 0 in the
       esp-open-rtos SDK default config sector, and may be unused
       by that version of the SDK?
    */
    .pa_vdd = 33,

    /* Note: untested with the esp-open-rtos SDK default config sector, may be unused? */
    .freq_correct_mode = FREQ_CORRECT_DISABLE,
    .force_freq_offset = 0,

    /* Note: is zero with the esp-open-rtos SDK default config sector, may be unused? */
    .rf_cal_mode = RF_CAL_MODE_SAVED,
};

sdk_phy_info_t* default_phy_info_ref = (sdk_phy_info_t*)&default_phy_info;
sdk_phy_info_t* get_default_phy_info_ref(void)
{
    return (sdk_phy_info_t*)&default_phy_info;
}

void get_default_phy_info(sdk_phy_info_t *info) __attribute__((weak, alias("get_sdk_default_phy_info")));

void get_sdk_default_phy_info(sdk_phy_info_t *info)
{
    memcpy(info, &default_phy_info, sizeof(sdk_phy_info_t));
}

void read_saved_phy_info(sdk_phy_info_t *info)
{
    sdk_spi_flash_read(sdk_flashchip.chip_size - sdk_flashchip.sector_size * 4, (uint32_t *)info, sizeof(sdk_phy_info_t));
}

void write_saved_phy_info(const sdk_phy_info_t *info)
{
    sdk_spi_flash_write(sdk_flashchip.chip_size - sdk_flashchip.sector_size * 4, (uint32_t *)info, sizeof(sdk_phy_info_t));
}

void dump_phy_info(const sdk_phy_info_t *info, bool raw)
{
    printf("version=%d\n", info->version);
    printf("spur_freq = %.3f (%d/%d)\n",
           (float)info->spur_freq_primary / info->spur_freq_divisor,
           info->spur_freq_primary,
           info->spur_freq_divisor);
    printf("spur_freq_en = 0x%02x 0x%02x\n", info->spur_freq_en_h,
           info->spur_freq_en_l);
    printf("target_power\n");
    for(int i = 0; i < 6; i++) {
        printf("  %d: %.2fdB (raw 0x%02x)\n", i,
               info->target_power[i]/4.0,
               info->target_power[i]);
    }
    printf("target_power_index_mcs:");
    for(int i = 0; i < 8; i++) {
        printf(" %d%c", info->target_power_index_mcs[i],
               i == 7 ? '\n' : ',');
    }

    printf("crystal_freq: %s (raw %d)\n",
           (info->crystal_freq == CRYSTAL_FREQ_40M ? "40MHz" :
            (info->crystal_freq == CRYSTAL_FREQ_26M ? "26MHz" :
             (info->crystal_freq == CRYSTAL_FREQ_24M ? "24MHz" : "???"))),
           info->crystal_freq);

    printf("sdio_config: %d\n", info->sdio_config);
    printf("bt_coexist config: %d protocol: 0x%02x\n",
           info->bt_coexist_config, info->bt_coexist_protocol);
    printf("dual_ant_config: %d\n", info->dual_ant_config);

    printf("crystal_sleep: %d\n", info->crystal_sleep);

    printf("spur_freq_2 = %.3f (%d/%d)\n",
           (float)info->spur_freq_2_primary / info->spur_freq_2_divisor,
           info->spur_freq_2_primary,
           info->spur_freq_2_divisor);
    printf("spur_freq_2_en = 0x%02x 0x%02x\n", info->spur_freq_2_en_h,
           info->spur_freq_2_en_l);

    printf("spur_freq_cfg_msb = 0x%02x\n", info->spur_freq_cfg_msb);
    printf("spur_freq_2_)cfg_msb = 0x%02x\n", info->spur_freq_2_cfg_msb);
    printf("spur_freq_3_cfg = 0x%04x\n", info->spur_freq_3_cfg);
    printf("spur_freq_4_cfg = 0x%04x\n", info->spur_freq_4_cfg);

    printf("low_power_en = %d\n", info->low_power_en);
    printf("lp_atten_stage01 = 0x%02x\n", info->lp_atten_stage01);
    printf("lp_atten_bb = %.2f (raw 0x%02x)\n", info->lp_atten_bb / 4.0,
           info->lp_atten_bb);

    printf("pa_vdd = %d\n", info->pa_vdd);

    printf("freq_correct_mode = 0x%02x\n", info->freq_correct_mode);
    printf("force_freq_offset = %d\n", info->force_freq_offset);
    printf("rf_cal_mode = 0x%02x\n", info->rf_cal_mode);

    if(raw) {
        printf("Raw values:");
        uint8_t *p = (uint8_t *)info;
        for(unsigned int i = 0; i < sizeof(sdk_phy_info_t); i ++) {
            if(i % 8 == 0) {
                printf("\n0x%02x:", i);
            }
            printf(" %02x", p[i]);
        }
        printf("\n\n");
    }
}

#endif
