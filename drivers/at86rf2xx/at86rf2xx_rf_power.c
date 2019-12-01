/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Mapping between power in dbm and register values
 *              for AT86RF2XX devices
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include "kernel_defines.h"

/* See: Table 9-15. Recommended Mapping of TX Power, Frequency Band, and
 * PHY_TX_PWR (register 0x05), AT86RF212B data sheet. */
const uint8_t _212b_dbm_to_tx_pow_868[37] = { 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18,
                                              0x17, 0x15, 0x14, 0x13, 0x12, 0x11,
                                              0x10, 0x0f, 0x31, 0x30, 0x2f, 0x94,
                                              0x93, 0x91, 0x90, 0x29, 0x49, 0x48,
                                              0x47, 0xad, 0xcd, 0xcc, 0xcb, 0xea,
                                              0xe9, 0xe8, 0xe7, 0xe6, 0xe4, 0x80,
                                              0xa0 };
const uint8_t _212b_dbm_to_tx_pow_915[37] = { 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x17,
                                              0x16, 0x15, 0x14, 0x13, 0x12, 0x11,
                                              0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b,
                                              0x09, 0x91, 0x08, 0x07, 0x05, 0x27,
                                              0x04, 0x03, 0x02, 0x01, 0x00, 0x86,
                                              0x40, 0x84, 0x83, 0x82, 0x80, 0xc1,
                                              0xc0 };
/* AT86RF212B: [-110, -54] */
const int16_t _212b_rx_sens_to_dbm[16] = { -110, -98, -94, -91, -88, -85, -82,
                                           -79, -76, -73, -70, -67, -63, -60, -57,
                                           -54 };
const uint8_t _212b_dbm_to_rx_sens[57] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                           0x01, 0x01, 0x01, 0x01, 0x02, 0x02,
                                           0x02, 0x03, 0x03, 0x03, 0x04, 0x04,
                                           0x04, 0x05, 0x05, 0x05, 0x06, 0x06,
                                           0x06, 0x07, 0x07, 0x07, 0x08, 0x08,
                                           0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a,
                                           0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x0c,
                                           0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0e,
                                           0x0e, 0x0e, 0x0f };

/* AT86RF233 [-17, 4]*/
const int16_t _233_tx_pow_to_dbm[16] = { 4, 4, 3, 3, 2, 2, 1,
                                         0, -1, -2, -3, -4, -6, -8, -12, -17 };
const uint8_t _233_dbm_to_tx_pow[22] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                         0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c,
                                         0x0b, 0x0b, 0x0a, 0x09, 0x08, 0x07,
                                         0x06, 0x05, 0x03, 0x00 };
/* AT86RF233: [-101, -52]*/
const int16_t _233_rx_sens_to_dbm[16] = { -101, -94, -91, -88, -85, -82, -79,
                                          -76, -73, -70, -67, -64, -61, -58, -55, -52 };
const uint8_t _233_dbm_to_rx_sens[50] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x01, 0x01, 0x01, 0x02, 0x02,
                                          0x02, 0x03, 0x03, 0x03, 0x04, 0x04,
                                          0x04, 0x05, 0x05, 0x05, 0x06, 0x06,
                                          0x06, 0x07, 0x07, 0x07, 0x08, 0x08,
                                          0x08, 0x09, 0x09, 0x09, 0x0a, 0x0a,
                                          0x0a, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c,
                                          0x0c, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e,
                                          0x0e, 0x0f };

/* AT86RF231: [-17, 3] */
/* AT86RF232: [-17, 3] */
const int16_t _231_232_tx_pow_to_dbm[16] = { 3, 3, 2, 2, 1, 1, 0,
                                             -1, -2, -3, -4, -5, -7, -9, -12, -17 };
const uint8_t _231_232_dbm_to_tx_pow[21] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                             0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c,
                                             0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                             0x05, 0x03, 0x00 };
/* AT86RF231: [-101, -49] */
/* AT86RF232: [-101, -49] */
const int16_t _231_232_rx_sens_to_dbm[16] = { -101, -91, -88, -85, -82, -79, -76
                                              -73, -70, -67, -64, -61, -58, -55, -52,
                                              -49 };
const uint8_t _231_232_a1_r2_dbm_to_rx_sens[53] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                    0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
                                                    0x01, 0x02, 0x02, 0x02, 0x03, 0x03,
                                                    0x03, 0x04, 0x04, 0x04, 0x05, 0x05,
                                                    0x05, 0x06, 0x06, 0x06, 0x07, 0x07,
                                                    0x07, 0x08, 0x08, 0x08, 0x09, 0x09,
                                                    0x09, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b,
                                                    0x0b, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d,
                                                    0x0d, 0x0e, 0x0e, 0x0e, 0x0f };

/* AT86RFA1:  [-16.5, 3.5] */
/* AT86RFR2:  [-16.5, 3.5] */
const int16_t _a1_r2_tx_pow_to_dbm[16] = { 4, 3, 3, 2, 2, 1, 0,
                                          -1, -2, -3, -4, -5, -7, -9, -12, -17 };
const uint8_t _a1_r2_dbm_to_tx_pow[22] = { 0x0f, 0x0f, 0x0f, 0x0e, 0x0e, 0x0e,
                                           0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c,
                                           0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06,
                                           0x05, 0x04, 0x02, 0x00 };
/* AT86RFA1: [-100, -48] */
/* AT86RFR2: [-100, -48] */
const int16_t _a1_r2_rx_sens_to_dbm[16] = { -100, -90, -87, -84, -83, -78, -75
                                            -72, -69, -66, -63, -60, -57, -54, -51,
                                            -48 };
