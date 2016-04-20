/*
 * Copyright (C) 2013 INRIA
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_cc110x
 * @{
 *
 * @file
 * @brief   TI Chipcon CC110x default settings
 *
 * @author    Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author    Heiko Will <hwill@inf.fu-berlin.de>
 * @author    Oliver Hahm <oliver.hahm@inria.fr>
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "board.h"
#include "cc110x.h"

/**
 * @brief   PATABLE with available output powers
 * @note    If changed in size, adjust MAX_OUTPUT_POWER definition
 *          in CC110x interface
*/
const char cc110x_default_pa_table[8] = {
    0x00,   /*< -52 dBm */
    0x0D,   /*< -20 dBm */
    0x34,   /*< -10 dBm */
    0x57,   /*< - 5 dBm */
    0x8E,   /*<   0 dBm */
    0x85,   /*< + 5 dBm */
    0xCC,   /*< + 7 dBm */
    0xC3    /*< +10 dBm */
};

const char cc110x_default_base_freq[3] = { 0x21, 0x71, 0x7F };

/**
 * @brief cc110x default settings
 */
const char cc110x_default_conf[] = {
    0x06, /* IOCFG2 */
    0x2E, /* IOCFG1 */
    /* some boards use cc110x' GDO0 as clock source, so for those, we allow
     * overriding of the corresponding setting, e.g., in board.h */
#ifdef CC110X_IOCONF0_VAL
    CC110X_IOCONF0_VAL,
#else
    0x0E, /* IOCFG0 */
#endif
    0x07, /* FIFOTHR */
    0x9B, /* SYNC1 */
    0xAD, /* SYNC0 */
    0xFF, /* PKTLEN */
    0x06, /* PKTCTRL1 */
    0x45, /* PKTCTRL0 (variable packet length) */
    0xFF, /* ADDR */
    0x00, /* CHANNR */
    0x0F, /* FSCTRL1 */
    0x00, /* FSCTRL0 */
    0x21, /* FREQ2 */
    0x71, /* FREQ1 */
    0x7A, /* FREQ0 */
    0x7C, /* MDMCFG4 */
    0x7A, /* MDMCFG3 */
    0x06, /* MDMCFG2 */
    0xC0, /* MDMCFG1 */
    0xF8, /* MDMCFG0 */
    0x44, /* DEVIATN */
    0x07, /* MCSM2 */
    0x03, /* MCSM1 */
    0x18, /* MCSM0 */
    0x16, /* FOCCFG */
    0x6C, /* BSCFG */
    0x45, /* AGCCTRL2 */
    0x40, /* AGCCTRL1 */
    0x91, /* AGCCTRL0 */
    0x87, /* WOREVT1 */
    0x6B, /* WOREVT0 */
    0xF8, /* WORCTRL */
    0x56, /* FREND1 */
    0x17, /* FREND0 */
    0xEA, /* FSCAL3 */
    0x2A, /* FSCAL2 */
    0x00, /* FSCAL1 */
    0x1F, /* FSCAL0 */
    0x00  /* padding to 4 bytes */
};

/**
 * @brief The size of the configuration array for CC110X in bytes
 * */
const uint8_t cc110x_default_conf_size = sizeof(cc110x_default_conf);
