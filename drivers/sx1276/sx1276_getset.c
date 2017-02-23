/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx1276
 * @{
 * @file
 * @brief       Implementation of get and set functions for SX1276
 *
 * @author      Eugene P. [ep@unwds.com]
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @}
 */

#include "sx1276.h"
#include "sx1276_registers.h"
#include "sx1276_internal.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

sx1276_lora_bandwidth_t sx1276_get_bandwidth(sx1276_t *dev)
{
    return dev->settings.lora.bandwidth;
}

void _low_datarate_optimize(sx1276_t *dev)
{
    if (((dev->settings.lora.bandwidth == SX1276_BW_125_KHZ) 
			&& ((dev->settings.lora.datarate == SX1276_SF11) 
					|| (dev->settings.lora.datarate == SX1276_SF12)))
        			|| ((dev->settings.lora.bandwidth == SX1276_BW_250_KHZ) 
							&& (dev->settings.lora.datarate == SX1276_SF12))) {
        dev->settings.lora.low_datarate_optimize = 0x01;
    }
    else {
        dev->settings.lora.low_datarate_optimize = 0x00;
    }

    sx1276_reg_write(dev,
                     SX1276_REG_LR_MODEMCONFIG3,
                     (sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG3)
                      & SX1276_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK)
                     | (dev->settings.lora.low_datarate_optimize << 3));

}
void sx1276_set_bandwidth(sx1276_t *dev, sx1276_lora_bandwidth_t bandwidth)
{
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG1);
    tmp &= SX1276_RF_LORA_MODEMCONFIG1_BW_MASK;
    tmp |= bandwidth << 4;

    dev->settings.lora.bandwidth = bandwidth;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, tmp);
    _low_datarate_optimize(dev);
    /* ERRATA sensetivity tweaks */
    if ((dev->settings.lora.bandwidth == SX1276_BW_500_KHZ) && (SX1276_RF_MID_BAND_THRESH)) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x02);
        sx1276_reg_write(dev, SX1276_REG_LR_TEST3A, 0x64);
    }
    else if (dev->settings.lora.bandwidth == SX1276_BW_500_KHZ) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x02);
        sx1276_reg_write(dev, SX1276_REG_LR_TEST3A, 0x7F);
    }
    else {
        /* ERRATA 2.1 - Sensitivity Optimization with another Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x03);
    }
}

sx1276_lora_spreading_factor_t sx1276_get_spreading_factor(sx1276_t *dev)
{
    return dev->settings.lora.datarate;
}

void sx1276_set_spreading_factor(sx1276_t *dev, sx1276_lora_spreading_factor_t sf)
{
    dev->settings.lora.datarate = sf;
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG2);
    tmp &= SX1276_RF_LORA_MODEMCONFIG2_SF_MASK;
    tmp |= sf << 4;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG2, tmp);

    _low_datarate_optimize(dev);

    sx1276_reg_write(dev, SX1276_REG_LR_DETECTOPTIMIZE, SX1276_RF_LORA_DETECTIONOPTIMIZE_SF7_TO_SF12);
    sx1276_reg_write(dev, SX1276_REG_LR_DETECTIONTHRESHOLD, SX1276_RF_LORA_DETECTIONTHRESH_SF7_TO_SF12);
}

sx1276_lora_coding_rate_t sx1276_get_coding_rate(sx1276_t *dev)
{
    return dev->settings.lora.coderate;
}

void sx1276_set_coding_rate(sx1276_t *dev, sx1276_lora_coding_rate_t coderate)
{
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG1);
    tmp &= SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK;
    tmp |= coderate << 1;

    dev->settings.lora.coderate = coderate;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, tmp);
}

sx1276_radio_state_t sx1276_get_status(sx1276_t *dev)
{
    return dev->settings.state;
}

uint32_t sx1276_get_channel(sx1276_t *dev)
{
    return ((sx1276_reg_read(dev, SX1276_REG_FRFMSB) << 16) | (sx1276_reg_read(dev, SX1276_REG_FRFMID) << 8) | sx1276_reg_read(dev, SX1276_REG_FRFLSB)) * SX1276_FREQ_STEP;

}

uint8_t sx1276_get_syncword(sx1276_t *dev)
{
    return sx1276_reg_read(dev, SX1276_REG_LR_SYNCWORD);
}

void sx1276_set_syncword(sx1276_t *dev, uint8_t syncword)
{
    sx1276_reg_write(dev, SX1276_REG_LR_SYNCWORD, syncword);
}

void sx1276_set_channel(sx1276_t *dev, uint32_t freq)
{
    /* Save current operating mode */
    dev->settings.channel = freq;
    uint8_t prev_mode = sx1276_reg_read(dev, SX1276_REG_OPMODE);

    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_STANDBY);

    freq = (uint32_t)((double) freq / (double) SX1276_FREQ_STEP);

    /* Write frequency settings into chip */
    sx1276_reg_write(dev, SX1276_REG_FRFMSB, (uint8_t)((freq >> 16) & 0xFF));
    sx1276_reg_write(dev, SX1276_REG_FRFMID, (uint8_t)((freq >> 8) & 0xFF));
    sx1276_reg_write(dev, SX1276_REG_FRFLSB, (uint8_t)(freq & 0xFF));

    /* Restore previous operating mode */
    sx1276_reg_write(dev, SX1276_REG_OPMODE, prev_mode);
}

void sx1276_set_modem(sx1276_t *dev, sx1276_radio_modems_t modem)
{
    dev->settings.modem = modem;

    switch (dev->settings.modem) {
        case SX1276_MODEM_LORA:
            sx1276_set_op_mode(dev, SX1276_RF_OPMODE_SLEEP);
            sx1276_reg_write(dev,
                             SX1276_REG_OPMODE,
                             (sx1276_reg_read(dev, SX1276_REG_OPMODE)
                              & SX1276_RF_LORA_OPMODE_LONGRANGEMODE_MASK)
                             | SX1276_RF_LORA_OPMODE_LONGRANGEMODE_ON);

            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING1, 0x00);
            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING2, 0x10); /* DIO5=ClkOut */
            break;

        case SX1276_MODEM_FSK:
            sx1276_set_op_mode(dev, SX1276_RF_OPMODE_SLEEP);
            sx1276_reg_write(dev,
                             SX1276_REG_OPMODE,
                             (sx1276_reg_read(dev, SX1276_REG_OPMODE)
                              & SX1276_RF_LORA_OPMODE_LONGRANGEMODE_MASK)
                             | SX1276_RF_LORA_OPMODE_LONGRANGEMODE_OFF);

            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING1, 0x00);
            break;
        default:
            break;
    }
}

uint32_t sx1276_get_time_on_air(sx1276_t *dev)
{
    uint32_t air_time = 0;

    uint8_t pkt_len = dev->settings.time_on_air_pkt_len;
    sx1276_radio_modems_t modem = dev->settings.modem;
    switch (modem) {
        case SX1276_MODEM_FSK:
            break;

        case SX1276_MODEM_LORA:
        {
            double bw = 0.0;

            /* Note: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported. */
            switch (dev->settings.lora.bandwidth) {
                case 7: /* 125 kHz */
                    bw = 125e3;
                    break;
                case 8: /* 250 kHz */
                    bw = 250e3;
                    break;
                case 9: /* 500 kHz */
                    bw = 500e3;
                    break;
            }

            /* Symbol rate : time for one symbol [secs] */
            double rs = bw / (1 << dev->settings.lora.datarate);
            double ts = 1 / rs;

            /* time of preamble */
            double t_preamble = (dev->settings.lora.preamble_len + 4.25) * ts;

            /* Symbol length of payload and time */
            double tmp =
                ceil(
                    (8 * pkt_len - 4 * dev->settings.lora.datarate + 28
                     + 16 * dev->settings.lora.crc_on
                     - (!dev->settings.lora.implicit_header ? 20 : 0))
                    / (double) (4 * dev->settings.lora.datarate
                                - ((dev->settings.lora.low_datarate_optimize
                                    > 0) ? 2 : 0)))
                * (dev->settings.lora.coderate + 4);
            double n_payload = 8 + ((tmp > 0) ? tmp : 0);
            double t_payload = n_payload * ts;

            /* Time on air */
            double t_on_air = t_preamble + t_payload;

            /* return seconds */
            air_time = floor(t_on_air * 1e6 + 0.999);
        }
        break;
    }

    return air_time;
}

uint8_t sx1276_get_op_mode(sx1276_t *dev)
{
    return sx1276_reg_read(dev, SX1276_REG_OPMODE) & ~SX1276_RF_OPMODE_MASK;
}

void sx1276_set_op_mode(sx1276_t *dev, uint8_t op_mode)
{
    static uint8_t op_mode_prev = 0;

    op_mode_prev = sx1276_get_op_mode(dev);

    if (op_mode != op_mode_prev) {
        /* Replace previous mode value and setup new mode value */
        sx1276_reg_write(dev, SX1276_REG_OPMODE, (op_mode_prev & SX1276_RF_OPMODE_MASK) | op_mode);
    }
}

void sx1276_set_max_payload_len(sx1276_t *dev, uint8_t maxlen)
{
    sx1276_radio_modems_t modem = dev->settings.modem;
    switch (modem) {
        case SX1276_MODEM_FSK:
            break;

        case SX1276_MODEM_LORA:
            sx1276_reg_write(dev, SX1276_REG_LR_PAYLOADMAXLENGTH, maxlen);
            break;
    }
}

void sx1276_set_rx_single(sx1276_t *dev, uint8_t single)
{
    dev->settings.lora.rx_continuous = single ? false : true;
}

uint8_t sx1276_get_rx_single(sx1276_t *dev)
{
    return dev->settings.lora.rx_continuous ? false : true;
}

bool sx1276_get_crc(sx1276_t *dev)
{
    return sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG2) & SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK;
}

void sx1276_set_crc(sx1276_t *dev, bool crc)
{
    dev->settings.lora.crc_on = crc;
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG2);
    tmp &= SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK;
    tmp |= crc << 2;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG2, tmp);
}

void sx1276_set_hop_period(sx1276_t *dev, uint8_t hop_period)
{
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_PLLHOP);
    dev->settings.lora.hop_period = hop_period;
    if (dev->settings.lora.freq_hop_on) {
        tmp |= SX1276_RF_LORA_PLLHOP_FASTHOP_ON;
        sx1276_reg_write(dev, SX1276_REG_LR_PLLHOP, tmp);
        sx1276_reg_write(dev, SX1276_REG_LR_HOPPERIOD, hop_period);
    }
}

uint8_t sx1276_get_hop_period(sx1276_t *dev)
{
    return sx1276_reg_read(dev, SX1276_REG_LR_HOPPERIOD);
}

void sx1276_set_implicit_mode(sx1276_t *dev, bool implicit)
{
    dev->settings.lora.implicit_header = implicit;
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG1);
    tmp &= SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK;
    tmp |= implicit;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, tmp);
}

bool  sx12376_get_implicit_mode(sx1276_t *dev)
{
    return true;
}

void sx1276_set_payload_length(sx1276_t *dev, uint8_t len)
{
    if (dev->settings.lora.implicit_header) {
        dev->settings.lora.payload_len = len;
        sx1276_reg_write(dev, SX1276_REG_LR_PAYLOADLENGTH, len);
    }
}

uint8_t sx1276_get_payload_length(sx1276_t *dev)
{
    return 0;
}

static inline uint8_t sx1276_get_pa_select(uint32_t channel)
{
    if (channel < SX1276_RF_MID_BAND_THRESH) {
        return SX1276_RF_PACONFIG_PASELECT_PABOOST;
    }
    else {
        return SX1276_RF_PACONFIG_PASELECT_RFO;
    }
}

void sx1276_set_power(sx1276_t *dev, uint8_t power)
{
    uint8_t pa_config = 0;
    uint8_t pa_dac = 0;

    pa_config = sx1276_reg_read(dev, SX1276_REG_PACONFIG);
    pa_dac = sx1276_reg_read(dev, SX1276_REG_PADAC);

    pa_config = (pa_config & SX1276_RF_PACONFIG_PASELECT_MASK) | sx1276_get_pa_select(dev->settings.channel) << 7;
    pa_config = (pa_config & SX1276_RF_PACONFIG_MAX_POWER_MASK) | (0x05 << 4); /* max power is 14dBm */

    sx1276_reg_write(dev, SX1276_REG_PARAMP, SX1276_RF_PARAMP_0050_US);

    if ((pa_config & SX1276_RF_PACONFIG_PASELECT_PABOOST)
        == SX1276_RF_PACONFIG_PASELECT_PABOOST) {
        if (power > 17) {
            pa_dac = (pa_dac & SX1276_RF_PADAC_20DBM_MASK) | SX1276_RF_PADAC_20DBM_ON;
        }
        else {
            pa_dac = (pa_dac & SX1276_RF_PADAC_20DBM_MASK) | SX1276_RF_PADAC_20DBM_OFF;
        }
        if ((pa_dac & SX1276_RF_PADAC_20DBM_ON) == SX1276_RF_PADAC_20DBM_ON) {
            if (power < 5) {
                power = 5;
            }
            if (power > 20) {
                power = 20;
            }

            pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                        | (uint8_t)((uint16_t)(power - 5) & 0x0F);
        }
        else {
            if (power < 2) {
                power = 2;
            }
            if (power > 17) {
                power = 17;
            }

            pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                        | (uint8_t)((uint16_t)(power - 2) & 0x0F);
        }
    }
    else {
        if (power < -1) {
            power = -1;
        }
        if (power > 14) {
            power = 14;
        }

        pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                    | (uint8_t)((uint16_t)(power + 1) & 0x0F);
    }

    dev->settings.lora.power = power;
    sx1276_reg_write(dev, SX1276_REG_PACONFIG, pa_config);
    sx1276_reg_write(dev, SX1276_REG_PADAC, pa_dac);
}

uint8_t sx1276_get_power(sx1276_t *dev)
{
    return 0;
}

void sx1276_set_preamble_length(sx1276_t *dev, uint16_t preamble)
{
    dev->settings.lora.preamble_len = preamble;
    sx1276_reg_write(dev, SX1276_REG_LR_PREAMBLEMSB,
                     (preamble >> 8) & 0xFF);
    sx1276_reg_write(dev, SX1276_REG_LR_PREAMBLELSB,
                     preamble & 0xFF);
}

uint16_t sx1276_get_preamble_length(sx1276_t *dev)
{
    return 0;
}

void sx1276_set_rx_timeout(sx1276_t *dev, uint32_t timeout)
{
    dev->settings.window_timeout = timeout;
}
void sx1276_set_tx_timeout(sx1276_t *dev, uint32_t timeout)
{
    dev->settings.lora.tx_timeout = timeout;
}
void sx1276_set_symbol_timeout(sx1276_t *dev, uint16_t timeout)
{
    dev->settings.lora.rx_timeout = timeout;
    uint8_t tmp = sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG2);
    tmp &= SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    tmp |= (timeout >> 8) & ~SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK;
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG2, tmp);
    sx1276_reg_write(dev, SX1276_REG_LR_SYMBTIMEOUTLSB,timeout & 0xFF);
}

void sx1276_set_iq_invert(sx1276_t *dev, bool iq_invert)
{
    dev->settings.lora.iq_inverted = iq_invert;
}

void sx1276_set_freq_hop(sx1276_t *dev, bool freq_hop_on)
{
    dev->settings.lora.freq_hop_on = freq_hop_on;
}
