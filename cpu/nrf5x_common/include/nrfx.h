/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           nrfx compatibility layer
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#ifndef NRFX_H
#define NRFX_H

#include "cpu_conf.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the internal DC/DC power converter for the NRF5x MCU.
 *
 * The internal DC/DC converter is more efficient compared to the LDO regulator.
 * The downside of the DC/DC converter is that it requires an external inductor
 * to be present on the board. Enabling the DC/DC converter is guarded with
 * NRF5X_ENABLE_DCDC, this macro must be defined if the DC/DC converter is to be
 * enabled.
 */
static inline void nrfx_dcdc_init(void)
{
#ifdef NRF5X_ENABLE_DCDC
    NRF_POWER->DCDCEN = 1;

    /* on CPUs that support high voltage power supply via VDDH and thus use a
     * two stage regulator, we also enable the DC/DC converter for the first
     * state. */
#ifdef POWER_MAINREGSTATUS_MAINREGSTATUS_High
    if (NRF_POWER->MAINREGSTATUS == POWER_MAINREGSTATUS_MAINREGSTATUS_High) {
        NRF_POWER->DCDCEN0 = 1;
    }
#endif
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* NRFX_H */
/** @} */
