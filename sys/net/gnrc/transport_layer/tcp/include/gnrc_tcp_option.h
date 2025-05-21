/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP option handling declarations.
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 */

#include <stdint.h>
#include "assert.h"
#include "net/tcp.h"
#include "net/gnrc/tcp/tcb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Helper function to build the MSS option.
 *
 * @param[in] mss   MSS value that should be set.
 *
 * @returns   MSS option value.
 */
static inline uint32_t _gnrc_tcp_option_build_mss(uint16_t mss)
{
    return (((uint32_t) TCP_OPTION_KIND_MSS << 24) |
            ((uint32_t) TCP_OPTION_LENGTH_MSS << 16) | mss);
}

/**
 * @brief Helper function to build the combined option and control flag field.
 *
 * @param[in]  nopts   Number of options.
 * @param[in]  ctl     Control flag field.
 *
 * @returns   Bitfield with encoded control bits and number of options.
 */
static inline uint16_t _gnrc_tcp_option_build_offset_control(uint16_t nopts, uint16_t ctl)
{
    assert(TCP_HDR_OFFSET_MIN <= nopts && nopts <= TCP_HDR_OFFSET_MAX);
    return (nopts << 12) | ctl;
}

/**
 * @brief Parses options of a given TCP header.
 *
 * @param[in,out] tcb   TCB holding the connection information.
 * @param[in]     hdr   TCP header to be parsed.
 *
 * @returns   Zero on success.
 *            Negative value on error.
 */
int _gnrc_tcp_option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

/** @} */
