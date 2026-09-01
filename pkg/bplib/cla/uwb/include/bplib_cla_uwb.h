/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once
/**
 * @defgroup pkg_bplib_cla_uwb UWB CLA Implementation
 * @ingroup pkg_bplib_cla
 * @brief Implementation of an experimental CLA over UWB using decadriver
 *
 * # About
 * Sends bundles via decadriver as 802.15.4 frames
 *
 * To use this, call bplib_cla_uwb_start(). The remote short address will
 * be taken from the @ref pkg_bplib_nc tables, so bplib_contact_set_out_addr()
 * needs to be configured before. The remote PAN is set as the port.
 * The local address and PAN is set with the corresponding
 * bplib_contact_set_in_addr() function.
 * Any incoming transmissions are accepted currently.
 *
 * The contact state is not managed by the CLA currently and needs to also be
 * set externally via BPLib_CLA_ContactStart() and related functions.
 *
 * @{
 *
 * @file
 * @brief       UWB CLA implementation.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */
#include <stdint.h>

#include "thread_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of the send AND receive buffer, each.
 *
 * This can be larger than the frame length, bundles will then be fragmented.
 */
#ifndef CONFIG_BPLIB_CLA_UWB_BUFLEN
#  define CONFIG_BPLIB_CLA_UWB_BUFLEN       (1023)
#endif

/**
 * @brief Maximum size of a single frame.
 *
 * For normal IEEE 802.15.4 operation this should be 127, but for DW3000
 * extended frames this can be up to 1023
 */
#ifndef CONFIG_BPLIB_CLA_UWB_MAX_FRAMELEN
#  define CONFIG_BPLIB_CLA_UWB_MAX_FRAMELEN (127)
#endif

/**
 * @brief The CLA polling from the socket and from bplib is stopped after this time [ms]
 */
#ifndef CONFIG_BPLIB_CLA_UWB_TIMEOUT
#  define CONFIG_BPLIB_CLA_UWB_TIMEOUT     (10000)
#endif

/**
 * @brief Number of retransmission attempts for all frames
 *
 * The first attempt is not counted, so the Min is 0.
 * Max is 255 (limited by uint8).
 */
#ifndef CONFIG_BPLIB_CLA_UWB_MAX_RETRANS
#  define CONFIG_BPLIB_CLA_UWB_MAX_RETRANS  (5)
#endif

/**
 * @brief Timeout for ACK receival on the sending side
 *
 * Unit is approx. us (really: UWB microseconds where 1 := 1.0256 us).
 * Max is 0xFFFFF (~1s)
 */
#ifndef CONFIG_BPLIB_CLA_UWB_ACK_TO
#  define CONFIG_BPLIB_CLA_UWB_ACK_TO       (0x00FFF)
#endif

/**
 * @brief Stack of RX thread
 *
 * Needs to be sufficiently large to do all of the bplib processing
 */
#ifndef BPLIB_CLA_UWB_RX_STACK_SIZE
#  define BPLIB_CLA_UWB_RX_STACK_SIZE       (THREAD_STACKSIZE_LARGE)
#endif

/**
 * @brief Stack of the TX thread
 *
 * Needs to be sufficiently large for the bplib egress processing
 */
#ifndef BPLIB_CLA_UWB_TX_STACK_SIZE
#  define BPLIB_CLA_UWB_TX_STACK_SIZE       (THREAD_STACKSIZE_MEDIUM)
#endif

/**
 * @brief Start the decadriver CLA
 *
 * @param contact_id The contact to claim and egress bundles from
 * @retval 0 on success
 * @retval -EINVAL on misconfiguration of addresses / already running
 * @retval other negative errno codes from thread_create
 */
int bplib_cla_uwb_start(uint32_t contact_id);

/**
 * @brief Initiates a stop of the CLA
 *
 * bplib's egress function may block the TX thread up to
 * CONFIG_BPLIB_CLA_UWB_TIMEOUT further ms until the thread exits.
 */
void bplib_cla_uwb_stop(void);

#ifdef __cplusplus
}
#endif

/** @} */
