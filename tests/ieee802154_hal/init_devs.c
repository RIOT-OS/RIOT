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
#include "event.h"
#include "event/thread.h"

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_CC2538_RF
extern ieee802154_dev_t cc2538_rf_dev;
#endif

#ifdef MODULE_NRF802154
extern ieee802154_dev_t nrf802154_hal_dev;
#endif

#ifdef MODULE_AT86RF2XX
#define AT86RF2XX_NUM ARRAY_SIZE(at86rf2xx_params)
static at86rf2xx_t at86rf2xx_devs[AT86RF2XX_NUM];

#endif

/* TODO: Fix */
#define RADIOS_NUMOF IS_USED(MODULE_CC2538_RF) + \
                     IS_USED(MODULE_NRF802154) + \
                     IS_USED(MODULE_AT86RF2XX)

#if RADIOS_NUMOF == 0
#error "Radio is not supported"
#endif

static ieee802154_dev_t _radios[RADIOS_NUMOF];
static void at86rf2xx_handler(event_t *ev)
{
    (void) ev;
   at86rf2xx_task_handler(&_radios[0]);
}

static event_t ev = {
    .handler = at86rf2xx_handler,
};

void at86rf2xx_irq_handler(void *arg)
{
    (void) arg;
    at86rf2xx_devs[0].pending_irq = true;
    event_post(EVENT_PRIO_HIGHEST, &ev);
}

static void _register_radios(void)
{
    int i=0;

#ifdef MODULE_CC2538_RF
    _radios[i++] = &cc2538_rf_dev;
#endif

#ifdef MODULE_NRF802154
    _radios[i++] = &nrf802154_hal_dev;
#endif

    /* TODO: fix */
#ifdef MODULE_AT86RF2XX
    at86rf2xx_init_hal(&at86rf2xx_devs[0], &_radios[0]);
    i++;
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

#ifdef MODULE_AT86RF2XX
    at86rf2xx_init(&at86rf2xx_devs[0], &at86rf2xx_params[0], NULL);
#endif
}

ieee802154_dev_t *ieee802154_hal_test_get_dev(int id)
{
    assert(id < RADIOS_NUMOF);
    return &_radios[id];
}
