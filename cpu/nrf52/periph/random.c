/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf52832
 * @{
 *
 * @file
 * @brief       Driver for the NRF52832 random number generator
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/random.h"

/* guard file in case no random device was specified */
#if RANDOM_NUMOF

void random_init(void)
{
    /* NRF_RNG->POWER = 1; */
    NRF_RNG->TASKS_START = 1;
}

int random_read(char *buf, unsigned int num)
{
    unsigned int count = 0;

    while (count < num) {
        while (NRF_RNG->EVENTS_VALRDY == 0);

        NRF_RNG->EVENTS_VALRDY = 0;
        buf[count++] = (char)NRF_RNG->VALUE;
    }

    return count;
}

#endif /* RANDOM_NUMOF */
