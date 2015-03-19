/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf212b
 * @{
 *
 * @file
 * @brief       Register access functions for the AT86RF212B device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Pon Nidhya Elango, TICET, IIT Bombay <nidhya.e@gmail.com>
 *
 * @}
 */

#include "at86rf212b_spi.h"
#include "at86rf212b.h"
#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

void at86rf212b_reg_write(uint8_t addr, uint8_t value)
{
    addr |= AT86RF212B_ACCESS_REG;
    addr |= AT86RF212B_ACCESS_WRITE;
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF212B_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF212B_CS);
    /* write to register */
    spi_transfer_reg(AT86RF212B_SPI, addr, value, 0);
    /* End the SPI transfer */
    gpio_set(AT86RF212B_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF212B_SPI);
}

uint8_t at86rf212b_reg_read(uint8_t addr)
{
    char value;
    addr |= AT86RF212B_ACCESS_REG;
    addr |= AT86RF212B_ACCESS_READ; 
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF212B_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF212B_CS);
    /* read from register */
    spi_transfer_reg(AT86RF212B_SPI, addr, 0, &value);
    /* End the SPI transfer */
    gpio_set(AT86RF212B_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF212B_SPI);
    return (uint8_t)value;
}

void at86rf212b_read_fifo(uint8_t *data, radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF212B_SPI);
    /* Start the SPI transfer */
    gpio_clear(AT86RF212B_CS);
    /* Read a number of bytes from the devices frame buffer */
    spi_transfer_regs(AT86RF212B_SPI, AT86RF212B_ACCESS_FRAMEBUFFER | AT86RF212B_ACCESS_READ,
                      0, (char*)data, length);
    /* End the SPI transfer */
    gpio_set(AT86RF212B_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF212B_SPI);
}

void at86rf212b_write_fifo(const uint8_t *data, radio_packet_length_t length)
{
    /* Acquire exclusive access to the bus. */
    spi_acquire(AT86RF212B_SPI);    
    /* Start the SPI transfer */
    gpio_clear(AT86RF212B_CS);
    /* Send Frame Buffer Write access */
    spi_transfer_regs(AT86RF212B_SPI, AT86RF212B_ACCESS_FRAMEBUFFER | AT86RF212B_ACCESS_WRITE,
                      (char*)data, 0, length);
    /* End the SPI transfer */
    gpio_set(AT86RF212B_CS);
    /* Release the bus for other threads. */
    spi_release(AT86RF212B_SPI);
}

uint8_t at86rf212b_get_status(void)
{
    return (at86rf212b_reg_read(AT86RF212B_REG__TRX_STATUS)
           & AT86RF212B_TRX_STATUS_MASK__TRX_STATUS);
}
