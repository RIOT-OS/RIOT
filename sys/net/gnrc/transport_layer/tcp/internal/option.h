/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief       TCP option handling declarations
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef GNRC_TCP_INTERNAL_OPTION_H
#define GNRC_TCP_INTERNAL_OPTION_H

#include <stdint.h>
#include "assert.h"
#include "net/tcp.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Helper Function to build the MSS Option
 *
 * @param[in]  mss   tcp header to be checked
 *
 * @return   Valid MSS Option.
 */
inline static uint32_t _option_build_mss(uint16_t mss)
{
    return (((uint32_t) TCP_OPTION_KIND_MSS << 24) |
            ((uint32_t) TCP_OPTION_LENGTH_MSS << 16) | mss);
}

/**
 * @brief Helper Function to build the combined option and control flag field
 *
 * @param[in]  nopts   Number of Options
 * @param[in]  ctl     Control Flags
 *
 * @return   Valid option size and control field.
 */
inline static uint16_t _option_build_offset_control(uint16_t nopts, uint16_t ctl)
{
    assert(TCP_HDR_OFFSET_MIN <= nopts && nopts <= TCP_HDR_OFFSET_MAX);
    return (nopts << 12) | ctl;
}

/**
 * @brief Parses options of a given tcp-header pktsnip.
 *
 * @param[out] tcb   transmission control block to memorize options.
 * @param[in]  hdr   tcp header to be checked
 *
 * @return   Zero on success
 * @return   A negative value on error
 */
int _option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_OPTION_H*/
/** @} */
