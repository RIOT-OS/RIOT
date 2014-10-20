/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       Register access functions for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "at86rf231_spi.h"
#include "at86rf231.h"
#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

void at86rf231_reg_write(uint8_t addr, uint8_t value)
{
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* write to register */
    spi_transfer_reg(AT86RF231_SPI, AT86RF231_ACCESS_REG | AT86RF231_ACCESS_WRITE | addr, value, 0);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
}

uint8_t at86rf231_reg_read(uint8_t addr)
{
    char value;

    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* read from register */
    spi_transfer_reg(AT86RF231_SPI, AT86RF231_ACCESS_REG | AT86RF231_ACCESS_READ | addr, 0, &value);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
    return (uint8_t)value;
}

void at86rf231_read_fifo(uint8_t *data, radio_packet_length_t length)
{
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* Read a number of bytes from the devices frame buffer */
    spi_transfer_regs(AT86RF231_SPI, AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_READ,
                      0, (char*)data, length);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
}

void at86rf231_write_fifo(const uint8_t *data, radio_packet_length_t length)
{
    /* Start the SPI transfer */
    gpio_clear(AT86RF231_CS);
    /* Send Frame Buffer Write access */
    spi_transfer_regs(AT86RF231_SPI, AT86RF231_ACCESS_FRAMEBUFFER | AT86RF231_ACCESS_WRITE,
                      (char*)data, 0, length);
    /* End the SPI transfer */
    gpio_set(AT86RF231_CS);
}

uint8_t at86rf231_get_status(void)
{
    return (at86rf231_reg_read(AT86RF231_REG__TRX_STATUS)
            & AT86RF231_TRX_STATUS_MASK__TRX_STATUS);
}
