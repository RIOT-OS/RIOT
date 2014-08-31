/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ryan Kurte <ryankurte@gmail.com>
 *
 * @}
 */

#include <math.h>

//#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/spi.h"

#include "em_device.h"
#include "em_usart.h"
#include "em_gpio.h"

#if SPI_NUMOF

static uint32_t speed_to_baud(spi_speed_t speed);
static uint32_t conf_to_cpol(spi_conf_t conf);

//SPI configuration storage
//static spi_state_t spi_config[SPI_NUMOF];
//TODO: add configuration for async mode

int spi_init_master(spi_t spi, spi_conf_t conf, spi_speed_t speed)
{
    USART_TypeDef *dev;
    GPIO_Port_TypeDef port;
    uint32_t mosi_pin, miso_pin, clk_pin, cs_pin, route;

    //Default configuration
    //USART_InitSync_TypeDef spi_init = USART_INITSYNC_DEFAULT;

    USART_InitSync_TypeDef spi_init = {
        .enable         = usartEnable,
        .refFreq        = 0,
        .baudrate       = 1000000,
        .databits       = usartDatabits8,
        .master         = true,
        .msbf           = true,
        .clockMode      = usartClockMode0,
        .prsRxEnable    = false,
        .prsRxCh        = usartPrsRxCh0,
        .autoTx         = false,
    };

    spi_init.baudrate = speed_to_baud(speed);
    spi_init.clockMode = conf_to_cpol(conf);

    /* enable SPI and port clocks and select devices */
    switch (spi) {
#if SPI_0_EN
    case SPI_0:
        dev = SPI_0_DEV;
        port = SPI_0_PORT;
        clk_pin  = SPI_0_CLK_PIN;
        miso_pin = SPI_0_MISO_PIN;
        mosi_pin = SPI_0_MOSI_PIN;
        cs_pin = SPI_0_CS_PIN;
        route = SPI_0_ROUTE;
        /* enable clocks */
        SPI_0_CLKEN();
        SPI_0_PORT_CLKEN();
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        dev = SPI_1_DEV;
        port = SPI_1_PORT;
        clk_pin  = SPI_1_CLK_PIN;
        miso_pin = SPI_1_MISO_PIN;
        mosi_pin = SPI_1_MOSI_PIN;
        cs_pin = SPI_1_CS_PIN;
        route = SPI_1_ROUTE;
        /* enable clocks */
        SPI_1_CLKEN();
        SPI_1_PORT_CLKEN();
        break;
#endif
    default:
        return -1;
    }

    // Configure pins
    GPIO_PinModeSet(port, clk_pin,  gpioModePushPull, 1);
    GPIO_PinModeSet(port, cs_pin,   gpioModePushPull, 1);
    GPIO_PinModeSet(port, mosi_pin, gpioModePushPull, 1);
    GPIO_PinModeSet(port, miso_pin, gpioModeInput,    0);

    // Configure SPI
    USART_InitSync(dev, &spi_init);

    // Set route
    dev->ROUTE = USART_ROUTE_CLKPEN | USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | route;
    dev->CTRL &= ~(USART_CTRL_AUTOCS);

    return 0;
}

int spi_transfer_byte(spi_t spi, char out, char *in)
{
    USART_TypeDef *dev;
    GPIO_Port_TypeDef port;
    int pin;

    switch (spi) {
#if SPI_0_EN
    case SPI_0:
        dev = SPI_0_DEV;
        port = SPI_0_PORT;
        pin = SPI_0_CS_PIN;
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        dev = SPI_1_DEV;
        port = SPI_1_PORT;
        pin = SPI_1_CS_PIN;
        break;
#endif
    default:
        return -1;
    }

    // Enable TX and RX
    USART_Enable(dev, usartEnable);

    GPIO_PinOutClear(port, pin);

    //Send and receive
    if (in != NULL) {
        (*in) = USART_SpiTransfer(dev, out);
    //Send only
    } else {
        USART_SpiTransfer(dev, out);
    }

    GPIO_PinOutSet(port, pin);
    
    //Disable TX and RX
    USART_Enable(dev, usartDisable);

    return 0;
}

int spi_transfer_bytes(spi_t spi, char *out, char *in, unsigned int length)
{
    USART_TypeDef *dev;
    GPIO_Port_TypeDef port;
    int pin;

    switch (spi) {
#if SPI_0_EN
    case SPI_0:
        dev = SPI_0_DEV;
        port = SPI_0_PORT;
        pin = SPI_0_CS_PIN;
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        dev = SPI_1_DEV;
        port = SPI_1_PORT;
        pin = SPI_1_CS_PIN;
        break;
#endif
    default:
        return -1;
    }

    // Enable TX and RX
    USART_Enable(dev, usartEnable);

    //Assert CS (low)
    GPIO_PinOutClear(port, pin);

    //Send and receive
    if ((in != NULL) && (out != NULL)) {
        for (unsigned int i = 0; i < length; i++) {
            in[i] = USART_SpiTransfer(dev, out[i]);
        }
    //Send only
    } else if (in != NULL) {
        for (unsigned int i = 0; i < length; i++) {
            in[i] = USART_SpiTransfer(dev, 0xFF);
        }
    //Receive only
    } else if (out != NULL) {
        for (unsigned int i = 0; i < length; i++) {
            USART_SpiTransfer(dev, out[i]);
        }
    }

    //Deassert CS (high)
    GPIO_PinOutSet(port, pin);

    //Disable TX and RX
    USART_Enable(dev, usartDisable);

    return 0;
}

//Convert speeds to integers
static uint32_t speed_to_baud(spi_speed_t speed)
{
    uint32_t baud;

    switch (speed) {
    case SPI_SPEED_100KHZ:
        baud = 100000;
        break;
    case SPI_SPEED_400KHZ:
        baud = 400000;
        break;
    case SPI_SPEED_1MHZ:
        baud = 1000000;
        break;
    case SPI_SPEED_5MHZ:
        baud = 5000000;
        break;
    case SPI_SPEED_10MHZ:
        baud = 10000000;
        break;
    default:
        baud = 1000000;
        break;
    }

    return baud;
}

//Convert to normal modes
static uint32_t conf_to_cpol(spi_conf_t conf)
{
    uint32_t cpol;

    switch (conf) {
    case SPI_CONF_IDLE_LOW_RISING:
        cpol = usartClockMode0;
        break;
    case SPI_CONF_IDLE_LOW_FALLING:
        cpol = usartClockMode1;
        break;
    case SPI_CONF_IDLE_HIGH_RISING:
        cpol = usartClockMode3;
        break;
    case SPI_CONF_IDLE_HIGH_FALLING:
        cpol = usartClockMode2;
        break;
    default:
        cpol = usartClockMode0;
        break;
    }

    return cpol;
}

#endif
