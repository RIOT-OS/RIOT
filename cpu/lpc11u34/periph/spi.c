/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    LPC_SSPx_Type *spi;
    /* power on the SPI device */
    spi_poweron(dev);

    /* configure SCK, MISO and MOSI pin */
    spi_conf_pins(dev);

    switch(dev) {
#if SPI_0_EN
        case SPI_0:
            spi = LPC_SSP0;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = LPC_SSP1;
            break;
#endif
        default:
            return -1;
    }

    /* Master mode, SPI disabled */
    spi->CR1 = 0;
    /* Base clock frequency : 12MHz */
    spi->CPSR = 4;
    /* configure bus clock speed */
    switch (speed) {
        case SPI_SPEED_100KHZ:
            spi->CR0 |= (119 << 8);
            break;
        case SPI_SPEED_400KHZ:
            spi->CR0 |= (29 << 8);
            break;
        case SPI_SPEED_1MHZ:
            spi->CR0 |= (11 << 8);
            break;
        case SPI_SPEED_5MHZ:
            spi->CR0 |= (2 << 8); /* Actual : 4MHz */
            break;
        case SPI_SPEED_10MHZ:
            spi->CR0 |= (0 << 8); /* Actual : 12MHz */
            break;
    }
    /* Set mode and 8-bit transfer */
    spi->CR0 |= 0x07 | (conf << 6);
    /* Enable SPI */
    spi->CR1 |= (1 << 1);

    /* Wait while the BUSY flag is set */
    while(spi->SR & (1 << 4)) {}
    /* Clear the RX FIFO */
    while(spi->SR & (1 << 2)) {
        spi->DR;
    }

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    /* Slave mode not supported */
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            /* SPI0 : MISO */
            LPC_IOCON->PIO0_8 |= 1;
            /* SPI0 : MOSI */
            LPC_IOCON->PIO0_9 |= 1;
            /* SPI0 : SCK */
            LPC_IOCON->SWCLK_PIO0_10 |= 2;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            /* SPI1 : MISO */
            LPC_IOCON->PIO1_21 |= 2;
            /* SPI1 : MOSI */
            LPC_IOCON->PIO0_21 |= 2;
            /* SPI1 : SCK */
            LPC_IOCON->PIO1_20 |= 2;
#endif
        default:
            return -1;
    }

    return 0;
}

int spi_acquire(spi_t dev)
{
    if ((unsigned int)dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if ((unsigned int)dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    char tmp;
    LPC_SSPx_Type *spi;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = LPC_SSP0;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = LPC_SSP1;
            break;
#endif
        default:
            return 0;
    }

    /* Wait while the BUSY flag is set */
    while(spi->SR & (1 << 4)) {}
    /* Put byte in the TX Fifo */
    *((volatile uint8_t *)(&spi->DR)) = (uint8_t)out;
    /* Wait until the current byte is transfered */
    while(!(spi->SR & (1 << 2)) ) {}
    /* Read the returned byte */
    tmp = *((volatile uint8_t *)(&spi->DR));
    /* 'return' response byte if wished for */
    if (in) {
        *in = tmp;
    }

    return 1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* Slave mode not supported */
}

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            /* De-assert SPI0 */
            LPC_SYSCON->PRESETCTRL |= (1 << 0);
            /* Enable SPI0 clock */
            LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 11);
            /* Clock div : 48MHz */
            LPC_SYSCON->SSP0CLKDIV = 1;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            /* De-assert SPI1 */
            LPC_SYSCON->PRESETCTRL |= (1 << 2);
            /* Enable SPI1 clock */
            LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 18);
            /* Clock div : 48MHz */
            LPC_SYSCON->SSP1CLKDIV = 1;
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            /* Assert SPI0 */
            LPC_SYSCON->PRESETCTRL &= ~(1 << 0);
            /* Disable SPI0 clock */
            LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 11);
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            /* Assert SPI1 */
            LPC_SYSCON->PRESETCTRL &= ~(1 << 2);
            /* Disable SPI1 clock */
            LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 18);
            break;
#endif
    }
}

#endif /* SPI_NUMOF */
