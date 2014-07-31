/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup   drivers_cc110x_ng
 * @{
 *
 * @file
 * @brief   TI Chipcon CC110x default settings
 *
 * @author    Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author    Heiko Will <hwill@inf.fu-berlin.de>
 * @author    Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "cc110x_ng.h"

// Default PA table index (output power)
#define PATABLE                 (11)

/**
 * Usable, non overlapping channels and corresponding frequencies
 * for use with CC1100. CHANNR is the register for selecting a channel.
 *
 * channel number | CHANNR | frequency [MHz]
 * -----------------------------------------
 *              0 |      0 | 869.525
 *              1 |     10 | 871.61
 *              2 |     20 | 873.58     ~ seems to be bad (hang-ups with this channel)
 *              3 |     30 | 875.61
 *              4 |     40 | 877.58
 *              5 |     50 | 879.61
 *              6 |     60 | 881.58
 *              7 |     70 | 883.61
 *              8 |     80 | 885.58
 *              9 |     90 | 887.61
 *             10 |    100 | 889.58
 *             11 |    110 | 891.57
 *             12 |    120 | 893.58
 *             13 |    130 | 895.61
 *             14 |    140 | 897.58
 *             15 |    150 | 899.57
 *             16 |    160 | 901.57
 *             17 |    170 | 903.61
 *             18 |    180 | 905.57
 *             19 |    190 | 907.57
 *             20 |    200 | 909.57
 *             21 |    210 | 911.57
 *             22 |    220 | 913.57
 *             23 |    230 | 915.61
 *             24 |    240 | 917.61
 */

// 400 kbps, MSK, X-tal: 26 MHz (Chip Revision F)
char cc110x_conf[] = {
    0x06, // IOCFG2
    0x2E, // IOCFG1
    0x0E, // IOCFG0
    0x0F, // FIFOTHR
    0x9B, // SYNC1
    0xAD, // SYNC0
    0x3D, // PKTLEN     (maximum value of packet length byte = 61)
    0x06, // PKTCTRL1
    0x45, // PKTCTRL0   (variable packet length)
    0xFF, // ADDR
    CC1100_DEFAULT_CHANNR * 10, // CHANNR
    0x0B, // FSCTRL1
    0x00, // FSCTRL0
    0x21, // FREQ2
    0x71, // FREQ1
    0x7A, // FREQ0
    0x2D, // MDMCFG4
    0xF8, // MDMCFG3
    0x73, // MDMCFG2
    0x42, // MDMCFG1
    0xF8, // MDMCFG0
    0x00, // DEVIATN
    0x07, // MCSM2
    0x03, // MCSM1
    0x18, // MCSM0
    0x1D, // FOCCFG
    0x1C, // BSCFG
    0xC0, // AGCCTRL2
    0x49, // AGCCTRL1, (old value was 0x49 -> made carrier sense less sensitive!)
          //            0x47 - 7 dB above MAGN_TARGET setting
    0xB2, // AGCCTRL0
    0x87, // WOREVT1
    0x6B, // WOREVT0
    0xF8, // WORCTRL
    0xB6, // FREND1
    0x10, // FREND0
    0xEA, // FSCAL3
    0x2A, // FSCAL2
    0x00, // FSCAL1
    0x1F, // FSCAL0
    0x00  // padding to 4 bytes
};

uint8_t pa_table_index = PATABLE; ///< Current PATABLE Index
uint8_t pa_table[] = {        ///< PATABLE with available output powers
              0x00,         ///< -52 dBm
              0x03,         ///< -30 dBm
              0x0D,         ///< -20 dBm
              0x1C,         ///< -15 dBm
              0x34,         ///< -10 dBm
              0x57,         ///< - 5 dBm
              0x3F,         ///< - 1 dBm
              0x8E,         ///<   0 dBm
              0x85,         ///< + 5 dBm
              0xCC,         ///< + 7 dBm
              0xC6,         ///< + 9 dBm
              0xC3          ///< +10 dBm
}; // If PATABLE is changed in size, adjust MAX_OUTPUT_POWER definition in CC1100 interface!
