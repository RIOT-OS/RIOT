/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf52
 * @{
 *
 * @file
 * @brief       Implementation of the random number generator interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/random.h"

void random_init(void)
{
    /* nothing to do here */
}

int random_read(char *buf, unsigned int num)
{
    unsigned int count = 0;

    NRF_RNG->TASKS_START = 1;

    while (count < num) {
        while (NRF_RNG->EVENTS_VALRDY == 0);

        NRF_RNG->EVENTS_VALRDY = 0;
        buf[count++] = (char)NRF_RNG->VALUE;
    }

    NRF_RNG->TASKS_STOP = 1;

    return count;
}

void random_poweron(void)
{
    /* nothing to do here */
}

void random_poweroff(void)
{
    /* nothing to do here */
}
