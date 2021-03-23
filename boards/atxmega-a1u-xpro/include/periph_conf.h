/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_atxmega-a1u-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the ATxmegaA1U Xplained Pro board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */
#include "mutex.h"

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <avr/io.h>

#include "periph_cpu.h"

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev            = (void *)&TCC1,
        .type           = TC_TYPE_1,
        .int_lvl        = { CPU_INT_LVL_LOW,
                            CPU_INT_LVL_OFF,
                            CPU_INT_LVL_OFF,
                            CPU_INT_LVL_OFF },
    },
    {
        .dev            = (void *)&TCC0,
        .type           = TC_TYPE_0,
        .int_lvl        = { CPU_INT_LVL_LOW,
                            CPU_INT_LVL_LOW,
                            CPU_INT_LVL_LOW,
                            CPU_INT_LVL_LOW },
    }
};

#define TIMER_0_ISRA      TCC1_CCA_vect

#define TIMER_1_ISRA      TCC0_CCA_vect
#define TIMER_1_ISRB      TCC0_CCB_vect
#define TIMER_1_ISRC      TCC0_CCC_vect
#define TIMER_1_ISRD      TCC0_CCD_vect

#define TIMER_NUMOF       ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {   /* CDC-ACM */
        .dev            = &USARTE0,
        .rx_pin         = GPIO_PIN(PORT_E, 2),
        .tx_pin         = GPIO_PIN(PORT_E, 3),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin        = GPIO_UNDEF,
        .cts_pin        = GPIO_UNDEF,
#endif
        .rx_int_lvl     = CPU_INT_LVL_LOW,
        .tx_int_lvl     = CPU_INT_LVL_LOW,
        .dre_int_lvl    = CPU_INT_LVL_OFF,
    },
};

/* interrupt function name mapping */
#define UART_0_RXC_ISR    USARTE0_RXC_vect    /* Reception Complete Interrupt */
#define UART_0_DRE_ISR    USARTE0_DRE_vect    /* Data Register Empty Interrupt */
#define UART_0_TXC_ISR    USARTE0_TXC_vect    /* Transmission Complete Interrupt */

#define UART_NUMOF        ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_common.h"

#endif /* PERIPH_CONF_H */
/** @} */
