/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_xmc1000
 * @{
 *
 * @file
 * @brief           Code shared by peripheral drivers using the USIC
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef USIC_H_
#define USIC_H_

#include "cpu.h"
#include "periph/gating.h"
#include "periph/usic_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_xmc1000_usic USIC configuration
 * @ingroup cpu_xmc1000
 * @{
 */

/**
 * @brief   USIC channel initialization function
 * @{
 */
static inline void usic_init(const usic_channel_t *usic_ch,
                             const usic_brg_t brg,
                             const usic_fdr_t fdr)
{
    USIC_CH_TypeDef *usic = usic_ch->usic;

    const usic_mode_t *controls = usic_ch->mode;

    /* initialize input pin */
    gpio_init(usic_ch->rx_pin & 0xff, GPIO_DIR_IN, GPIO_NOPULL);

    /* clear gating, enable clock */
    GATING_CLEAR(USIC0);

    /* enable the USIC module */
    usic->KSCFG =
        (1 << USIC_CH_KSCFG_MODEN_Pos) |
        (1 << USIC_CH_KSCFG_BPMODEN_Pos);

    /* clear channel control register (reset mode bit) */
    usic->CCR = 0;

    /* configure fractional divider */
    usic->FDR = fdr.reg;

    /* configure baud rate generator */
    usic->BRG = brg.reg;

    /* configure input stages DX0 & DX2 */
    usic->DX0CR = (usic_ch->rx_pin >> 8) << USIC_CH_DX0CR_DSEL_Pos;
    usic->DX2CR =  controls->dx2_dsel << USIC_CH_DX2CR_DSEL_Pos;

    /* interrupt node point register */
    usic->INPR = controls->inpr;
    /* shift control register */
    usic->SCTR = controls->sctr;
    /* transmit control/status register */
    usic->TCSR = controls->tcsr;
    /* protocol control register */
    usic->PCR  = controls->pcr;
    /* channel control register */
    usic->CCR  = controls->ccr;

    /* ready to activate output pins */
    gpio_init(usic_ch->tx_pin & 0xff, GPIO_DIR_OUT | (usic_ch->tx_pin >> 8), GPIO_NOPULL);
}
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* USIC_H_ */
