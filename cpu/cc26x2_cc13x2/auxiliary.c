/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 AUX functions
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <assert.h>
#include "cpu.h"

/**
 * @brief   Order of operation modes
 *
 * This is to calculate which mode follows the other, to change step-by-step
 * the mode.
 *
 * @{
 */
#define OPMODE_PDA_ORDER  (0)
#define OPMODE_A_ORDER    (1)
#define OPMODE_LP_ORDER   (2)
#define OPMODE_PDLP_ORDER (3)
/** @} */

/** Array to map an operation mode to it's order when changing it */
static const uint8_t _opmode_to_order[4] = {
    OPMODE_A_ORDER,
    OPMODE_LP_ORDER,
    OPMODE_PDA_ORDER,
    OPMODE_PDLP_ORDER
};

/** Array to map an order to an operation mode, used to get the next operation
 * mode. This is because we need to change the operation in ordered steps */
static const uint8_t _order_to_opmode[4] = {
    AUX_SYSIF_OPMODEREQ_REQ_PDA,
    AUX_SYSIF_OPMODEREQ_REQ_A,
    AUX_SYSIF_OPMODEREQ_REQ_LP,
    AUX_SYSIF_OPMODEREQ_REQ_PDLP
};

void aux_sysif_opmode_change(uint32_t target_opmode)
{
    assert((target_opmode == AUX_SYSIF_OPMODEREQ_REQ_PDLP) ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_PDA)  ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_LP)   ||
           (target_opmode == AUX_SYSIF_OPMODEREQ_REQ_A));

    uint32_t current_opmode;
    uint32_t next_mode;

    /* Change AUX operation mode following hardware rules, operation mode
     * change needs to be done in order, and one step at a time:
     *
     * PDA -> A -> LP -> PDLP
     */
    do {
        current_opmode = AUX_SYSIF->OPMODEREQ;

        /* Wait for change ACK */
        while (current_opmode != AUX_SYSIF->OPMODEACK) {}

        if (current_opmode == target_opmode) {
            break;
        }

        /* At this point we aren't in the mode we want, now we calculate which
         * mode follows this and make the change to that mode, this is repeated
         * in this loop until we get to the desired mode */
        uint32_t current_order = _opmode_to_order[current_opmode];
        if (current_order < _opmode_to_order[target_opmode]) {
            next_mode = _order_to_opmode[current_order + 1];
        }
        else {
            next_mode = _order_to_opmode[current_order - 1];
        }

        /* Request next mode */
        AUX_SYSIF->OPMODEREQ = next_mode;
    } while (current_opmode != target_opmode);
}
