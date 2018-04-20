/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>

#include "fmt.h"
#include "log.h"
#include "xtimer.h"
#include "ublox.h"

/**
 * @ingroup     drivers_gsm_ublox
 * @{
 *
 * @file
 * @brief       Generic Ublox implementation.
 *
 * @author
 *
 * @}
 */

#define LOG_HEADER  "ublox: "

static void ublox_reset(gsm_t *dev)
{
    ublox_params_t *params = (ublox_params_t *)dev->params;

    if (params->pwr_on_pin != GPIO_UNDEF) {
        gpio_set(params->pwr_on_pin);
    }

    if (params->reset_pin != GPIO_UNDEF) {
        gpio_set(params->reset_pin);
        xtimer_usleep(51 * US_PER_MS);
        gpio_clear(params->reset_pin);
    }

    if (params->pwr_on_pin != GPIO_UNDEF) {
        xtimer_usleep(150 * US_PER_MS);
        gpio_clear(params->pwr_on_pin);
        xtimer_sleep(5);
    }
}


static int change_baudrate(gsm_t *dev, uint32_t baudrate)
{
    char buf[64];
    char *pos = buf;

    pos += fmt_str(pos, "AT+IPR=");
    pos += fmt_u32_dec(pos, baudrate);
    *pos = '\0';

    return at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
}


static int ublox_init(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev) {
        ublox_params_t *params = (ublox_params_t *)dev->params;
        char buf[128];
        char *pos;

        assert(params->pwr_on_pin != GPIO_UNDEF);

        rmutex_lock(&dev->mutex);

        /* make sure reset pin at modem is (high) idle */
        if(params->reset_pin != GPIO_UNDEF) {
            gpio_write(params->reset_pin, 1);
        }

        if(params->dtr_pin != GPIO_UNDEF) {
            gpio_clear(params->dtr_pin);
            xtimer_usleep(100);
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT+UPSV=3",
            GSM_SERIAL_TIMEOUT_US);
        } else {
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT+UPSV=1",
            GSM_SERIAL_TIMEOUT_US);
        }

        /* Error messages */
        at_send_cmd_wait_ok(&dev->at_dev, "AT+CMEE=1", GSM_SERIAL_TIMEOUT_US);
        /* Auto answer */
        at_send_cmd_wait_ok(&dev->at_dev, "ATS0=1", GSM_SERIAL_TIMEOUT_US);

        if(dev->params->baudrate != params->change_over_baudrate) {
            if(change_baudrate(dev, params->change_over_baudrate) != 0) {
                LOG_ERROR(LOG_HEADER"failed to change baudrate to %lu", params->change_over_baudrate);
            }
        }

        /* GPIO1 */
        if(params->gpio1_mode != UBLOX_GPIO_MODE_DEFAULT) {
            pos = buf;
            pos += fmt_str(pos, "AT+UGPIOC=");
            pos += fmt_u16_dec(pos, UBLOX_GPIO1);
            pos += fmt_str(pos, ",");
            pos += fmt_u16_dec(pos,
                    params->gpio1_mode & UBLOX_GPIO_MODE_DISABLED);
            if(params->gpio1_mode == UBLOX_GPIO_MODE_OUTPUT) {
                pos += fmt_str(pos, ",");
                if(params->gpio1_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                    pos += fmt_u16_dec(pos, 1);
                } else {
                    pos += fmt_u16_dec(pos, 0);
                }
            }
            *pos = '\0';
            at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
        }
        /* GPIO2 */
        if(params->gpio2_mode != UBLOX_GPIO_MODE_DEFAULT) {
            pos = buf;
            pos += fmt_str(pos, "AT+UGPIOC=");
            pos += fmt_u16_dec(pos, UBLOX_GPIO2);
            pos += fmt_str(pos, ",");
            pos += fmt_u16_dec(pos,
                    params->gpio1_mode & UBLOX_GPIO_MODE_DISABLED);
            if(params->gpio2_mode == UBLOX_GPIO_MODE_OUTPUT) {
                pos += fmt_str(pos, ",");
                if(params->gpio2_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                    pos += fmt_u16_dec(pos, 1);
                } else {
                    pos += fmt_u16_dec(pos, 0);
                }
            }
            *pos = '\0';
            at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
        }
        /* GPIO3 */
        if(params->gpio3_mode != UBLOX_GPIO_MODE_DEFAULT) {
            pos = buf;
            pos += fmt_str(pos, "AT+UGPIOC=");
            pos += fmt_u16_dec(pos, UBLOX_GPIO3);
            pos += fmt_str(pos, ",");
            pos += fmt_u16_dec(pos,
                    params->gpio3_mode & UBLOX_GPIO_MODE_DISABLED);
            if(params->gpio3_mode == UBLOX_GPIO_MODE_OUTPUT) {
                pos += fmt_str(pos, ",");
                if(params->gpio3_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                    pos += fmt_u16_dec(pos, 1);
                } else {
                    pos += fmt_u16_dec(pos, 0);
                }
            }
            *pos = '\0';
            at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
        }
        /* GPIO4 */
        if(params->gpio4_mode != UBLOX_GPIO_MODE_DEFAULT) {
            pos = buf;
            pos += fmt_str(pos, "AT+UGPIOC=");
            pos += fmt_u16_dec(pos, UBLOX_GPIO4);
            pos += fmt_str(pos, ",");
            pos += fmt_u16_dec(pos,
                    params->gpio4_mode & UBLOX_GPIO_MODE_DISABLED);
            if(params->gpio4_mode == UBLOX_GPIO_MODE_OUTPUT) {
                pos += fmt_str(pos, ",");
                if(params->gpio4_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                    pos += fmt_u16_dec(pos, 1);
                } else {
                    pos += fmt_u16_dec(pos, 0);
                }
            }
            *pos = '\0';
            at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
        }

        if(params->gps_connected) {
            if(params->gps_pm_exti) {
                /* Turn on GPS */
                at_send_cmd_wait_ok(&dev->at_dev, "AT+UGPS=1,0",
                GSM_SERIAL_TIMEOUT_US);

                /* configure UBX-CFG-PM2 to use EXTI */
                at_send_cmd_get_lines(&dev->at_dev,
                        "AT+UGUBX=\"B5 62 06 3B 30 00 02 06 00 00 60 14 43 01 E8 03 00 00 10 "
                                "27 00 00 00 00 00 00 00 00 00 00 2C 01 00 00 4F C1 03 00 86 02 00 00 "
                                "FE 00 00 00 64 40 01 00 00 00 00 00 BE 58\"",
                        buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT_US);

                /* Turn off GPS */
                at_send_cmd_wait_ok(&dev->at_dev, "AT+UGPS=0",
                GSM_SERIAL_TIMEOUT_US);
            }

            /* setup A-GPS */
            if(params->agps_token) {
                pos = buf;
                pos += fmt_str(pos, "AT+UGSRV=\"");
                pos += fmt_str(pos, params->agps_server);
                pos += fmt_str(pos, "\",\"");
                pos += fmt_str(pos, params->agps_server2);
                pos += fmt_str(pos, "\",\"");
                pos += fmt_str(pos, params->agps_token);
                pos += fmt_str(pos, "\"");
                *pos = '\0';
                at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);
            }

            at_send_cmd_wait_ok(&dev->at_dev, "AT+UGRMC=1",
            GSM_SERIAL_TIMEOUT_US);
            at_send_cmd_wait_ok(&dev->at_dev, "AT+UGGSV=1",
            GSM_SERIAL_TIMEOUT_US);
            at_send_cmd_wait_ok(&dev->at_dev, "AT+UGGGA=1",
            GSM_SERIAL_TIMEOUT_US);
            at_send_cmd_wait_ok(&dev->at_dev, "AT+UGGSA=1",
            GSM_SERIAL_TIMEOUT_US);
        }

        dev->state = GSM_ON;
    }

    rmutex_unlock(&dev->mutex);

    return err;
}

static void ublox_sleep(gsm_t *dev)
{
    ublox_params_t *params = (ublox_params_t *)dev->params;

    if (params->dtr_pin != GPIO_UNDEF) {
        gpio_set(params->dtr_pin);
    }
}

static int ublox_wake_up(gsm_t *dev)
{
    ublox_params_t *params = (ublox_params_t *)dev->params;

        if (params->dtr_pin != GPIO_UNDEF) {
        gpio_clear(params->dtr_pin);
        xtimer_usleep(100);
        return 0;
    }

    return -1;
}


const gsm_driver_t ublox_driver = {
    .init_base  = ublox_init,
    .power_on   = NULL,
    .power_off  = NULL,
    .sleep      = ublox_sleep,
    .reset      = ublox_reset,
    .wake_up    = ublox_wake_up,
};
