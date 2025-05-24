/*
 * Copyright (C) 2021 Franz Freitag, Justus Krebs, Nick Weiler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_rpi_pico
 * @ingroup     boards_rpi_pico_w
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BOARD_RPI_PICO

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = LED0_NAME,
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
    }
};

#endif

#ifdef BOARD_RPI_PICO_W

/**
 * @brief    Empty GPIO pin configuration
 */
#define SAUL_GPIO_NUMOF 0

#endif

#ifdef __cplusplus
}
#endif

/** @} */
