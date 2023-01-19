/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RFC_H
#define CC26X2_CC13X2_RFC_H

#include <stdint.h>
#include <stdbool.h>

#include "cc26xx_cc13xx_rfc_common_cmd.h"
#include "cc26xx_cc13xx_rfc_mailbox.h"
#include "cc26xx_cc13xx_rfc_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Request the RF Core to power on.
 *
 *  - Powers on the radio core power domain.
 *  - Enables the RF Core clock gate.
 *  - Boot RF Core.
 */
void cc26x2_cc13x2_rfc_request_on(void);

/**
 * @brief   Confirm that the RF Core is on.
 *
 * @pre cc26x2_cc13x2_rfc_request_on MUST have been called before.
 *
 * @return -EAGAIN if the RF Core has not been started yet.
 * @return 0 if started.
 */
int cc26x2_cc13x2_rfc_confirm_on(void);

/**
 * @brief   Finalize the power on of the RF Core.
 *
 * @pre cc26x2_cc13x2_rfc_confirm_on MUST have been previously
 *      reported that the RF Core is on.
 *
 * - Switches the HF clock source to the XOSC crystal.
 * - Patches the CPE if @p cpe_patch_fn is provided.
 * - Turns on the clock line to the radio core, necessary
 *   for @ref RFC_CMD_SYNC_START_RAT commands.
 *
 * @param[in]   cpe_patch_fn    Function used to patch the CPE, can
 *                              be NULL if it doesn't needs to be
 *                              patched.
 */
void cc26x2_cc13x2_rfc_finish_on(void (* cpe_patch_fn)(void));

/**
 * @brief   Request the RF Core to execute a command.
 *
 * @param[in]   cmd   The command to send.
 *
 * @return CMDSTA register value.
 */
uint32_t cc26x2_cc13x2_rfc_request_execute(uintptr_t cmd);

/**
 * @brief   Confirm execution of the previously requested command
 *          execution.
 *
 * @pre cc26x2_cc13x2_rfc_request_execture MUST have been called before.
 *
 * @return -EAGAIN if the command has not been finished.
 * @return 0 if the command succesfully finished.
 */
int cc26x2_cc13x2_rfc_confirm_execute(void);

/**
 * @brief   Abort a running command.
 */
void cc26x2_cc13x2_rfc_abort_cmd(void);

/**
 * @brief   Is data available on the data queue?
 *
 * Loops over the queue to check for finished entries.
 *
 * @note The data queue must be configured as a circular buffer (no last entry).
 *
 * @note After processing the data entry you must set the `status` field to
 *       @ref RFC_DATA_ENTRY_PENDING so the radio CPU can use it again.
 *
 * @pre @p queue != NULL
 *
 * @param[in] queue The data queue to check.
 *
 * @return uint8_t * The finished data entry.
 * @return NULL      No data available.
 */
uint8_t *cc26x2_cc13x2_rfc_queue_recv(rfc_data_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* CC26X2_CC13X2_RFC_H */

/*@}*/
