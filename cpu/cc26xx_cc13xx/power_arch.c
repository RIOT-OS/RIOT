/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @{
 *
 * @file
 * @brief       Power management abstractions
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "cpu.h"

#define DOMAIN_ON (1)

bool power_is_domain_enabled(const power_domain_t domain)
{
    switch (domain) {
        case POWER_DOMAIN_PERIPHERALS:
            return PRCM->PDSTAT0PERIPH == DOMAIN_ON;

        case POWER_DOMAIN_SERIAL:
            return PRCM->PDSTAT0SERIAL == DOMAIN_ON;

        case POWER_DOMAIN_RFC:
            /* At least one of the registers need to indicate that the power
             * domain is on */
            return (PRCM->PDSTAT1RFC == DOMAIN_ON) ||
                   (PRCM->PDSTAT0RFC == DOMAIN_ON);

        case POWER_DOMAIN_VIMS:
            return PRCM->PDSTAT1VIMS == DOMAIN_ON;

        case POWER_DOMAIN_CPU:
            return PRCM->PDSTAT1CPU == DOMAIN_ON;

        default:
            return false;
    }

    return false;
}

void power_enable_domain(const power_domain_t domain)
{
    switch (domain) {
        case POWER_DOMAIN_PERIPHERALS:
            PRCM->PDCTL0PERIPH = DOMAIN_ON;
            break;

        case POWER_DOMAIN_SERIAL:
            PRCM->PDCTL0SERIAL = DOMAIN_ON;
            break;

        case POWER_DOMAIN_RFC:
            /* On CC26x0 MCUs PDCTL1RFC needs to be written too in order to
             * enable the RF Core power domain. On `cc13x2_cc26x2` it's not
             * necessary and domain is powered normally. */
            PRCM->PDCTL0RFC = DOMAIN_ON;
            PRCM->PDCTL1RFC = DOMAIN_ON;
            break;

        case POWER_DOMAIN_CPU:
            PRCM->PDCTL1CPU = DOMAIN_ON;
            break;

        case POWER_DOMAIN_VIMS:
            PRCM->PDCTL1VIMS = DOMAIN_ON;
            break;
    }

    while (!power_is_domain_enabled(domain)) {}
}
