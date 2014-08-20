/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_iot-lab_M3
 * @{
 *
 * @file        at86rf231_driver.c
 * @brief       Board specific implementations for the at86rf231 radio driver
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "sched.h"
#include "vtimer.h"

#include "arch/thread_arch.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"

#include "at86rf231.h"
#include "at86rf231_spi.h"

/*
SPI1
  SCLK : PA5
  MISO : PA6
  MOSI : PA7
  CS : PA4

GPIO
  IRQ0 : PC4 : Frame buff empty indicator
  DIG2 : ? : RX Frame Time stamping XXX : NOT USED
  Reset : PC1 : active low, enable chip
  SLEEP : PA2 : control sleep, tx & rx state
*/

uint8_t at86rf231_get_status(void)
{
    return at86rf231_reg_read(AT86RF231_REG__TRX_STATUS)
           & AT86RF231_TRX_STATUS_MASK__TRX_STATUS;
}


void at86rf231_spi_select(void)
{
    gpio_clear(SPI_0_CS_GPIO);
}

void at86rf231_spi_unselect(void)
{
    gpio_set(SPI_0_CS_GPIO);
}

void at86rf231_slp_set(void)
{
    gpio_set(SPI_0_SLEEP_GPIO);
}

void at86rf231_slp_clear(void)
{
    gpio_clear(SPI_0_SLEEP_GPIO);
}

void at86rf231_rst_set(void)
{
    gpio_clear(SPI_0_RESET_GPIO);
}

void at86rf231_rst_clear(void)
{
    gpio_set(SPI_0_RESET_GPIO);
}

void at86rf231_enable_interrupts(void)
{
    gpio_irq_enable(SPI_0_IRQ0_GPIO);
}

void at86rf231_disable_interrupts(void)
{
    gpio_irq_disable(SPI_0_IRQ0_GPIO);
}

void at86rf231_gpio_spi_interrupts_init(void)
{
    /* set up GPIO pins */
    /* SCLK and MOSI*/
    GPIOA->CRL &= ~(0xf << (5 * 4));
    GPIOA->CRL |= (0xb << (5 * 4));
    GPIOA->CRL &= ~(0xf << (7 * 4));
    GPIOA->CRL |= (0xb << (7 * 4));
    /* MISO */
    gpio_init_in(SPI_0_MISO_GPIO, GPIO_NOPULL);

    /* SPI init */
    spi_init_master(SPI_0, SPI_CONF_FIRST_RISING, SPI_SPEED_5MHZ);

    spi_poweron(SPI_0);

    /* IRQ0 */
    gpio_init_in(SPI_0_IRQ0_GPIO, GPIO_NOPULL);
    gpio_init_int(SPI_0_IRQ0_GPIO, GPIO_NOPULL, GPIO_RISING, (gpio_cb_t)at86rf231_rx_irq, NULL);

    /* Connect EXTI4 Line to PC4 pin */
    at86rf231_enable_interrupts();

    /* CS */
    gpio_init_out(SPI_0_CS_GPIO, GPIO_NOPULL);
    /* SLEEP */
    gpio_init_out(SPI_0_SLEEP_GPIO, GPIO_NOPULL);
    /* RESET */
    gpio_init_out(SPI_0_RESET_GPIO, GPIO_NOPULL);

}

void at86rf231_reset(void)
{
    /* force reset */
    at86rf231_rst_set();

    /* put pins to default values */
    at86rf231_spi_unselect();
    at86rf231_slp_clear();

    /* additional waiting to comply to min rst pulse width */
    uint8_t delay = 50;
    while (delay--){}

    at86rf231_rst_clear();

    /* Send a FORCE TRX OFF command */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

    /* busy wait for TRX_OFF state */
    uint8_t status;
    uint8_t max_wait = 100;

    do {
        status = at86rf231_get_status();

        if (!--max_wait) {
            printf("at86rf231 : ERROR : could not enter TRX_OFF mode\n");
            break;
        }
    } while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS)
             != AT86RF231_TRX_STATUS__TRX_OFF);
}

uint8_t at86rf231_spi_transfer_byte(uint8_t byte)
{
    char ret;
    spi_transfer_byte(SPI_0, byte, &ret);
    return ret;
}

void at86rf231_spi_transfer(const uint8_t *data_out, uint8_t *data_in, uint16_t length)
{
    spi_transfer_bytes(SPI_0, (char*)data_out, (char*)data_in, length);
}
