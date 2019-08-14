/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Functions to communicate with the CC1100/CC1101 transceiver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "cc110x.h"
#include "cc110x_constants.h"

int cc110x_power_on(cc110x_t *dev)
{
    gpio_t cs = dev->params.cs;

    if (gpio_init(cs, GPIO_OUT)) {
        return -EIO;
    }
    gpio_clear(cs);
    xtimer_usleep(150);
    gpio_set(cs);
    spi_init_cs(dev->params.spi, dev->params.cs);
    return 0;
}

uint8_t cc110x_read(cc110x_t *dev, uint8_t addr, uint8_t *dest)
{
    uint8_t status;

    addr |= CC110X_SINGLE_BYTE_READ;
    spi_transfer_bytes(dev->params.spi, dev->params.cs, true,
                       &addr, &status, 1);
    spi_transfer_bytes(dev->params.spi, dev->params.cs, false, NULL, dest, 1);
    return status;
}

uint8_t cc110x_read_reliable(cc110x_t *dev, uint8_t addr, uint8_t *dest)
{
    uint8_t status, tmp;

    do {
        cc110x_read(dev, addr, &tmp);
        status = cc110x_read(dev, addr, dest);
    } while (*dest != tmp);

    return status;
}

uint8_t cc110x_write(cc110x_t *dev, uint8_t addr, uint8_t data)
{
    uint8_t status;

    addr |= CC110X_SINGLE_BYTE_WRITE;
    spi_transfer_bytes(dev->params.spi, dev->params.cs, true,
                       &addr, &status, 1);
    spi_transfer_bytes(dev->params.spi, dev->params.cs, false, &data, NULL, 1);
    return status;
}

uint8_t cc110x_burst_read(cc110x_t *dev, uint8_t addr, void *dest, size_t len)
{
    uint8_t status;

    addr |= CC110X_BURST_READ;
    spi_transfer_bytes(dev->params.spi, dev->params.cs, true,
                       &addr, &status, 1);
    spi_transfer_bytes(dev->params.spi, dev->params.cs, false, NULL, dest, len);
    return status;
}

uint8_t cc110x_burst_write(cc110x_t *dev, uint8_t addr,
                           const void *src, size_t len)
{
    uint8_t status;

    addr |= CC110X_BURST_WRITE;
    spi_transfer_bytes(dev->params.spi, dev->params.cs, true,
                       &addr, &status, 1);
    spi_transfer_bytes(dev->params.spi, dev->params.cs, false, src, NULL, len);
    return status;
}

uint8_t cc110x_cmd(cc110x_t *dev, uint8_t cmd_strobe)
{
    uint8_t status;

    spi_transfer_bytes(dev->params.spi, dev->params.cs, false,
                       &cmd_strobe, &status, 1);
    return status;
}

uint8_t cc110x_status(cc110x_t *dev)
{
    uint8_t status;

    do {
        status = cc110x_cmd(dev, CC110X_STROBE_STATUS);
    } while (status != cc110x_cmd(dev, CC110X_STROBE_STATUS));
    return status;
}

void cc110x_wakeup_from_rx(cc110x_t *dev)
{
    if (dev->state != CC110X_STATE_RX_MODE ||
        dev->state != CC110X_STATE_RECEIVING) {
        return;
    }

    cc110x_cmd(dev, CC110X_STROBE_IDLE);
    dev->state = CC110X_STATE_IDLE;
}

void cc110x_power_off(cc110x_t *dev)
{
    /* Ensure device is on IDLE state */
    cc110x_wakeup_from_rx(dev);

    /*
     * Datasheet page 9 table 4.
     *
     * To achieve the lowest power consumption GDO's must
     * be programmed to 0x2F
     */
    cc110x_write(dev, CC110X_REG_IOCFG2, CC110X_GDO_CONSTANT_LOW);
    cc110x_write(dev, CC110X_REG_IOCFG1, CC110X_GDO_CONSTANT_LOW);
    cc110x_write(dev, CC110X_REG_IOCFG0, CC110X_GDO_CONSTANT_LOW);

    /* go to sleep */
    cc110x_cmd(dev, CC110X_STROBE_OFF);

    /*
     * Set CS pin high so the device gets de-selected, and thus
     * the sleep mode transition takes place
     * See figure 13 on page 28 of the datasheet.
     */
    gpio_t cs = dev->params.cs;
    gpio_set(cs);
}
