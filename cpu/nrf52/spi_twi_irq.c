/*
 * Copyright (C) 2020 Inria
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
 * @brief       Shared IRQ handling between SPI and TWI peripherals on the nRF52
 *              devices
 *
 * I2C is called TWI (Two Wire Interface) in the datasheets from Nordic
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"

/* nRF52811 ISR names */
#if defined(CPU_MODEL_NRF52811XXAA)

#define ISR_SPIM0   isr_spi0
#define ISR_SPIM1   isr_spi1_twi0

/* nRF52832 and nRF52840 ISR names */
#elif (defined(CPU_MODEL_NRF52840XXAA) ||  defined(CPU_MODEL_NRF52832XXAA))

#define ISR_SPIM0   isr_spi0_twi0
#define ISR_SPIM1   isr_spi1_twi1

#else
#error Unknown nrf52 MCU model
#endif

static spi_twi_irq_cb_t _irq[SPIM_COUNT];
static void *_irq_arg[SPIM_COUNT];

/* I2C and SPI share peripheral addresses */
static size_t _spi_dev2num(void *dev)
{
    if (dev == NRF_SPIM0) {
        return 0;
    }
    else if (dev == NRF_SPIM1) {
        return 1;
    }
#ifdef NRF_SPIM2
    else if (dev == NRF_SPIM2) {
        return 2;
    }
#endif /* NRF_SPIM2 */
#ifdef NRF_SPIM3
    else if (dev == NRF_SPIM3) {
        return 3;
    }
#endif /* NRF_SPIM3 */
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
    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,

#ifdef CPU_MODEL_NRF52811XXAA
    TWIM0_TWIS0_TWI0_SPIM1_SPIS1_SPI1_IRQn,
#else
    SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
#endif

#ifdef NRF_SPIM2
    SPIM2_SPIS2_SPI2_IRQn,
#endif
#ifdef NRF_SPIM3
    SPIM3_IRQn,
#endif /* CPU_MODEL_NRF52840XXAA */
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

void ISR_SPIM0(void)
{
    _irq[0](_irq_arg[0]);
    cortexm_isr_end();
}

void ISR_SPIM1(void)
{
    _irq[1](_irq_arg[1]);
    cortexm_isr_end();
}

#ifdef NRF_SPIM2
void isr_spi2(void)
{
    _irq[2](_irq_arg[2]);
    cortexm_isr_end();
}
#endif

#ifdef NRF_SPIM3
void isr_spi3(void)
{
    _irq[3](_irq_arg[3]);
    cortexm_isr_end();
}
#endif /* NRF_SPIM3 */
