/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_pindbg
 * @{
 *
 * @file
 * @brief       Initialization of configured direct controlled debug pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "pindbg.h"
#include "periph/gpio.h"

void pindbg_init(void)
{
#ifdef PINDBG0
    gpio_init(PINDBG0, GPIO_OUT);
#endif
#ifdef PINDBG1
    gpio_init(PINDBG1, GPIO_OUT);
#endif
}
