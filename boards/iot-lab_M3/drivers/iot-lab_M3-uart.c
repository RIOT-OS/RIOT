/**
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file   iot-lab_M3-uart.c
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "stm32f10x.h"
#include "periph/uart.h"

int fw_puts(char *astring, int length)
{
    for (int i = 0; i < length; i++) {
        uart_write_blocking(UART_0, astring[i]);
    }

    return length;
}
