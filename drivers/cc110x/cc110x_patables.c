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
 * @brief       Default configuration of the transceiver output power for the
 *              different frequency bands
 *
 * See Section "24 Output Power Programming" on page 59 in the data sheet for
 * details.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include "cc110x.h"
#include "cc110x_internal.h"

const cc110x_patable_t cc110x_patable_433mhz = {
    .data =  {
        /* Settings for 433 MHz */
        0x12,   /**< -30 dBm */
        0x0E,   /**< -20 dBm */
        0x1D,   /**< -15 dBm */
        0x34,   /**< -10 dBm */
        0x60,   /**<   0 dBm */
        0x84,   /**<   5 dBm */
        0xC8,   /**<   7 dBm */
        0xC0,   /**<  10 dBm */
    }
};

const cc110x_patable_t cc110x_patable_868mhz = {
    .data =  {
        /* Settings for 868 MHz */
        0x03,   /**< -30 dBm */
        0x0F,   /**< -20 dBm */
        0x1E,   /**< -15 dBm */
        0x27,   /**< -10 dBm */
        0x50,   /**<   0 dBm */
        0x81,   /**<   5 dBm */
        0xCB,   /**<   7 dBm */
        0xC2,   /**<  10 dBm */
    }
};

const cc110x_patable_t cc110x_patable_915mhz = {
    .data =  {
        /* Settings for 915 MHz */
        0x03,   /**< -30 dBm */
        0x0E,   /**< -20 dBm */
        0x1E,   /**< -15 dBm */
        0x27,   /**< -10 dBm */
        0x8E,   /**<   0 dBm */
        0xCD,   /**<   5 dBm */
        0xC7,   /**<   7 dBm */
        0xC0,   /**<  10 dBm */
    }
};
