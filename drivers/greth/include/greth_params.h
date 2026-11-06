/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_greth
 * @{
 *
 * @file
 * @brief       Default configuration for the GRETH Ethernet driver
 *
 * Board-specific values (base address, IRQ) are expected to be defined in
 * the board's @c periph_conf.h via @c GRETH_PARAM_BASE and @c GRETH_PARAM_IRQ.
 *
 * @author      Matvii Ivashchenko
 */

#include "greth.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GRETH default parameters
 *
 * Override in periph_conf.h or at compile time with -DGRETH_PARAM_BASE=...
 * @{
 */
#ifndef GRETH_PARAM_BASE
#  define GRETH_PARAM_BASE    (0xff984000UL) /**< APB base address */
#endif

#ifndef GRETH_PARAM_IRQ
#  define GRETH_PARAM_IRQ     (5U)           /**< PLIC interrupt line */
#endif

#ifndef GRETH_PARAM_MAC
/** @brief Default MAC address */
#  define GRETH_PARAM_MAC     { 0x02, 0x00, 0x00, 0x00, 0x00, 0x01 }
#endif

#ifndef GRETH_PARAMS
/** @brief Driver initialization parameter set */
#  define GRETH_PARAMS        { .base_addr = GRETH_PARAM_BASE, \
                                 .irq       = GRETH_PARAM_IRQ,  \
                                 .mac       = GRETH_PARAM_MAC }
#endif
/** @} */

/**
 * @brief   GRETH device configuration table
 */
static const greth_params_t greth_params[] = {
    GRETH_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
