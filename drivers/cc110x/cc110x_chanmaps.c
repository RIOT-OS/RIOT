/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Channel maps that translate "virtual" channels to "physical"
 *              channels.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include "cc110x.h"
#include "cc110x_internal.h"

const cc110x_chanmap_t cc110x_chanmap_433mhz_300khz = {
    .map =  {
        0,      /**< base + 0.000MHz = 433.225 MHz (up to 350 kHz wide channel) */
        7,      /**< base + 0.350MHz = 433.575 MHz (up to 350 kHz wide channel) */
        14,     /**< base + 0.700MHz = 433.925 MHz (up to 350 kHz wide channel) */
        21,     /**< base + 1.050MHz = 434.275 MHz (up to 350 kHz wide channel) */
        28,     /**< base + 1.400MHz = 434.625 MHz (only up to *330* kHz wide) */
        255,    /**< License free range (433.05 MHz - 434.79 MHz) exhausted :-( */
        255,    /**< License free range (433.05 MHz - 434.79 MHz) exhausted :-( */
        255,    /**< License free range (433.05 MHz - 434.79 MHz) exhausted :-( */
    }
};

const cc110x_chanmap_t cc110x_chanmap_433mhz_50khz = {
    .map =  {
        0,      /**< base + 0.000MHz = 433.100 MHz (LDP433 Channel 2) */
        4,      /**< base + 0.200MHz = 433.300 MHz (LDP433 Channel 10)*/
        8,      /**< base + 0.400MHz = 433.500 MHz (LDP433 Channel 18) */
        12,     /**< base + 0.600MHz = 433.700 MHz (LDP433 Channel 26) */
        16,     /**< base + 0.800MHz = 433.900 MHz (LDP433 Channel 34) */
        20,     /**< base + 1.000MHz = 434.100 MHz (LDP433 Channel 42) */
        24,     /**< base + 1.200MHz = 434.300 MHz (LDP433 Channel 50) */
        28,     /**< base + 1.400MHz = 434.500 MHz (LDP433 Channel 58) */
    }
};

const cc110x_chanmap_t cc110x_chanmap_433mhz_50khz_alt = {
    .map =  {
        2,      /**< base + 0.100MHz = 433.200 MHz (LDP433 Channel 6) */
        6,      /**< base + 0.300MHz = 433.400 MHz (LDP433 Channel 14)*/
        10,     /**< base + 0.500MHz = 433.600 MHz (LDP433 Channel 22) */
        14,     /**< base + 0.700MHz = 433.800 MHz (LDP433 Channel 30) */
        18,     /**< base + 0.900MHz = 434.000 MHz (LDP433 Channel 38) */
        22,     /**< base + 1.100MHz = 434.200 MHz (LDP433 Channel 46) */
        26,     /**< base + 1.300MHz = 434.400 MHz (LDP433 Channel 54) */
        30,     /**< base + 1.500MHz = 434.600 MHz (LDP433 Channel 62) */
    }
};

const cc110x_chanmap_t cc110x_chanmap_868mhz_lora = {
    .map =  {
        0,      /**< base + 0.000MHz = 865.20 MHz = LoRa868 Channel 10 */
        6,      /**< base + 0.300MHz = 865.50 MHz = LoRa868 Channel 11 */
        12,     /**< base + 0.600MHz = 865.80 MHz = LoRa868 Channel 12 */
        18,     /**< base + 0.900MHz = 866.10 MHz = LoRa868 Channel 13 */
        24,     /**< base + 1.200MHz = 866.40 MHz = LoRa868 Channel 14 */
        30,     /**< base + 1.500MHz = 866.70 MHz = LoRa868 Channel 15 */
        36,     /**< base + 1.800MHz = 867.00 MHz = LoRa868 Channel 16 */
        56,     /**< base + 2.800MHz = 868.00 MHz = LoRa868 Channel 17 */
    }
};
