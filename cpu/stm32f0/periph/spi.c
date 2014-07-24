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
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/* this value will be send in return of the first transfered byte when in slave mode */
#define RESET_VALUE     (0x77)

/**
 * @brief unified interrupt handler to be shared between SPI devices
 *
 * @param[in] spi       Pointer to the devices base register
 * @param[in] dev       The device that triggered the interrupt
 */
static inline void irq_handler(SPI_TypeDef *spi, spi_t dev);

/**
 * @brief structure that defines the state for an SPI device
 */
typedef struct {
    char (*cb)(char data);
} spi_state_t;

/**
 * @brief array with one field for each possible SPI device
 */
static spi_state_t spi_config[SPI_NUMOF];


int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_TypeDef *spi = 0;
    GPIO_TypeDef *port = 0;
    int pin[3];        /* 3 pins: sck, miso, mosi */
    int af;

    /* power on the SPI device */
    spi_poweron(dev);

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            port = SPI_0_PORT;
            pin[0] = SPI_0_PIN_SCK;
            pin[1] = SPI_0_PIN_MISO;
            pin[2] = SPI_0_PIN_MOSI;
            af = SPI_0_PIN_AF;
            SPI_0_PORT_CLKEN();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            port = SPI_1_PORT;
            pin[0] = SPI_1_PIN_SCK;
            pin[1] = SPI_1_PIN_MISO;
            pin[2] = SPI_1_PIN_MOSI;
            af = SPI_1_PIN_AF;
            SPI_0_PORT_CLKEN();
            break;
#endif
    }

    /* configure pins for their correct alternate function */
    for (int i = 0; i < 3; i++) {
        port->MODER &= ~(3 << (pin[i] * 2));
        port->MODER |= (2 << (pin[i] * 2));
        int hl = (pin[i] < 8) ? 0 : 1;
        port->AFR[hl] &= (0xf << ((pin[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pin[i] - (hl * 8)) * 4));
    }

    /* reset SPI configuration registers */
    spi->CR1 = 0;
    spi->CR2 = 0;
    spi->I2SCFGR = 0;       /* this makes sure SPI mode is selected */

    /* set data-size to 8-bit */
    spi->CR2 |= (7 << 8);

    /* configure bus clock speed */
    switch (speed) {
        case SPI_SPEED_100KHZ:
            spi->CR1 |= (7 << 3);       /* actual clock: 187.5KHz (lowest possible) */
            break;
        case SPI_SPEED_400KHZ:
            spi->CR1 |= (6 << 3);       /* actual clock: 375KHz */
            break;
        case SPI_SPEED_1MHZ:
            spi->CR1 |= (4 << 3);       /* actual clock: 1.5MHz */
            break;
        case SPI_SPEED_5MHZ:
            spi->CR1 |= (2 << 3);       /* actual clock: 6MHz */
            break;
        case SPI_SPEED_10MHZ:
            spi->CR1 |= (1 << 3);       /* actual clock 12MHz */
    }

    /* select clock polarity and clock phase */
    spi->CR1 |= conf;

    /* select master mode */
    spi->CR1 |= SPI_CR1_MSTR;

    /* enable the SPI device */
    spi->CR1 |= SPI_CR1_SPE;

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    SPI_TypeDef *spi = 0;
    GPIO_TypeDef *port = 0;
    int pin[3];        /* 3 pins: sck, miso, mosi */
    int af;

    /* enable the SPI modules clock */
    spi_poweron(dev);

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            port = SPI_0_PORT;
            pin[0] = SPI_0_PIN_SCK;
            pin[1] = SPI_0_PIN_MISO;
            pin[2] = SPI_0_PIN_MOSI;
            af = SPI_0_PIN_AF;
            SPI_0_PORT_CLKEN();
            NVIC_SetPriority(SPI_0_IRQ, SPI_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_0_IRQ);
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            port = SPI_1_PORT;
            pin[0] = SPI_1_PIN_SCK;
            pin[1] = SPI_1_PIN_MISO;
            pin[2] = SPI_1_PIN_MOSI;
            af = SPI_1_PIN_AF;
            SPI_0_PORT_CLKEN();
            NVIC_SetPriority(SPI_1_IRQ, SPI_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_1_IRQ);
            break;
#endif
    }

    /* set callback */
    spi_config[dev].cb = cb;

    /* configure pins for their correct alternate function */
    for (int i = 0; i < 3; i++) {
        port->MODER &= ~(3 << (pin[i] * 2));
        port->MODER |= (2 << (pin[i] * 2));
        int hl = (pin[i] < 8) ? 0 : 1;
        port->AFR[hl] &= (0xf << ((pin[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pin[i] - (hl * 8)) * 4));
    }

    /* reset SPI configuration registers */
    spi->CR1 = 0;
    spi->CR2 = 0;
    spi->I2SCFGR = 0;       /* this makes sure SPI mode is selected */

    /* set data-size to 8-bit */
    spi->CR2 |= (7 << 8);

    /* select clock polarity and clock phase */
    spi->CR1 |= conf;

    /* enable interrupt for arriving data: 'receive register no empty' */
    spi->CR2 |= SPI_CR2_RXNEIE;

    /* enable the SPI device */
    spi->CR1 |= SPI_CR1_SPE;

    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    char tmp;
    SPI_TypeDef *spi = 0;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            break;
#endif
    }

    /* wait for an eventually previous byte to be readily transferred */
    while(!(spi->SR & SPI_SR_TXE));

    /* put next byte into the output register */
    spi->DR = out;

    /* wait until the current byte was successfully transferred */
    while( !(spi->SR & SPI_SR_RXNE) );

    /* read response byte to reset flags */
    tmp = spi->DR;

    /* 'return' response byte if wished for */
    if (in) {
        *in = tmp;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    char res;

    for (int i = 0; i < length; i++) {
        if (out) {
            spi_transfer_byte(dev, out[i], &res);
        }
        else {
            spi_transfer_byte(dev, 0, &res);
        }
        if (in) {
            in[i] = res;
        }
    }

    return length;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_bytes(dev, out, in, length);
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


static inline void irq_handler(SPI_TypeDef *spi, spi_t dev)
{
    char data;

    /* call owner when new byte was receive (asserts SPI is in slave mode) */
    if (spi->SR & SPI_SR_RXNE) {
        /* read received byte from data register */
        data = spi->DR;
        /* call callback for receiving the answer of the received byte */
        data = spi_config[dev].cb(data);
        /* set answer byte to be transferred next */
        spi->DR = data;
    }

    /* see if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if SPI_0_EN
__attribute__((naked)) void SPI_0_ISR(void)
{
    ISR_ENTER();
    irq_handler(SPI_0_DEV, SPI_0);
    ISR_EXIT();
}
#endif

#if SPI_1_EN
__attribute__((naked)) void SPI_1_ISR(void)
{
    ISR_ENTER();
    irq_handler(SPI_0_DEV, SPI_1);
    ISR_EXIT();
}
#endif

#endif /* SPI_NUMOF */
