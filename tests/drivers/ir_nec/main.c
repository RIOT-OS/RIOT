/*
 * Copyright (C) 2022 Dario Petrillo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
