/** Internal Espressif SDK "PHY info" data structure

   The data structure (sdk_phy_info_t) is used to configure the
   ESP8266 PHY layer via the SDK. The fields here are not written
   directly to hardware, the SDK code (mostly in libphy) parses this
   structure and configures the hardware.

   The structure loaded at reset time from a flash configuration
   sector (see read_saved_phy_info()) (Espressif's SDK sources this
   from a file "esp_init_data_default.bin"). If no valid structure is
   found in the flash config sector then the SDK loads default values
   (see get_default_phy_info()). It is possible to implement a custom
   get_default_phy_info() to change the PHY default settings (see the
   'version' field below).

   @note It is possible that the SDK will quietly write a new
   configuration sector to flash itself following internal
   calibration, etc. However this does not seem to happen, you need to
   flash it explicitly if you want it stored there.

   @note Most of what is below is unconfirmed, except where a @note
   says that it has been confirmed to work as expected. Please
   consider submitting notes if you find behaviour here that works or
   doesn't work as expected.

   Information on the meaning/offset of fields comes from Espressif's
   flash download tool, which uses an Excel spreadsheet (in the
   init_data directory of the ZIP file) to configure and a Python
   script to convert an esp_init_data_custom.bin file to flash:
   http://bbs.espressif.com/viewtopic.php?f=5&t=433

   Many fields remain undocumented (& disassembly of libphy suggests
   that some documented fields supported undocumented values.)

   A few additional notes about the phy_info fields can be found
   in the ESP Arduino ESP8266 phy_init_data structure (however most of
   that content is verbatim from Espressif's spreadsheet):
   https://github.com/esp8266/Arduino/blob/master/cores/esp8266/core_esp8266_phy.c#L29

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

#ifndef PHY_INFO_H
#define PHY_INFO_H

#ifndef DOXYGEN

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CRYSTAL_FREQ_xx values as used by sdk_phy_info_t.crystal_freq */
#define CRYSTAL_FREQ_40M 0
#define CRYSTAL_FREQ_26M 1
#define CRYSTAL_FREQ_24M 2

/* SDIO_CONFIG_xx values as used by sdk_phy_info_t.sdio_config */
#define SDIO_CONFIG_AUTO 0   /* Uses pin strapping to determine */
#define SDIO_CONFIG_SDIOV1_1 /* Data output on negative edge */
#define SDIO_CONFIG_SDIOV2_0 /* data output on positive edge */

/* BT_COEXIST_CONFIG_xx values as used by sdk_phy_info_t.bt_coexist */
/* No bluetooth */
#define BT_COEXIST_CONFIG_NONE 0
/* Coexistence configuration A:
   GPIO 0  - WLAN_ACTIVE
   GPIO 14 - BT_ACTIVE
   GPIO 13 - BT_PRIORITY
   GPIO 3  - ANT_SEL_BT
*/
#define BT_COEXIST_CONFIG_A 1
/* No coexistence, but Bluetooth enabled?
   Unsure how this works?
 */
#define BT_COEXIST_CONFIG_PRESENT 2
/* Coexistence configuration B:
   GPIO 0 - WLAN_ACTIVE
   GPIO 14 - BT_PRIORITY
   GPIO 13 - BT_ACTIVE
   GPIO 3  - ANT_SEL_BT
*/
#define BT_COEXIST_CONFIG_B 3

/* BT_COEXIST_PROTOCOL_xx values for coexistence protocol,
   field sdk_phy_info_t.bt_coexist_protocol
 */
#define BT_COEXIST_PROTOCOL_WIFI_ONLY 0
#define BT_COEXIST_PROTOCOL_BT_ONLY 1

/* Coexistence is enabled, Bluetooth has its own antenna */
#define BT_COEXIST_PROTOCOL_FLAG_SEPARATE_ANT 2
/* Coexistence is enabled, Bluetooth shares WiFi antenna */
#define BT_COEXIST_PROTOCOL_FLAG_SHARE_ANT 4

/* Coexistence is enabled, use only BT_ACTIVE signal */
#define BT_COEXIST_PROTOCOL_FLAG_BT_ACTIVE_ONLY 0
/* Coexistence is enabled, use both BT_ACTIVE and BT_PRIORITY signals */
#define BT_COEXIST_PROTOCOL_FLAG_BT_ACTIVE_PRIORITY 1

/* DUAL_ANT_CONFIG_xx values for dual antenna config,
   field sdk_phy_info_t.dual_ant_config

   (Not really clear how this feature works, if at all.)
*/
#define DUAL_ANT_CONFIG_NONE 0
/* antenna diversity for WiFi, use GPIO0 + U0RXD (?) */
#define DUAL_ANT_CONFIG_DUAL 1
/* TX/RX switch for external PA & LNA: GPIO 0 high, GPIO 3 low during TX */
#define DUAL_ANT_CONFIG_TX_GPIO0_HIGH_GPIO3_LOW
/* TX/RX switch for external PA & LNA: GPIO 0 low, GPIO 3 high during TX */
#define DUAL_ANT_CONFIG_TX_GPIO0_LOW_GPIO3_HIGH


/* CRYSTAL_SLEEP_xx values used for sdk_phy_info_t.crystal_sleep
 */
#define CRYSTAL_SLEEP_OFF 0
#define CRYSTAL_SLEEP_ON 1
#define CRYSTAL_SLEEP_GPIO16 2
#define CRYSTAL_SLEEP_GPIO2 3

/* RF Stage 0 & 1 attenuation constants. Use for sdk_phy_info_t.lp_atten_stage01

   @note These values have been tested and are confirmed to work as
   expected by measuring RSSI w/ rt73 USB adapter in monitor mode
   (some values also checked on spectrum analyzer) - provided
   low_power_en is set then the signal is attenuated as per this
   setting.

   (It may look like LP_ATTEN_STAGE01_11_5DB is out of order, but
   according to monitor mode captures this is the correct ordering of
   these constants.)

   Setting the numeric values in between these constants appears to
   also attenuate the signal, but not necessarily by the amount you'd
   expect.
*/
#define LP_ATTEN_STAGE01_0DB    0x0f /*     0dB */
#define LP_ATTEN_STAGE01_2_5DB  0x0e /*  -2.5dB */
#define LP_ATTEN_STAGE01_6DB    0x0d /*    -6dB */
#define LP_ATTEN_STAGE01_8_5DB  0x09 /*  -8.5dB */
#define LP_ATTEN_STAGE01_11_5DB 0x0c /* -11.5dB */
#define LP_ATTEN_STAGE01_14DB   0x08 /*   -14dB */
#define LP_ATTEN_STAGE01_17_5DB 0x04 /* -17.5dB */
#define LP_ATTEN_STAGE01_23DB   0x00 /*   -23dB */

/* Constant for sdk_phy_info_t.pa_vdd */
#define PA_VDD_MEASURE_VCC 0xFF

/* Bitmask flags for sdk_phy_info_t.freq_correct_mode */

/* Set this flag to disable frequency offset correction */
#define FREQ_CORRECT_DISABLE 0

/* Set this flag to enable frequency offset correction */
#define FREQ_CORRECT_ENABLE BIT(0)

/* Set = Baseband PLL frequency is 160MHz (can only apply +ve offset)
 * Unset = Baseband PLL frequency is 168MHz (can apply +ve/-ve offset */
#define FREQ_CORRECT_BB_160M BIT(1)

/* Set = use force_freq_offset field to correct, Unset = automatically
   measure & correct offset
*/
#define FREQ_CORRECT_FORCE BIT(2)


/* RF_CAL_MODE_xx fields used for sdk_phy_info_t.rf_cal_mode
 */
/* Use saved RF CAL data from flash, only. RF init takes 2ms. */
#define RF_CAL_MODE_SAVED 0
/* Calibrate TX power control only, use saved RF CAL data for others.
   RF init takes 20ms. */
#define RF_CAL_MODE_TXPOWER_ONLY 1
/* Unclear if/how this mode is different to 2? */
#define RF_CAL_MODE_SAVED_2 2
/* Run full RF CAL routine. RF init takes approx 200ms. */
#define RF_CAL_MODE_FULL 3

/* Data structure that maps to the phy_info configuration block */
typedef struct __attribute__((packed)) {
    uint8_t _reserved00[0x05]; /* 0x00 - 0x04 */

    /* This "version" field was set to 5 in the SDK phy_info,
       and the original SDK startup code checks it is 5 and then loads
       default PHY configuration otherwise.

       esp-open-rtos will load phy_info from get_default_phy_info() if
       the value stored in flash has a different value to the value
       returned in get_default_phy_info(). This means you can
       increment the version return by get_default_phy_info() (to any
       value but 0xFF), and know that the new defaults will replace
       any older stored values.

       @notes It's not clear whether this is actually a version field
       (the other 24 bytes here have equally arbitrary numbers in
       them.) Changing the "version" to other values does not seem to
       effect WiFi performance at all, neither does zeroing out the
       first 5 reserved bytes in _reserved00. However zeroing bytes in
       the _reserved06 region will break WiFi entirely.
    */
    uint8_t version;          /* 0x05 */
    int8_t _reserved06[0x14]; /* 0x06 - 0x19 */

    /* spur_freq = spur_freq_primary / spur_freq_divisor */
    uint8_t spur_freq_primary; /* 0x1a */
    uint8_t spur_freq_divisor; /* 0x1b */

    /* Bitmask to enable spur_freq for each channel
       Appears to be a big endian short word?
     */
    uint8_t spur_freq_en_h;      /* 0x1c */
    uint8_t spur_freq_en_l;      /* 0x1d */

    uint8_t _reserved1e[4];   /* 0x1e - 0x21 */

    /* Each value is a target power level.
       Units are 1/4 dBm ie value 64 = 16dBm.

       SDK defaults to using these transmit powers:
       20.5dBm, 19.5dBm, 18.5dBm, 17dBm, 16dBm, 14dBm

       @note Adjusting these values is confirmed to reduce
       transmit power accordingly.
    */
    uint8_t target_power[6]; /* 0x22 - 0x27 */

    /* Maps 8 MCS (modulation & coding schemes) types for 802.11b, g &
     * n to a target_power level index (0-5), set above.

     This mapping of MCS slot to MCS type is derived from the
     spreadsheet and also a table sent by Espressif, but is untested
     and may be SDK version dependendent (especially any 802.11n
     rates). However the general relationship is confirmed to hold
     (higher MCS index = higher bit rate).

     MCS 0: 1Mbps/2Mbps/5.5Mbps/11Mbps (802.11b) / 6Mbps/9Mbps (802.11g)
         default target_power 0 (default 20.5dBm)
         (see also pwr_ind_11b_en)

     MCS 1: 12Mbps (802.11g)
         default target_power 0 (default 20.5dBm)

     MCS 2: 18Mbps (802.11g)
         default target_power 1 (19.5dBm)

     MCS 3: 24Mbps (802.11g)
         default target_power 1 (19.5dBm)

     MCS 4: 36Mbps (802.11g)
         default target_power 2 (18.5dBm)

     MCS 5: 48Mbps (802.11g)
         default target_power 3 (17dBm)

     MCS 6: 54Mbps (802.11g)
         default target_power 4 (16dBm)

     MCS 7: 65Mbps (802.11n) - unclear if ever used?
         default target_power 5 (14dBm)
    */
    uint8_t target_power_index_mcs[8]; /* 0x28 - 0x2f */

    /* One of CRYSTAL_FREQ_40M / CRYSTAL_FREQ_26M / CRYSTAL_FREQ_24M

       The crystal configured here is the input to the PLL setting
       calculations which are used to derive the CPU & APB peripheral
       clock frequency, and probably the WiFi PLLs (unconfirmed.)
     */
    uint8_t crystal_freq; /* 0x30 */

    uint8_t _unused31; /* 0x31: Possibly high byte of crystal freq? */

    /* One of SDIO_CONFIG_AUTO, SDIO_CONFIG_SDIOV1_1, SDIO_CONFIG_SDIOV2_0 */
    uint8_t sdio_config; /* 0x32 */

    /* BT coexistence pin configuration.

       One of BT_COEXIST_CONFIG_NONE, BT_COEXIST_CONFIG_A,
       BT_COEXIST_CONFIG_PRESENT, BT_COEXIST_CONFIG_B
    */
    uint8_t bt_coexist_config; /* 0x33 */

    /* BT coexistence pin protocol.

       If no coexistence:
       Either BT_COEXIST_PROTOCOL_WIFI_ONLY, or
       BT_COEXIST_PROTOCOL_BT_ONLY.

       If coexistence:
       Combine one of
       BT_COEXIST_PROTOCOL_FLAG_SEPARATE_ANT or
       BT_COEXIST_PROTOCOL_FLAG_SHARE_ANT
       with one of
       BT_COEXIST_PROTOCOL_FLAG_BT_ACTIVE_ONLY or
       BT_COEXIST_PROTOCOL_FLAG_BT_ACTIVE_BT_PRIORITY
    */
    uint8_t bt_coexist_protocol; /* 0x34 */

    /* Dual antenna configuration

       One of DUAL_ANT_CONFIG_NONE, DUAL_ANT_CONFIG_DUAL,
       DUAL_ANT_CONFIG_TX_GPIO0_HIGH_GPIO3_LOW,
       DUAL_ANT_CONFIG_TX_GPIO0_LOW_GPIO3_HIGH
    */
    uint8_t dual_ant_config; /* 0x35 */

    uint8_t _reserved34; /* 0x36 */

    /* For sharing crystal clock with other devices:
       one of CRYSTAL_SLEEP_OFF, CRYSTAL_SLEEP_ON,
       CRYSTAL_SLEEP_GPIO16, CRYSTAL_SLEEP_GPIO2
    */
    uint8_t crystal_sleep; /* 0x37 */

    uint8_t _unused38[8];

    /* spur_freq_2 = spur_freq_2_primary / spur_freq_2_divisor */
    uint8_t spur_freq_2_primary; /* 0x40 */
    uint8_t spur_freq_2_divisor; /* 0x41 */

    /* Bitmask to enable spur_freq_2 for each channel?
       Appears to be a big endian short word?
    */
    uint8_t spur_freq_2_en_h; /* 0x42 */
    uint8_t spur_freq_2_en_l; /* 0x43 */

    /* Not really clear what these do */
    uint8_t spur_freq_cfg_msb; /* 0x44 */
    uint8_t spur_freq_2_cfg_msb; /* 0x45 */
    uint16_t spur_freq_3_cfg; /* 0x46 - 0x47 */
    uint16_t spur_freq_4_cfg; /* 0x48 - 0x49 */

    uint8_t _reserved4a[4]; /* 0x4a - 0x4d */

    uint8_t _unused78[15]; /* 0x4e - 0x5c */

    /* Flag to enable low power mode */
    uint8_t low_power_en; /* 0x5d */

    /* Low Power attenuation of RF gain stages 0 & 1

       Attenuates transmit power if/when low_power_en is set.

       Use one of the constants LP_ATTEN_STAGE01_0DB,
       LP_ATTEN_STAGE01_2_5DB, LP_ATTEN_STAGE01_6DB,
       LP_ATTEN_STAGE01_8_5DB, LP_ATTEN_STAGE01_11_5DB,
       LP_ATTEN_STAGE01_14DB, LP_ATTEN_STAGE01_17_5DB,
       LP_ATTEN_STAGE01_23DB.
     */
    uint8_t lp_atten_stage01; /* 0x5e */

    /* Low Power(?) attenuation of baseband gain

       Units are minus 1/4 dB, ie value 4 == -1dB.

       Maximum value is 24 (0x18) == -6dB
     */
    uint8_t lp_atten_bb; /* 0x5f */

    /* I believe this means, when pwr_ind_11b_en == 0 then the 802.11g
       MCS 0 level from target_power_index_mcs are used to
       determine 802.11b transmit power level.

       However, when pwr_ind_11b_en == 1 then the index values in
       pwr_ind_11b_0 & pwr_ind_11b_1 are used for 802.11b instead.

       This is all unconfirmed, if you can confirm then please update
       this comment.
     */
    uint8_t pwr_ind_11b_en; /* 0x60 */

    /* 802.11b low data rate power index (0~5).
       Sets the power level index for operation at 1 & 2Mbps
    */
    uint8_t pwr_ind_11b_0; /* 0x61 */

    /* 802.11b high data rate power index (0~5)
       Sets the power level index for operation at 5.5 & 11Mbps
    */
    uint8_t pwr_ind_11b_1; /* 0x62 */

    uint8_t _unused63[8]; /* 0x63 - 0x6a */

    /* Set the voltage of PA_VDD, which appears to be an internal analog
       reference voltage(?)

       This field is called vdd33_const in the Arduino phy fields,
       and relates to usage of the TOUT pin (ADC pin).

       Set to PA_VDD_MEASURE_VCC (0xFF) and leave TOUT (ADC) pin
       floating in order to use the ADC to measure the 3.3V input
       voltage.

       Set to value in the range 18-36 (1.8V to 3.6V) to set a
       reference voltage(?) when using TOUT pin as an ADC input. I
       think this is the reference voltage used to scale the 0-1V
       which is allowed on the pin, in order to get an accurate
       reading. So it should be set to a value that matches system
       VCC... I think!
    */
    uint8_t pa_vdd; /* 0x6b */

    /* Disable RF calibration cycle for this many times */
    uint8_t disable_rfcal_count; /* 0x6c */

    uint8_t _unused6d[3];

    /* Flags for frequency correction

       A bitmask combination of any of: FREQ_CORRECT_DISABLE,
       FREQ_CORRECT_ENABLE, FREQ_CORRECT_BB_160M, FREQ_CORRECT_FORCE
     */
    uint8_t freq_correct_mode; /* 0x70 */

    /* Force frequency offset adjustment (instead of auto measuring)
       units are 1 = 8kHz, full range +/- 1016kHz.

       Only used if FREQ_CORRECT_ENABLE and FREQ_CORRECT_FORCE are
       set in freq_correct_mode.

       Unclear whether setting FREQ_CORRECT_BB_160M (which allows only positive offsets) changes the usable range.
    */
    int8_t force_freq_offset; /* 0x71 */

    /* Use stored data in flash for RF calibration.

       This field was previously called rf_cal_use_flash.

       Acceptable values one of RF_CAL_MODE_SAVED, RF_CAL_MODE_TXPOWER_ONLY, RF_CAL_MODE_SAVED_2, RF_CAL_MODE_FULL.
     */
    uint8_t rf_cal_mode; /* 0x72 */

    uint8_t _unused73[13];
} sdk_phy_info_t;

/* Some sanity check static assertions. These can probably be
   removed after this structure has been better tested.
*/
_Static_assert(sizeof(sdk_phy_info_t) == 128, "sdk_phy_info_t is wrong size!");
_Static_assert(offsetof(sdk_phy_info_t, version) == 5, "version at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, target_power) == 34, "target_power_qdb at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, bt_coexist_protocol) == 52, "bt_coexist_protocol at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, spur_freq_2_primary) == 64, "spur_freq_2_primary at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, lp_atten_stage01) == 94, "lp_atten_stage01 at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, pa_vdd) == 107, "pa_vdd aka vdd33_const at wrong offset");
_Static_assert(offsetof(sdk_phy_info_t, rf_cal_mode) == 114, "rf_cal_use_flash at wrong offset!");

/* Read the default PHY info into the supplied structure.

   This function is weak-aliased to get_sdk_default_phy_info() so you
   can replace it with your own if you want to vary the default values
   - suggested way to do this is to call get_sdk_default_phy_info()
   and then only update the fields you care about.

   The default PHY info is used at startup whenever the version field
   in the default sdk_phy_info_t does not match the version field
   stored in flash. So you can increment the version field to force a
   reset to defaults, regardless of what values are in flash.
*/
void get_default_phy_info(sdk_phy_info_t *info);

/* Read the "SDK default" PHY info as used by the Espressif SDK */
void get_sdk_default_phy_info(sdk_phy_info_t *info);

/* Read the PHY info currently stored in the SPI flash SDK configuration sector.

   This PHY info is updated by the SDK following RF calibration, etc.

   Note that the saved data may be corrupt - read the 'version' field to verify.
*/
void read_saved_phy_info(sdk_phy_info_t *info);

/* Update the saved PHY info in the SPI flash. A reset is necessary to use these values.

   Note that the SDK may clobber these values, so it's recommended you reset ASAP after updating them.
*/
void write_saved_phy_info(const sdk_phy_info_t *info);

/* Dump known fields in the phy info structure to stdout,
   if 'raw' flag is set then the raw hex values are also dumped.
*/
void dump_phy_info(const sdk_phy_info_t *info, bool raw);

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* PHY_INFO_H */
