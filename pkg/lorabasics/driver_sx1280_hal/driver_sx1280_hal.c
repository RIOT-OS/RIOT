/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lorabasicsmodem
 * @{
 *
 * @file
 * @brief       HAL implementation for the SX1280 LoRa radio driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Nicolas Albarel <nicolas.albarel@univ-grenoble-alpes.fr>
 * @author      Didier Donsez <didier.donsez@univ-grenoble-alpes.fr>
 * @author      Olivier Alphand <olivier.alphand@univ-grenoble-alpes.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include "irq.h"
#include "periph/spi.h"
#include "ztimer.h"

#include "sx1280.h"
#include "sx1280_constants.h"
#include "sx1280_driver/src/sx1280_hal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Wait until radio busy pin is reset to 0
 */
static void sx1280_hal_wait_on_busy(const void *context)
{
    sx1280_t *dev = (sx1280_t *)context;

    while (gpio_read(dev->params->dio0_pin)) {}
}

sx1280_hal_status_t sx1280_hal_write(const void *context, const uint8_t *command,
                                     const uint16_t command_length,
                                     const uint8_t *data, const uint16_t data_length)
{
    sx1280_t *dev = (sx1280_t *)context;

    sx1280_hal_wakeup(context);

    DEBUG("[sx1280_hal_write]: command_length=%d data_length=%d\n", command_length, data_length);

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, dev->params->spi_mode, dev->params->spi_clk);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, data_length != 0,
                       command, NULL, command_length);
    if (data_length) {
        spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, data,
                           NULL, data_length);
    }
    spi_release(dev->params->spi);

    /* 0x84 - SX1280_SET_SLEEP opcode. In sleep mode the radio dio is stuck
       to 1 => do not test it */
    if (command[0] != 0x84) {
        sx1280_hal_wait_on_busy(context);
    }
    return SX1280_HAL_STATUS_OK;
}

sx1280_hal_status_t sx1280_hal_read(const void *context, const uint8_t *command,
                                    const uint16_t command_length,
                                    uint8_t *data, const uint16_t data_length)
{
    sx1280_t *dev = (sx1280_t *)context;

    sx1280_hal_wakeup(context);

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, dev->params->spi_mode, dev->params->spi_clk);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, true, \
                       command, NULL, command_length);
    spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, \
                       NULL, data, data_length);
    spi_release(dev->params->spi);

    return SX1280_HAL_STATUS_OK;
}

void sx1280_hal_reset( const void *context )
{
    sx1280_t *dev = (sx1280_t *)context;

    gpio_clear(dev->params->reset_pin);
    ztimer_sleep(ZTIMER_MSEC, SX1280_RESET_MS);
    gpio_set(dev->params->reset_pin);
    ztimer_sleep(ZTIMER_MSEC, SX1280_WAKEUP_TIME_MS);
}

sx1280_hal_status_t sx1280_hal_wakeup(const void *context)
{
    sx1280_t *dev = (sx1280_t *)context;

    if (dev->mode == SX1280_HAL_OP_MODE_SLEEP) {
        /* Busy is HIGH in sleep mode, wake-up the device */
        gpio_clear(dev->params->nss_pin);
        sx1280_hal_wait_on_busy(context);
        gpio_set(dev->params->nss_pin);

        /* Radio is awake in STDBY_RC mode */
        dev->mode = SX1280_HAL_OP_MODE_STDBY_RC;
    }
    else {
        /* if the radio is awake, just wait until busy pin get low */
        sx1280_hal_wait_on_busy(context);
    }

    return SX1280_HAL_STATUS_OK;
}

sx1280_hal_operating_mode_t sx1280_hal_get_operating_mode(const void *context)
{
    sx1280_t *dev = (sx1280_t *)context;

    return dev->mode;
}

void sx1280_hal_set_operating_mode(const void *context, const sx1280_hal_operating_mode_t op_mode)
{
    sx1280_t *dev = (sx1280_t *)context;

    dev->mode = op_mode;
}
