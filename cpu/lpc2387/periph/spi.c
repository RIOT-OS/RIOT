/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc2387
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

#if SPI_0_EN

#define SPI_TX_EMPTY                (SSP0SR & SSPSR_TFE)
#define SPI_BUSY                    (SSP0SR & SSPSR_BSY)
#define SPI_RX_AVAIL                (SSP0SR & SSPSR_RNE)

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
#if SPI_2_EN
    [SPI_2] = MUTEX_INIT
#endif
};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    if (dev) {
        return -1;
    }

    uint32_t   f_baud = 0;
    switch(speed)
    {
    case SPI_SPEED_100KHZ:
        f_baud = 100;
        break;
    case SPI_SPEED_400KHZ:
        f_baud = 400;
        break;
    case SPI_SPEED_1MHZ:
        f_baud = 1000;
        break;
    case SPI_SPEED_5MHZ:
        f_baud = 5000;
        break;
    case SPI_SPEED_10MHZ:
        f_baud = 10000;
        break;
    }

#if 0
    /* TODO */
    switch(conf)
    {
    case SPI_CONF_FIRST_RISING:
        /**< first data bit is transacted on the first rising SCK edge */
        cpha = 0;
        cpol = 0;
        break;
    case SPI_CONF_SECOND_RISING:
        /**< first data bit is transacted on the second rising SCK edge */
        cpha = 1;
        cpol = 0;
        break;
    case SPI_CONF_FIRST_FALLING:
        /**< first data bit is transacted on the first falling SCK edge */
        cpha = 0;
        cpol = 1;
        break;
    case SPI_CONF_SECOND_FALLING:
        /**< first data bit is transacted on the second falling SCK edge */
        cpha = 1;
        cpol = 1;
        break;
    }
#endif

    /* Power*/
    PCONP |= PCSSP0;                /* Enable power for SSP0 (default is on)*/

    /* PIN Setup*/
    spi_conf_pins(dev);

    /* Interface Setup*/
    SSP0CR0 = 7;

    /* Clock Setup*/
    uint32_t pclksel;
    uint32_t cpsr;
    lpc2387_pclk_scale(F_CPU / 1000, f_baud, &pclksel, &cpsr);
    PCLKSEL1 &= ~(BIT10 | BIT11);   /* CCLK to PCLK divider*/
    PCLKSEL1 |= pclksel << 10;
    SSP0CPSR = cpsr;

    /* Enable*/
    SSP0CR1 |= BIT1;                /* SSP-Enable*/
    int dummy;

    /* Clear RxFIFO:*/
    while (SPI_RX_AVAIL) {          /* while RNE (Receive FIFO Not Empty)...*/
        dummy = SSP0DR;             /* read data*/
    }

    /* to suppress unused-but-set-variable */
    (void) dummy;
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    (void)dev;
    (void)conf;
    (void)cb;
    printf("%s:%s(): stub\n", RIOT_FILE_RELATIVE, __func__);
    /* TODO */
    return -1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    (void)dev;
    (void)reset_val;
    printf("%s:%s(): stub\n", RIOT_FILE_RELATIVE, __func__);
    /* TODO*/
}

int spi_acquire(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    while (!SPI_TX_EMPTY);
    SSP0DR = out;
    while (SPI_BUSY);
    while (!SPI_RX_AVAIL);

    char tmp = (char)SSP0DR;

    if (in != NULL) {
        *in = tmp;
    }

    return 1;
}

void spi_poweron(spi_t dev)
{
}

void spi_poweroff(spi_t dev)
{
}

int spi_conf_pins(spi_t dev)
{
    switch (dev) {
        case 0:
            PINSEL3 |= BIT8 + BIT9;     /* SCLK */
            PINSEL3 |= BIT14 + BIT15;   /* MISO */
            PINSEL3 |= BIT16 + BIT17;   /* MOSI */
            return 0;
        default:
            return -1;
    }
}

#endif /* SPI_0_EN */
