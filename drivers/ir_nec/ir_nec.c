/*
 * SPDX-FileCopyrightText: 2022 Dario Petrillo
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ir_nec
 * @{
 *
 * @file
 * @brief       Device driver implementation for the IR NEC Remote receiver
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 *
 * @}
 */

#include "ir_nec.h"
#include "ir_nec_constants.h"
#include "ir_nec_params.h"

#include <stdlib.h>
#include "ztimer.h"

#include "ir_nec.h"

static void ir_nec_isr(void *arg)
{
    ir_nec_t *ir = (ir_nec_t *)arg;

    if (gpio_read(ir->pin) != 0) { // rising edge
        ir->last_rising = ztimer_now(ZTIMER_USEC);
    }
    else { // falling edge
        int length = ztimer_now(ZTIMER_USEC) - ir->last_rising;

        if (abs(length - IR_NEC_START_US) <= IR_NEC_EPS_US) {
            ir->data = 0;
            ir->read_bits = 0;
        }
        else if ((length - IR_NEC_ZERO_US) <= IR_NEC_EPS_US && ir->read_bits < 32) {
            ir->data = (ir->data << 1);
            ir->read_bits++;
        }
        else if (abs(length - IR_NEC_ONE_US) <= IR_NEC_EPS_US && ir->read_bits < 32) {
            ir->data = (ir->data << 1) | 1;
            ir->read_bits++;
        }
        else { // Interpacket delay
            ir->read_bits = 0xff;
        }

        if (ir->read_bits == 32) {
            ir->read_bits = 0xff;

            uint8_t addr = (ir->data >> 24) & 0xff;
            uint8_t addr_inv = (ir->data >> 16) & 0xff;
            uint8_t cmd = (ir->data >> 8) & 0xff;
            uint8_t cmd_inv = ir->data & 0xff;

            if ((addr ^ addr_inv) == 0xff && (cmd ^ cmd_inv) == 0xff) {
                ir_nec_cmd_t command = { .addr = addr, .cmd = cmd };
                isrpipe_write(&ir->isrpipe, (void*)&command, sizeof(command));
            }
        }
    }
}

int ir_nec_read(ir_nec_t *ir, ir_nec_cmd_t *command)
{
    int to_read = sizeof(ir_nec_cmd_t);
    if (isrpipe_read(&ir->isrpipe, (void*)command, to_read) != to_read) {
        return -1;
    }
    return 0;
}

int ir_nec_init(ir_nec_t *ir, const ir_nec_params_t *params)
{
    ir->pin = params->pin;
    ir->data = 0;
    ir->last_rising = 0;
    ir->read_bits = 0;

    isrpipe_init(&ir->isrpipe, ir->isrpipe_buf, sizeof(ir->isrpipe_buf));
    gpio_init_int(ir->pin, GPIO_IN, GPIO_BOTH, ir_nec_isr, ir);

    return 0;
}
