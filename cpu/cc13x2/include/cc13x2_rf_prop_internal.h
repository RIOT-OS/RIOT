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
 * @brief   (IEEE 802.15.4-2006) PSDU.FCF.frameType
 */
#define IEEE802154_FRAME_TYPE_MASK 0x7

/**
 * @brief   (IEEE 802.15.4-2006) frame type: ACK
 */
#define IEEE802154_FRAME_TYPE_ACK 0x2

/**
 * @brief   (IEEE 802.15.4-2006) PSDU.FCF.bAR
 */
#define IEEE802154_ACK_REQUEST (1 << 5)

/**
 * @brief   (IEEE 802.15.4-2006) PSDU.sequenceNumber
 */
#define IEEE802154_DSN_OFFSET 2

/**
 * @brief   (IEEE 802.15.4-2006) macMinBE
 */
#define IEEE802154_MAC_MIN_BE 1

/**
 * @brief   (IEEE 802.15.4-2006) macMaxBE
 */
#define IEEE802154_MAC_MAX_BE 5

/**
 * @brief   (IEEE 802.15.4-2006) macMaxCSMABackoffs
 */
#define IEEE802154_MAC_MAX_CSMA_BACKOFFS 4

/**
 * @brief   (IEEE 802.15.4-2006) macMaxFrameRetries
 */
#define IEEE802154_MAC_MAX_FRAMES_RETRIES 3

/**
 * @brief   (IEEE 802.15.4-2006 7.4.1) MAC constants
 */
#define IEEE802154_A_UINT_BACKOFF_PERIOD 20

/**
 * @brief   (IEEE 802.15.4-2006 6.4.1) PHY constants
 */
#define IEEE802154_A_TURNAROUND_TIME 12

/**
 * @brief   (IEEE 802.15.4-2006 6.4.2) PHY PIB attribute, specifically the
 *          O-QPSK PHY
 */
#define IEEE802154_PHY_SHR_DURATION 10

/**
 * @brief   (IEEE 802.15.4-2006 6.4.2) PHY PIB attribute, specifically the
 *          O-QPSK PHY
 */
#define IEEE802154_PHY_SYMBOLS_PER_OCTET 2

/**
 * @brief   (IEEE 802.15.4-2006 7.4.2) macAckWaitDuration PIB attribute
 */
#define IEEE802154_MAC_ACK_WAIT_DURATION \
    (IEEE802154_A_UINT_BACKOFF_PERIOD +  \
     IEEE802154_A_TURNAROUND_TIME +      \
     IEEE802154_PHY_SHR_DURATION +       \
     (6 * IEEE802154_PHY_SYMBOLS_PER_OCTET))

/**
 * @brief   (IEEE 802.15.4-2006 6.5.3.2) O-QPSK symbol rate
 */
#define IEEE802154_SYMBOLS_PER_SEC 62500

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

/**
 * @brief This enum represents the state of a radio.
 *
 * Initially, a radio is in the Disabled state.
 *
 * The following are valid radio state transitions for the cc13x2:
 *
 *                                    (Radio ON)
 *  +----------+  Enable()  +-------+  Receive()   +---------+   Transmit()   +----------+
 *  |          |----------->|       |------------->|         |--------------->|          |
 *  | Disabled |            | Sleep |              | Receive |                | Transmit |
 *  |          |<-----------|       |<-------------|         |<---------------|          |
 *  +----------+  Disable() |       |   Sleep()    |         | AckFrame RX or +----------+
 *                          |       | (Radio OFF)  +---------+ sTxCmdChainDone == true
 *                          +-------+
 *
 * | state            | description                                        |
 * |------------------|----------------------------------------------------|
 * | Disabled         | The rfcore powerdomain is off and the RFCPE is off |
 * | Sleep            | The RFCORE PD is on, and the RFCPE is in PROP mode |
 * | Receive          | The RFCPE is running a CMD_PROP_RX_ADV             |
 * | Transmit         | The RFCPE is running a transmit command string     |
 * | TransmitComplete | The transmit command string has completed          |
 */
typedef enum cc13x2_PhyState
{
    cc13x2_stateDisabled = 0,
    cc13x2_stateSleep,
    cc13x2_stateReceive,
    cc13x2_stateTransmit,
} cc13x2_PhyState_t;

void cc13x2_rf_init(void);

int_fast8_t cc13x2_rf_enable(void);
int_fast8_t cc13x2_rf_disable(void);

int8_t cc13x2_rf_get_txpower(void);
int_fast8_t cc13x2_rf_set_txpower(int8_t aPower);

int8_t cc13x2_rf_get_rssi(void);
int_fast8_t cc13x2_rf_rx_start(void);
int_fast8_t cc13x2_rf_rx_stop(void);
uint8_t cc13x2_rf_get_chan(void);
void cc13x2_rf_set_chan(uint16_t channel);
void cc13x2_rf_get_ieee_eui64(uint8_t *aIeeeEui64);
void cc13x2_rf_irq_set_handler(void(*handler)(void *), void * arg);
int cc13x2_rf_recv(void *buf, size_t len, netdev_ieee802154_rx_info_t *rx_info);
int cc13x2_rf_recv_avail(void);
int cc13x2_rf_send(const iolist_t *iolist);
void cc13x2_rf_set_pan(uint16_t aPanid);
uint16_t cc13x2_rf_get_pan(void);

unsigned cc13x2_rf_irq_is_enabled(unsigned irq);
void cc13x2_rf_irq_enable(unsigned irq);
void cc13x2_rf_irq_disable(unsigned irq);
unsigned cc13x2_rf_get_flags(void);

extern volatile cc13x2_PhyState_t _cc13x2_rf_state;

#endif /* CC13X2_RF_PROP_INTERNAL_H */
/** @} */
