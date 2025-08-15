/*
 * SPDX-FileCopyrightText: 2016 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "io1_xplained.h"
#include "io1_xplained_internals.h"

#include "at30tse75x.h"
#include "sdcard_spi.h"

#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/*---------------------------------------------------------------------------*
 *                          IO1 Xplained Core API                            *
 *---------------------------------------------------------------------------*/

int io1_xplained_init(io1_xplained_t *dev, const io1_xplained_params_t *params)
{
    dev->params = *params;

    /* Initialize I2C interface */
    if (at30tse75x_init(&dev->temp,
                        I2C_DEV(0),
                        (IO1_TEMPERATURE_BASE_ADDR | dev->params.addr)) < 0) {
        DEBUG("[io1_xplained] Cannot initialize temperature sensor.\n");
        return -IO1_XPLAINED_NOTEMP;
    }

    /* Use maximum resolution */
    at30tse75x_set_resolution(&dev->temp, AT30TSE75X_RESOLUTION_12BIT);

    if (gpio_init(IO1_SDCARD_SPI_PARAM_DETECT, GPIO_IN) < 0) {
        DEBUG("[io1_xplained] SD Card detect initialization failed\n");
        return -IO1_XPLAINED_NOSDCARD;
    }

    /* Card detect pin is inverted */
    if (!gpio_read(IO1_SDCARD_SPI_PARAM_DETECT)) {
        /* Initialize the SD Card */
        sdcard_spi_params_t sdcard_params = {
            .spi_dev        = IO1_SDCARD_SPI_PARAM_SPI,
            .cs             = IO1_SDCARD_SPI_PARAM_CS,
            .clk            = IO1_SDCARD_SPI_PARAM_CLK,
            .mosi           = IO1_SDCARD_SPI_PARAM_MOSI,
            .miso           = IO1_SDCARD_SPI_PARAM_MISO,
            .power          = IO1_SDCARD_SPI_PARAM_POWER,
            .power_act_high = IO1_SDCARD_SPI_PARAM_POWER_AH
        };
        if (sdcard_spi_init(&dev->sdcard, &sdcard_params) != 0) {
            DEBUG("[io1_xplained] SD Card initialization failed\n");
            return -IO1_XPLAINED_NOSDCARD;
        }
    }

    if (adc_init(IO1_LIGHT_ADC_LINE) < 0) {
        DEBUG("[io1_xplained] Light sensor (ADC) initialization failed\n");
        return -IO1_XPLAINED_NOLIGHT;
    }

    if (gpio_init(IO1_LED_PIN, GPIO_OUT) < 0) {
        DEBUG("[io1_xplained] LED initialization failed\n");
        return -IO1_XPLAINED_NOLED;
    }

    if (gpio_init(IO1_GPIO1_PIN, GPIO_OUT) < 0) {
        DEBUG("[io1_xplained] GPIO1 initialization failed\n");
        return -IO1_XPLAINED_NOGPIO1;
    }

    if (gpio_init(IO1_GPIO2_PIN, GPIO_OUT) < 0) {
        DEBUG("[io1_xplained] GPIO2 initialization failed\n");
        return -IO1_XPLAINED_NOGPIO2;
    }

    DEBUG("IO1 Xplained extension initialized with success!\n");

    return IO1_XPLAINED_OK;
}

int io1_xplained_read_light_level(uint16_t *light)
{
    int sample = adc_sample(IO1_LIGHT_ADC_LINE, IO1_LIGHT_ADC_RES);

    if (sample < 0) {
        DEBUG("[io1_xplained] Light sensor read failed\n");
        return -IO1_XPLAINED_READ_ERR;
    }

    *light = (1023 - sample);

    return IO1_XPLAINED_READ_OK;
}
