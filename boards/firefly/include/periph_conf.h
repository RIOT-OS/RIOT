/*
 * SPDX-FileCopyrightText: 2017 DAI Labor Technische Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_firefly
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Firefly  board revision A
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#include "cfg_adc_default.h"
#include "cfg_clk_default.h"
#include "cfg_i2c_default.h"
#include "cfg_timer_default.h"
#include "cfg_uart_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(PORT_B, 1),
        .miso_pin = GPIO_PIN(PORT_B, 3),
        .sck_pin  = GPIO_PIN(PORT_B, 2),
        .cs_pin   = SPI_CS_UNDEF,
    },
    {
        .num      = 1,
        .mosi_pin = GPIO_PIN(PORT_C, 5),
        .miso_pin = GPIO_PIN(PORT_C, 6),
        .sck_pin  = GPIO_PIN(PORT_C, 4),
        .cs_pin   = GPIO_PIN(PORT_A, 7)
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
