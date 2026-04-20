/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once
/**
 * @defgroup pkg_bplib_cla_ble BLE L2CAP CLA Implementation
 * @ingroup pkg_bplib_cla
 * @brief Implementation of a basic CLA over BLE L2CAP.
 *
 * # About
 * Sends bundles via BLE between two statically connected devices.
 *
 * To use this, call bplib_cla_ble_start(). The remote MAC address of the
 * client will be taken from the @ref pkg_bplib_nc tables, so
 * bplib_contact_set_out_addr() needs to be configured before. The server
 * currently accepts any connection.
 *
 * Now the Convergence Layer Adapter (CLA) is ready to receive but bplib does
 * not know this contact is active, so you also have to call BPLib_CLA_ContactSetup()
 * and BPLib_CLA_ContactStart().
 *
 * For an extended BLE CLA which also handles discovery, these functions should
 * be called from within the CLA. A basic implementation of this can be enabled with
 * bplib_cla_ble_manage_state(), which lets the CLA start and stop the contact when
 * the BLE connection is disconnected.
 *
 * This configuration makes sense only for the client, since
 * for the server an "active" contact is not configured and any incoming connection
 * is accepted. So, the reachable (Endpoint IDs) EIDs, configured for a contact,
 * are not known to be valid for this connection.
 *
 * In the future this could be extended to feature an exchange of reachable endpoints,
 * automatic discovery and more reliable means of detecting when a connection is stable.
 *
 * @{
 *
 * @file
 * @brief       BLE CLA implementation.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Timeout after all CLA threads are stopped after bplib_cla_ble_stop, in ms */
#ifndef CONFIG_BPLIB_CLA_BLE_TIMEOUT
#  define CONFIG_BPLIB_CLA_BLE_TIMEOUT  10000
#endif

/** @brief MTU of an L2CAP message. Nimble's MSYS has to have enough buffer space. */
#ifndef CONFIG_BPLIB_CLA_BLE_MTU
#  define CONFIG_BPLIB_CLA_BLE_MTU      1000
#endif

/** @brief L2CAP CID to use. */
#ifndef CONFIG_BPLIB_CLA_BLE_CID
#  define CONFIG_BPLIB_CLA_BLE_CID      0xabc1
#endif

/**
 * @brief Start the CLA on BLE.
 *
 * No bundles are sent until BPLib_CLA_ContactSetup and BPLib_CLA_ContactStart
 * are called.
 *
 * @param contact_id The contact table index. This table should contain the
 *        destination MAC address to connect to
 * @param client true of the CLA should send bundles, false if it should receive
 *        bundles. Bidirectional connection are currently not supported.
 * @retval 0 on success
 * @retval 1 when the CLA is already started
 * @retval <0 negative errno codes from thread creation attempts if these fail.
 */
int bplib_cla_ble_start(uint32_t contact_id, bool client);

/**
 * @brief If the CLA should change the state of the contact when a disconnect is detected
 *
 * If this is enabled, the CLA will call BPLib_CLA_ContactStart and BPLib_CLA_ContactStop
 * functions, increasing the chances that bundles will be stored when the connection is
 * down. In the case of very unstable connections, i.e. repetitive disconnects and connects
 * this does not really help with increasing the reliability, since bundles get pulled from
 * bplib and BLE loses the bundle anyways.
 *
 * @param manage_state true to enable, false otherwise. Default: false
 */
void bplib_cla_ble_manage_state(bool manage_state);

/**
 * @brief Stops the CLA, and the BLE connection.
 *
 * May take up to CONFIG_BPLIB_CLA_BLE_TIMEOUT until the threads are stopped, since
 * they wait (timed) for bplib functions.
 */
void bplib_cla_ble_stop(void);

#ifdef __cplusplus
}
#endif

/** @} */
