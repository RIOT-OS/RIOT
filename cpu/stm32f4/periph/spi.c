/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"
#include "vtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(SPI_TypeDef *spi, spi_t dev);

static spi_state_t spi_config[SPI_NUMOF];

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{

    uint8_t speed_devider;
    SPI_TypeDef *spi_port;

    switch (speed) {
        case SPI_SPEED_100KHZ:
            return -2;          /* not possible for stm32f4 */
            break;
        case SPI_SPEED_400KHZ:
            speed_devider = 7;  /* makes 656 kHz */
            break;
        case SPI_SPEED_1MHZ:
            speed_devider = 6;  /* makes 1.3 MHz */
            break;
        case SPI_SPEED_5MHZ:
            speed_devider = 4;  /* makes 5.3 MHz */
            break;
        case SPI_SPEED_10MHZ:
            speed_devider = 3;  /* makes 10.5 MHz */
            break;
        default:
            return -2;
    }

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_MISO_PORT_CLKEN();
            SPI_0_MOSI_PORT_CLKEN();

            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode */
            SPI_0_SCK_PORT->MODER &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->MODER |= (2 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->MODER &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->MODER |= (2 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->MODER &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->MODER |= (2 << (2 * SPI_0_MOSI_PIN));
            /* Set speed */
            SPI_0_SCK_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->OSPEEDR |= (3 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->OSPEEDR |= (3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->OSPEEDR |= (3 << (2 * SPI_0_MOSI_PIN));
            /* Set to push-pull configuration */
            SPI_0_SCK_PORT->OTYPER &= ~(1 << SPI_0_SCK_PIN);
            SPI_0_MISO_PORT->OTYPER &= ~(1 << SPI_0_MISO_PIN);
            SPI_0_MOSI_PORT->OTYPER &= ~(1 << SPI_0_MOSI_PIN);
            /* Configure push-pull resistors */
            SPI_0_SCK_PORT->PUPDR &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->PUPDR |= (2 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->PUPDR &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->PUPDR |= (2 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->PUPDR &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->PUPDR |= (2 << (2 * SPI_0_MOSI_PIN));
            /* Configure GPIOs to for the SPI0 alternate function */
#if (SPI_0_SCK_PIN < 8)
            SPI_0_SCK_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->AFR[0] |= (SPI_0_SCK_AF << (4 * SPI_0_SCK_PIN));

#else
            SPI_0_SCK_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_SCK_PIN - 8)));
            SPI_0_SCK_PORT->AFR[1] |= (SPI_0_SCK_AF << (4 * (SPI_0_SCK_PIN - 8)));
#endif

#if (SPI_0_MISO_PIN < 8)
            SPI_0_MISO_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->AFR[0] |= (SPI_0_MISO_AF << (4 * SPI_0_MISO_PIN));
#else
            SPI_0_MISO_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_MISO_PIN - 8)));
            SPI_0_MISO_PORT->AFR[1] |= (SPI_0_MISO_AF << (4 * (SPI_0_MISO_PIN - 8)));
#endif

#if (SPI_0_MOSI_PIN < 8)
            SPI_0_MOSI_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->AFR[0] |= (SPI_0_MOSI_AF << (4 * SPI_0_MOSI_PIN));
#else
            SPI_0_MOSI_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_MOSI_PIN - 8)));
            SPI_0_MOSI_PORT->AFR[1] |= (SPI_0_MOSI_AF << (4 * (SPI_0_MOSI_PIN - 8)));
#endif
            break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
            spi_port = SPI_1_DEV;
            /* enable clocks */
            SPI_1_CLKEN();
            SPI_1_SCK_PORT_CLKEN();
            SPI_1_MISO_PORT_CLKEN();
            SPI_1_MOSI_PORT_CLKEN();

            /************************* GPIO-Init *************************/
            /* Set GPIOs to AF mode */
            SPI_1_SCK_PORT->MODER &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->MODER |= (2 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->MODER &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->MODER |= (2 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->MODER &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->MODER |= (2 << (2 * SPI_1_MOSI_PIN));
            /* Set speed */
            SPI_1_SCK_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->OSPEEDR |= (3 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->OSPEEDR |= (3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->OSPEEDR |= (3 << (2 * SPI_1_MOSI_PIN));
            /* Set to push-pull configuration */
            SPI_1_SCK_PORT->OTYPER &= ~(1 << SPI_1_SCK_PIN);
            SPI_1_MISO_PORT->OTYPER &= ~(1 << SPI_1_MISO_PIN);
            SPI_1_MOSI_PORT->OTYPER &= ~(1 << SPI_1_MOSI_PIN);
            /* Configure push-pull resistors */
            SPI_1_SCK_PORT->PUPDR &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->PUPDR |= (2 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->PUPDR &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->PUPDR |= (2 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->PUPDR &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->PUPDR |= (2 << (2 * SPI_1_MOSI_PIN));
            /* Configure the pins alternate function */
#if (SPI_1_SCK_PIN < 8)
            SPI_1_SCK_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->AFR[0] |= (SPI_1_SCK_AF << (4 * SPI_1_SCK_PIN));
#else
            SPI_1_SCK_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_SCK_PIN - 8)));
            SPI_1_SCK_PORT->AFR[1] |= (SPI_1_SCK_AF << (4 * (SPI_1_SCK_PIN - 8)));
#endif

#if (SPI_1_MISO_PIN < 8)
            SPI_1_MISO_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->AFR[0] |= (SPI_1_MISO_AF << (4 * SPI_1_MISO_PIN));
#else
            SPI_1_MISO_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_MISO_PIN - 8)));
            SPI_1_MISO_PORT->AFR[1] |= (SPI_1_MISO_AF << (4 * (SPI_1_MISO_PIN - 8)));
#endif

#if (SPI_1_MOSI_PIN < 8)
            SPI_1_MOSI_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->AFR[0] |= (SPI_1_MOSI_AF << (4 * SPI_1_MOSI_PIN));
#else
            SPI_1_MOSI_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_MOSI_PIN - 8)));
            SPI_1_MOSI_PORT->AFR[1] |= (SPI_1_MOSI_AF << (4 * (SPI_1_MOSI_PIN - 8)));
#endif
            break;
#endif /* SPI_1_EN */
        default:
            return -1;
    }

    /**************** SPI-Init *****************/
    spi_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);/* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    spi_port->CR1 = 0;
    spi_port->CR2 = 0;
    /* the NSS (chip select) is managed purely by software */
    spi_port->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    spi_port->CR1 |= (speed_devider << 3);  /* Define serial clock baud rate. 001 leads to f_PCLK/4 */
    spi_port->CR1 |= (SPI_CR1_MSTR);  /* 1: master configuration */
    spi_port->CR1 |= (conf);
    /* enable SPI */
    spi_port->CR1 |= (SPI_CR1_SPE);
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{
    SPI_TypeDef *spi_port;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            /* enable clocks */
            SPI_0_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            SPI_0_MISO_PORT_CLKEN();
            SPI_0_MOSI_PORT_CLKEN();
            /* configure interrupt channel */
            NVIC_SetPriority(SPI_0_IRQ, SPI_IRQ_PRIO); /* set SPI interrupt priority */
            NVIC_EnableIRQ(SPI_0_IRQ); /* set SPI interrupt priority */

            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode (not especially input or output) */
            SPI_0_SCK_PORT->MODER &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->MODER |= (2 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->MODER &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->MODER |= (2 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->MODER &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->MODER |= (2 << (2 * SPI_0_MOSI_PIN));
            /* Set speed */
            SPI_0_SCK_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->OSPEEDR |= (3 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->OSPEEDR |= (3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->OSPEEDR |= (3 << (2 * SPI_0_MOSI_PIN));
            /* Set to push-pull configuration (not open drain) */
            SPI_0_SCK_PORT->OTYPER &= ~(1 << SPI_0_SCK_PIN);
            SPI_0_MISO_PORT->OTYPER &= ~(1 << SPI_0_MISO_PIN);
            SPI_0_MOSI_PORT->OTYPER &= ~(1 << SPI_0_MOSI_PIN);
            /* Configure push-pull resistors */
            SPI_0_SCK_PORT->PUPDR &= ~(3 << (2 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->PUPDR |= (2 << (2 * SPI_0_SCK_PIN));
            SPI_0_MISO_PORT->PUPDR &= ~(3 << (2 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->PUPDR |= (2 << (2 * SPI_0_MISO_PIN));
            SPI_0_MOSI_PORT->PUPDR &= ~(3 << (2 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->PUPDR |= (2 << (2 * SPI_0_MOSI_PIN));
            /* configure the pins alternate function */
#if (SPI_0_SCK_PIN < 8)
            SPI_0_SCK_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_SCK_PIN));
            SPI_0_SCK_PORT->AFR[0] |= (SPI_0_SCK_AF << (4 * SPI_0_SCK_PIN));
#else
            SPI_0_SCK_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_SCK_PIN - 8)));
            SPI_0_SCK_PORT->AFR[1] |= (SPI_0_SCK_AF << (4 * (SPI_0_SCK_PIN - 8)));
#endif
#if (SPI_0_MISO_PIN < 8)
            SPI_0_MISO_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_MISO_PIN));
            SPI_0_MISO_PORT->AFR[0] |= (SPI_0_MISO_AF << (4 * SPI_0_MISO_PIN));
#else
            SPI_0_MISO_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_MISO_PIN - 8)));
            SPI_0_MISO_PORT->AFR[1] |= (SPI_0_MISO_AF << (4 * (SPI_0_MISO_PIN - 8)));
#endif
#if (SPI_0_MOSI_PIN < 8)
            SPI_0_MOSI_PORT->AFR[0] &= ~(0xf << (4 * SPI_0_MOSI_PIN));
            SPI_0_MOSI_PORT->AFR[0] |= (SPI_0_MOSI_AF << (4 * SPI_0_MOSI_PIN));
#else
            SPI_0_MOSI_PORT->AFR[1] &= ~(0xf << (4 * (SPI_0_MOSI_PIN - 8)));
            SPI_0_MOSI_PORT->AFR[1] |= (SPI_0_MOSI_AF << (4 * (SPI_0_MOSI_PIN - 8)));
#endif
            break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
            spi_port = SPI_1_DEV;
            /* enable clocks */
            SPI_1_CLKEN();
            SPI_1_SCK_PORT_CLKEN();
            SPI_1_MISO_PORT_CLKEN();
            SPI_1_MOSI_PORT_CLKEN();
            /* configure interrupt channel */
            NVIC_SetPriority(SPI_1_IRQ, SPI_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_1_IRQ);

            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode (not especially input or output) */
            SPI_1_SCK_PORT->MODER &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->MODER |= (2 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->MODER &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->MODER |= (2 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->MODER &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->MODER |= (2 << (2 * SPI_1_MOSI_PIN));
            /* Set speed */
            SPI_1_SCK_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->OSPEEDR |= (3 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->OSPEEDR |= (3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->OSPEEDR |= (3 << (2 * SPI_1_MOSI_PIN));
            /* Set to push-pull configuration (not open drain) */
            SPI_1_SCK_PORT->OTYPER &= ~(1 << SPI_1_SCK_PIN);
            SPI_1_MISO_PORT->OTYPER &= ~(1 << SPI_1_MISO_PIN);
            SPI_1_MOSI_PORT->OTYPER &= ~(1 << SPI_1_MOSI_PIN);
            /* Configure push-pull resistors */
            SPI_1_SCK_PORT->PUPDR &= ~(3 << (2 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->PUPDR |= (2 << (2 * SPI_1_SCK_PIN));
            SPI_1_MISO_PORT->PUPDR &= ~(3 << (2 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->PUPDR |= (2 << (2 * SPI_1_MISO_PIN));
            SPI_1_MOSI_PORT->PUPDR &= ~(3 << (2 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->PUPDR |= (2 << (2 * SPI_1_MOSI_PIN));

#if (SPI_1_SCK_PIN < 8)
            SPI_1_SCK_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_SCK_PIN));
            SPI_1_SCK_PORT->AFR[0] |= (SPI_1_SCK_AF << (4 * SPI_1_SCK_PIN));
#else
            SPI_1_SCK_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_SCK_PIN - 8)));
            SPI_1_SCK_PORT->AFR[1] |= (SPI_1_SCK_AF << (4 * (SPI_1_SCK_PIN - 8)));
#endif
#if (SPI_1_MISO_PIN < 8)
            SPI_1_MISO_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_MISO_PIN));
            SPI_1_MISO_PORT->AFR[0] |= (SPI_1_MISO_AF << (4 * SPI_1_MISO_PIN));
#else
            SPI_1_MISO_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_MISO_PIN - 8)));
            SPI_1_MISO_PORT->AFR[1] |= (SPI_1_MISO_AF << (4 * (SPI_1_MISO_PIN - 8)));
#endif
#if (SPI_1_MOSI_PIN < 8)
            SPI_1_MOSI_PORT->AFR[0] &= ~(0xf << (4 * SPI_1_MOSI_PIN));
            SPI_1_MOSI_PORT->AFR[0] |= (SPI_1_MOSI_AF << (4 * SPI_1_MOSI_PIN));
#else
            SPI_1_MOSI_PORT->AFR[1] &= ~(0xf << (4 * (SPI_1_MOSI_PIN - 8)));
            SPI_1_MOSI_PORT->AFR[1] |= (SPI_1_MOSI_AF << (4 * (SPI_1_MOSI_PIN - 8)));
#endif
            break;
#endif /* SPI_1_EN */
        default:
            return -1;
    }

    /***************** SPI-Init *****************/
    spi_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);
    spi_port->CR1 = 0;
    spi_port->CR2 = 0;
    /* enable RXNEIE flag to enable rx buffer not empty interrupt */
    spi_port->CR2 |= (SPI_CR2_RXNEIE); /*1:not masked */
    spi_port->CR1 |= (conf);
     /* the NSS (chip select) is managed by software and NSS is low (slave enabled) */
    spi_port->CR1 |= SPI_CR1_SSM;
    /* set callback */
    spi_config[dev].cb = cb;
    /* enable SPI device */
    spi_port->CR1 |= SPI_CR1_SPE;
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_TypeDef *spi_port;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi_port = SPI_1_DEV;
            break;
#endif
        default:
            return -1;
    }

    while (!(spi_port->SR & SPI_SR_TXE));
    spi_port->DR = out;

    while (!(spi_port->SR & SPI_SR_RXNE));

    if (in != NULL) {
        *in = spi_port->DR;
    }
    else {
        spi_port->DR;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{

    int i, trans_ret, trans_bytes = 0;
    char in_temp;

    for (i = 0; i < length; i++) {
        if (out) {
            trans_ret = spi_transfer_byte(dev, out[i], &in_temp);
        }
        else {
            trans_ret = spi_transfer_byte(dev, 0, &in_temp);
        }
        if (trans_ret < 0) {
            return -1;
        }
        if (in != NULL) {
            in[i] = in_temp;
        }
        trans_bytes++;
    }

    return trans_bytes++;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, in);
    if (trans_ret < 0) {
        return -2;
    }
    trans_ret = spi_transfer_byte(dev, out, in);
    if (trans_ret < 0) {
        return -3;
    }

    return 1;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, in);
    if (trans_ret < 0) {
        return -2;
    }
    trans_ret = spi_transfer_bytes(dev, out, in, length);
    if (trans_ret < 0) {
        return -3;
    }

    return trans_ret;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_DEV->DR = reset_val;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_1_DEV->DR = reset_val;
            break;
#endif
    }
}

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_1_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_BSY);
            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            while (SPI_1_DEV->SR & SPI_SR_BSY);
            SPI_1_CLKDIS();
            break;
#endif
    }
}

static inline void irq_handler_transfer(SPI_TypeDef *spi, spi_t dev)
{

    if (spi->SR & SPI_SR_RXNE) {
        char data;
        data = spi->DR;
        data = spi_config[dev].cb(data);
        spi->DR = data;
    }
    /* see if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if SPI_0_EN
__attribute__((naked)) void SPI_0_IRQ_HANDLER(void)
{
    ISR_ENTER();
    irq_handler_transfer(SPI_0_DEV, SPI_0);
    ISR_EXIT();
}
#endif

#if SPI_1_EN
__attribute__((naked)) void SPI_1_IRQ_HANDLER(void)
{
    ISR_ENTER();
    irq_handler_transfer(SPI_1_DEV, SPI_1);
    ISR_EXIT();
}
#endif

#endif /* SPI_NUMOF */
