/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Shared CPU specific function for the STM32 CPU family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "periph_conf.h"
#include "periph_cpu_common.h"

uint32_t periph_apb_clk(uint8_t bus)
{
    if (bus == APB1) {
        return CLOCK_APB1;
    }
    else {
        return CLOCK_APB2;
    }
}

void periph_clk_en(uint8_t bus, uint32_t mask)
{
    if (bus == APB1) {
        RCC->APB1ENR |= mask;
    } else {
        RCC->APB2ENR |= mask;
    }
}

void periph_clk_dis(uint8_t bus, uint32_t mask)
{
    if (bus == APB1) {
        RCC->APB1ENR &= ~(mask);
    } else {
        RCC->APB2ENR &= ~(mask);
    }
}
