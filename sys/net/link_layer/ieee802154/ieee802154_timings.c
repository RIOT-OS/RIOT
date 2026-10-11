/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     net_ieee802154
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 MAC/PHY timing calculations
 *
 * @author       Stepan Konoplev <stepan.konoplev@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>

#include "modules.h"
#include "net/ieee802154_timings.h"

uint16_t ieee802154_get_symbol_duration(const ieee802154_phy_conf_t *conf)
{
    int res = 0;

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
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            return IEEE802154_MR_FSK_SYMBOL_TIME_US;
        case IEEE802154_PHY_MR_OFDM:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            return IEEE802154_MR_OFDM_SYMBOL_TIME_US;
        case IEEE802154_PHY_MR_OQPSK: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_oqpsk_conf_t *oqpsk = (const ieee802154_mr_oqpsk_conf_t *)conf;
            /* 802.15.4g, Table 183 / Table 165 */
            if (oqpsk->chips == IEEE802154_MR_OQPSK_CHIPS_100) {
                return 320;
            }
            if (oqpsk->chips == IEEE802154_MR_OQPSK_CHIPS_200) {
                return 160;
            }
            /* 1000 and 2000 kchip/s */
            return 64;
        }
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
    }

    return 0;
}

/* phySHRDuration in microseconds */
static uint32_t _get_shr_duration(const ieee802154_phy_conf_t *conf)
{
    uint32_t shr_symbols = 0;
    int res = 0;

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
            /* 14.1: preamble 32 symbols (4 octets),
             * 13.1.2.3: SFD 1 octet -> 8 symbols (1 bit per symbol) */
            shr_symbols = 32 + 8;
            break;
        case IEEE802154_PHY_OQPSK:
            /* 13.1.2.2: preamble 8 symbols (4 octets),
             * 13.1.2.3: SFD 1 octet -> 2 symbols (4 bits per symbol) */
            shr_symbols = 8 + 2;
            break;
        case IEEE802154_PHY_MR_FSK: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_fsk_conf_t *fsk = (const ieee802154_mr_fsk_conf_t *)conf;
            /* 20.2.2.2: 8 symbols per preamble repetition (2-FSK and 4-FSK)
             * 20.2.2.3: SFD 16 symbols (2-FSK: 2 octets, 4-FSK: 4 octets) */
            shr_symbols = ieee802154_mr_fsk_plen(fsk->srate) * 8 * 8 + 16;
            break;
        }
        case IEEE802154_PHY_MR_OFDM:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            shr_symbols = 6;
            break;
        case IEEE802154_PHY_MR_OQPSK: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_oqpsk_conf_t *oqpsk = (const ieee802154_mr_oqpsk_conf_t *)conf;
            /* 802.15.4g, Table 184 / Table 165 */
            shr_symbols = (oqpsk->chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 48 : 72;
            break;
        }
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
        return 0;
    }

    return shr_symbols * ieee802154_get_symbol_duration(conf);
}

/*
 * MR-OQPSK timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */
static uint8_t _mr_oqpsk_spreading(uint8_t chips, uint8_t mode)
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

/* MR-FSK symbols per octet */
static uint8_t _mr_fsk_symbols_per_octet(const ieee802154_mr_fsk_conf_t *fsk)
{
    /* 2-FSK: 1 bit per symbol, 4-FSK: 2 bits per symbol */
    uint8_t symbols = (fsk->mod_ord == 4) ? 4 : 8;

    /* forward error correction halves data rate */
    return fsk->fec ? symbols * 2 : symbols;
}

/* PSDU duration in microseconds */
static uint32_t _get_psdu_duration(const ieee802154_phy_conf_t *conf, uint16_t length)
{
    uint32_t psdu_symbols = 0;
    int res = 0;

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
            /* 1 bit per symbol -> 8 symbols per octet */
            psdu_symbols = length * 8;
            break;
        case IEEE802154_PHY_OQPSK:
            /* 4 bits per symbol -> 2 symbols per octet */
            psdu_symbols = length * 2;
            break;
        case IEEE802154_PHY_MR_FSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_fsk_conf_t *fsk = (const ieee802154_mr_fsk_conf_t *)conf;
            psdu_symbols = length * _mr_fsk_symbols_per_octet(fsk);
            break;
        case IEEE802154_PHY_MR_OFDM: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_ofdm_conf_t *ofdm = (const ieee802154_mr_ofdm_conf_t *)conf;
            /* Table 150 - phySymbolsPerOctet values for MR-OFDM PHY, IEEE 802.15.4g-2012 */
            static const uint8_t quot[] = { 3, 3, 6, 12, 18, 24, 36 };
            const uint8_t option = ofdm->option - 1;

            psdu_symbols = ((length + 1) * (1 << option) + quot[ofdm->scheme] - 1) /
                           quot[ofdm->scheme];
            break;
        }
        case IEEE802154_PHY_MR_OQPSK: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            /* only valid for ACK frames Nd fixed to 63 */
            assert(length <= 7);
            const ieee802154_mr_oqpsk_conf_t *oqpsk = (const ieee802154_mr_oqpsk_conf_t *)conf;
            /* pg. 119, section 18.3.2.14 */
            static const uint8_t sym_len[] = { 32, 32, 64, 128 };
            const uint8_t Ns = sym_len[oqpsk->chips];
            const uint8_t Rspread = _mr_oqpsk_spreading(oqpsk->chips, oqpsk->rate_mode);
            /* Nd == 63, since ACK length is 5 or 7 octets only */
            const uint16_t Npsdu = Rspread * 2 * 63;

            psdu_symbols = (Npsdu + Ns / 2) / Ns + (Npsdu + 8 * Ns) / (16 * Ns);
            break;
        }
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
        return 0;
    }

    return psdu_symbols * ieee802154_get_symbol_duration(conf);
}

/* phyPHRDuration in microseconds */
static uint32_t _get_phr_duration(const ieee802154_phy_conf_t *conf)
{
    uint32_t phr_symbols = 0;
    int res = 0;

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
            /* PHR: 1 octet, 1 bit per symbol */
            phr_symbols = 8;
            break;
        case IEEE802154_PHY_OQPSK:
            /* PHR: 1 octet, 4 bits per symbol */
            phr_symbols = 2;
            break;
        case IEEE802154_PHY_MR_FSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            /* PHR: 2 octets transmitted at the PSDU data rate (incl. FEC) */
            phr_symbols = 2 * _mr_fsk_symbols_per_octet((const ieee802154_mr_fsk_conf_t *)conf);
            break;
        case IEEE802154_PHY_MR_OFDM: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_ofdm_conf_t *ofdm = (const ieee802154_mr_ofdm_conf_t *)conf;
            /* PHR: 3 symbols (option 1) or 6 symbols (options 2-4) */
            phr_symbols = (ofdm->option == 1) ? 3 : 6;
            break;
        }
        case IEEE802154_PHY_MR_OQPSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            /* PHR: 15 symbols */
            phr_symbols = 15;
            break;
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
        return 0;
    }

    return phr_symbols * ieee802154_get_symbol_duration(conf);
}

uint32_t ieee802154_get_turnaround_time(const ieee802154_phy_conf_t *conf)
{
    int res = 0;

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
        case IEEE802154_PHY_OQPSK:
            /* Table 12-1: 12 symbol periods */
            return IEEE802154_ATURNAROUNDTIME_IN_SYMBOLS * ieee802154_get_symbol_duration(conf);
        case IEEE802154_PHY_MR_FSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            return IEEE802154G_ATURNAROUNDTIME_US;
        case IEEE802154_PHY_MR_OFDM:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            return IEEE802154G_ATURNAROUNDTIME_US;
        case IEEE802154_PHY_MR_OQPSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            return IEEE802154G_ATURNAROUNDTIME_US;
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
    }

    return 0;
}

uint32_t ieee802154_get_cca_time(const ieee802154_phy_conf_t *conf)
{
    uint32_t cca_symbols = 0;
    int res = 0;

    switch (conf->phy_mode) {
        case IEEE802154_PHY_BPSK:
        case IEEE802154_PHY_OQPSK:
            /* Table 12-2: 8 symbol periods if not specified by the PHY clause */
            cca_symbols = IEEE802154_CCA_DURATION_IN_SYMBOLS;
            break;
        case IEEE802154_PHY_MR_FSK:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_FSK)) {
                res = -ENOTSUP;
                break;
            }
            /* Table 12-2: 8 symbol periods if not specified by the PHY clause */
            cca_symbols = IEEE802154_CCA_DURATION_IN_SYMBOLS;
            break;
        case IEEE802154_PHY_MR_OFDM:
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OFDM)) {
                res = -ENOTSUP;
                break;
            }
            /* Table 12-2: 8 symbol periods if not specified by the PHY clause */
            cca_symbols = IEEE802154_CCA_DURATION_IN_SYMBOLS;
            break;
        case IEEE802154_PHY_MR_OQPSK: {
            if (!IS_USED(MODULE_IEEE802154_PHY_MR_OQPSK)) {
                res = -ENOTSUP;
                break;
            }
            const ieee802154_mr_oqpsk_conf_t *oqpsk = (const ieee802154_mr_oqpsk_conf_t *)conf;
            /* 802.15.4g, Table 188 */
            cca_symbols = (oqpsk->chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 4 : 8;
            break;
        }
        default:
            res = -ENOTSUP;
            break;
    }

    if (res == -ENOTSUP) {
        /* other PHYs not supported yet */
        assert(0);
        return 0;
    }

    return cca_symbols * ieee802154_get_symbol_duration(conf);
}

uint32_t ieee802154_calculate_unit_backoff_period(const ieee802154_phy_conf_t *conf)
{
    return ieee802154_get_turnaround_time(conf)
         + ieee802154_get_cca_time(conf);
}

uint32_t ieee802154_calculate_ack_wait_duration(const ieee802154_phy_conf_t *conf)
{
    return ieee802154_calculate_unit_backoff_period(conf)
         + ieee802154_get_turnaround_time(conf)
         + _get_shr_duration(conf)
         + _get_phr_duration(conf)
         + _get_psdu_duration(conf, IEEE802154_ACK_FRAME_LEN);
}
