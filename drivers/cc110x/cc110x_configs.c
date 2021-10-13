/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Transceiver configuration for different base bands, modulation
 *              rate and channel bandwidth.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include "cc110x.h"
#include "cc110x_internal.h"

const cc110x_config_t cc110x_config_433mhz_250kbps_300khz = {
    /* 0x10A99A * 26MHz / 65536 = 433.2252 MHz (LPD433 Channel 7)
     *
     * This is 175 kHz above lower end of the license free range, thus up to 5
     * 350 kHz wide channels can be used (see @ref cc110x_conf_t::deviatn).
     */
    .base_freq = { 0x10, 0xA9, 0x9A },
    /* Intermediate frequency: 0x0C * 26MHz / 1024 = 304.7kHz */
    .fsctrl1 = 0x0C,
    /* 541.67 kHz channel filter bandwidth */
    .mdmcfg4 = 0x2D,
    /* 250 kBaud symbol rate ==> 250 kbps data rate with GFSK */
    .mdmcfg3 = 0x3B,
    /* Deviation of +- 126.953 kHz ==> channel bandwidth about 300 kHz */
    .deviatn = 0x62,
};

const cc110x_config_t cc110x_config_433mhz_38kbps_50khz = {
    /* 0x10A85F * 26MHz / 65536 = 433.1002 MHz (LPD433 Channel 2) */
    .base_freq = { 0x10, 0xA8, 0x5F },
    /* Intermediate frequency: 0x06 * 26000kHz / 1024 = 152.3kHz */
    .fsctrl1 = 0x06,
    /* 101.5625 kHz channel filter bandwidth */
    .mdmcfg4 = 0xCA,
    /* 38.38 kBaud symbol rate ==> 38.38 kbps data rate with GFSK */
    .mdmcfg3 = 0x83,
    /* Deviation of +- 20.63 kHz ==> channel bandwidth about 50 kHz */
    .deviatn = 0x35,
};

const cc110x_config_t cc110x_config_868mhz_250kbps_300khz = {
    /* 0x2146e4 * 26MHz / 65536 = 865.1998 MHz (LoRa 868 Channel 10) */
    .base_freq = { 0x21, 0x46, 0xE4 },
    /* Intermediate frequency: 0x0C * 26MHz / 1024 = 304.7kHz */
    .fsctrl1 = 0x0C,
    /* 541.67 kHz channel filter bandwidth */
    .mdmcfg4 = 0x2D,
    /* 250 kBaud symbol rate ==> 250 kbps data rate with GFSK */
    .mdmcfg3 = 0x3B,
    /* Deviation of +- 126.953 kHz ==> channel bandwidth about 300 kHz */
    .deviatn = 0x62,
};
