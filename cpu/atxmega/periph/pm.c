/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level PM driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <avr/sleep.h>

#include "periph_conf.h"
#include "periph/pm.h"
#include "cpu_pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define PWR_REG_BASE        ((uint16_t)&PR)
#define PWR_REG_OFFSET      (0x01)

/**
 * @brief     Extract the device id of the given power reduction mask
 */
static inline uint8_t _device_mask(pwr_reduction_t pwr)
{
    return (pwr & 0xff);
}

/**
 * @brief     Extract the register id of the given power reduction mask
 */
static inline uint8_t _register_id(pwr_reduction_t pwr)
{
    return (pwr >> 8) & 0xff;
}

/**
 * @brief     Generate the register index of the given power reduction mask
 */
static inline uint8_t *_register_addr(pwr_reduction_t pwr)
{
    uint8_t id = _register_id(pwr);
    uint16_t addr = PWR_REG_BASE + (id * PWR_REG_OFFSET);

    return (uint8_t *)addr;
}

void pm_reboot(void)
{
    DEBUG("Reboot Software Reset\n" );

    /* XMEGA AU [MANUAL] p. 116 CTRL->Control register
     * page 13 3.12.1 Sequence for write operation to protected I/O registers
     * page 15 3.14.1 CCP – Configuration Change Protection register
     */

    /* Disable CCP for Protected IO registerand set new value*/
    _PROTECTED_WRITE(RST_CTRL, RST_SWRST_bm);
    while (1) {}
}

/*
 * DEBUG may affect this routine.
 *
 * --- Do NOT add DEBUG macro here ---
 *
 */
void pm_set(unsigned mode)
{
    unsigned irq_state = irq_disable();

    if (avr8_is_uart_tx_pending() && mode < 4) {
        irq_restore(irq_state);
        return;
    }

    switch (mode) {
    case 0:
        set_sleep_mode(SLEEP_SMODE_PDOWN_gc);
        break;
    case 1:
        set_sleep_mode(SLEEP_SMODE_PSAVE_gc);
        break;
    case 2:
        set_sleep_mode(SLEEP_SMODE_STDBY_gc);
        break;
    case 3:
        set_sleep_mode(SLEEP_SMODE_ESTDBY_gc);
        break;
    default:
        set_sleep_mode(SLEEP_SMODE_IDLE_gc);
    }
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
    irq_restore(irq_state);
}

void pm_periph_enable(pwr_reduction_t pwr)
{
    uint8_t mask = _device_mask(pwr);
    uint8_t *reg = _register_addr(pwr);

    *reg &= ~mask;
}

void pm_periph_disable(pwr_reduction_t pwr)
{
    uint8_t mask = _device_mask(pwr);
    uint8_t *reg = _register_addr(pwr);

    *reg |= mask;
}

void pm_periph_power_off(void)
{
    uint8_t *reg = _register_addr(PWR_GENERAL_POWER);
    uint8_t i;

    /* Freeze all peripheral clocks */
    for (i = 0; i <= 7; i++) {
        reg[i] = 0xff;
    }
}
