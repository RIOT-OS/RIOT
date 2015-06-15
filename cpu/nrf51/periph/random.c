/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Driver for the NRF51822 random number generator
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
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
    /* The RNG is initiated every time when RNG read is called
     * This reduces power consumption when RNG is not needed
     */
}

int random_read(char *buf, unsigned int num)
{
    unsigned int count = 0;

    /* power on RNG */
    NRF_RNG->POWER = 1;
    /* Generate events when RNG is finished */
    NRF_RNG->INTENSET = RNG_INTENSET_VALRDY_Msk;
    /* Start Task */
    NRF_RNG->TASKS_START = 1;
    /* NRF51 PAN #21 */
    NRF_RNG->EVENTS_VALRDY = 0;

    while (count < num) {
        /* sleep until number is generated */
        while (NRF_RNG->EVENTS_VALRDY == 0) {
            /* enable wake up on events for __WFE CPU sleep */
            SCB->SCR |= SCB_SCR_SEVONPEND_Msk;
            /* sleep until next event */
            __SEV();
            __WFE();
            __WFE();
        }

        buf[count++] = (char)NRF_RNG->VALUE;
        /* NRF51 PAN #21 */
        NRF_RNG->EVENTS_VALRDY = 0;
        /* clear interrupt state */
        NVIC_ClearPendingIRQ(RNG_IRQn);
    }

    /* power off RNG */
    NRF_RNG->POWER = 0;

    return count;
}

void random_poweron(void)
{
    /* RNG is powered on when needed */
}

void random_poweroff(void)
{
    /* RNG is powered off automaticly */
}

#endif /* RANDOM_NUMOF */
