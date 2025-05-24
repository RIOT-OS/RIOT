/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_atxmega-a1-xplained
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the ATxmegaA1 Xplained board.
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */
#include "mutex.h"

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
        .pwr            = PWR_RED_REG(PWR_PORT_C, PR_TC1_bm),
        .type           = TC_TYPE_1,
        .int_lvl        = { CPU_INT_LVL_LOW,
                            CPU_INT_LVL_OFF,
                            CPU_INT_LVL_OFF,
                            CPU_INT_LVL_OFF },
    },
    {
        .dev            = (void *)&TCC0,
        .pwr            = PWR_RED_REG(PWR_PORT_C, PR_TC0_bm),
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
    {   /* J1 */
        .dev            = &USARTF0,
        .pwr            = PWR_RED_REG(PWR_PORT_F, PR_USART0_bm),
        .rx_pin         = GPIO_PIN(PORT_F, 2),
        .tx_pin         = GPIO_PIN(PORT_F, 3),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin        = GPIO_UNDEF,
        .cts_pin        = GPIO_UNDEF,
#endif
        .rx_int_lvl     = CPU_INT_LVL_LOW,
        .tx_int_lvl     = CPU_INT_LVL_LOW,
        .dre_int_lvl    = CPU_INT_LVL_OFF,
    },
    {   /* J3 - Shared with SAUL */
        .dev            = &USARTD0,
        .pwr            = PWR_RED_REG(PWR_PORT_D, PR_USART0_bm),
        .rx_pin         = GPIO_PIN(PORT_D, 2),
        .tx_pin         = GPIO_PIN(PORT_D, 3),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin        = GPIO_UNDEF,
        .cts_pin        = GPIO_UNDEF,
#endif
        .rx_int_lvl     = CPU_INT_LVL_LOW,
        .tx_int_lvl     = CPU_INT_LVL_LOW,
        .dre_int_lvl    = CPU_INT_LVL_OFF,
    },
    {   /* J4 */
        .dev            = &USARTC0,
        .pwr            = PWR_RED_REG(PWR_PORT_C, PR_USART0_bm),
        .rx_pin         = GPIO_PIN(PORT_C, 2),
        .tx_pin         = GPIO_PIN(PORT_C, 3),
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
#define UART_0_RXC_ISR    USARTF0_RXC_vect    /* Reception Complete Interrupt */
#define UART_0_DRE_ISR    USARTF0_DRE_vect    /* Data Register Empty Interrupt */
#define UART_0_TXC_ISR    USARTF0_TXC_vect    /* Transmission Complete Interrupt */

#define UART_1_RXC_ISR    USARTD0_RXC_vect
#define UART_1_DRE_ISR    USARTD0_DRE_vect
#define UART_1_TXC_ISR    USARTD0_TXC_vect

#define UART_2_RXC_ISR    USARTC0_RXC_vect
#define UART_2_DRE_ISR    USARTC0_DRE_vect
#define UART_2_TXC_ISR    USARTC0_TXC_vect

#define UART_NUMOF        ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev                = &TWIF,
        .pwr                = PWR_RED_REG(PWR_PORT_F, PR_TWI_bm),
        .sda_pin            = GPIO_PIN(PORT_F, 0),
        .scl_pin            = GPIO_PIN(PORT_F, 1),
        .speed              = I2C_SPEED_NORMAL,
        .int_lvl            = CPU_INT_LVL_LOW,
    },
    {
        .dev                = &TWIC,
        .pwr                = PWR_RED_REG(PWR_PORT_C, PR_TWI_bm),
        .sda_pin            = GPIO_PIN(PORT_C, 0),
        .scl_pin            = GPIO_PIN(PORT_C, 1),
        .speed              = I2C_SPEED_NORMAL,
        .int_lvl            = CPU_INT_LVL_LOW,
    },
};

#define I2C_0_ISR           TWIF_TWIM_vect
#define I2C_1_ISR           TWIC_TWIM_vect

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev            = &SPIF,
        .pwr            = PWR_RED_REG(PWR_PORT_F, PR_SPI_bm),
        .sck_pin        = GPIO_PIN(PORT_F, 7),
        .miso_pin       = GPIO_PIN(PORT_F, 6),
        .mosi_pin       = GPIO_PIN(PORT_F, 5),
        .ss_pin         = GPIO_PIN(PORT_F, 4),
    },
    {
        .dev            = &SPIC,
        .pwr            = PWR_RED_REG(PWR_PORT_C, PR_SPI_bm),
        .sck_pin        = GPIO_PIN(PORT_C, 7),
        .miso_pin       = GPIO_PIN(PORT_C, 6),
        .mosi_pin       = GPIO_PIN(PORT_C, 5),
        .ss_pin         = GPIO_PIN(PORT_C, 4),
    },
};

#define SPI_NUMOF         ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name EBI configuration
 *
 * For more information, see ebi_conf_t structure.
 *
 * @{
 */
static const ebi_conf_t ebi_config = {
    .addr_bits              = 12,
    .flags                  = (EBI_PORT_SDRAM | EBI_PORT_3PORT),
    .sram_ale               = 0,
    .lpc_ale                = 0,
    .sdram                  = {
                                  0,
                                  1024,
                                  6400,
                                  EBI_CS_SDMODE_NORMAL_gc,
                                  EBI_SDRAM_CAS_LAT_3CLK,
                                  EBI_SDRAM_ROW_BITS_12,
                                  EBI_SDCOL_10BIT_gc,
                                  EBI_MRDLY_2CLK_gc,
                                  EBI_ROWCYCDLY_7CLK_gc,
                                  EBI_RPDLY_7CLK_gc,
                                  EBI_WRDLY_1CLK_gc,
                                  EBI_ESRDLY_7CLK_gc,
                                  EBI_ROWCOLDLY_7CLK_gc,
                              },
    .cs                     = { { EBI_CS_MODE_DISABLED_gc,
                                  0,
                                  EBI_CS_SRWS_0CLK_gc,
                                  0x0UL,
                                },
                                { EBI_CS_MODE_DISABLED_gc,
                                  0,
                                  EBI_CS_SRWS_0CLK_gc,
                                  0x0UL,
                                },
                                { EBI_CS_MODE_DISABLED_gc,
                                  0,
                                  EBI_CS_SRWS_0CLK_gc,
                                  0x0UL,
                                },
                                { EBI_CS_MODE_SDRAM_gc,
                                  EBI_CS_ASIZE_8MB_gc,
                                  EBI_CS_SRWS_0CLK_gc,
                                  0x0UL,
                                },
                              },
};
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_common.h"

/** @} */
