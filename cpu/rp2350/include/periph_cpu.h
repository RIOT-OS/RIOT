/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Peripheral CPU definitions for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include <stdbool.h>
#include <stdint.h>

#include "RP2350.h"
#include "clock_conf.h"
#include "cpu.h"
#include "gpio_conf.h"
#include "helpers.h"
#include "uart_conf.h"

/** Overwrite the default GPIO type to use uint32_t */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

/* Im currently copying the original rp2040 def but this causes the other port to not be addressable (I think)*/
#define GPIO_PIN(port, pin)     (((port) & 0) | (pin))

/* This is a define used throughout the pico sdk */
#define _u(x) ((uint32_t)(x))

#include "periph/gpio.h"

/** GPIO Pin ID for oscillator debugging */
#define OSC_DEBUG_PIN_ID 15u

/** Reset bit for the system PLL */
#define RESET_PLL_SYS (1u << 14u)

/** Reset bit for the pads bank 0 */
#define RESET_PADS_BANK0 (1u << 9u)

/** Reset bit for UART0 peripheral */
#define RESET_UART0 (1u << 26u)

/** Reset bit for UART1 peripheral */
#define RESET_UART1 (1u << 27u)

/** Reset bit for the IO bank 0 */
#define RESET_IO_BANK0 (1u << 6u)

/** Input enable bit for GPIO0 in PADS_BANK0 */
#define PADS_BANK0_GPIO0_IE_BITS (1u << 6u)

/** Isolation bits for PADS_BANK0 */
#define PADS_BANK0_ISO_BITS (1u << 8u)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Calculate the address of the GPIO pad register for a given pin
 * @param   pin     The GPIO pin number
 * @return  The address of the GPIO pad register for the given pin
 */
static inline uint32_t calculate_gpio_pad_register_addr(gpio_t pin) {
    /* Each pin has a 4 byte register, so we can calculate the address
    * by adding 4 bytes for each pin, starting at the base address of PADS_BANK0
    * and adding 4 bytes to skip VOLTAGE_SELECT */
    return PADS_BANK0_BASE + 4 * (pin + 1);
}

/**
 * @brief   Calculate the address of the GPIO IO status register for a given pin
 * @param   pin     The GPIO pin number
 * @return  The address of the GPIO IO status register for the given pin
 */
static uint32_t calculate_gpio_io_status_register_addr(gpio_t pin) {
    /* Each status register is followed by a ctrl register, */
    return IO_BANK0_BASE + 8 * pin;
}

/**
 * @brief   Calculate the address of the GPIO IO control register for a given
 *          pin
 * @param   pin     The GPIO pin number
 * @return  The address of the GPIO IO control register for the given pin
 */
static inline uint32_t calculate_gpio_io_ctrl_register_addr(gpio_t pin) {
    /* Each pin has a 8 byte register (4 Bytes of Status, 4 Bytes of CTRL),
     * so we can calculate the address by adding 8 bytes for each pin,
     * starting at the base address of IO_BANK0 */
    return calculate_gpio_io_status_register_addr(pin) + 4;
}

#ifdef __cplusplus
}
#endif

/** @} */
