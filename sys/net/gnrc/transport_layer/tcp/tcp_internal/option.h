/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_INTERNAL_OPTION_H_
#define GNRC_TCP_INTERNAL_OPTION_H_

#include "helper.h"
#include "net/gnrc/tcp/hdr.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Offset without Options(in 32-Bit words) */
 #define OPTION_OFFSET_BASE 0x5
 #define OPTION_OFFSET_MAX  0xF
 
 #define GET_OFFSET( x ) ((( x ) & MSK_OFFSET) >> 12)

/* TCP Option Defines */
#define OPT_KIND_EOL   00      /* End of List */
#define OPT_KIND_NOP   01      /* No Operatrion */
#define OPT_KIND_MSS   02      /* Maximum Segment Size */

#define OPT_LENGTH_MSS 04      /* MSS Option Size is 4 byte */

/* Complete Option Macros */
#define OPT_MSS( x ) ((OPT_KIND_MSS << 24) | (OPT_LENGTH_MSS << 16) | ( x ))

/* Option Extraction Macros */
#define OPT_GET_KIND( x )   (((x) & 0xFF000000) >> 24)
#define OPT_GET_LENGTH( x ) (((x) & 0x00FF0000) >> 16)
#define OPT_GET_VAL_2B( x ) (((x) & 0x0000FFFF))

/**
 * @brief Parses options of a given tcp-header pktsnip.
 *
 * @param[out] tcb   transmission control block to memorize options.
 * @param[in]  hdr   tcp header to be checked
 *
.* @return           zero on success
 * @return           a negative value on error
 */
int8_t _option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif
