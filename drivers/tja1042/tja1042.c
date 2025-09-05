/*
 * SPDX-FileCopyrightText: 2016-2018 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_tja1042
 * @{
 *
 * @file
 * @brief       generic CAN transceiver implementation for tja1042
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <assert.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "tja1042.h"

int tja1042_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode)
{
    tja1042_trx_t *tja1042 = (tja1042_trx_t *)dev;
    int ret;

    DEBUG("tja1042_trx_set_mode: dev=%p, mode=%d\n", (void *)dev, (int)mode);

    switch (mode) {
    case TRX_NORMAL_MODE:
    case TRX_SILENT_MODE:
        gpio_clear(tja1042->stb_pin);
        ret = 0;
        break;

    case TRX_SLEEP_MODE:
        gpio_set(tja1042->stb_pin);
        ret = 0;
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}

int tja1042_trx_init(can_trx_t *dev)
{
    assert(dev != NULL);

    tja1042_trx_t *tja1042 = (tja1042_trx_t *)dev;

    gpio_init(tja1042->stb_pin, GPIO_OUT);
    tja1042_trx_set_mode(dev, TRX_NORMAL_MODE);

    return 0;
}

const trx_driver_t tja1042_driver = {
    .init = tja1042_trx_init,
    .set_mode = tja1042_trx_set_mode,
};
