/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Configuration of the MR-FSK PHY on the AT86RF215 chip
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "at86rf215.h"
#include "at86rf215_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* symbol time is always 20 µs for MR-FSK (table 0, pg. 7) */
#define FSK_SYMBOL_TIME_US  20

/* also used by at86rf215_netdev.c */
const uint8_t _at86rf215_fsk_srate_10kHz[] = {
    [FSK_SRATE_50K]  = 5,
    [FSK_SRATE_100K] = 10,
    [FSK_SRATE_150K] = 15,
    [FSK_SRATE_200K] = 20,
    [FSK_SRATE_300K] = 30,
    [FSK_SRATE_400K] = 40
};

/* also used by at86rf215_netdev.c */
const uint8_t _at86rf215_fsk_channel_spacing_25kHz[] = {
    [FSK_CHANNEL_SPACING_200K] = 8,
    [FSK_CHANNEL_SPACING_400K] = 16,
};

/* IEEE Std 802.15.4™-2015
 * Table 10-10—Channel numbering for SUN PHYs,
 * index is channel spacing */
static const uint16_t _chan_center_freq0_subghz_25khz[] = {
    [FSK_CHANNEL_SPACING_200K] = 863125U / 25,
    [FSK_CHANNEL_SPACING_400K] = 863225U / 25
};

/* IEEE Std 802.15.4™-2015
 * Table 10-10—Channel numbering for SUN PHYs,
 * index is channel spacing */
static const uint16_t _chan_center_freq0_24ghz_25khz[] = {
    [FSK_CHANNEL_SPACING_200K] = (2400200U - CCF0_24G_OFFSET) / 25,
    [FSK_CHANNEL_SPACING_400K] = (2400400U - CCF0_24G_OFFSET) / 25
};

/* Table 6-57, index is symbol rate */
static const uint8_t _FSKPE_Val[3][6] = {
    { 0x02, 0x0E, 0x3E, 0x74, 0x05, 0x13 }, /* FSKPE0 */
    { 0x03, 0x0F, 0x3F, 0x7F, 0x3C, 0x29 }, /* FSKPE1 */
    { 0xFC, 0xF0, 0xC0, 0x80, 0xC3, 0xC7 }  /* FSKPE2 */
};

/* table 6-51: Symbol Rate Settings */
static uint8_t _TXDFE_SR(at86rf215_t *dev, uint8_t srate)
{
    const uint8_t version = at86rf215_reg_read(dev, RG_RF_VN);

    switch (srate) {
    case FSK_SRATE_50K:
        return version == 1 ? RF_SR_400K : RF_SR_500K;
    case FSK_SRATE_100K:
        return version == 1 ? RF_SR_800K : RF_SR_1000K;
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
        return RF_SR_2000K;
    case FSK_SRATE_300K:
    case FSK_SRATE_400K:
        return RF_SR_4000K;
    }

    return 0;
}

/* table 6-51: Symbol Rate Settings */
static uint8_t _RXDFE_SR(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_SR_400K;
    case FSK_SRATE_100K:
        return RF_SR_800K;
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
        return RF_SR_1000K;
    case FSK_SRATE_300K:
    case FSK_SRATE_400K:
        return RF_SR_2000K;
    }

    return 0;
}

/* table 6-53 Recommended Configuration of the Transmitter Frontend */
static uint8_t _TXCUTC_PARAMP(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_PARAMP32U;
    case FSK_SRATE_100K:
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
        return RF_PARAMP16U;
    case FSK_SRATE_300K:
    case FSK_SRATE_400K:
        return RF_PARAMP8U;
    }

    return 0;
}

/* Table 6-53. Recommended Configuration of the Transmitter Frontend (modulation index 0.5) */
static uint8_t _TXCUTC_LPFCUT_half(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_FLC80KHZ;
    case FSK_SRATE_100K:
        return RF_FLC100KHZ;
    case FSK_SRATE_150K:
        return RF_FLC160KHZ;
    case FSK_SRATE_200K:
        return RF_FLC200KHZ;
    case FSK_SRATE_300K:
        return RF_FLC315KHZ;
    case FSK_SRATE_400K:
        return RF_FLC400KHZ;
    }
    return 0;
}

/* Table 6-54. Recommended Configuration of the Transmitter Frontend (modulation index 1) */
static uint8_t _TXCUTC_LPFCUT_full(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_FLC80KHZ;
    case FSK_SRATE_100K:
        return RF_FLC160KHZ;
    case FSK_SRATE_150K:
        return RF_FLC250KHZ;
    case FSK_SRATE_200K:
        return RF_FLC315KHZ;
    case FSK_SRATE_300K:
        return RF_FLC500KHZ;
    case FSK_SRATE_400K:
        return RF_FLC625KHZ;
    }
    return 0;
}

static inline uint8_t _TXCUTC_LPFCUT(uint8_t srate, bool half)
{
    return half ? _TXCUTC_LPFCUT_half(srate) : _TXCUTC_LPFCUT_full(srate);
}

/* Table 6-60. Recommended Configuration values of the sub-1GHz Receiver Frontend (modulation index 1/2) */
static uint8_t _RXBWC_BW_subGHz_half(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_BW160KHZ_IF250KHZ;
    case FSK_SRATE_100K:
        return RF_BW200KHZ_IF250KHZ;
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
        return RF_BW320KHZ_IF500KHZ;
    case FSK_SRATE_300K:
        return (RF_BW500KHZ_IF500KHZ | (1 << RXBWC_IFS_SHIFT));
    case FSK_SRATE_400K:
        return RF_BW630KHZ_IF1000KHZ;
    }

    return 0;
}

/* Table 6-61. Recommended Configuration values of the 2.4GHz Receiver Frontend (modulation index 1/2) */
static uint8_t _RXBWC_BW_2dot4GHz_half(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_BW160KHZ_IF250KHZ;
    case FSK_SRATE_100K:
        return RF_BW200KHZ_IF250KHZ;
    case FSK_SRATE_150K:
        return RF_BW320KHZ_IF500KHZ;
    case FSK_SRATE_200K:
        return RF_BW400KHZ_IF500KHZ;
    case FSK_SRATE_300K:
        return RF_BW630KHZ_IF1000KHZ;
    case FSK_SRATE_400K:
        return RF_BW800KHZ_IF1000KHZ;
    }

    return 0;
}

/* Table 6-62. Recommended Configuration values of the sub-1GHz Receiver Frontend (modulation index 1) */
static uint8_t _RXBWC_BW_subGHz_full(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_BW160KHZ_IF250KHZ;
    case FSK_SRATE_100K:
        return RF_BW320KHZ_IF500KHZ;
    case FSK_SRATE_150K:
        return RF_BW400KHZ_IF500KHZ;
    case FSK_SRATE_200K:
        return (RF_BW500KHZ_IF500KHZ | (1 << RXBWC_IFS_SHIFT));
    case FSK_SRATE_300K:
        return RF_BW630KHZ_IF1000KHZ;
    case FSK_SRATE_400K:
        return (RF_BW1000KHZ_IF1000KHZ | (1 << RXBWC_IFS_SHIFT));
    }

    return 0;
}

/* Table 6-63. Recommended Configuration values of the 2.4 GHz Receiver Frontend (modulation index 1) */
static uint8_t _RXBWC_BW_2dot4GHz_full(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return RF_BW200KHZ_IF250KHZ;
    case FSK_SRATE_100K:
        return RF_BW400KHZ_IF500KHZ;
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
        return RF_BW630KHZ_IF1000KHZ;
    case FSK_SRATE_300K:
        return RF_BW800KHZ_IF1000KHZ;
    case FSK_SRATE_400K:
        return (RF_BW1000KHZ_IF1000KHZ | (1 << RXBWC_IFS_SHIFT));
    }

    return 0;
}

static inline uint8_t _RXBWC_BW(uint8_t srate, bool subGHz, bool half)
{
    if (subGHz) {
        return half ? _RXBWC_BW_subGHz_half(srate) : _RXBWC_BW_subGHz_full(srate);
    } else {
        return half ? _RXBWC_BW_2dot4GHz_half(srate) : _RXBWC_BW_2dot4GHz_full(srate);
    }
}

static uint8_t _RXDFE_RCUT_half(uint8_t srate, bool subGHz)
{
    if (srate == FSK_SRATE_200K) {
        return RF_RCUT_FS_BY_5P3;
    }

    if (srate == FSK_SRATE_400K && !subGHz) {
        return RF_RCUT_FS_BY_5P3;
    }

    return RF_RCUT_FS_BY_8;
}

static uint8_t _RXDFE_RCUT_full(uint8_t srate, bool subGHz)
{
    switch (srate) {
    case FSK_SRATE_50K:
    case FSK_SRATE_100K:
    case FSK_SRATE_300K:
        return RF_RCUT_FS_BY_5P3;
    case FSK_SRATE_150K:
    case FSK_SRATE_400K:
        return subGHz ? RF_RCUT_FS_BY_5P3 : RF_RCUT_FS_BY_4;
    case FSK_SRATE_200K:
        return subGHz ? RF_RCUT_FS_BY_4 : RF_RCUT_FS_BY_2P6;
    }

    return 0;
}

/* Table 6-64. Minimum Preamble Length */
static uint8_t _FSKPL(uint8_t srate)
{
    switch (srate) {
    case FSK_SRATE_50K:
        return 2;
    case FSK_SRATE_100K:
        return 3;
    case FSK_SRATE_150K:
    case FSK_SRATE_200K:
    case FSK_SRATE_300K:
        return 8;
    case FSK_SRATE_400K:
        return 10;
    }

    return 0;
}

/* fsk modulation indices / 8 */
static const uint8_t _fsk_mod_idx[] = {
    3, 4, 6, 8, 10, 12, 14, 16
};

/* FSK modulation scale / 8 */
static const uint8_t _fsk_mod_idx_scale[] = {
    7, 8, 9, 10
};

static void _fsk_mod_idx_get(uint8_t num, uint8_t *idx, uint8_t *scale)
{
    *idx   = 0;
    *scale = 0;

    uint8_t diff = 0xFF;
    for (uint8_t i = 0; i < ARRAY_SIZE(_fsk_mod_idx_scale); ++i) {
        for (uint8_t j = 0; j < ARRAY_SIZE(_fsk_mod_idx); ++j) {
            if (abs(num - _fsk_mod_idx_scale[i] * _fsk_mod_idx[j]) < diff) {
                diff   = abs(num - _fsk_mod_idx_scale[i] * _fsk_mod_idx[j]);
                *idx   = j;
                *scale = i;
            }
        }
    }
}

static inline uint8_t _RXDFE_RCUT(uint8_t srate, bool subGHz, bool half)
{
    return half ? _RXDFE_RCUT_half(srate, subGHz) : _RXDFE_RCUT_full(srate, subGHz);
}

void at86rf215_FSK_prepare_rx(at86rf215_t *dev)
{
    at86rf215_reg_write(dev, dev->BBC->RG_FSKPLL, dev->fsk_pl);

    /* Preamble detection takes RSSI values into account if the preamble length is less than 8 octets. */
    if (dev->fsk_pl < 8) {
        at86rf215_reg_or(dev, dev->BBC->RG_FSKC2, FSKC2_PDTM_MASK);
    } else {
        at86rf215_reg_and(dev, dev->BBC->RG_FSKC2, (uint8_t) ~FSKC2_PDTM_MASK);
    }
}

void at86rf215_FSK_prepare_tx(at86rf215_t *dev)
{
    /* send long preamble when TXing */
    at86rf215_reg_write(dev, dev->BBC->RG_FSKPLL, dev->fsk_pl * 8);
}

static void _set_srate(at86rf215_t *dev, uint8_t srate, bool mod_idx_half)
{
    /* Set Receiver Bandwidth: fBW = 160 kHz, fIF = 250 kHz */
    at86rf215_reg_write(dev, dev->RF->RG_RXBWC, _RXBWC_BW(srate, is_subGHz(dev), mod_idx_half));
    /* fS = 400 kHz; fCUT = fS/5.333 = 75 kHz */
    at86rf215_reg_write(dev, dev->RF->RG_RXDFE, _RXDFE_SR(srate)
                                              | _RXDFE_RCUT(srate, is_subGHz(dev), mod_idx_half));
    /* Power Amplifier Ramp Time = 32 µs; fLPCUT = 80 kHz */
    at86rf215_reg_write(dev, dev->RF->RG_TXCUTC, _TXCUTC_PARAMP(srate)
                                               | _TXCUTC_LPFCUT(srate, mod_idx_half));
    /* fS = 500 kHz; fCUT = fS/2 = 250 kHz */
    at86rf215_reg_write(dev, dev->RF->RG_TXDFE, _TXDFE_SR(dev, srate)
                                              | (mod_idx_half ? RF_RCUT_FS_BY_8 : RF_RCUT_FS_BY_2)
                                              | TXDFE_DM_MASK);

    /* configure pre-emphasis */
    at86rf215_reg_write(dev, dev->BBC->RG_FSKPE0, _FSKPE_Val[0][srate]);
    at86rf215_reg_write(dev, dev->BBC->RG_FSKPE1, _FSKPE_Val[1][srate]);
    at86rf215_reg_write(dev, dev->BBC->RG_FSKPE2, _FSKPE_Val[2][srate]);

    /* set preamble length in octets */
    dev->fsk_pl = _FSKPL(srate);

    at86rf215_FSK_prepare_rx(dev);

    /* t_on = t_off = t_min (time to TX minimal preamble length) */
    uint8_t t_on = 4 * _FSKPL(srate) * 100 / _at86rf215_fsk_srate_10kHz[srate];

    at86rf215_reg_write(dev, dev->BBC->RG_FSKRPCONT, t_on);
    at86rf215_reg_write(dev, dev->BBC->RG_FSKRPCOFFT, t_on);

    DEBUG("[at86rf215] t_on: %d µs\n", t_on);

    /* set symbol rate, preamble is less than 256 so set high bits 0 */
    at86rf215_reg_write(dev, dev->BBC->RG_FSKC1, srate);
}

static void _set_ack_timeout(at86rf215_t *dev, bool mord4, bool fec)
{
    uint8_t ack_len = AT86RF215_ACK_PSDU_BYTES;

    /* PHR uses same data rate as PSDU */
    ack_len += 2;

    /* 4-FSK doubles data rate */
    if (mord4) {
        ack_len /= 2;
    }

    /* forward error correction halves data rate */
    if (fec) {
        ack_len *= 2;
    }

    dev->ack_timeout_usec = dev->csma_backoff_period
                          + IEEE802154G_ATURNAROUNDTIME_US
                          /* long Preamble + SFD; SFD=2 */
                          + ((dev->fsk_pl * 8 + 2)
                          + ack_len) * 8 * FSK_SYMBOL_TIME_US;

    DEBUG("[%s] ACK timeout: %"PRIu32" µs\n", "FSK", dev->ack_timeout_usec);
}

static void _set_csma_backoff_period(at86rf215_t *dev)
{
    dev->csma_backoff_period = IEEE802154_CCA_DURATION_IN_SYMBOLS * FSK_SYMBOL_TIME_US
                             + IEEE802154G_ATURNAROUNDTIME_US;

    DEBUG("[%s] CSMA BACKOFF: %"PRIu32" µs\n", "FSK", dev->csma_backoff_period);
}

int at86rf215_configure_FSK(at86rf215_t *dev, uint8_t srate, uint8_t mod_idx, uint8_t mod_order, uint8_t fec)
{
    if (srate > FSK_SRATE_400K) {
        DEBUG("[%s] invalid symbol rate: %d\n", __func__, srate);
        return -EINVAL;
    }

    bool mod_idx_half = mod_idx <= 32;
    uint8_t _mod_idx, _mod_idx_scale;
    _fsk_mod_idx_get(mod_idx, &_mod_idx, &_mod_idx_scale);

    at86rf215_await_state_end(dev, RF_STATE_TX);

    /* disable radio */
    at86rf215_reg_write(dev, dev->BBC->RG_PC, 0);

    _set_srate(dev, srate, mod_idx_half);

    /* set receiver gain target according to data sheet */
    at86rf215_reg_write(dev, dev->RF->RG_AGCS, 1 << AGCS_TGT_SHIFT);
    /* enable automatic receiver gain */
    at86rf215_reg_write(dev, dev->RF->RG_AGCC, AGCC_EN_MASK);

    /* set Bandwidth Time Product, Modulation Index & Modulation Order */
    /* effective modulation index = MIDXS * MIDX */
    at86rf215_reg_write(dev, dev->BBC->RG_FSKC0, FSK_BT_20
                                               | (_mod_idx << FSKC0_MIDX_SHIFT)
                                               | (_mod_idx_scale << FSKC0_MIDXS_SHIFT)
                                               | mod_order
                                               );

    /* enable direct modulation */
    at86rf215_reg_write(dev, dev->BBC->RG_FSKDM, FSKDM_EN_MASK | FSKDM_PE_MASK);

    /* 16 µs base time */
    uint8_t fskrpc = 0x5;

    /* Enable / Disable Reduced Power Consumption */
    if (dev->flags & AT86RF215_OPT_RPC) {
        fskrpc |= FSKRPC_EN_MASK;
    }
    at86rf215_reg_write(dev, dev->BBC->RG_FSKRPC, fskrpc);

    /* set forward error correction */
    at86rf215_FSK_set_fec(dev, fec);

    at86rf215_FSK_set_channel_spacing(dev, FSK_CHANNEL_SPACING_400K);

    at86rf215_enable_radio(dev, BB_MRFSK);

    return 0;
}

uint8_t at86rf215_FSK_get_mod_order(at86rf215_t *dev)
{
    return at86rf215_reg_read(dev, dev->BBC->RG_FSKC0) & FSKC0_MORD_MASK;
}

int at86rf215_FSK_set_mod_order(at86rf215_t *dev, uint8_t mod_order) {

    at86rf215_await_state_end(dev, RF_STATE_TX);

    if (mod_order) {
        at86rf215_reg_or(dev, dev->BBC->RG_FSKC0, FSK_MORD_4SFK);
    } else {
        at86rf215_reg_and(dev, dev->BBC->RG_FSKC0, ~FSK_MORD_4SFK);
    }

    _set_ack_timeout(dev, mod_order, at86rf215_FSK_get_fec(dev));
    return 0;
}

uint8_t at86rf215_FSK_get_mod_idx(at86rf215_t *dev)
{
    uint8_t fskc0 = at86rf215_reg_read(dev, dev->BBC->RG_FSKC0);
    uint8_t _mod_idx = (fskc0 & FSKC0_MIDX_MASK) >> FSKC0_MIDX_SHIFT;
    uint8_t _mod_idx_scale = (fskc0 & FSKC0_MIDXS_MASK) >> FSKC0_MIDXS_SHIFT;

    return _fsk_mod_idx[_mod_idx] * _fsk_mod_idx_scale[_mod_idx_scale];
}

int at86rf215_FSK_set_mod_idx(at86rf215_t *dev, uint8_t mod_idx)
{
    uint8_t _mod_idx, _mod_idx_scale;

    at86rf215_await_state_end(dev, RF_STATE_TX);

    _set_srate(dev, at86rf215_FSK_get_srate(dev), mod_idx <= 32);

    _fsk_mod_idx_get(mod_idx, &_mod_idx, &_mod_idx_scale);
    at86rf215_reg_write(dev, dev->BBC->RG_FSKC0, FSK_BT_20
                                               | (_mod_idx << FSKC0_MIDX_SHIFT)
                                               | (_mod_idx_scale << FSKC0_MIDXS_SHIFT)
                                               | at86rf215_FSK_get_mod_order(dev)
                                               );
    return 0;
}

uint8_t at86rf215_FSK_get_srate(at86rf215_t *dev)
{
    return at86rf215_reg_read(dev, dev->BBC->RG_FSKC1) & FSKC1_SRATE_MASK;
}

int at86rf215_FSK_set_srate(at86rf215_t *dev, uint8_t srate)
{
    if (srate > FSK_SRATE_400K) {
        return -1;
    }

    at86rf215_await_state_end(dev, RF_STATE_TX);

    _set_srate(dev, srate, at86rf215_FSK_get_mod_idx(dev) <= 32);
    _set_csma_backoff_period(dev);

    return 0;
}

int at86rf215_FSK_set_fec(at86rf215_t *dev, uint8_t mode)
{
    at86rf215_await_state_end(dev, RF_STATE_TX);

    switch (mode) {
    case IEEE802154_FEC_NONE:
        at86rf215_reg_and(dev, dev->BBC->RG_FSKPHRTX, ~FSKPHRTX_SFD_MASK);
        break;
    case IEEE802154_FEC_NRNSC:
        at86rf215_reg_or(dev, dev->BBC->RG_FSKPHRTX, FSKPHRTX_SFD_MASK);
        at86rf215_reg_and(dev, dev->BBC->RG_FSKC2, ~FSKC2_FECS_MASK);
        break;
    case IEEE802154_FEC_RSC:
        at86rf215_reg_or(dev, dev->BBC->RG_FSKPHRTX, FSKPHRTX_SFD_MASK);
        at86rf215_reg_or(dev, dev->BBC->RG_FSKC2, FSKC2_FECS_MASK);
        break;
    default:
        return -1;
    }

    _set_ack_timeout(dev, mode, at86rf215_FSK_get_mod_order(dev));
    _set_csma_backoff_period(dev);

    return 0;
}

uint8_t at86rf215_FSK_get_fec(at86rf215_t *dev)
{
    /* SFD0 -> Uncoded IEEE mode */
    /* SFD1 -> Coded IEEE mode */
    if (!(at86rf215_reg_read(dev, dev->BBC->RG_FSKPHRTX) & FSKPHRTX_SFD_MASK)) {
        return IEEE802154_FEC_NONE;
    }

    if (at86rf215_reg_read(dev, dev->BBC->RG_FSKC2) & FSKC2_FECS_MASK) {
        return IEEE802154_FEC_RSC;
    } else {
        return IEEE802154_FEC_NRNSC;
    }
}

int at86rf215_FSK_set_channel_spacing(at86rf215_t *dev, uint8_t ch_space)
{
    if (ch_space > FSK_CHANNEL_SPACING_400K) {
        return -1;
    }

    at86rf215_await_state_end(dev, RF_STATE_TX);

    /* set channel spacing, same for both sub-GHz & 2.4 GHz */
    at86rf215_reg_write(dev, dev->RF->RG_CS, _at86rf215_fsk_channel_spacing_25kHz[ch_space]);

    /* set center frequency */
    if (is_subGHz(dev)) {
        at86rf215_reg_write16(dev, dev->RF->RG_CCF0L, _chan_center_freq0_subghz_25khz[ch_space]);
    } else {
        at86rf215_reg_write16(dev, dev->RF->RG_CCF0L, _chan_center_freq0_24ghz_25khz[ch_space]);
    }

    DEBUG("CCF0 configured as: %lu kHz\n",
          25UL * at86rf215_reg_read16(dev, dev->RF->RG_CCF0L) + (is_subGHz(dev) ? 0 : CCF0_24G_OFFSET));

    /* adjust channel spacing */
    dev->num_chans = is_subGHz(dev) ? 34 / (ch_space + 1) : (416 / (ch_space + 1)) - (ch_space * 2);
    dev->netdev.chan = at86rf215_chan_valid(dev, dev->netdev.chan);
    at86rf215_reg_write16(dev, dev->RF->RG_CNL, dev->netdev.chan);

    return 0;
}
