/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       GPIO driver implementation for the RP2040
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>

#include "bitarithm.h"
#include "board.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "io_reg.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define GPIO_PIN_NUMOF  30U

#ifdef MODULE_PERIPH_GPIO_IRQ
static gpio_cb_t _cbs[GPIO_PIN_NUMOF];
static void *_args[GPIO_PIN_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    assert(pin < GPIO_PIN_NUMOF);
    SIO->GPIO_OE_CLR = 1LU << pin;
    SIO->GPIO_OUT_CLR = 1LU << pin;

    switch (mode) {
    case GPIO_IN:
        {
            gpio_pad_ctrl_t pad_config = {
                .input_enable = 1,
                .schmitt_trig_enable = 1
            };
            gpio_io_ctrl_t io_config = {
                .function_select = FUNCTION_SELECT_SIO,
            };
            gpio_set_pad_config(pin, pad_config);
            gpio_set_io_config(pin, io_config);
        }
        break;
    case GPIO_IN_PD:
        {
            gpio_pad_ctrl_t pad_config = {
                .pull_down_enable = 1,
                .input_enable = 1,
                .schmitt_trig_enable = 1
            };
            gpio_io_ctrl_t io_config = {
                .function_select = FUNCTION_SELECT_SIO,
            };
            gpio_set_pad_config(pin, pad_config);
            gpio_set_io_config(pin, io_config);
        }
        break;
    case GPIO_IN_PU:
        {
            gpio_pad_ctrl_t pad_config = {
                .pull_up_enable = 1,
                .input_enable = 1,
                .schmitt_trig_enable = 1
            };
            gpio_io_ctrl_t io_config = {
                .function_select = FUNCTION_SELECT_SIO,
            };
            gpio_set_pad_config(pin, pad_config);
            gpio_set_io_config(pin, io_config);
        }
        break;
    case GPIO_OUT:
        {
            gpio_pad_ctrl_t pad_config = {
                .drive_strength = DRIVE_STRENGTH_12MA,
            };
            gpio_io_ctrl_t io_config = {
                .function_select = FUNCTION_SELECT_SIO,
            };
            gpio_set_pad_config(pin, pad_config);
            gpio_set_io_config(pin, io_config);
        }
        SIO->GPIO_OE_SET = 1LU << pin;
        break;
    default:
        return -ENOTSUP;
    }
    return 0;
}

bool gpio_read(gpio_t pin)
{
    if (SIO->GPIO_OE & (1LU << pin)) {
        /* pin is output: */
        return SIO->GPIO_OUT & (1LU << pin);
    }
    /* pin is input: */
    return SIO->GPIO_IN & (1LU << pin);
}

void gpio_set(gpio_t pin)
{
    SIO->GPIO_OUT_SET = 1LU << pin;
}

void gpio_clear(gpio_t pin)
{
    SIO->GPIO_OUT_CLR = 1LU << pin;
}

void gpio_toggle(gpio_t pin)
{
    SIO->GPIO_OUT_XOR = 1LU << pin;
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static void _irq_enable(gpio_t pin, unsigned flank)
{
    volatile uint32_t *irq_enable_regs = &IO_BANK0->PROC0_INTE0;
    volatile uint32_t *irq_ack_regs = &IO_BANK0->INTR0;
    /* There are 4 bits to control IRQs per pin, hence the configuration is split across multiple
     * I/O registers. The following calculates the position the four bits matching the given pin,
     * where idx refers to the I/O register and shift_amount to the position in the I/O register.
     */
    unsigned shift_amount = (pin & 0x7) << 2;
    unsigned idx = pin >> 3;
    /* make access atomic by disabling IRQs */
    unsigned irq_state = irq_disable();
    uint32_t value = irq_enable_regs[idx];
    /* clear any stray IRQs */
    io_reg_atomic_clear(&irq_ack_regs[idx], 0xFLU << shift_amount);
    /* first, clear previous setting */
    value &= ~(0xFLU << shift_amount);
    /* then, apply new setting */
    value |= flank << shift_amount;
    irq_enable_regs[idx] = value;
    irq_restore(irq_state);
    NVIC_EnableIRQ(IO_IRQ_BANK0_IRQn);
}

void gpio_irq_enable(gpio_t pin)
{
    io_reg_atomic_clear(gpio_io_register(pin), IO_BANK0_GPIO1_CTRL_IRQOVER_Msk);
}

void gpio_irq_disable(gpio_t pin)
{
    /* Beware: The two-bit IRQOVER value needs to be set to 0b10 == IRQ_OVERRIDE_LOW. This
     * implementation will set IRQOVER only to either 0b00 == IRQ_OVERRIDE_NORMAL or
     * 0b10 == IRQ_OVERRIDE_LOW. If we just set the most significant bit, this will result in
     * IRQOVER set to IRQ_OVERRIDE_LOW.
     *
     * IRQOVER must not be set by user code for this to work, though.
     */
    io_reg_atomic_set(gpio_io_register(pin), IRQ_OVERRIDE_LOW << IO_BANK0_GPIO1_CTRL_IRQOVER_Pos);
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    assert(pin < GPIO_PIN_NUMOF);
    int retval = gpio_init(pin, mode);
    if (retval) {
        return retval;
    }

    _cbs[pin] = cb;
    _args[pin] = arg;
    _irq_enable(pin, flank);

    return 0;
}

void isr_io_bank0(void)
{
    unsigned offset = 0;
    volatile uint32_t *irq_status_regs = &IO_BANK0->PROC0_INTS0;
    volatile uint32_t *irq_ack_regs = &IO_BANK0->INTR0;

    DEBUG("[rp0x00] GPIO IRQ mask:      %08x, %08x, %08x, %08x\n",
          (unsigned)IO_BANK0->PROC0_INTE0, (unsigned)IO_BANK0->PROC0_INTE1,
          (unsigned)IO_BANK0->PROC0_INTE2, (unsigned)IO_BANK0->PROC0_INTE3);
    DEBUG("[rp0x00] GPIO IRQ status:    %08x, %08x, %08x, %08x\n",
          (unsigned)IO_BANK0->PROC0_INTS0, (unsigned)IO_BANK0->PROC0_INTS1,
          (unsigned)IO_BANK0->PROC0_INTS2, (unsigned)IO_BANK0->PROC0_INTS3);

    /* There are four IRQ status bits per pin, so there is info for 8 pins per I/O register.
     * We will iterate over all IRQ status I/O registers in the outer loop, and over all 8 pins
     * per register in the inner loop */
    for (unsigned i = 0; i < (GPIO_PIN_NUMOF + 7) / 8; i++, offset += 8) {
        unsigned status = irq_status_regs[i];
        irq_ack_regs[i] = status;
        for (unsigned pin = 0; pin < 8; pin++) {
            /* Note: Not use bitarithm_test_and_clear() here, since two bits for a single pin could
             * be set. This happens if both rising and falling flank (1 bit used to track each) have
             * occurred until the ISR started to handle the first IRQ.
             */
            if (status & (0xFLU << (pin << 2))) {
                if (_cbs[pin + offset]) {
                    _cbs[pin + offset](_args[pin + offset]);
                }
            }
        }
    }
    cortexm_isr_end();
}

#endif /* MODULE_PERIPH_GPIO_IRQ */
