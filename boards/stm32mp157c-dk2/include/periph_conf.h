/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32mp157c-dk2
 * @{
 *
 * @file
 * @brief       Board specific implementations for the STM32MP157C-DK2 board
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board provides an LSE */
#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE    1
#endif

/* This board provides an HSE */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE    1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART3,
        .rcc_mask   = RCC_MC_APB1ENSETR_USART3EN,
        .rx_pin     = GPIO_PIN(PORT_B, 12),
        .tx_pin     = GPIO_PIN(PORT_B, 10),
        .rx_af      = GPIO_AF8,
        .tx_af      = GPIO_AF7,
        .bus        = APB1,
        .irqn       = USART3_IRQn,
        .type       = STM32_USART,
#ifdef UART_USE_DMA
        .dma_stream = 4,
        .dma_chan   = 4,
#endif
#if IS_USED(MODULE_STM32MP1_ENG_MODE)
        .clk_src    = RCC_UART35CKSELR_UART35SRC_4, /* HSE clock source */
#else
        .clk_src    = RCC_UART35CKSELR_UART35SRC_2, /* HSI clock source */
#endif
    },
};

#define UART_0_ISR          (isr_usart3)
#define UART_0_DMA_ISR      (isr_dma1_stream4)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
