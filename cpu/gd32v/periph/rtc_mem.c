/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @{
 * @file
 * @brief       Low-level RTC backup memory implementation for GD32VF103
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/rtc_mem.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define RTC_MEM_SIZE    84      /* RTC data register size in byte */

extern void rtc_lock(void);
extern void rtc_unlock(void);

/* Since the registers are only 16-bit, but 32-bit aligned and not linearly
 * addressed, it makes more sense to write and read byte by byte instead of
 * using memcpy */

static volatile uint16_t *_rtc_mem_data_reg(unsigned addr)
{
    /* This function determines the register address of the 16-bit BKP data
     * register which are 32-bit aligned and not addressed linearly. The
     * layout is the following:
     *
     * addr  0,  1, ...,  9 are @0x40006c00 + 0x04, 0x08, ...,0x28
     * addr 10, 11, ..., 41 are @0x40006c00 + 0x40, 0x44, ...,0xbc
     */

    /* 16-bit data register index */
    unsigned reg_index = addr >> 1;
    /* 16-bit data register address as multiple of 32 bit */
    return (reg_index < 10) ? &BKP->DATA0 + (reg_index << 1)
                            : &BKP->DATA10 + ((reg_index - 10) << 1);
}

static void _rtc_mem_write_byte(unsigned addr, uint8_t byte)
{
    volatile uint16_t *reg = _rtc_mem_data_reg(addr);
    if (addr % 2) {
        /* high byte */
        *reg &= 0x00ff;
        *reg |= (uint16_t)byte << 8;
    }
    else {
        /* low byte */
        *reg &= 0xff00;
        *reg |= byte;
    }
}

static uint8_t _rtc_mem_read_byte(unsigned addr)
{
    volatile uint16_t *reg = _rtc_mem_data_reg(addr);
    return (addr % 2) ? (*reg & 0xff00) >> 8 : *reg & 0x00ff;
}

size_t rtc_mem_size(void)
{
    return RTC_MEM_SIZE;
}

void rtc_mem_write(unsigned offset, const void *data, size_t len)
{
    assert(offset + len <= rtc_mem_size());

    /* enable APB1 clocks */
    RCU->APB1EN |= RCU_APB1EN_PMUEN_Msk | RCU_APB1EN_BKPIEN_Msk;

    /* enable write access to backup domain registers */
    PMU->CTL |= PMU_CTL_BKPWEN_Msk;

    for (unsigned i = 0; i < len; i++) {
        _rtc_mem_write_byte(offset++, ((uint8_t *)data)[i]);
    }

    /* disable write access to backup domain registers */
    PMU->CTL &= ~PMU_CTL_BKPWEN_Msk;
}

void rtc_mem_read(unsigned offset, void *data, size_t len)
{
    assert(offset + len <= rtc_mem_size());

    /* enable APB1 clocks */
    RCU->APB1EN |= RCU_APB1EN_PMUEN_Msk | RCU_APB1EN_BKPIEN_Msk;

    for (unsigned i = 0; i < len; i++) {
        ((uint8_t *)data)[i] = _rtc_mem_read_byte(offset++);
    }
}
