/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "log.h"
#include "xtimer.h"
#include "quectel.h"

/**
 * @ingroup     drivers_gsm_quectel
 * @{
 *
 * @file
 * @brief       Generic Quectel implementation.
 *
 * @author      Max van Kessel <maxvankessel@betronic.nl>
 *
 * @}
 */

#define LOG_HEADER  "quectel: "

int quectel_init(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev){
        uint8_t retries = 3;
        gsm_quectel_t * priv = (gsm_quectel_t *)dev;
        gsm_quectel_params_t *params = (gsm_quectel_params_t *)dev->params;

        assert(params->invert_power_pin != GPIO_UNDEF);

        rmutex_lock(&dev->mutex);

        /* make sure reset pin at modem is (high) idle */
        if(params->reset_pin != GPIO_UNDEF) {
            gpio_write(params->reset_pin, params->invert_reset_pin ? 0 : 1);
        }

        if(params->dtr_pin != GPIO_UNDEF) {
            gpio_clear(params->dtr_pin);
        }

        /* send AT to see if it's alive */
        do {
            xtimer_usleep(GSM_SERIAL_TIMEOUT_US);
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT", GSM_SERIAL_TIMEOUT_US);
        } while(err != 0 && (retries--));

        if(err) {
            LOG_INFO(LOG_HEADER"no response on AT\n");

            /* TODO no response on AT check if device is in command mode */
        }

        rmutex_unlock(&dev->mutex);
    }
    return err;
}

int quectel_power_on(gsm_t *dev)
{
    int err = -EINVAL;

    if((dev) && (dev->params)){
        bool status = false;
        gsm_quectel_params_t * params = (gsm_quectel_params_t *)dev->params;

        rmutex_lock(&dev->mutex);

        if(params->status_pin != GPIO_UNDEF) {
            status = gpio_read(params->status_pin);
            if(params->invert_status_pin) {
                status = !status;
            }
        }

        if(!status){
            if(params->reset_pin != GPIO_UNDEF) {
                gpio_write(params->reset_pin, params->invert_reset_pin ? 0 : 1);
            }

            gpio_write(params->power_pin, params->invert_power_pin ? 0 : 1);

            if(params->status_pin != GPIO_UNDEF) {
                uint32_t timeout = QUECTEL_STATUS_DETECT_TIMEOUT_US;
                while((!status) && (timeout--)) {
                    xtimer_usleep(1);

                    status = gpio_read(params->status_pin);
                    if(params->invert_status_pin) {
                        status = !status;
                    }
                }
            }
            else {
                xtimer_usleep(QUECTEL_POWER_KEY_ON_TIME_US);
            }

            gpio_write(params->power_pin, params->invert_power_pin ? 1 : 0);
        }

        err = !status;

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int quectel_power_off(gsm_t *dev)
{

}

void quectel_sleep(gsm_t *dev)
{

}

const gsm_driver_t gsm_generic_driver = {
    .init_base  = quectel_init,
    .power_on   = quectel_power_on,
    .power_off  = quectel_power_off,
    .sleep      = quectel_sleep,
};
