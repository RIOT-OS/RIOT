/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf9160
 * @{
 *
 * @file
 * @brief       Shared IRQ handling between SPI and TWI peripherals on the nRF52
 *              devices
 *
 *              I2C is called TWI (Two Wire Interface) in the datasheets from Nordic
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"

static spi_twi_irq_cb_t _irq[TWIM_COUNT];
static void *_irq_arg[TWIM_COUNT];

#if TWIM_COUNT != SPIM_COUNT
#error Possible configuration issue, please update this file
#endif

/* I2C and SPI share peripheral addresses */
static size_t _spi_dev2num(void *dev)
{
    if (dev == NRF_SPIM0_S) {
        return 0;
    }
    else if (dev == NRF_SPIM1_S) {
        return 1;
    }
    else if (dev == NRF_SPIM2_S) {
        return 2;
    }
    else if (dev == NRF_SPIM3_S) {
        return 3;
    }
    else {
        assert(false);
        return 0;
    }
}

static inline size_t _i2c_dev2num(void *dev)
{
    return _spi_dev2num(dev);
}

static const IRQn_Type _isr[] = {
UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn,
UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn,
UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQn,
UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQn
};

void spi_twi_irq_register_spi(NRF_SPIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg)
{
    size_t num = _spi_dev2num(bus);

    _irq[num] = cb;
    _irq_arg[num] = arg;
    NVIC_EnableIRQ(_isr[num]);
}

void spi_twi_irq_register_i2c(NRF_TWIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg)
{
    size_t num = _i2c_dev2num(bus);

    _irq[num] = cb;
    _irq_arg[num] = arg;

    NVIC_EnableIRQ(_isr[num]);
}

/* Check if UART driver doesn't already use the same IRQ */
#ifndef UART_0_ISR
void isr_uarte0_spim0_spis0_twim0_twis0(void)
{
    _irq[0](_irq_arg[0]);
    cortexm_isr_end();
}
#endif /* ndef UART_0_ISR */

#ifndef UART_1_ISR
void isr_uarte1_spim1_spis1_twim1_twis1(void)
{
    _irq[1](_irq_arg[1]);
    cortexm_isr_end();
}
#endif /* ndef UART_1_ISR */

#ifndef UART_2_ISR
void isr_uarte2_spim2_spis2_twim2_twis2(void)
{
    _irq[2](_irq_arg[2]);
    cortexm_isr_end();
}
#endif /* ndef UART_2_ISR */

#ifndef UART_3_ISR
void isr_uarte3_spim3_spis3_twim3_twis3(void)
{
    _irq[3](_irq_arg[3]);
    cortexm_isr_end();
}
#endif /* ndef UART_3_ISR */
