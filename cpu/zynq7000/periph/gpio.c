/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author		Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "zynq_interrupts.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define GPIO_BASEADDRESS	0xE000A000
#define BANK_OFFSET			0x40
#define DATA_OFFSET			0x40
#define DATA_RO_OFFSET		0x60
#define	DIRM_OFFSET			0x204
#define OEN_OFFSET			0x208
#define INT_MASK_OFFSET		0x20C
#define INT_ENABLE_OFFSET	0x210
#define INT_DISABLE_OFFSET	0x214
#define INT_STATUS_OFFSET	0x218
#define INT_TYPE_OFFSET		0x21C
#define INT_POL_OFFSET		0x220
#define INT_ANY_OFFSET		0x224

#define write_gpio_reg(offset, data) \
(*(volatile uint32_t *) (GPIO_BASEADDRESS + offset) = data)

#define read_gpio_reg(offset) \
(*(volatile uint32_t *) (GPIO_BASEADDRESS + offset))

#define BANK0_END 31
#define BANK1_END 53
#define BANK2_END 85
#define BANK3_END 117
#define NUMOF_BANK 4
#define NUMOF_GPIO BANK3_END

void gpio_isr (void *arg);

typedef struct
{
    gpio_cb_t cb; /**< callback to call on GPIO interrupt */
    void *arg; /**< argument passed to the callback */
} gpio_handle_t;

static gpio_handle_t gpio_config[NUMOF_BANK][32];

void gpio_get_bank_pin (gpio_t pin, uint8_t *bank, uint8_t *bank_position, uint32_t *bank_mask) {
    if (pin < BANK0_END) {
        *bank = 0;
        *bank_position = pin;
    }
    else if (pin < BANK1_END) {
        *bank = 1;
        *bank_position = pin - BANK0_END;
    }
    else if (pin < BANK2_END) {
        *bank = 2;
        *bank_position = pin - BANK0_END - BANK1_END;
    }
    else {
        *bank = 3;
        *bank_position = pin - BANK0_END - BANK1_END - BANK2_END;
    }
    *bank_mask = (1 << *bank_position);
}

int gpio_init (gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup) {

    if (pin > BANK3_END) {
        DEBUG("GPIO pin invalid\n");
        return -1;
    }
    uint8_t bank, bank_position;
    uint32_t bank_mask;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);

    uint32_t dirm_value = read_gpio_reg(DIRM_OFFSET+(bank*BANK_OFFSET));
    uint32_t oe_value = read_gpio_reg(OEN_OFFSET+(bank*BANK_OFFSET));

    if (dir == GPIO_DIR_OUT) {
        dirm_value |= bank_mask;
        oe_value |= bank_mask;
    }
    else {
        dirm_value &= ~bank_mask;
        oe_value |= bank_mask;
    }

    write_gpio_reg(DIRM_OFFSET+(bank*BANK_OFFSET), dirm_value);
    write_gpio_reg(OEN_OFFSET+(bank*BANK_OFFSET), oe_value);

    return 0;
}

int gpio_init_int (gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank, gpio_cb_t cb, void *arg) {

    uint32_t int_type, int_polarity, int_any;
    uint8_t bank, bank_position;
    uint32_t bank_mask;

    if (gpio_init (pin, GPIO_DIR_IN, pullup)) {
        return -1;
    }

    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);

    gpio_config[bank][bank_position].arg = arg;
    gpio_config[bank][bank_position].cb = cb;

    /* set trigger type */
    int_type = read_gpio_reg(INT_TYPE_OFFSET+(bank*BANK_OFFSET));
    int_polarity = read_gpio_reg(INT_POL_OFFSET+(bank*BANK_OFFSET));
    int_any = read_gpio_reg(INT_ANY_OFFSET+(bank*BANK_OFFSET));

    switch (flank)
        {
        case GPIO_RISING:
            int_type |= bank_mask;
            int_polarity |= bank_mask;
            int_any &= ~bank_mask;
            break;
        case GPIO_FALLING:
            int_type |= bank_mask;
            int_polarity &= ~bank_mask;
            int_any &= ~bank_mask;
            break;
        case GPIO_BOTH:
            int_type |= bank_mask;
            int_any |= bank_mask;
            break;
        case GPIO_LEVEL_LOW:
            int_type &= ~bank_mask;
            int_polarity &= ~bank_mask;
            break;
        case GPIO_LEVEL_HIGH:
            int_type &= ~bank_mask;
            int_polarity |= bank_mask;
            break;
        }

    write_gpio_reg(INT_TYPE_OFFSET+(bank*BANK_OFFSET), int_type);
    write_gpio_reg(INT_POL_OFFSET+(bank*BANK_OFFSET), int_polarity);
    write_gpio_reg(INT_ANY_OFFSET+(bank*BANK_OFFSET), int_any);

    /* clear int status */
    gpio_irq_enable (pin);

    register_interrupt_handler (GPIO_INT_ID, INT_HIGH_LEVEL, &gpio_isr, NULL);
    return 0;
}

void gpio_irq_enable (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask, value;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);

    /* clear int status */
    value = read_gpio_reg(INT_STATUS_OFFSET+(bank*BANK_OFFSET));
    value |= bank_mask;
    write_gpio_reg(INT_STATUS_OFFSET+(bank*BANK_OFFSET), value);

    /* enable int */
    value = read_gpio_reg(INT_ENABLE_OFFSET+(bank*BANK_OFFSET));
    value |= bank_mask;
    write_gpio_reg(INT_ENABLE_OFFSET+(bank*BANK_OFFSET), value);
}

void gpio_irq_disable (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask, value;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);

    /* disable int */
    value = read_gpio_reg(INT_DISABLE_OFFSET+(bank*BANK_OFFSET));
    value |= bank_mask;
    write_gpio_reg(INT_DISABLE_OFFSET+(bank*BANK_OFFSET), value);
}

int gpio_read (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);
    uint32_t value = read_gpio_reg(DATA_RO_OFFSET+(bank*4));
    value &= bank_mask;
    return (value = !0);
}

void gpio_set (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);
    uint32_t value = read_gpio_reg(DATA_OFFSET+(bank*4));
    value |= bank_mask;
    write_gpio_reg(DATA_OFFSET+(bank*4), value);
}

void gpio_clear (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);
    uint32_t value = read_gpio_reg(DATA_OFFSET+(bank*4));
    value &= ~bank_mask;
    write_gpio_reg(DATA_OFFSET+(bank*4), value);
}

void gpio_toggle (gpio_t pin) {
    uint8_t bank, bank_position;
    uint32_t bank_mask;
    gpio_get_bank_pin (pin, &bank, &bank_position, &bank_mask);
    uint32_t value = read_gpio_reg(DATA_OFFSET+(bank*4));
    value ^= bank_mask;
    write_gpio_reg(DATA_OFFSET+(bank*4), value);
}

void gpio_write (gpio_t pin, int value) {
    if (value) {
        gpio_set (pin);
    }
    else {
        gpio_clear (pin);
    }
}

void gpio_isr (void* arg) {
    uint32_t isr, bit;
    int bank, i;
    for (bank = 0; bank < NUMOF_BANK; bank++) {
        isr = read_gpio_reg(INT_STATUS_OFFSET+(bank*4)) & read_gpio_reg(INT_MASK_OFFSET+(bank*4));
        if (isr) {
            for (bit = 1, i = 0; i < 32; bit <<= 1, i++) {
                if (isr & bit) {
                    if (gpio_config[bank][i].cb) {
                        gpio_config[bank][i].cb (gpio_config[bank][i].arg);
                        write_gpio_reg(INT_STATUS_OFFSET+(bank*4), bit);
                    }
                }
            }
        }
    }
}
