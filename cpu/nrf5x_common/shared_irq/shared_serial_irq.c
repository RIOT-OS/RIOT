/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Shared IRQ handling between UART, SPI and TWI peripherals
 *              on the nRF53/9160 devices
 *
 *              I2C is called TWI (Two Wire Interface) in Nordic's documentation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph_cpu.h"
#include "periph_conf.h"

static shared_irq_cb_t _irq[SPIM_COUNT];
static void *_irq_arg[SPIM_COUNT];

static mutex_t _locks[SPIM_COUNT];

/* UART, I2C and SPI share peripheral addresses */
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
#ifdef NRF_SPIM4_S
    else if (dev == NRF_SPIM4_S) {
        return 4;
    }
#endif
    else {
        assert(false);
        return 0;
    }
}

static inline size_t _i2c_dev2num(void *dev)
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

static inline size_t _uart_dev2num(void *dev)
{
    /* I2C and UART have the same amount of instances */
    return _i2c_dev2num(dev);
}

#ifdef CPU_MODEL_NRF5340_APP
static const IRQn_Type _isr[] = {
SERIAL0_IRQn,
SERIAL1_IRQn,
SERIAL2_IRQn,
SERIAL3_IRQn,
SPIM4_IRQn
};
#define SERIAL0_ISR isr_serial0
#define SERIAL1_ISR isr_serial1
#define SERIAL2_ISR isr_serial2
#define SERIAL3_ISR isr_serial3
#define SERIAL4_ISR isr_spim4
#elif defined(CPU_NRF9160)
static const IRQn_Type _isr[] = {
UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn,
UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn,
UARTE2_SPIM2_SPIS2_TWIM2_TWIS2_IRQn,
UARTE3_SPIM3_SPIS3_TWIM3_TWIS3_IRQn
};
#define SERIAL0_ISR isr_uarte0_spim0_spis0_twim0_twis0
#define SERIAL1_ISR isr_uarte1_spim1_spis1_twim1_twis1
#define SERIAL2_ISR isr_uarte2_spim2_spis2_twim2_twis2
#define SERIAL3_ISR isr_uarte3_spim3_spis3_twim3_twis3
#else
#error "Missing shared IRQ configuration for this MCU."asm
#endif

void shared_irq_register_spi(NRF_SPIM_Type *bus,
                              shared_irq_cb_t cb, void *arg)
{
    size_t num = _spi_dev2num(bus);

    _irq[num] = cb;
    _irq_arg[num] = arg;
    NVIC_EnableIRQ(_isr[num]);
}

void shared_irq_register_i2c(NRF_TWIM_Type *bus,
                              shared_irq_cb_t cb, void *arg)
{
    size_t num = _i2c_dev2num(bus);

    _irq[num] = cb;
    _irq_arg[num] = arg;

    NVIC_EnableIRQ(_isr[num]);
}

void shared_irq_register_uart(NRF_UARTE_Type *bus,
                              shared_irq_cb_t cb, void *arg)
{
    size_t num = _uart_dev2num(bus);

    _irq[num] = cb;
    _irq_arg[num] = arg;

    NVIC_EnableIRQ(_isr[num]);
}

void nrf5x_i2c_acquire(NRF_TWIM_Type *bus,
                       shared_irq_cb_t cb, void *arg)
{
    size_t num = _i2c_dev2num(bus);
    mutex_lock(&_locks[num]);
    _irq[num] = cb;
    _irq_arg[num] = arg;
}

void nrf5x_spi_acquire(NRF_SPIM_Type *bus,
                       shared_irq_cb_t cb, void *arg)
{
    size_t num = _spi_dev2num(bus);
    mutex_lock(&_locks[num]);
    _irq[num] = cb;
    _irq_arg[num] = arg;
}

void nrf5x_i2c_release(NRF_TWIM_Type *bus)
{
    size_t num = _i2c_dev2num(bus);
    mutex_unlock(&_locks[num]);
}

void nrf5x_spi_release(NRF_SPIM_Type *bus)
{
    size_t num = _spi_dev2num(bus);
    mutex_unlock(&_locks[num]);
}

/* ISR Routines */
void SERIAL0_ISR(void)
{
    _irq[0](_irq_arg[0]);
    cortexm_isr_end();
}

void SERIAL1_ISR(void)
{
    _irq[1](_irq_arg[1]);
    cortexm_isr_end();
}

void SERIAL2_ISR(void)
{
    _irq[2](_irq_arg[2]);
    cortexm_isr_end();
}

void SERIAL3_ISR(void)
{
    _irq[3](_irq_arg[3]);
    cortexm_isr_end();
}

#ifdef SERIAL4_ISR
void serial4_isr(void)
{
    _irq[4](_irq_arg[4]);
    cortexm_isr_end();
}
#endif
