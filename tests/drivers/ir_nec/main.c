/*
 * SPDX-FileCopyrightText: 2022 Dario Petrillo
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the ir_nec driver
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include "ir_nec.h"
#include "ir_nec_params.h"

ir_nec_t remote;
ir_nec_cmd_t cmd;

int main(void)
{
    ir_nec_init(&remote, &ir_nec_params[0]);

    for (;;) {
        if (ir_nec_read(&remote, &cmd)) {
            puts("Error reading packet");
            return -1;
        }
        printf("Received packet with addr = 0x%X, cmd = 0x%X\n", cmd.addr, cmd.cmd);
    }
}
