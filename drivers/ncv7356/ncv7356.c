/*
 * SPDX-FileCopyrightText: 2016-2018 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ncv7356
 * @{
 *
 * @file
 * @brief       generic can transceiver implementation for ncv7356
 *
 * @author      Aurelien GONCE <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include "ncv7356.h"

int ncv7356_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode)
{
    ncv7356_trx_t *ncv7356 = (ncv7356_trx_t *)dev;
    int ret;

    switch (mode) {
    case TRX_NORMAL_MODE:
    case TRX_SILENT_MODE:
        gpio_set(ncv7356->mode0_pin);
        gpio_set(ncv7356->mode1_pin);
        ret = 0;
        break;

    case TRX_HIGH_SPEED_MODE:
        gpio_set(ncv7356->mode0_pin);
        gpio_clear(ncv7356->mode1_pin);
        ret = 0;
        break;

    case TRX_HIGH_VOLTAGE_WAKE_UP_MODE:
        gpio_clear(ncv7356->mode0_pin);
        gpio_set(ncv7356->mode1_pin);
        ret = 0;
        break;

    case TRX_SLEEP_MODE:
        gpio_clear(ncv7356->mode0_pin);
        gpio_clear(ncv7356->mode1_pin);
        ret = 0;
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}

int ncv7356_trx_init(can_trx_t *dev)
{
    ncv7356_trx_t *ncv7356 = (ncv7356_trx_t *)dev;

    gpio_init(ncv7356->mode0_pin, GPIO_OUT);
    gpio_init(ncv7356->mode1_pin, GPIO_OUT);
    ncv7356_trx_set_mode((can_trx_t *)dev, TRX_NORMAL_MODE);

    return 0;
}

const trx_driver_t ncv7356_driver = {
    .init = ncv7356_trx_init,
    .set_mode = ncv7356_trx_set_mode,
};
