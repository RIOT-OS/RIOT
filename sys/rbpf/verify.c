/*
 * Copyright (C) 2023 Inria
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "rbpf.h"
#include "rbpf/builtin_shared.h"
#include "rbpf/instruction.h"
#include "rbpf/config.h"

static bool _rbpf_check_call(uint32_t num)
{
    switch (num) {
    default:
        return rbpf_get_external_call(num) ? true : false;
    }
}


int rbpf_application_verify_preflight(rbpf_application_t *rbpf)
{
    const bpf_instruction_t *application = rbpf_application_text(rbpf);
    size_t length = rbpf_application_text_len(rbpf);

    if (rbpf->flags & RBPF_FLAG_PREFLIGHT_DONE) {
        return RBPF_OK;
    }

    if (length & 0x7) {
        return RBPF_ILLEGAL_LEN;
    }


    for (const bpf_instruction_t *i = application;
         i < (bpf_instruction_t *)((uint8_t *)application + length); i++) {
        /* Check if register values are valid */
        if (i->dst >= 11 || i->src >= 11) {
            return RBPF_ILLEGAL_REGISTER;
        }

        /* Double length instruction */
        if (i->opcode == 0x18) {
            i++;
            continue;
        }

        /* Only instruction-specific checks here */
        if ((i->opcode & BPF_INSTRUCTION_CLS_MASK) == BPF_INSTRUCTION_CLS_BRANCH) {
            intptr_t target = (intptr_t)(i + i->offset);
            /* Check if the jump target is within bounds. The address is
             * incremented after the jump by the regular PC increase */
            if ((target >= (intptr_t)((uint8_t *)application + length))
                || (target < (intptr_t)application)) {
                return RBPF_ILLEGAL_JUMP;
            }
        }

        if (i->opcode == (BPF_INSTRUCTION_BRANCH_CALL | BPF_INSTRUCTION_CLS_BRANCH)) {
            if (!_rbpf_check_call(i->immediate)) {
                return RBPF_ILLEGAL_CALL;
            }
        }
    }

    size_t num_instructions = length / sizeof(bpf_instruction_t);

    /* Check if the last instruction is a return instruction */
    if (application[num_instructions - 1].opcode != 0x95 &&
        !(rbpf->flags & RBPF_CONFIG_NO_RETURN)) {
        return RBPF_NO_RETURN;
    }
    rbpf->flags |= RBPF_FLAG_PREFLIGHT_DONE;
    return RBPF_OK;
}
