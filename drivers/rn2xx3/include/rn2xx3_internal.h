/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Internal driver definitions for the RN2483/RN2903 LoRa modules
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdint.h>

#include "rn2xx3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Convert a string of hex to an array of bytes
 *
 * This functions is faster than `fmt_hex_bytes` that doesn't work if it is
 * used in the module UART interrupt callback.
 *
 * @param[in] hex          The string of hex to convert
 * @param[out] byte_array  The resulting array of bytes
 */
void rn2xx3_hex_to_bytes(const char *hex, uint8_t *byte_array);

/**
 * @brief   Sets internal device state
 *
 * @param[in] dev          The device descriptor
 * @param[in] state        The desired state
 */
void rn2xx3_set_internal_state(rn2xx3_t *dev, uint8_t state);

/**
 * @brief   Starts writing a command with the content of the device command buffer
 *
 * @param[in] dev          The device descriptor
 */
 void rn2xx3_cmd_start(rn2xx3_t *dev);

/**
 * @brief   Appends data to a command
 *
 * @param[in] dev          The device descriptor
 * @param[in] data         The data buffer
 * @param[in] data_len     The data max length
 */
void rn2xx3_cmd_append(rn2xx3_t *dev, const uint8_t *data, uint8_t data_len);

/**
 * @brief   Finalize a command
 *
 * @param[in] dev          The device descriptor
 *
 * @return                 RN2XX3_OK if the command succeeded
 * @return                 RN2XX3_ERR_* otherwise
 */
int rn2xx3_cmd_finalize(rn2xx3_t *dev);

/**
 * @brief   Starts writing a tx command
 *
 * @param[in] dev          The device descriptor
 */
 void rn2xx3_mac_tx_start(rn2xx3_t *dev);

/**
 * @brief   Finalize the TX command
 *
 * @param[in] dev          The device descriptor
 *
 * @return                 RN2XX3_OK if the command succeeded
 * @return                 RN2XX3_REPLY_* otherwise
 */
int rn2xx3_mac_tx_finalize(rn2xx3_t *dev);

/**
 * @brief   Process a command immediate response
 *
 * @param[in] dev          The device descriptor
 *
 * @return                 RN2XX3_OK if the command succeeded
 * @return                 RN2XX3_ERR_* otherwise
 */
int rn2xx3_process_response(rn2xx3_t *dev);

/**
 * @brief   Process a command network reply
 *
 * @param[in] dev          The device descriptor
 *
 * @return                 RN2XX3_OK if the command succeeded
 * @return                 RN2XX3_REPLY_* otherwise
 */
int rn2xx3_process_reply(rn2xx3_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
