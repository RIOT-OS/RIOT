/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_saml21
 * @{
 *
 * @file
 * @brief           SAML21 IO helper stuff
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef SAML21_PERIPH_H
#define SAML21_PERIPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "cpu-conf.h"
#include "panic.h"

/**
 * @brief gpio dev to gpio id mapping table (used internally by implementation)
 */
extern const uint8_t gpio_table[];

/**
 * @brief return PORT ptr for GPIO pin
 *
 * @param[in] dev GPIO dev to get PORT for
 *
 * @returns pointer to PORT instance
 */
static inline PortGroup* _port(gpio_t dev)
{
    /* Config validity check. See saml21 periph_conf.h.
     * This gets optimized out if the configuration is correct.
     * I put it here so it crashes the board on first GPIO use. */
    if (GPIO_NUMOF != GPIO_UNUSED) {
        core_panic(0, "unmatched GPIO_NUMOF and GPIO_UNUSED. Fix periph_conf.\n");
    }
    return &PORT->Group[gpio_table[dev]/32];
}

/**
 * @brief return pin nr for gpio dev
 *
 * E.g., PA02 -> 2, PC19 -> 19
 *
 * @param[in] dev GPIO dev to get pin nr from
 * @returns pin nr of GPIO dev
 */
static inline uint32_t _pin(gpio_t dev)
{
    return gpio_table[dev] % 32;
}

/**
 * @brief Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] dev The Port ID to work on (port*32 + pin)
 * @param[in] mux required PMUX setting (Mapping A = 0, B=1, ...)
 *
 * @returns       0 on success, <0 on error
 */
int gpio_init_mux(gpio_t dev, uint32_t mux);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* SAML21_PERIPH_H */
