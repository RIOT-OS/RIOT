/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx Power management
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_POWER_H
#define CC26XX_CC13XX_POWER_H

#include <cc26xx_cc13xx.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Power domains
 */
typedef enum {
    POWER_DOMAIN_PERIPHERALS, /**< Peripherals domain */
    POWER_DOMAIN_SERIAL, /**< Serial domain */
    POWER_DOMAIN_RFC, /**< RF Core domain */
    POWER_DOMAIN_CPU, /**< CPU domain */
    POWER_DOMAIN_VIMS, /**< VIMS domain */
} power_domain_t;

/**
 * @brief   Is power domain enabled?
 *
 * @param[in] domain The domain.
 *
 * @return true Is enabled.
 * @return false Is not enabled.
 */
bool power_is_domain_enabled(const power_domain_t domain);

/**
 * @brief   Enable the specified power domain.
 *
 * @params[in] domain The domain.
 */
void power_enable_domain(const power_domain_t domain);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26XX_CC13XX_POWER_H */

/*@}*/
