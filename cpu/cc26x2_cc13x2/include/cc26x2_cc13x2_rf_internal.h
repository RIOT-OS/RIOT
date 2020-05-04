/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 IEEE 802.15.4g Sub-1-GHz driver internal
 *                  functions.
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RF_INTERNAL_H
#define CC26X2_CC13X2_RF_INTERNAL_H

#include <stdbool.h>

#include "net/eui64.h"
#include "iolist.h"
#include "byteorder.h"
#include "macros/units.h"

#include "cc26xx_cc13xx_rfc_common_cmd.h"
#include "cc26xx_cc13xx_rfc_prop_cmd.h"

#include "cc26xx_cc13xx_rfc_mailbox.h"
#include "cc26xx_cc13xx_rfc_prop_mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of receive buffers
 */
#ifndef CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF
#define CONFIG_CC26x2_CC13X2_RF_RX_BUF_NUMOF (4)
#endif

/* XXX: shouldn't this reside on net/ieee802154.h */
/**
 * @name    SUN PHY frequency band definitions
 *
 * These are the frequency band identifiers for the SUN PHY.
 *
 * @see IEEE Std 802.154.g-2012, Table 68f
 * @{
 */
#define IEEE802154_SUN_PHY_FREQ_169_MHZ     0   /**< 169.400-169.475 (Europe) */
#define IEEE802154_SUN_PHY_FREQ_450_MHZ     1   /**< 450-470 (US FCC Part 22/90) */
#define IEEE802154_SUN_PHY_FREQ_470_MHZ     2   /**< 470-510 (China) */
#define IEEE802154_SUN_PHY_FREQ_780_MHZ     3   /**< 779-787 (China) */
#define IEEE802154_SUN_PHY_FREQ_863_MHZ     4   /**< 863-870 (Europe) */
#define IEEE802154_SUN_PHY_FREQ_896_MHZ     5   /**< 896-901 (US FCC Part 90) */
#define IEEE802154_SUN_PHY_FREQ_901_MHZ     6   /**< 901-902 (US FCC Part 24) */
#define IEEE802154_SUN_PHY_FREQ_915_MHZ     7   /**< 902-928 (US) */
#define IEEE802154_SUN_PHY_FREQ_917_MHZ     8   /**< 917-923.5 (Korea) */
#define IEEE802154_SUN_PHY_FREQ_920_MHZ     9   /**< 920-928 (Japan) */
#define IEEE802154_SUN_PHY_FREQ_928_MHZ     10  /**< 928-960 (US, non-contiguous) */
#define IEEE802154_SUN_PHY_FREQ_950_MHZ     11  /**< 950-958 (Japan) */
#define IEEE802154_SUN_PHY_FREQ_1427_MHZ    12  /**< 1427-1518 (US and Canada, non-contiguous) */
#define IEEE802154_SUN_PHY_FREQ_2450_MHZ    13  /**< 2400-2483.5 */
/** @} */

/**
 * @brief   SUN PHY frequency used
 */
#ifndef CC26X2_CC13X2_RF_SUN_PHY_FREQ
#define CC26X2_CC13X2_RF_SUN_PHY_FREQ       IEEE802154_SUN_PHY_FREQ_915_MHZ
#endif

/* TODO: extended with the other frequencies, also teach the HAL about
 * SUN PHY frequencies? */
/**
 * @brief   SUN PHY channel information
 * @{
 */
#if CC26X2_CC13X2_RF_SUN_PHY_FREQ == IEEE802154_SUN_PHY_FREQ_915_MHZ

#define CC26X2_CC13X2_RF_CHAN_MIN           (0)
#define CC26X2_CC13X2_RF_CHAN_MAX           (128)
#define CC26X2_CC13X2_RF_CHAN_CENTER_FREQ0  KHZ(902200)
#define CC26X2_CC13X2_RF_CHAN_SPACING       KHZ(200)

#else
#error "915 MHZ supported only for now!"
#endif
/** @} */

/**
 * @brief   Length of the status length
 * The status section resides at the end of a received packet and
 * contains information such as the RSSI, sync word (SFD) that
 * was detected, etc
 *
 * @note    If this value is updated @ref rf_cmd_prop_rx_adv MUST
 *          be updated accordingly. For now it only includes the
 *          RSSI which is all we need.
 *
 *          The status doesn't include the LQI! The LQI of the last
 *          packet received MUST be readed from an RF Core register,
 *          However, it requires some shenanigans to correctly read
 *          it for the packet we want, e.g. reading it at the ISR
 *          for `CPE_IRQ_RX_ENTRY_DONE` and saving it elsewhere.
 */
#define CC26X2_CC13X2_RF_RX_STATUS_LEN 1

/**
 * @brief   Align the size of a buffer to a 4-byte boundary.
 *
 * For example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * static uint8_t buf[ALIGN_TO_4(2047)];
 *
 * assert(sizeof(buf) == 2048);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define ALIGN_TO_4(size) (((size) + 3) & ~3)

/**
 * @brief   Get center frequency of a channel
 *
 * @return Frequency, in Hz.
 */
static inline uint64_t cc26x2_cc13x2_rf_chan_freq(uint16_t chan)
{
    const uint64_t chan_center_freq0 = CC26X2_CC13X2_RF_CHAN_CENTER_FREQ0;
    const uint64_t spacing = CC26X2_CC13X2_RF_CHAN_SPACING;

    return chan_center_freq0 + (chan * spacing);
}

/**
 * @brief   Convert a frequency in Hz, to a frequency that the CMD_FS
 *          command can understand.
 *
 * @param[in]   new_freq    New frequency, in Hz.
 * @param[out]  freq        Frequency, in MHz.
 * @param[out]  fract       Fractional part of the frequency.
 */
static inline void cc26x2_cc13x2_rf_freq_convert(uint64_t new_freq,
                                                 uint16_t *freq,
                                                 uint16_t *fract)
{
    /* convert to MHz, discarding any decimal */
    *freq = (uint16_t)(new_freq / MHZ(1));

    /* convert it back to Hz again and get difference from new_freq, to get
     * the fractional part. Doing the calculation this way we can get a
     * value (freq * (fract / 0x10000))) MHz that is close or equal
     * to new_freq.
     *
     * See Section 25.3.3.1.5 CMD_FS: Frequency Synthesizer Controls Command
     * on TRM (swcu185d) */
    const uint64_t diff = new_freq - ((*freq) * MHZ(1));
    *fract = (uint16_t)((diff * 0x10000) / 1000000);
}

/**
 * @brief   CMD_SYNC_START_RAT
 *
 * Synchronously start the RAT
 */
extern rfc_cmd_sync_start_rat_t rf_cmd_sync_start_rat;

/**
 * @brief   CMD_PROP_RADIO_DIV_SETUP
 *
 * Proprietary Mode Radio Setup Command for All Frequency Bands
 */
extern rfc_cmd_prop_radio_div_setup_t rf_cmd_prop_radio_div_setup;

/**
 * @brief   CMD_FS
 *
 * Frequency Synthesizer
 */
extern rfc_cmd_fs_t rf_cmd_fs;

/**
 * @brief   CMD_PROP_TX_ADV
 *
 * Proprietary Mode Advanced Transmit Command
 */
extern rfc_cmd_prop_tx_adv_t rf_cmd_prop_tx_adv;

/**
 * @brief   CMD_PROP_RX_ADV
 *
 * Proprietary Mode Advanced Receive Command
 */
extern rfc_cmd_prop_rx_adv_t rf_cmd_prop_rx_adv;

/**
 * @brief   CMD_PROP_CS
 *
 * Proprietary Mode Carrier Sense Command
 */
extern rfc_cmd_prop_cs_t rf_cmd_prop_cs;

/**
 * @brief   CMD_CLEAR_RX
 *
 * Clear the RX queue
 */
extern rfc_cmd_clear_rx_t rf_cmd_clear_rx;

/**
 * @brief   CMD_SET_TX_POWER
 *
 * Set TX Power
 */
extern rfc_cmd_set_tx_power_t rf_cmd_set_tx_power;

/**
 * @brief   Initialize internal variables
 */
void cc26x2_cc13x2_rf_internal_init(void);

/**
 * @brief   Request to turn on the radio
 */
void cc26x2_cc13x2_rf_request_on(void);

/**
 * @brief   Confirm that the RF Core is on
 *
 * @pre cc26x2_cc13x2_rf_request_on MUST have been called before.
 *
 * @return -EAGAIN if the RF Core is still starting.
 * @return 0 if the RF Core is on.
 */
int cc26x2_cc13x2_rf_confirm_on(void);

/**
 * @brief   Request a CCA
 *
 * @note The CCA is confirmed when the interrupt
 *       `CPE_IRQ_LAST_COMMAND_DONE` is raised AND
 *       rf_cmd_prop_cs.status is either #RFC_PROP_DONE_IDLE
 *       or `RFC_PROP_DONE_BUSY`.
 *       `CPE_IRQ_LAST_COMMAND_DONE` MUST be enabled.
 *
 * @return 0 on success.
 * @return negative errno value on error.
 */
int cc26x2_cc13x2_rf_request_cca(void);

/**
 * @brief   Is RX on?
 *
 * @return true if RX is on, false otherwise.
 */
bool cc26x2_cc13x2_rf_rx_is_on(void);

/**
 * @brief   Start RX
 *
 * @pre @ref cc26x2_cc13x2_rf_rx_is_on MUST have returned true before.
 *
 * @return 0 on success.
 * @return negative errno value on error.
 */
int cc26x2_cc13x2_rf_rx_start(void);

/**
 * @brief   Stop RX
 *
 * @pre @ref cc26x2_cc13x2_rf_rx_is_on MUST have returned true before.
 */
void cc26x2_cc13x2_rf_rx_stop(void);

/**
 * @brief   Receive a packet from the packet queue.
 *
 * @pre An ISR should inform that a packet is ready in the
 * queue.
 *
 * @return NULL if no packet exists on the queue.
 * @return pointer to packet on success.
 */
rfc_data_entry_general_t *cc26x2_cc13x2_rf_recv(void);

/**
 * @brief   Write PDSU into transmit buffer
 *
 * @return 0 on success
 * @return negative errno value on error.
 */
int cc26x2_cc13x2_rf_write(const iolist_t *iolist);

/**
 * @brief   Request the RF Core to transmit our packet.
 *
 * @pre @ref cc26x2_cc13x2_rf_write MUST have been called.
 */
int cc26x2_cc13x2_rf_request_transmit(void);

/**
 * @brief   Get the PSDU length of the last written packet with
 *          @ref cc26x2_cc13x2_rf_write.
 *
 * @return PSDU length.
 */
size_t cc26x2_cc13x2_rf_tx_psdu_len(void);

/**
 * @brief   Set the CCA threshold for the radio.
 *
 * @param[in]   threshold   Threshold for CCA in dBm.
 */
void cc26x2_cc13x2_rf_set_cca_threshold(int8_t threshold);

/**
 * @brief   Change channel
 *
 * @param[in]   channel Channel to set.
 *
 * @return 0 on success.
 * @return negative errno value on error.
 */
int cc26x2_cc13x2_rf_change_chan(uint16_t channel);

/**
 * @brief   Set transmit power
 *
 * @param[in]   pow   Transmit power in dBm.
 *
 * @return 0 on success.
 * @return negative errno value on error.
 */
int cc26x2_cc13x2_rf_set_tx_pwr(int16_t pow);

/**
 * @brief   Get transmit power
 *
 * @return  Trasnmit power in dBm.
 */
int16_t cc26x2_cc13x2_rf_get_tx_pwr(void);

/**
 * @brief   Get HW address of the device.
 *
 * @return  EUI-64 address.
 */
eui64_t cc26x2_cc13x2_rf_get_hwaddr(void);

/**
 * @brief   Set PAN ID.
 *
 * @param[in]   pan_id The PAN ID.
 */
void cc26x2_cc13x2_rf_set_pan_id(uint16_t pan_id);

/**
 * @brief   Set short IEEE 802.154.g address.
 *
 * @param[in]   short_addr The short address.
 */
void cc26x2_cc13x2_rf_set_short_addr(network_uint16_t short_addr);

/**
 * @brief   Set long IEEE 802.15.4g MAC address.
 *
 * @param[in]   long_addr The long address.
 */
void cc26x2_cc13x2_rf_set_long_addr(eui64_t long_addr);

/**
 * @brief   Filter an L2 payload.
 *
 * @note @p mhr length should be enough to access the destination PAN ID and
 *       address.
 *
 * @brief[in]   mhr   MAC header.
 *
 * @return true if it passed the filter, false otherwise.
 */
bool cc26x2_cc13x2_rf_l2_filter(uint8_t *mhr);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_INTERNAL_H */

/*@}*/
