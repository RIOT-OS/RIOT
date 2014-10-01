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
#define ENABLE_DEBUG (1)
#include "debug.h"
/*
SPI
  SCLK : PC18
  MISO : PC19
  MOSI : PB30
  CS   : PB31

GPIO
  IRQ0  : PB00 : Frame buff empty indicator
  DIG2  : PB17 : RX Frame Time stamping XXX : NOT USED
  Reset : PB15 : active low, enable chip
  SLEEP : PA20 : control sleep, tx & rx state
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
    DEBUG("AT86RF231 gpio/spi init \n");
    /* RF control */
    PM->APBCMASK.reg |= PM_APBCMASK_RFCTRL;
    
    PORT->Group[0].DIRSET.reg = (1<<(PIN_RFCTRL1 % 32));
    PORT->Group[0].DIRSET.reg = (1<<(PIN_RFCTRL2 % 32));
    PORT->Group[0].PINCFG[PIN_RFCTRL1 % 32].bit.PMUXEN = true;
    PORT->Group[0].PINCFG[PIN_RFCTRL2 % 32].bit.PMUXEN = true;
    PORT->Group[0].PMUX[PIN_RFCTRL1 % 32].bit.PMUXO = 5;
    PORT->Group[0].PMUX[PIN_RFCTRL2 % 32].bit.PMUXE = 5;
    REG_RFCTRL_FECFG = RFCTRL_CFG_ANT_DIV;

    /* SPI init */
    spi_init_master(SPI_0, SPI_CONF_FIRST_RISING, SPI_SPEED_100KHZ);
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
    // while(1)
    // {
    //     char i = 0;
    //     spi_transfer_byte(SPI_0, 0xAA, &i);
    // }

    DEBUG("CS: %d, SLEEP: %d, RESET %d \n", SPI_0_CS_GPIO, SPI_0_SLEEP_GPIO, SPI_0_RESET_GPIO);

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

