/*
 * gpio.c - GPIO driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * This file is part of RIOT.
 */

#include "gpio.h"

static inline void gpio_pad_dir(volatile uint64_t data)
{
        GPIO->PAD_DIR0 = (data & 0xffffffff);
        GPIO->PAD_DIR1 = (data >> 32);
}

static inline void gpio_data(volatile uint64_t data)
{
        GPIO->DATA0 = (data & 0xffffffff);
        GPIO->DATA1 = (data >> 32);
}

static inline uint64_t gpio_data_get(volatile uint64_t bits) {
        uint64_t rdata = 0;

        rdata = GPIO->DATA0 & (bits & 0xffffffff);
        rdata |= (GPIO->DATA1 & (bits >> 32)) << 32;

        return rdata;
}

static inline void gpio_pad_pu_en(volatile uint64_t data)
{
        GPIO->PAD_PU_EN0 = (data & 0xffffffff);
        GPIO->PAD_PU_EN1 = (data >> 32);
}

static inline void gpio_data_sel(volatile uint64_t data)
{
        GPIO->DATA_SEL0 = (data & 0xffffffff);
        GPIO->DATA_SEL1 = (data >> 32);
}

static inline void gpio_pad_pu_sel(volatile uint64_t data)
{
        GPIO->PAD_PU_SEL0 = (data & 0xffffffff);
        GPIO->PAD_PU_SEL1 = (data >> 32);
}

static inline void gpio_data_set(volatile uint64_t data)
{
        GPIO->DATA_SET0 = (data & 0xffffffff);
        GPIO->DATA_SET1 = (data >> 32);
}

static inline void gpio_data_reset(volatile uint64_t data)
{
        GPIO->DATA_RESET0 = (data & 0xffffffff);
        GPIO->DATA_RESET1 = (data >> 32);
}

static inline void gpio_pad_dir_set(volatile uint64_t data)
{
        GPIO->PAD_DIR_SET0 = (data & 0xffffffff);
        GPIO->PAD_DIR_SET1 = (data >> 32);
}

static inline void gpio_pad_dir_reset(volatile uint64_t data)
{
        GPIO->PAD_DIR_RESET0 = (data & 0xffffffff);
        GPIO->PAD_DIR_RESET1 = (data >> 32);
}
