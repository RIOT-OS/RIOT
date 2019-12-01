/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief     Lookup tables for the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <stdint.h>

#include "nrf24l01p_channels.h"

/* frequency range: 2400 - 2525 Mhz
   occupied bandwidth for each channel:
        250 kbps: < 1Mhz
       1000 kbps: < 1Mhz
       2000 kbps: < 2Mhz

    -> spaces between each channel´s
       base frequency must be at least
       2 Mhz, to be sufficient for all
       data rates.

    Here, 8 Mhz channel spacing is used
    because 4 bit allow 16 channels and
    (2525 - 2400) / 16 ~ 8. */
/**
 * @brief Map of channel base frequencies
 */
const uint8_t vchanmap[NRF24L01P_NUM_CHANNELS] = {
    2,      /* 2400 Mhz +   2 Mhz */
    10,     /* 2400 Mhz +  10 Mhz */
    18,     /* 2400 Mhz +  18 Mhz */
    26,     /* 2400 Mhz +  26 Mhz */
    34,     /* 2400 Mhz +  34 Mhz */
    42,     /* 2400 Mhz +  42 Mhz */
    50,     /* 2400 Mhz +  50 Mhz */
    58,     /* 2400 Mhz +  56 Mhz */
    66,     /* 2400 Mhz +  66 Mhz */
    74,     /* 2400 Mhz +  74 Mhz */
    82,     /* 2400 Mhz +  82 Mhz */
    90,     /* 2400 Mhz +  90 Mhz */
    98,     /* 2400 Mhz +  98 Mhz */
    106,    /* 2400 Mhz + 106 Mhz */
    114,    /* 2400 Mhz + 114 Mhz */
    122     /* 2400 Mhz + 122 Mhz */
};
