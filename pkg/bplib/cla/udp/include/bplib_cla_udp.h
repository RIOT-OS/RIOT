/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once
/**
 * @defgroup pkg_bplib_cla_udp UDP CLA Implementation
 * @ingroup pkg_bplib_cla
 * @brief Implementation of the UDPCL.
 *
 * # About
 * Sends bundles via UDP, according to the legacy UDPCL. This legacy UDPCL
 * literally sends the bundles directly as UDP payload.
 *
 * To use this, call bplib_cla_udp_start(). Addresses will be taken from
 * the @ref pkg_bplib_nc tables, so bplib_contact_set_out_addr() and
 * bplib_contact_set_in_addr() need to be configured before.
 * Now the CLA is ready to receive but bplib does not know this contact is
 * active, so you also have to call BPLib_CLA_ContactSetup() and
 * BPLib_CLA_ContactStart().
 *
 * @{
 *
 * @file
 * @brief       UDPCL implementation.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include <stdint.h>

#include "thread_config.h"
#include "net/sock/udp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Size of the send AND receive buffer, each.
 */
#ifndef CONFIG_BPLIB_CLA_UDP_BUFLEN
#  define CONFIG_BPLIB_CLA_UDP_BUFLEN      (1024)
#endif

/**
 * @brief   The CLA polling from the socket and from bplib is stopped after this time [ms]
 */
#ifndef CONFIG_BPLIB_CLA_UDP_TIMEOUT
#  define CONFIG_BPLIB_CLA_UDP_TIMEOUT     (10000)
#endif

/** @brief RX stack size, including receive buffer */
#define BPLIB_CLA_UDP_RX_STACK_SIZE (THREAD_STACKSIZE_MEDIUM + CONFIG_BPLIB_CLA_UDP_BUFLEN)
/** @brief TX stack size, including egress buffer */
#define BPLIB_CLA_UDP_TX_STACK_SIZE (THREAD_STACKSIZE_MEDIUM + CONFIG_BPLIB_CLA_UDP_BUFLEN)

/**
 * @brief Holds an instance of a UDP CLA, including its stacks.
 *
 * This should only be used with the bplib_cla_udp_start() and bplib_cla_udp_stop()
 * functions
 */
typedef struct {
    /** @brief Whether the CLA is running */
    volatile int running;
    /** @brief Stack of the egress thread (bplib -> sock send) */
    char stack_tx[BPLIB_CLA_UDP_TX_STACK_SIZE];
    /** @brief Stock of the ingress thread (sock recv -> bplib) */
    char stack_rx[BPLIB_CLA_UDP_RX_STACK_SIZE];

    /** @brief Index of the contact in the contact table */
    uint32_t contact_id;

    /** @brief Connection socket */
    sock_udp_t sock;
} bplib_cla_udp_t;

/**
 * @brief Starts the UDP CLA.
 *
 * Should be called BEFORE starting the CLA on bplib's side, with
 * BPLib_CLA_ContactSetup() and BPLib_CLA_ContactStart().
 *
 * @param cla Instance struct, will be initialized. Has to be available during
 *            the lifetime of the CLA, until bplib_cla_udp_stop() is called.
 * @param contact_id The index of the contact in the contact table.
 * @return 0 on success, negative errno codes passed from sock_udp_create() or thread_create()
 */
int bplib_cla_udp_start(bplib_cla_udp_t* cla, uint32_t contact_id);

/**
 * @brief Stops the CLA.
 *
 * Since the ingress and egress threads are running and waiting for data of the UDP
 * socket or bplib, the CLA can only be guaranteed to stop only when the timeouts expire,
 * after BPLIC_CLA_UDP_TIMEOUT ms. The instance should still be valid during this time.
 *
 * Should be called AFTER stopping the CLA on bplib's side, with
 * BPLib_CLA_ContactStop() and BPLib_CLA_ContactTeardown()
 *
 * @param cla The instance of the UDP CLA.
 */
void bplib_cla_udp_stop(bplib_cla_udp_t* cla);

#ifdef __cplusplus
}
#endif

/** @} */
