/*
* Copyright (C) 2014 Hamburg University of Applied Sciences
*
* This file is subject to the terms and conditions of the GNU Lesser General
* Public License v2.1. See the file LICENSE in the top level directory for more
* details.
*/

/**
 * @ingroup     cpu_sam3x8e
 * @{
 *
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Maxime Blanloeil <maxime.blanloeil@phelma.grenoble-inp.fr>
 * @author      Peter Kietzmann  <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph/spi.h"
#include "sam3x8e.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(Spi *spi, spi_t dev);

static spi_state_t spi_config[SPI_NUMOF];

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            SPI_0_MISO_PORT_CLKEN();
            SPI_0_MOSI_PORT_CLKEN();
            SPI_0_SCK_PORT_CLKEN();
            break;
#endif /* SPI_0_EN */
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            while (!(SPI_0_DEV->SPI_SR & SPI_SR_SPIENS)); /* not busy anymore */
            SPI_0_CLKDIS();
            NVIC_DisableIRQ(SPI_0_IRQ);
            break;
#endif /* SPI_0_EN */
    }
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    uint8_t speed_divider;
    Spi *spi_port;

    spi_poweron(dev);

    switch (speed) {
        case SPI_SPEED_400KHZ:
            speed_divider = 210;
            break;

        case SPI_SPEED_1MHZ:
            speed_divider = 84;
            break;

        case SPI_SPEED_5MHZ:
            speed_divider = 17;
            break;

        case SPI_SPEED_10MHZ: /* this might be too fast */
            speed_divider = 8;
            break;

        default:
            return -1;
            break;
    }

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            /***************** PIO-Init *****************/
            /* Push-pull configuration */
            SPI_0_MISO_PORT->PIO_MDER &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_MDDR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_MDER &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_MDDR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_MDER &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_MDDR |= SPI_0_SCK_PIN;

            /* With pull-up resistors */
            SPI_0_MISO_PORT->PIO_PUDR &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_PUER |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_PUDR &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_PUER |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_PUDR &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_PUER |= SPI_0_SCK_PIN;

            /* Clear output */
            SPI_0_MISO_PORT->PIO_SODR &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_CODR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_SODR &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_CODR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_SODR &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_CODR |= SPI_0_SCK_PIN;

            /* Peripheral Function Selection */
            SPI_0_MISO_PORT->PIO_PER &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_PDR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_PER &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_PDR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_PER &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_PDR |= SPI_0_SCK_PIN;

            /* Peripheral A */
            SPI_0_MISO_PORT->PIO_ABSR &= ~SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_ABSR &= ~SPI_0_SCK_PIN;

            break;
#endif /* SPI_0_EN */
        default:
            return -2;
    }

    /***************** SPI-Init *****************/

    /* Chip Select Register */
    spi_port->SPI_CSR[0] = 0; /* This is index 0 since we don't use internal CS-Signals */

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

        case SPI_CONF_SECOND_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;

        case SPI_CONF_FIRST_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

        case SPI_CONF_SECOND_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~ SPI_CSR_NCPHA;
            break;

        default:
            return -2;
    }

    spi_port->SPI_CSR[0] |= SPI_CSR_SCBR(speed_divider);
    spi_port->SPI_CSR[0] |= SPI_CSR_BITS_8_BIT;

    /* Control Register */
    spi_port->SPI_CR |= SPI_CR_SPIEN;
    /* Mode Register */
    spi_port->SPI_MR = 0;
    spi_port->SPI_MR |= SPI_MR_MSTR;
    spi_port->SPI_MR |= SPI_MR_MODFDIS;
    spi_port->SPI_MR &= ~SPI_MR_PS;
    spi_port->SPI_MR &= ~SPI_MR_PCS(0);

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{
    Spi *spi_port;

    spi_poweron(dev);

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            NVIC_SetPriority(SPI_0_IRQ, SPI_0_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_0_IRQ);

            /***************** PIO-Init *****************/

            /* Initialize predefined NSS pin as output so it is "disabled" */
            PIOA->PIO_PER |= PIO_PA28A_SPI0_NPCS0;
            PIOA->PIO_OER |= PIO_PA28A_SPI0_NPCS0;

            /* Push-pull configuration */
            SPI_0_MISO_PORT->PIO_MDER &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_MDDR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_MDER &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_MDDR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_MDER &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_MDDR |= SPI_0_SCK_PIN;

            /* With pull-up resistors */
            SPI_0_MISO_PORT->PIO_PUDR &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_PUER |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_PUDR &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_PUER |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_PUDR &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_PUER |= SPI_0_SCK_PIN;

            /* Clear output */
            SPI_0_MISO_PORT->PIO_SODR &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_CODR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_SODR &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_CODR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_SODR &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_CODR |= SPI_0_SCK_PIN;

            /* Peripheral Function Selection */
            SPI_0_MISO_PORT->PIO_PER &= ~SPI_0_MISO_PIN;
            SPI_0_MISO_PORT->PIO_PDR |= SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_PER &= ~SPI_0_MOSI_PIN;
            SPI_0_MOSI_PORT->PIO_PDR |= SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_PER &= ~SPI_0_SCK_PIN;
            SPI_0_SCK_PORT->PIO_PDR |= SPI_0_SCK_PIN;

            /* Peripheral A */
            SPI_0_MISO_PORT->PIO_ABSR &= ~SPI_0_MISO_PIN;
            SPI_0_MOSI_PORT->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            SPI_0_SCK_PORT->PIO_ABSR &= ~SPI_0_SCK_PIN;

            break;
#endif /* SPI_0_EN */
        default:
            return -1;
    }

    /***************** SPI-Init *****************/

    /* Chip Select Register */
    spi_port->SPI_CSR[0] = 0;

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

        case SPI_CONF_SECOND_RISING:
            spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
            break;

        case SPI_CONF_FIRST_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
            break;

        case SPI_CONF_SECOND_FALLING:
            spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
            spi_port->SPI_CSR[0] &= ~ SPI_CSR_NCPHA;
            break;

        default:
            return -1;
    }

    /* Control Register */
    spi_port->SPI_CR |= SPI_CR_SPIEN;
    /* Mode Register */
    spi_port->SPI_MR = 0;
    spi_port->SPI_MR |= SPI_MR_MODFDIS;
    /* Enable SPI interrupts */
    spi_port->SPI_IER = 0;
    spi_port->SPI_IDR = ~(0);
    spi_port->SPI_IER |= 1;
    spi_port->SPI_IDR &= ~SPI_IDR_RDRF;

    /* Set callback */
    spi_config[dev].cb = cb;

    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    Spi *spi_port;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi_port = SPI_0_DEV;
            break;
#endif /* SPI_0_EN */
        default:
            return -1;
    }

    while (!(spi_port->SPI_SR & SPI_SR_TDRE));

    spi_port->SPI_TDR = SPI_TDR_TD(out);

    while (!(spi_port->SPI_SR & SPI_SR_RDRF));

    *in = spi_port->SPI_RDR & SPI_RDR_RD_Msk;

    return 1;
}


int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    int trans_ret, trans_bytes = 0;
    char in_temp;

    for (int i = 0; i < length; i++) {

        if (out) {
            trans_ret = spi_transfer_byte(dev, out[i], &in_temp);
        }
        else {
            trans_ret = spi_transfer_byte(dev, 0, &in_temp);
        }

        if (trans_ret < 0) {
            return -1;
        }

        if (in) {
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
        return -1;
    }

    trans_ret = spi_transfer_byte(dev, out, in);

    if (trans_ret < 0) {
        return -1;
    }

    return 1;
}


int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, in);

    if (trans_ret < 0) {
        return -1;
    }

    trans_ret = spi_transfer_bytes(dev, out, in, length);

    if (trans_ret < 0) {
        return -1;
    }

    return trans_ret;
}


void spi_transmission_begin(spi_t dev, char reset_val)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_DEV->SPI_TDR = SPI_TDR_TD(reset_val);
            break;
#endif /* SPI_0_EN */
    }
}

static inline void irq_handler_transfer(Spi *spi, spi_t dev)
{
    if (spi->SPI_SR & SPI_SR_RDRF) {
        char data;
        data = spi->SPI_RDR & SPI_RDR_RD_Msk;
        data = spi_config[dev].cb(data);
        spi->SPI_TDR = SPI_TDR_TD(data);
    }

    /* See if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }
}

__attribute__((naked))
void SPI_0_IRQ_HANDLER(void)
{
    if (SPI_0_DEV->SPI_SR & SPI_SR_RDRF) {
        ISR_ENTER();
        irq_handler_transfer(SPI_0_DEV, SPI_0);
        ISR_EXIT();
    }
}
#endif /* SPI_NUMOF */
