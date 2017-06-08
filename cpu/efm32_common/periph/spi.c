/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 *
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_usart.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Convert speeds to integers
 */
static uint32_t speed_to_baud(spi_speed_t speed)
{
    uint32_t baud = 1000000;

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
    }

    return baud;
}

/**
 * @brief Convert to normal modes
 */
static uint32_t conf_to_cpol(spi_conf_t conf)
{
    uint32_t cpol = usartClockMode0;

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            cpol = usartClockMode0;
            break;
        case SPI_CONF_SECOND_RISING:
            cpol = usartClockMode1;
            break;
        case SPI_CONF_FIRST_FALLING:
            cpol = usartClockMode2;
            break;
        case SPI_CONF_SECOND_FALLING:
            cpol = usartClockMode3;
            break;
    }

    return cpol;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    /* check if device is valid */
    if (dev >= UART_NUMOF) {
        return -1;
    }

    /* configure the miso/mosi/clk pins */
    spi_conf_pins(dev);

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(spi_config[dev].cmu, true);

    /* configure SPI */
    USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
    
    init.baudrate = speed_to_baud(speed);
    init.clockMode = conf_to_cpol(conf);
    init.msbf = true;

    USART_InitSync(spi_config[dev].dev, &init);

#ifdef _SILICON_LABS_32B_PLATFORM_1
    spi_config[dev].dev->ROUTE = (spi_config[dev].loc |
                                  USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN);
#else
    spi_config[dev].dev->ROUTEPEN =
        USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN;
    spi_config[dev].dev->ROUTELOC0 = spi_config[dev].loc;
#endif

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    /* configure the pins */
    gpio_init(spi_config[dev].clk_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(spi_config[dev].mosi_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(spi_config[dev].miso_pin, GPIO_DIR_IN, GPIO_PULLDOWN);

    return 0;
}

int spi_acquire(spi_t dev)
{
    mutex_lock((mutex_t *) &spi_config[dev].lock);

    return 0;
}

int spi_release(spi_t dev)
{
    mutex_unlock((mutex_t *) &spi_config[dev].lock);

    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    if (in != NULL) {
        // Send and receive
        (*in) = USART_SpiTransfer(spi_config[dev].dev, out);
    }
    else {
        // Send only
        USART_SpiTransfer(spi_config[dev].dev, out);
    }

    return 0;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    return;
}

void spi_poweron(spi_t dev)
{
    CMU_ClockEnable(spi_config[dev].cmu, true);
}

void spi_poweroff(spi_t dev)
{
    CMU_ClockEnable(spi_config[dev].cmu, false);
}

#endif /* SPI_NUMOF */
