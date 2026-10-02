/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init
 * @{
 *
 * @file
 * @brief       Auto initialization for RadioLib package
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include "radiolib_riotos.h"
#include "radiolib_riotos_params.h"

#include <RadioLib.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static RiotHal _hal(CONFIG_RADIOLIB_SPI, CONFIG_RADIOLIB_SPI_CLK);

static Module _mod(&_hal,
                   CONFIG_RADIOLIB_SPI_NSS,
                   CONFIG_RADIOLIB_IRQ,
                   CONFIG_RADIOLIB_RESET,
                   CONFIG_RADIOLIB_GPIO);

#if IS_USED(MODULE_RADIOLIB_SX127X)
static SX1276 _radio = &_mod;
#elif IS_USED(MODULE_RADIOLIB_LR11X0)
static LR1110 _radio = &_mod;
#else
#  error "radiolib: no transceiver driver module selected"
#endif

/**
 * @brief   Board's RadioLib transceiver, ready to use after auto-init.
 *
 * NULL if @ref auto_init_radiolib failed to bring up the radio.
 */
PhysicalLayer *radiolib_radio = &_radio;

extern "C" void auto_init_radiolib(void)
{
    int16_t state = _radio.begin();

    if (state != RADIOLIB_ERR_NONE) {
        DEBUG("[radiolib] begin failed: %d\n", state);
        radiolib_radio = NULL;
        return;
    }

    state = _radio.setFrequency(CONFIG_RADIOLIB_FREQ);
    if (state != RADIOLIB_ERR_NONE) {
        DEBUG("[radiolib] setFrequency failed: %d\n", state);
        radiolib_radio = NULL;
    }
}
