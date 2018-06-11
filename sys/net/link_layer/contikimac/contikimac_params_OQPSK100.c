/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       GNRC ContikiMAC timing settings for O-QPSK 100 kbit/s
 *
 * These timings are valid for STD IEEE 802.15.4 channel page 2 in the 868 MHz
 * band.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include "net/contikimac.h"

/* (usec) time to transmit one symbol */
#define SYMBOL_TIME 40
/* (usec) time to transmit one byte */
#define OCTET_TIME (SYMBOL_TIME * 2)
/* (usec) time to transmit the longest possible frame */
#define LONGEST_FRAME_TIME (OCTET_TIME * (5 + 1 + 1 + 127))
/* (usec) timeout waiting for Ack after TX complete */
#define ACK_TIMEOUT (54 * SYMBOL_TIME)

const contikimac_params_t contikimac_params_OQPSK100 = {
    .cca_cycle_period = ACK_TIMEOUT / 2,                /* T_c = T_i / (n_c - 1) */
    .inter_packet_interval = ACK_TIMEOUT,               /* T_i = Ack timeout */
    .after_ed_scan_timeout = LONGEST_FRAME_TIME + 200,  /* > T_l */
    .after_ed_scan_interval = (ACK_TIMEOUT * 3) / 4,    /* < T_i */
    .listen_timeout = ACK_TIMEOUT + 1000,               /* > T_i */
    .rx_timeout = LONGEST_FRAME_TIME + 500,             /* > T_l */
    .cca_count_max = 3,                                 /* = n_c */
};
