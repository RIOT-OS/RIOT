/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
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
 * @brief       Driver implementation of the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "kernel_types.h"
#include "transceiver.h"
#include "vtimer.h"
#include "hwtimer.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static uint16_t radio_pan;
static uint8_t  radio_channel;
static uint16_t radio_address;
static uint64_t radio_address_long;

uint8_t  driver_state;

uint8_t at86rf231_get_status(void);
void at86rf231_gpio_spi_interrupts_init(void);
void at86rf231_reset(void);


void at86rf231_init(kernel_pid_t tpid)
{
    transceiver_pid = tpid;

    at86rf231_gpio_spi_interrupts_init();

    at86rf231_reset();

    // TODO : Enable addr decode, auto ack, auto crc
    // and configure security, power, channel, pan

    radio_pan = 0;
    radio_pan = 0x00FF & (uint16_t)at86rf231_reg_read(AT86RF231_REG__PAN_ID_0);
    radio_pan |= (uint16_t)at86rf231_reg_read(AT86RF231_REG__PAN_ID_1) << 8;

    radio_channel = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA) & AT86RF231_PHY_CC_CCA_MASK__CHANNEL;

    radio_address = 0x00FF & (uint16_t)at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_0);
    radio_address |= at86rf231_reg_read(AT86RF231_REG__SHORT_ADDR_1) << 8;

    radio_address_long = 0x00000000000000FF & (uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_0);
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 8;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 16;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 24;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 32;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 40;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 48;
    radio_address_long |= ((uint64_t)at86rf231_reg_read(AT86RF231_REG__IEEE_ADDR_1)) << 56;

    at86rf231_switch_to_rx();
}

void at86rf231_switch_to_rx(void)
{
    gpio_irq_disable(AT86RF231_INT);

    // Send a FORCE TRX OFF command
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__FORCE_TRX_OFF);

    // Reset IRQ to TRX END only
    at86rf231_reg_write(AT86RF231_REG__IRQ_MASK, AT86RF231_IRQ_STATUS_MASK__TRX_END);

    // Read IRQ to clear it
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);

    // Enable IRQ interrupt
    gpio_irq_enable(AT86RF231_INT);

    // Start RX
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__RX_ON);

    // wait until it is on RX_ON state
    uint8_t status;
    uint8_t max_wait = 100;  

    do {
        status = at86rf231_get_status();
        hwtimer_wait(HWTIMER_TICKS(10));

        if (!--max_wait) {
            printf("at86rf231 : ERROR : could not enter RX_ON mode\n");
            break;
        }
    }
    while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__RX_ON);
}

void at86rf231_rx_irq(void *arg)
{
    (void)arg;

    /* check if we are in sending state */
    if (driver_state == AT_DRIVER_STATE_SENDING) {
        /* Read IRQ to clear it */
        at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);
        /* tx done, listen again */
        at86rf231_switch_to_rx();
        /* clear internal state */
        driver_state = AT_DRIVER_STATE_DEFAULT;
        return;
    }
    else {
        /* handle receive */
        at86rf231_rx_handler();
    }
}

radio_address_t at86rf231_set_address(radio_address_t address)
{
    radio_address = address;

    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_0, (uint8_t)(0x00FF & radio_address));
    at86rf231_reg_write(AT86RF231_REG__SHORT_ADDR_1, (uint8_t)(radio_address >> 8));

    return radio_address;
}

radio_address_t at86rf231_get_address(void)
{
    return radio_address;
}

uint64_t at86rf231_set_address_long(uint64_t address)
{
    radio_address_long = address;

    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_0, (uint8_t)(0x00000000000000FF & radio_address_long));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_1, (uint8_t)(0x000000000000FF00 & radio_address_long >> 8));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_2, (uint8_t)(0x0000000000FF0000 & radio_address_long >> 16));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_3, (uint8_t)(0x00000000FF000000 & radio_address_long >> 24));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_4, (uint8_t)(0x000000FF00000000 & radio_address_long >> 32));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_5, (uint8_t)(0x0000FF0000000000 & radio_address_long >> 40));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_6, (uint8_t)(0x00FF000000000000 & radio_address_long >> 48));
    at86rf231_reg_write(AT86RF231_REG__IEEE_ADDR_7, (uint8_t)(radio_address_long >> 56));

    return radio_address_long;
}

uint64_t at86rf231_get_address_long(void)
{
    return radio_address_long;
}

uint16_t at86rf231_set_pan(uint16_t pan)
{
    radio_pan = pan;

    at86rf231_reg_write(AT86RF231_REG__PAN_ID_0, (uint8_t)(0x00FF & radio_pan));
    at86rf231_reg_write(AT86RF231_REG__PAN_ID_1, (uint8_t)(radio_pan >> 8));

    return radio_pan;
}

uint16_t at86rf231_get_pan(void)
{
    return radio_pan;
}

int8_t at86rf231_set_channel(uint8_t channel)
{
    uint8_t cca_state;
    radio_channel = channel;

    if (channel < RF86RF231_MIN_CHANNEL ||
        channel > RF86RF231_MAX_CHANNEL) {
#if DEVELHELP
        puts("[at86rf231] channel out of range!");
#endif
        return -1;
    }

    cca_state = at86rf231_reg_read(AT86RF231_REG__PHY_CC_CCA) & ~AT86RF231_PHY_CC_CCA_MASK__CHANNEL;
    at86rf231_reg_write(AT86RF231_REG__PHY_CC_CCA, cca_state | (radio_channel & AT86RF231_PHY_CC_CCA_MASK__CHANNEL));

    return radio_channel;
}

uint8_t at86rf231_get_channel(void)
{
    return radio_channel;
}

void at86rf231_set_monitor(uint8_t mode)
{
    (void) mode;
    // TODO
}

void at86rf231_gpio_spi_interrupts_init(void)
{
    /* SPI init */
    spi_init_master(AT86RF231_SPI, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
    /* IRQ0 */
    gpio_init_int(AT86RF231_INT, GPIO_PULLDOWN, GPIO_RISING, at86rf231_rx_irq, NULL);
    /* CS */
    gpio_init_out(AT86RF231_CS, GPIO_NOPULL);
    /* SLEEP */
    gpio_init_out(AT86RF231_SLEEP, GPIO_NOPULL);
    /* RESET */
    gpio_init_out(AT86RF231_RESET, GPIO_NOPULL);
}

void at86rf231_reset(void)
{
    /* force reset */
    gpio_clear(AT86RF231_RESET);

    /* put pins to default values */
    gpio_set(AT86RF231_CS);
    gpio_clear(AT86RF231_SLEEP);

    /* additional waiting to comply to min rst pulse width */
    uint8_t volatile delay = 50; /* volatile to ensure it isn't optimized away */
    while (--delay);
    gpio_set(AT86RF231_RESET);

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

