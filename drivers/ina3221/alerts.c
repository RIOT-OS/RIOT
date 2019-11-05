/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ina3221
 * @{
 *
 * @file
 * @brief       Functions to enable/disable GPIO alerts
 *              for INA3221
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include "periph/gpio.h"
#include "ina3221.h"

int _ina3221_enable_alert(ina3221_t *dev, ina3221_alert_t alert,
                          ina3221_alert_cb_t cb, void *arg)
{
    if (alert >= INA3221_NUM_ALERTS) {
        return -ERANGE;
    }
    if (dev->params.upins.apins.alert_pins[alert] == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    dev->alert_callbacks[alert] = cb;
    dev->alert_callback_arguments[alert] = arg;
    int check = gpio_init_int(
        dev->params.upins.apins.alert_pins[alert],
        (dev->params.gpio_config & (1 << alert)) ? GPIO_IN_PU : GPIO_IN,
        GPIO_FALLING,
        cb,
        arg
        );
    return check ? check : INA3221_OK;
}

int _ina3221_disable_alert(ina3221_t *dev, ina3221_alert_t alert)
{
    if (alert >= INA3221_NUM_ALERTS) {
        return -ERANGE;
    }
    if (dev->params.upins.apins.alert_pins[alert] == GPIO_UNDEF) {
        return -ENOTSUP;
    }
    gpio_irq_disable(dev->params.upins.apins.alert_pins[alert]);
    return INA3221_OK;
}
