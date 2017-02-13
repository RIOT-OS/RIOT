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

#include <math.h>

#include "log.h"
#include "io1_xplained.h"
#include "io1_xplained_internals.h"
#include "io1_xplained_params.h"
#include "at30tse75x.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
io1_xplained_t io1_xplained_devs[IO1_XPLAINED_NUMOF];

/*---------------------------------------------------------------------------*
 *                          IO1 Xplained Core API                            *
 *---------------------------------------------------------------------------*/

int io1_xplained_init(io1_xplained_t *dev, uint8_t addr)
{
    /* Initialize I2C interface */
    if (at30tse75x_init(&dev->temp,
                        I2C_DEV(0),
                        I2C_SPEED_NORMAL, (TEMPERATURE_BASE_ADDR | addr)) < 0) {
        DEBUG("[Error] Cannot initialize temperature sensor.\n");
        return -1;
    }

    /* Use maximum resolution */
    at30tse75x_set_resolution(&dev->temp, AT30TSE75X_RESOLUTION_12BIT);

    if (gpio_init(IO1_LED_PIN, GPIO_OUT) < 0) {
        DEBUG("[Error] GPIO LED not enabled\n");
        return -1;
    }

    if (gpio_init(IO1_GPIO1_PIN, GPIO_OUT) < 0) {
        DEBUG("[Error] GPIO1 not enabled\n");
        return -1;
    }

    if (gpio_init(IO1_GPIO2_PIN, GPIO_OUT) < 0) {
        DEBUG("[Error] GPIO2 not enabled\n");
        return -1;
    }

    DEBUG("[Info] IO1 Xplained extension initialized!\n");

    return 0;
}

void io1_xplained_auto_init(void)
{
    for (unsigned i = 0; i < IO1_XPLAINED_NUMOF; i++) {
        if (io1_xplained_init(&io1_xplained_devs[i],
                              io1_xplained_params[i].addr) < 0) {
            LOG_ERROR("Unable to initialize IO1 Xplained #%i\n", i);
        }
#ifdef MODULE_SAUL_REG
        io1_xplained_saul_reg[i][0].dev = &io1_xplained_devs[i];
        saul_reg_add(&io1_xplained_saul_reg[i][0]);
#endif
#ifdef MODULE_SAUL_GPIO
        for (unsigned j = 1; j < 4; j++) {
            io1_xplained_saul_reg[i][j].dev = &(io1_xplained_saul_gpios[j-1]);
            saul_reg_add(&(io1_xplained_saul_reg[i][j]));
        }
#endif
    }
}

int io1_xplained_read_temperature(io1_xplained_t *dev, float *temperature)
{
    if (at30tse75x_get_temperature(&dev->temp, temperature) < 0) {
        DEBUG("[Error] Cannot read IO1 Xplained temperatuse sensor.\n");
        return -1;
    }
    return 0;
}

int io1_xplained_set_led(void)
{
    gpio_set(IO1_LED_PIN);
    return 0;
}

int io1_xplained_clear_led(void)
{
    gpio_clear(IO1_LED_PIN);
    return 0;
}

int io1_xplained_toggle_led(void)
{
    gpio_toggle(IO1_LED_PIN);
    return 0;
}
