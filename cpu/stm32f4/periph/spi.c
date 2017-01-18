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
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Data-structure holding the state for a SPI device
 */
typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(SPI_TypeDef *spi, spi_t dev);

/**
 * @brief Reserve memory for saving the SPI device's state
 */
static spi_state_t spi_config[SPI_NUMOF];

/* static bus div mapping */
static const uint8_t spi_bus_div_map[SPI_NUMOF] = {
#if SPI_0_EN
    [SPI_0] = SPI_0_BUS_DIV,
#endif
#if SPI_1_EN
    [SPI_1] = SPI_1_BUS_DIV,
#endif
#if SPI_2_EN
    [SPI_2] = SPI_2_BUS_DIV,
#endif
};

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
    uint8_t speed_devider;
    SPI_TypeDef *spi_port;

    switch (speed) {
        case SPI_SPEED_100KHZ:
            return -2;          /* not possible for stm32f4, APB2 minimum is 328 kHz */
            break;
        case SPI_SPEED_400KHZ:
            speed_devider = 0x05 + spi_bus_div_map[dev];  /* makes 656 kHz */
            break;
        case SPI_SPEED_1MHZ:
            speed_devider = 0x04 + spi_bus_div_map[dev];  /* makes 1.3 MHz */
            break;
        case SPI_SPEED_5MHZ:
            speed_devider = 0x02 + spi_bus_div_map[dev];  /* makes 5.3 MHz */
            break;
        case SPI_SPEED_10MHZ:
            speed_devider = 0x01 + spi_bus_div_map[dev];  /* makes 10.5 MHz */
            break;
        default:
            return -1;
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
            break;
#endif /* SPI_1_EN */
#if SPI_2_EN
        case SPI_2:
            spi_port = SPI_2_DEV;
            /* enable clocks */
            SPI_2_CLKEN();
            SPI_2_SCK_PORT_CLKEN();
            SPI_2_MISO_PORT_CLKEN();
            SPI_2_MOSI_PORT_CLKEN();
            break;
#endif /* SPI_2_EN */
        default:
            return -2;
    }

    /* configure SCK, MISO and MOSI pin */
    spi_conf_pins(dev);

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
            break;
#endif /* SPI_1_EN */
#if SPI_2_EN
        case SPI_2:
            spi_port = SPI_2_DEV;
            /* enable clocks */
            SPI_2_CLKEN();
            SPI_2_SCK_PORT_CLKEN();
            SPI_2_MISO_PORT_CLKEN();
            SPI_2_MOSI_PORT_CLKEN();
            /* configure interrupt channel */
            NVIC_SetPriority(SPI_2_IRQ, SPI_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_2_IRQ);
            break;
#endif /* SPI_2_EN */
        default:
            return -1;
    }

    /* configure sck, miso and mosi pin */
    spi_conf_pins(dev);

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

int spi_conf_pins(spi_t dev)
{
    GPIO_TypeDef *port[3];
    int pin[3], af[3];

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            port[0] = SPI_0_SCK_PORT;
            pin[0] = SPI_0_SCK_PIN;
            af[0] = SPI_0_SCK_AF;
            port[1] = SPI_0_MOSI_PORT;
            pin[1] = SPI_0_MOSI_PIN;
            af[1] = SPI_0_MOSI_AF;
            port[2] = SPI_0_MISO_PORT;
            pin[2] = SPI_0_MISO_PIN;
            af[2] = SPI_0_MISO_AF;
            break;
#endif /* SPI_0_EN */
#if SPI_1_EN
        case SPI_1:
            port[0] = SPI_1_SCK_PORT;
            pin[0] = SPI_1_SCK_PIN;
            af[0] = SPI_1_SCK_AF;
            port[1] = SPI_1_MOSI_PORT;
            pin[1] = SPI_1_MOSI_PIN;
            af[1] = SPI_1_MOSI_AF;
            port[2] = SPI_1_MISO_PORT;
            pin[2] = SPI_1_MISO_PIN;
            af[2] = SPI_1_MISO_AF;
            break;
#endif /* SPI_1_EN */
#if SPI_2_EN
        case SPI_2:
            port[0] = SPI_2_SCK_PORT;
            pin[0] = SPI_2_SCK_PIN;
            af[0] = SPI_2_SCK_AF;
            port[1] = SPI_2_MOSI_PORT;
            pin[1] = SPI_2_MOSI_PIN;
            af[1] = SPI_2_MOSI_AF;
            port[2] = SPI_2_MISO_PORT;
            pin[2] = SPI_2_MISO_PIN;
            af[2] = SPI_2_MISO_AF;
            break;
#endif /* SPI_2_EN */
        default:
            return -1;
    }

    /***************** GPIO-Init *****************/
    for (int i = 0; i < 3; i++) {
        /* Set GPIOs to AF mode */
        port[i]->MODER &= ~(3 << (2 * pin[i]));
        port[i]->MODER |= (2 << (2 * pin[i]));
        /* Set speed */
        port[i]->OSPEEDR &= ~(3 << (2 * pin[i]));
        port[i]->OSPEEDR |= (3 << (2 * pin[i]));
        /* Set to push-pull configuration */
        port[i]->OTYPER &= ~(1 << pin[i]);
        /* Configure push-pull resistors */
        port[i]->PUPDR &= ~(3 << (2 * pin[i]));
        port[i]->PUPDR |= (2 << (2 * pin[i]));
        /* Configure GPIOs for the SPI alternate function */
        int hl = (pin[i] < 8) ? 0 : 1;
        port[i]->AFR[hl] &= ~(0xf << ((pin[i] - (hl * 8)) * 4));
        port[i]->AFR[hl] |= (af[i] << ((pin[i] - (hl * 8)) * 4));
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
#if SPI_2_EN
        case SPI_2:
            spi_port = SPI_2_DEV;
            break;
#endif
        default:
            return -1;
    }

    while (!(spi_port->SR & SPI_SR_TXE)) {}
    spi_port->DR = out;

    while (!(spi_port->SR & SPI_SR_RXNE)) {}

    if (in != NULL) {
        *in = spi_port->DR;
    }
    else {
        spi_port->DR;
    }

    return 1;
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
#if SPI_2_EN
        case SPI_2:
            SPI_2_DEV->DR = reset_val;
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
#if SPI_2_EN
        case SPI_2:
            SPI_2_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_BSY) {}
            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            while (SPI_1_DEV->SR & SPI_SR_BSY) {}
            SPI_1_CLKDIS();
            break;
#endif
#if SPI_2_EN
        case SPI_2:
            while (SPI_2_DEV->SR & SPI_SR_BSY) {}
            SPI_2_CLKDIS();
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
    cortexm_isr_end();
}

#if SPI_0_EN
void SPI_0_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_0_DEV, SPI_0);
}
#endif

#if SPI_1_EN
void SPI_1_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_1_DEV, SPI_1);
}
#endif

#if SPI_2_EN
void SPI_2_IRQ_HANDLER(void)
{
    irq_handler_transfer(SPI_2_DEV, SPI_2);
}
#endif

#endif /* SPI_NUMOF */
