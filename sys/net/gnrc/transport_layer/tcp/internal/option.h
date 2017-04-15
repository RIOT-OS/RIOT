/*
 * Copyright (C) 2015 Simon Brummer
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
 * @brief      Defines and Macros for TCP option handling
 *
 * @author     Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef GNRC_TCP_INTERNAL_OPTION_H_
#define GNRC_TCP_INTERNAL_OPTION_H_

#include "helper.h"
#include "net/tcp.h"
#include "net/gnrc/tcp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extract offset value from offet and ctl bit field.
 */
#define GET_OFFSET( x ) (((x) & MSK_OFFSET) >> 12)

/**
 * @brief Helper Function to build the MSS Option
 *
 * @param[in]  mss   tcp header to be checked
 *
 * @return   Valid MSS Option.
 */
uint32_t _option_build_mss(uint16_t mss);

/**
 * @brief Helper Function to build the combined option and control flag field
 *
 * @param[in]  nopts   Number of Options
 * @param[in]  ctl     Control Flags
 *
 * @return   Valid option size and control field.
 */
uint16_t _option_build_offset_control(uint16_t nopts, uint16_t ctl);

/**
 * @brief Parses options of a given tcp-header pktsnip.
 *
 * @param[out] tcb   transmission control block to memorize options.
 * @param[in]  hdr   tcp header to be checked
 *
 * @return   Zero on success
 * @return   A negative value on error
 */
int _option_parse(gnrc_tcp_tcb_t* tcb, tcp_hdr_t *hdr);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_OPTION_H_*/
/** @} */
