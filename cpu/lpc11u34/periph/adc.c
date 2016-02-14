/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Paul Rathgeb <paul.rathgeb@skynet.be>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"

/**
 * @brief   Mutex to synchronize ADC access from different threads
 */
static mutex_t lock = MUTEX_INIT;

static inline uint32_t *pincfg_reg(adc_t line)
{
    int offset = (line < 6) ? (11 + line) : (16 + line);
    return ((uint32_t *)(LPC_IOCON)  + offset);
}

static inline void prep(void)
{
    mutex_lock(&lock);
    LPC_SYSCON->PDRUNCFG &= ~(1 << 4);
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
}

static inline void done(void)
{
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 13);
    LPC_SYSCON->PDRUNCFG |= (1 << 4);
    mutex_unlock(&lock);
}

int adc_init(adc_t line)
{
    uint32_t *pincfg;

    prep();

    /* ADC frequency : 3MHz */
    LPC_ADC->CR = (15 << 8);
    /* configure the connected pin */
    pincfg = pincfg_reg(line);
    /* Put the pin in its ADC alternate function */
    if (line < 5) {
        *pincfg |= 2;
    }
    else {
        *pincfg |= 1;
    }
    /* Configure ADMODE in analog input */
    *pincfg &= ~(1 << 7);

    done();

    return 0;
}

int adc_sample(adc_t line, adc_res_t res)
{
    int sample;

    /* check if resolution is valid */
    if (res < 0xff) {
        return -1;
    }

    /* prepare the device */
    prep();

    /* set resolution */
    LPC_ADC->CR &= ~(0x7 << 17);
    LPC_ADC->CR |= res;
    /* Start a conversion */
    LPC_ADC->CR |= (1 << line) | (1 << 24);
    /* Wait for the end of the conversion */
    while (!(LPC_ADC->DR[line] & (1 << 31))) {}
    /* Read and return result */
    sample = (LPC_ADC->DR[line] >> 6);

    done();

    return sample;
}
