/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_io1_xplained
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Atmel IO1 Xplained extension
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "log.h"
#include "io1_xplained.h"
#include "io1_xplained_internals.h"
#include "at30tse75x.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

/*---------------------------------------------------------------------------*
 *                          IO1 Xplained Core API                            *
 *---------------------------------------------------------------------------*/

int io1_xplained_init(io1_xplained_t *dev, const io1_xplained_params_t *params)
{
    dev->params = *params;

    /* Initialize I2C interface */
    if (at30tse75x_init(&dev->temp,
                        I2C_DEV(0),
                        I2C_SPEED_NORMAL, (TEMPERATURE_BASE_ADDR | dev->params.addr)) < 0) {
        LOG_ERROR("Cannot initialize temperature sensor.\n");
        return -IO1_XPLAINED_NOTEMP;
    }

    /* Use maximum resolution */
    at30tse75x_set_resolution(&dev->temp, AT30TSE75X_RESOLUTION_12BIT);

    if (gpio_init(IO1_LED_PIN, GPIO_OUT) < 0) {
        LOG_ERROR("GPIO LED not enabled\n");
        return -IO1_XPLAINED_NOLED;
    }

    if (gpio_init(IO1_GPIO1_PIN, GPIO_OUT) < 0) {
        LOG_ERROR("GPIO1 not enabled\n");
        return -IO1_XPLAINED_NOGPIO1;
    }

    if (gpio_init(IO1_GPIO2_PIN, GPIO_OUT) < 0) {
        LOG_ERROR("GPIO2 not enabled\n");
        return -IO1_XPLAINED_NOGPIO2;
    }

    LOG_DEBUG("IO1 Xplained extension initialized!\n");

    return IO1_XPLAINED_OK;
}

int io1_xplained_read_temperature(io1_xplained_t *dev, float *temperature)
{
    if (at30tse75x_get_temperature(&dev->temp, temperature) < 0) {
        LOG_ERROR("Cannot read temperature sensor.\n");
        return -IO1_XPLAINED_READ_ERR;
    }
    return IO1_XPLAINED_READ_OK;
}

void io1_xplained_set_led(void)
{
    gpio_set(IO1_LED_PIN);
}

void io1_xplained_clear_led(void)
{
    gpio_clear(IO1_LED_PIN);
}

void io1_xplained_toggle_led(void)
{
    gpio_toggle(IO1_LED_PIN);
}
