/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>

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

int quectel_power_on(gsm_t *dev);

static inline int configure_interface_control(gsm_t *dev);

int quectel_init(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev){
        quectel_params_t *params = (quectel_params_t *)dev->params;

        assert(params->power_pin != GPIO_UNDEF);

        rmutex_lock(&dev->mutex);

        /* make sure reset pin at modem is (high) idle */
        if(params->reset_pin != GPIO_UNDEF) {
            gpio_write(params->reset_pin, params->invert_reset_pin ? 0 : 1);
        }

        if(params->status_pin != GPIO_UNDEF) {
            bool status = gpio_read(params->status_pin);
            if(params->invert_status_pin) {
                status = !status;
            }

            if(status){
                err = configure_interface_control(dev);

                LOG_DEBUG(LOG_HEADER"device already on\n");

                dev->state = GSM_ON;
            }
        }

        err = 0;

        rmutex_unlock(&dev->mutex);
    }
    return err;
}

int quectel_power_on(gsm_t *dev)
{
    int err = -EINVAL;

    if((dev) && (dev->params)){
        bool status = false;
        quectel_params_t * params = (quectel_params_t *)dev->params;

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
            xtimer_usleep(QUECTEL_POWER_KEY_ON_TIME_US);
            gpio_write(params->power_pin, params->invert_power_pin ? 1 : 0);

            if(params->status_pin != GPIO_UNDEF) {
                uint32_t timeout = QUECTEL_STATUS_DETECT_TIMEOUT_US;
                while((!status) && (timeout--)) {
                    xtimer_usleep(1);

                    status = gpio_read(params->status_pin);
                    if(params->invert_status_pin) {
                        status = !status;
                    }
                }

                /* give modem some more time to boot */
                xtimer_usleep(US_PER_SEC);
            }
            else {
                status = gsm_is_alive(dev, 5);
            }
        }
        else {
            LOG_DEBUG(LOG_HEADER"device already on\n");
        }

        if(status){
            (void)configure_interface_control;
        }

        err = status ? 0 : -1;

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int quectel_power_off(gsm_t *dev)
{
    int err = -EINVAL;

    if((dev) && (dev->params)){
        bool status = true;
        quectel_params_t * params = (quectel_params_t *)dev->params;

        rmutex_lock(&dev->mutex);

        if(params->status_pin != GPIO_UNDEF) {
            status = gpio_read(params->status_pin);
            if(params->invert_status_pin) {
                status = !status;
            }
        }

        if(status){
            gpio_write(params->power_pin, params->invert_power_pin ? 1 : 0);
            xtimer_usleep(QUECTEL_POWER_KEY_OFF_TIME_US);
            gpio_write(params->power_pin, params->invert_power_pin ? 0 : 1);

            if(params->status_pin != GPIO_UNDEF) {
                uint32_t timeout = QUECTEL_STATUS_DETECT_TIMEOUT_US * 2;
                while((status) && (timeout--)) {
                    xtimer_usleep(1);

                    status = gpio_read(params->status_pin);
                    if(params->invert_status_pin) {
                        status = !status;
                    }
                }
            }
            else {
                status = false;
            }
        }
        else {
            LOG_DEBUG(LOG_HEADER"device already off\n");
        }

        err = (!status) ? 0 : -1;

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

void quectel_sleep(gsm_t *dev)
{
    (void)dev;
}

void quectel_reset(gsm_t *dev)
{
    if(dev){
        quectel_params_t * params = (quectel_params_t *)dev->params;

        /* make sure reset pin at modem is (high) idle */
        if(params->reset_pin != GPIO_UNDEF) {

            gpio_write(params->reset_pin, params->invert_reset_pin ? 0 : 1);
            xtimer_usleep(QUECTEL_RESET_TIME_US);
            gpio_write(params->reset_pin, params->invert_reset_pin ? 0 : 1);
        }
    }
}

static inline int configure_interface_control(gsm_t *dev)
{
    int err = 0;
    quectel_params_t *params = (quectel_params_t *)dev->params;

    if(params->dtr_pin != GPIO_UNDEF) {
        gpio_clear(params->dtr_pin);

        /* set DTR pin: change to command mode while connected call is active*/
        err = at_send_cmd_wait_ok(&dev->at_dev, "AT&D1", GSM_SERIAL_TIMEOUT_US);
    }

    if(params->dcd_pin != GPIO_UNDEF) {
        /* set DCD pin: function is ON only in presence of data carrier */
        err = at_send_cmd_wait_ok(&dev->at_dev, "AT&C1", GSM_SERIAL_TIMEOUT_US);
    }

    return err;
}


const gsm_driver_t quectel_driver = {
    .init_base  = quectel_init,
    .power_on   = quectel_power_on,
    .power_off  = quectel_power_off,
    .sleep      = quectel_sleep,
    .reset      = quectel_reset,
};
