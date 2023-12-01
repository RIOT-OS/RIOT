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

#include "cpu_pm.h"
#include "irq.h"
#include "periph/pm.h"
#include "periph_conf.h"

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

    /* EBI Must be always enabled when configured */
#if defined (__AVR_ATxmega64A1__)   || \
    defined (__AVR_ATxmega64A1U__)  || \
    defined (__AVR_ATxmega128A1__)  || \
    defined (__AVR_ATxmega128A1U__)
    if (ebi_config.addr_bits > 0) {
        reg[0] &= ~PR_EBI_bm;
    }
#endif
}
