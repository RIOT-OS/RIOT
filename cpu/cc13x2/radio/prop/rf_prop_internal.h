/*
 *  Copyright (c) 2018, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc13x2
 * @{
 *
 * @file
 * @brief           CC13x2 IEEE 802.15.4 netdev driver
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC13X2_RF_PROP_INTERNAL_H
#define CC13X2_RF_PROP_INTERNAL_H

#include <stdbool.h>

#include "iolist.h"
#include "net/netdev.h"
#include "net/ieee802154.h"
#include "net/netdev/ieee802154.h"

/**
 * @brief   If the EUI64 read from the ccfg is all ones then the customer did
 *          not set the address
 */
#define CCFG_UNKNOWN_EUI64 0xFF

/**
 * @brief   Creates a TX power entry for the default PA.
 *
 * The values for @a bias, @a gain, @a boost and @a coefficient are usually
 * measured by Texas Instruments for a specific front-end configuration. They
 * can then be obtained from SmartRF Studio.
 */
#define DEFAULT_PA_ENTRY(bias, gain, boost, coefficient) \
        ((bias) << 0) | ((gain) << 6) | ((boost) << 8) | ((coefficient) << 9)

/**
 * TX Power dBm lookup table
 */
typedef struct output_config
{
    int      dbm;
    uint16_t value;
} output_config_t;

/**
 * @brief     TX Power dBm lookup table
 */
static const output_config_t rgOutputPower[] = {
  // The original PA value (12.5 dBm) has been rounded to an integer value.
  {13, DEFAULT_PA_ENTRY(36, 0, 0, 89) },
  {12, DEFAULT_PA_ENTRY(16, 0, 0, 82) },
  {11, DEFAULT_PA_ENTRY(26, 2, 0, 51) },
  {10, DEFAULT_PA_ENTRY(18, 2, 0, 31) },
  {9, DEFAULT_PA_ENTRY(28, 3, 0, 31) },
  {8, DEFAULT_PA_ENTRY(24, 3, 0, 22) },
  {7, DEFAULT_PA_ENTRY(20, 3, 0, 19) },
  {6, DEFAULT_PA_ENTRY(17, 3, 0, 16) },
  {5, DEFAULT_PA_ENTRY(14, 3, 0, 14) },
  {4, DEFAULT_PA_ENTRY(13, 3, 0, 11) },
  {3, DEFAULT_PA_ENTRY(11, 3, 0, 10) },
  {2, DEFAULT_PA_ENTRY(10, 3, 0, 9) },
  {1, DEFAULT_PA_ENTRY(9, 3, 0, 9) },
  {0, DEFAULT_PA_ENTRY(8, 3, 0, 8) },
  {-5, DEFAULT_PA_ENTRY(4, 3, 0, 5) },
  {-10, DEFAULT_PA_ENTRY(2, 3, 0, 5) },
  {-15, DEFAULT_PA_ENTRY(1, 3, 0, 3) },
  {-20, DEFAULT_PA_ENTRY(0, 3, 0, 2) },
};

#define OUTPUT_CONFIG_COUNT (sizeof(rgOutputPower) / sizeof(rgOutputPower[0]))

/* Max and Min Output Power in dBm */
#define OUTPUT_POWER_MIN (rgOutputPower[OUTPUT_CONFIG_COUNT - 1].dbm)
#define OUTPUT_POWER_MAX (rgOutputPower[0].dbm)
#define OUTPUT_POWER_UNKNOWN 0xFFFF

/**
 * number of short addresses used for source matching
 */
#define CC1352_SHORTADD_SRC_MATCH_NUM 10

#undef IEEE802154_DEFAULT_SUBGHZ_CHANNEL
#define IEEE802154_DEFAULT_SUBGHZ_CHANNEL 7

#if IEEE802154_DEFAULT_SUBGHZ_CHANNEL == 2 /* 470 MHz */
#define IEEE802154_CHAN_MIN           0
#define IEEE802154_CHAN_MAX           198
#define IEEE802154_FREQ_SPACING       200
#define IEEE802154_CHAN0_FREQ         470200

#define PROP_MODE_CENTER_FREQ   0x01EA
#define PROP_MODE_LO_DIVIDER    0x0A
#elif IEEE802154_DEFAULT_SUBGHZ_CHANNEL == 3 /* 780 MHz */
#define IEEE802154_CHAN_MIN           0
#define IEEE802154_CHAN_MAX           38
#define IEEE802154_FREQ_SPACING       200
#define IEEE802154_CHAN0_FREQ         779200

#define PROP_MODE_CENTER_FREQ   0x030F
#define PROP_MODE_LO_DIVIDER    0x06
#elif IEEE802154_DEFAULT_SUBGHZ_CHANNEL == 4 /* 863 MHz */
#define IEEE802154_CHAN_MIN           0
#define IEEE802154_CHAN_MAX           33
#define IEEE802154_FREQ_SPACING       200
#define IEEE802154_CHAN0_FREQ         863125

#define PROP_MODE_CENTER_FREQ   0x0362
#define PROP_MODE_LO_DIVIDER    0x05
#elif IEEE802154_DEFAULT_SUBGHZ_CHANNEL == 7 /* 915 MHz */
#define IEEE802154_CHAN_MIN           0
#define IEEE802154_CHAN_MAX           128
#define IEEE802154_FREQ_SPACING       200
#define IEEE802154_CHAN0_FREQ         902200

#define PROP_MODE_CENTER_FREQ   0x0393
#define PROP_MODE_LO_DIVIDER    0x05
#else
#error "Sub-GHz channel not supported"
#endif

static inline uint32_t ieee802154_freq(const uint16_t chan)
{
    const uint32_t chan0 = IEEE802154_CHAN0_FREQ;
    const uint32_t spacing = IEEE802154_FREQ_SPACING;
    const uint32_t chan_min = IEEE802154_CHAN_MIN;
    return chan0 + spacing * ((uint32_t)chan - chan_min);
}

/**
 * size of length field in receive struct
 *
 * defined in Table 23-10 of the cc26xx TRM
 */
#define DATA_ENTRY_LENSZ_BYTE 1

#endif /* CC13X2_RF_PROP_INTERNAL_H */
/** @} */
