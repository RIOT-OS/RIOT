/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for ieee802154_hal
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "kernel_defines.h"
#include "net/ieee802154/radio.h"

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#ifdef MODULE_CC2538_RF
extern ieee802154_dev_t cc2538_rf_dev;
#endif

#ifdef MODULE_NRF802154
extern ieee802154_dev_t nrf802154_hal_dev;
#endif

#define RADIOS_NUMOF IS_USED(MODULE_CC2538_RF) + \
                     IS_USED(MODULE_NRF802154)

#if RADIOS_NUMOF == 0
#error "Radio is not supported"
#endif

static ieee802154_dev_t *_radios[RADIOS_NUMOF];

static void _register_radios(void)
{
    int i=0;

#ifdef MODULE_CC2538_RF
    _radios[i++] = &cc2538_rf_dev;
#endif

#ifdef MODULE_NRF802154
    _radios[i++] = &nrf802154_hal_dev;
#endif

    assert(i == RADIOS_NUMOF);
}

void ieee802154_hal_test_init_devs(void)
{
    /* Register all radios */
    _register_radios();

    /* Call the init function of the device (this should be handled by
     * `auto_init`) */
#ifdef MODULE_CC2538_RF
    cc2538_init();
#endif

#ifdef MODULE_NRF802154
    nrf802154_init();
#endif
}

ieee802154_dev_t *ieee802154_hal_test_get_dev(int id)
{
    assert(id < RADIOS_NUMOF);
    return _radios[id];
}
