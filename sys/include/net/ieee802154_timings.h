/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     net_ieee802154
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 MAC/PHY timing calculations
 *
 * @author       Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 */

#include <stdint.h>
#include "compiler_hints.h"

#include "net/ieee802154.h"
#include "net/ieee802154/radio.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint16_t _mr_oqpsk_symbol_duration_us(uint8_t chips)
{
    /* 802.15.4g, Table 183 / Table 165 */
    switch (chips) {
    case IEEE802154_MR_OQPSK_CHIPS_100:
        return 320;
    case IEEE802154_MR_OQPSK_CHIPS_200:
        return 160;
    case IEEE802154_MR_OQPSK_CHIPS_1000:
    case IEEE802154_MR_OQPSK_CHIPS_2000:
    default:
        return 64;
    }
}

/**
 * @brief   Get the symbol duration for the current PHY configuration.
 *          (according to 2024 Standard)
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return symbol duration in microseconds.
 */
static inline uint16_t ieee802154_get_symbol_duration(const ieee802154_phy_conf_t *conf)
{
    switch (conf->phy_mode) {
        case IEEE802154_PHY_OQPSK:
            /* 868 MHz (channel 0): 25 ksymbol/s
             * 780/915/2380/2450 MHz: 62.5 ksymbol/s */
            return (conf->channel == 0) ? 40 : 16;

        case IEEE802154_PHY_BPSK:
            /* 868 MHz (channel 0): 20 ksymbol/s
             * 915 MHz (channels 1-10): 40 ksymbol/s */
            return (conf->channel == 0) ? 50 : 25;

        case IEEE802154_PHY_MR_FSK:
            return IEEE802154_MR_FSK_SYMBOL_TIME_US;

        case IEEE802154_PHY_MR_OFDM:
            return IEEE802154_MR_OFDM_SYMBOL_TIME_US;
        case IEEE802154_PHY_MR_OQPSK:
#if IS_USED(IEEE802154_PHY_MR_OQPSK)
        const ieee802154_mr_oqpsk_conf_t *mr_oqpsk_conf = conf;
        return _mr_oqpsk_symbol_duration_us(mr_oqpsk_conf->chips);
        break;
#endif
        default:
            /* other PHYs not supported yet */
            assert(0);
            return 16;
    }
}

/**
 * @brief   Get the _phySHRDuration_ PHY constant value in microseconds.
 *          (according to 2024 Standard)
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return constant value in microseconds.
 */
static inline uint32_t ieee802154_get_shr_duration(const ieee802154_phy_conf_t *conf)
{
    uint32_t sym_dur = ieee802154_get_symbol_duration(conf);

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
            /* 14.1: preamble 32 symbols (4 octets),
             * 13.1.2.3: SFD 1 octet -> 8 symbols (1 bit per symbol) */
            return (32 + 8) * sym_dur;
        case IEEE802154_PHY_OQPSK:
            /* 13.1.2.2: preamble 8 symbols (4 octets),
             * 13.1.2.3: SFD 1 octet -> 2 symbols (4 bits per symbol) */
            return (8 + 2) * sym_dur;
        default:
            /* other PHYs not supported yet */
            assert(0);
            return 0;
    }
}

/**
 * @brief   Calculate the PHY PSDU duration value in microseconds.
 *          (according to 2024 Standard)
 *
 * @param[in] conf pointer to the config descriptor
 * @param[in] length PSDU length in bytes
 *
 * @return PSDU duration in microseconds.
 */
static inline uint32_t ieee802154_get_psdu_duration(const ieee802154_phy_conf_t *conf,
                                                    uint16_t length)
{
    uint32_t sym_dur = ieee802154_get_symbol_duration(conf);

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
            /* 1 bit per symbol -> 8 symbols per octet */
            return sym_dur * length * 8;
        case IEEE802154_PHY_OQPSK:
            /* 4 bits per symbol -> 2 symbols per octet */
            return sym_dur * length * 2;
        default:
            /* other PHYs not supported yet */
            assert(0);
            return 0;
    }
}

/**
 * @brief   Get the _aTurnaroundTime_ PHY constant value in microseconds.
 *          (according to 2024 Standard)
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return constant value in microseconds.
 */
static inline uint32_t ieee802154_get_turnaround_time(const ieee802154_phy_conf_t *conf)
{
    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
        case IEEE802154_PHY_OQPSK:
            /* Table 12-1: 12 symbol periods */
            return IEEE802154_ATURNAROUNDTIME_IN_SYMBOLS * ieee802154_get_symbol_duration(conf);
        case IEEE802154_PHY_MR_FSK:
        case IEEE802154_PHY_MR_OFDM:
        case IEEE802154_PHY_MR_OQPSK:
            return IEEE802154G_ATURNAROUNDTIME_US;
        default:
            /* other PHYs not supported yet */
            assert(0);
            return IEEE802154G_ATURNAROUNDTIME_US;
    }
}

static inline uint8_t _mr_oqpsk_cca_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 188 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 4 : 8;
}

/**
 * @brief   Get the _phyCcaDuration_ value in microseconds.
 *          (according to 2024 Standard)
 *
 * @param[in] conf pointer to the config descriptor
 *
 * @return CCA duration in microseconds.
 */
static inline uint32_t ieee802154_get_cca_time(const ieee802154_phy_conf_t *conf)
{
    uint32_t cca_duration_symbol = 0;

    switch (conf->phy_mode) {
    case IEEE802154_PHY_BPSK:
    case IEEE802154_PHY_OQPSK:
    case IEEE802154_PHY_MR_FSK:
    case IEEE802154_PHY_MR_OFDM:
        /* Table 12-2: 8 symbol periods if not specified by the PHY clause */
        cca_duration_symbol = IEEE802154_CCA_DURATION_IN_SYMBOLS;
        break;
    case IEEE802154_PHY_MR_OQPSK:
#if IS_USED(IEEE802154_PHY_MR_OQPSK)
        const ieee802154_mr_oqpsk_conf_t *mr_oqpsk_conf = conf;
        cca_duration_symbol =_mr_oqpsk_cca_duration_syms(mr_oqpsk_conf->chips);
#endif
        break;
    default:
        /* other PHYs not supported yet */
        assert(0);
        return 0;
    }
    return cca_duration_symbol * ieee802154_get_symbol_duration(conf);
}

static inline uint32_t ieee802154_calculate_unit_backoff_period(const ieee802154_phy_conf_t *conf)
{
    return ieee802154_get_turnaround_time(conf)
         + ieee802154_get_cca_time(conf);
}

static inline uint32_t ieee802154_calculate_ack_wait_duration(const ieee802154_phy_conf_t *conf)
{
    return ieee802154_calculate_unit_backoff_period(conf)
         + ieee802154_get_turnaround_time(conf)
         + ieee802154_get_shr_duration(conf)
         /* ack psdu with phr included */
         + ieee802154_get_psdu_duration(conf, 1 + IEEE802154_ACK_FRAME_LEN);
}

/*
 * MR-OQPSK timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */
static inline uint8_t _mr_oqpsk_spreading(uint8_t chips, uint8_t mode)
{
    if (mode == 4) {
        return 1;
    }

    uint8_t spread = 1 << (3 - mode);

    if (chips == IEEE802154_MR_OQPSK_CHIPS_1000) {
        return 2 * spread;
    }

    if (chips == IEEE802154_MR_OQPSK_CHIPS_2000) {
        return 4 * spread;
    }

    return spread;
}

static inline uint8_t _mr_oqpsk_shr_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 184 / Table 165 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 48 : 72;
}

static inline uint8_t _mr_oqpsk_ack_psdu_duration_syms(uint8_t chips, uint8_t mode)
{
    /* pg. 119, section 18.3.2.14 */
    static const uint8_t sym_len[] = { 32, 32, 64, 128 };
    const uint8_t Ns = sym_len[chips];
    const uint8_t Rspread = _mr_oqpsk_spreading(chips, mode);
    /* Nd == 63, since ACK length is 5 or 7 octets only */
    const uint16_t Npsdu = Rspread * 2 * 63;

    /* phyPSDUDuration = ceiling(Npsdu / Ns) + ceiling(Npsdu / Mp) */
    /* with Mp = Np * 16, see Table 182 */
    return (Npsdu + Ns/2) / Ns + (Npsdu + 8 * Ns) / (16 * Ns);
}

MAYBE_UNUSED
static inline uint16_t _mr_oqpsk_ack_timeout_us(const ieee802154_mr_oqpsk_conf_t *conf)
{
    /* see 802.15.4g-2012, p. 30 */
    uint16_t symbols = _mr_oqpsk_cca_duration_syms(conf->chips)
                     + _mr_oqpsk_shr_duration_syms(conf->chips)
                     + 15   /* PHR duration */
                     + _mr_oqpsk_ack_psdu_duration_syms(conf->chips, conf->rate_mode);

    return _mr_oqpsk_symbol_duration_us(conf->chips) * symbols
         + IEEE802154G_ATURNAROUNDTIME_US;
}

/*
 * MR-OFDM timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */
static inline unsigned _mr_ofdm_frame_duration(uint8_t option, uint8_t scheme, uint8_t bytes)
{
    /* Table 150 - phySymbolsPerOctet values for MR-OFDM PHY, IEEE 802.15.4g-2012 */
    static const uint8_t quot[] = { 3, 3, 6, 12, 18, 24, 36 };

    --option;
    /* phyMaxFrameDuration = phySHRDuration + phyPHRDuration
     *                     + ceiling [(aMaxPHYPacketSize + 1) x phySymbolsPerOctet] */
    const unsigned phySHRDuration = 6;
    const unsigned phyPHRDuration = option ? 6 : 3;
    const unsigned phyPDUDuration = ((bytes + 1) * (1 << option) + quot[scheme] - 1)
                                  / quot[scheme];

    return (phySHRDuration + phyPHRDuration + phyPDUDuration) * IEEE802154_MR_OFDM_SYMBOL_TIME_US;
}

MAYBE_UNUSED
static inline uint16_t _mr_ofdm_ack_timeout_us(const ieee802154_mr_ofdm_conf_t *conf)
{
    return ieee802154_calculate_unit_backoff_period((void *) conf);
         + IEEE802154G_ATURNAROUNDTIME_US
         + _mr_ofdm_frame_duration(conf->option, conf->scheme, IEEE802154_ACK_FRAME_LEN);
}

MAYBE_UNUSED
static inline uint16_t _mr_fsk_ack_timeout_us(const ieee802154_mr_fsk_conf_t *conf)
{
    uint8_t ack_len = IEEE802154_ACK_FRAME_LEN;
    uint8_t fsk_pl = ieee802154_mr_fsk_plen(conf->srate);

    /* PHR uses same data rate as PSDU */
    ack_len += 2;

    /* 4-FSK doubles data rate */
    if (conf->mod_ord == 4) {
        ack_len /= 2;
    }

    /* forward error correction halves data rate */
    if (conf->fec) {
        ack_len *= 2;
    }

    return ieee802154_calculate_unit_backoff_period((void *) conf);
         + IEEE802154G_ATURNAROUNDTIME_US
         /* long Preamble + SFD; SFD=2 */
         + ((fsk_pl * 8 + 2) + ack_len) * 8 * IEEE802154_MR_FSK_SYMBOL_TIME_US;
}

#ifdef __cplusplus
}
#endif

/** @} */
