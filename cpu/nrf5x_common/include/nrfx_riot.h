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

#ifndef NRFX_RIOT_H
#define NRFX_RIOT_H

#include "kernel_defines.h"
#include "cpu_conf.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the internal DC/DC power converter for the NRF5x MCU.
 *
 * In most cases, the internal DC/DC converter is more efficient compared to the
 * LDO regulator. The downside of the DC/DC converter is that it requires an
 * external LC filter to be present on the board. Per default, the DC/DC
 * converter is enabled if an LC filter is present (VDD_LC_FILTER_REGx feature).
 *
 * Independent of the presence of the LC filter, the DC/DC converter(s) can be
 * disabled by blacklisting the VDD_LC_FILTER_REGx feature, e.g. build using
 * `FEATURES_BLACKLIST=VDD_LC_FILTER_REG1 make all`.
 */
static inline void nrfx_dcdc_init(void)
{
    if (IS_ACTIVE(MODULE_VDD_LC_FILTER_REG1)) {
        NRF_POWER->DCDCEN = 1;
    }

#ifdef POWER_DCDCEN0_DCDCEN_Msk
    /* on CPUs that support high voltage power supply via VDDH and thus use a
     * two stage regulator, we also try to enable the DC/DC converter for the
     * first stage */
    if (IS_ACTIVE(MODULE_VDD_LC_FILTER_REG0) &&
        (NRF_POWER->MAINREGSTATUS == POWER_MAINREGSTATUS_MAINREGSTATUS_High)) {
        NRF_POWER->DCDCEN0 = 1;
    }
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NRFX_RIOT_H */
