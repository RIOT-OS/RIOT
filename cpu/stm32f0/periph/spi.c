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

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief unified interrupt handler to be shared between SPI devices
 *
 * @param[in] spi       Pointer to the devices base register
 * @param[in] dev       The device that triggered the interrupt
 */
static inline void irq_handler(SPI_TypeDev *spi, spi_t dev);

/**
 * @brief structure that defines the state for an SPI device
 */
typedef struct {
    char (*cb)(unsigned int seq, char data);
} spi_state_t;

/**
 * @brief array with one field for each possible SPI device
 */
static spi_state_t spi_config[SPI_NUMOF];


int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_TypeDef *spi;
    GPIO_TypeDef *port;
    int pins[3];        /* 3 pins: sck, miso, mosi */
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
        int hl = (pins[i] < 8) ? 0 : 1;
        port->AFR[hl] &= (0xf << ((pins[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pins[i] - (hl * 8)) * 4));
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

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(unsigned int seq, char data))
{
    SPI_TypeDef *spi;
    GPIO_TypeDef *port;
    int pins[3];        /* 3 pins: sck, miso, mosi */
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
        int hl = (pins[i] < 8) ? 0 : 1;
        port->AFR[hl] &= (0xf << ((pins[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pins[i] - (hl * 8)) * 4));
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
    SPI_TypeDef *spi;

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
    spi_transfer_byte(dev, out, in);
}

/* ###################################### Transfer RegisterS ###################################### */

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length){

    int trans_ret;
    char ret_status;

    trans_ret = spi_transfer_byte(dev, reg, &ret_status);

/* TODO */


    return trans_ret;
}

int spi_poweron(spi_t dev)
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


/* ###################################### IRQ ###################################### */

static inline void irq_handler(spi_t dev)
{

    char cb = 0;
    unsigned int seq = 0;
    SPI_TypeDef *spi;

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
        case SPI_UNDEFINED:
            break;
    }

        while( !(spi->SR & SPI_SR_TXE));
        spi->DR = cb_delay;

        while( !(spi->SR & SPI_SR_RXNE) );
        cb = spi->DR;

        config[dev].cb(seq, cb);
        /* return byte of callback is transferred to master in next transmission cycle */
        cb_delay = cb;
}


__attribute__((naked))
void isr_spi1(void)
{
    ISR_ENTER();
    /* Interrupt is cleared by reading operation in irq_handler */
    irq_handler(SPI_0);
    ISR_EXIT();
}
__attribute__((naked))
void isr_spi2(void)
{
    ISR_ENTER();
    irq_handler(SPI_1);
    ISR_EXIT();
}




#endif /* SPI_NUMOF */
