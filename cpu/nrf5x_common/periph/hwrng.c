/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Implementation of the hardware random number generator interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph/hwrng.h"
#ifndef MODULE_NORDIC_SOFTDEVICE_BLE
#include "assert.h"
#endif

void hwrng_init(void)
{
    /* enable bias correction */
    NRF_RNG->CONFIG = 1;
}

/*
 * The hardware peripheral is used by the SoftDevice. When the SoftDevice is
 * enabled, it shall only be accessed through the SoftDevice API
 */
#ifndef MODULE_NORDIC_SOFTDEVICE_BLE
void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    /* power on RNG */
#ifdef CPU_FAM_NRF51
    NRF_RNG->POWER = 1;
#endif
    NRF_RNG->INTENSET = RNG_INTENSET_VALRDY_Msk;
    NRF_RNG->TASKS_START = 1;

    /* read the actual random data */
    while (count < num) {
        /* sleep until number is generated */
        while (NRF_RNG->EVENTS_VALRDY == 0) {
            cortexm_sleep_until_event();
        }

        b[count++] = (uint8_t)NRF_RNG->VALUE;
        /* NRF51 PAN #21 -> read value before clearing VALRDY */
        NRF_RNG->EVENTS_VALRDY = 0;
        NVIC_ClearPendingIRQ(RNG_IRQn);
    }

    /* power off RNG */
    NRF_RNG->INTENCLR = RNG_INTENSET_VALRDY_Msk;
    NRF_RNG->TASKS_STOP = 1;
#ifdef CPU_FAM_NRF51
    NRF_RNG->POWER = 0;
#endif
}

#else

void hwrng_read(void *buf, unsigned int num)
{
    uint32_t ret;
    uint8_t avail;

    assert(num <= 0xff);

    /* this is not the most efficient, but this way we can assure that there are
     * enough bytes of random data available */
    do {
        sd_rand_application_bytes_available_get(&avail);
    } while (avail < (uint8_t)num);

    ret = sd_rand_application_vector_get((uint8_t *)buf, (uint8_t)num);
    assert(ret == NRF_SUCCESS);
    (void)ret;
}
#endif /* MODULE_NORDIC_SOFTDEVICE_BLE */
