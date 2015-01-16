/*
 * Copyright (C) 2015555555555555555555555555555555555555555555555555555555 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file        kw2xrf_spi.c
 * @brief       Implementation of SPI-functions for the kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */
#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_internal.h"

#include "cpu-conf.h"
#include "periph/gpio.h"
#include "irq.h"

/* SPI1 connected to the modem (ieee802.15.4 radio) */
#define KW2XDRF_SPI                   SPI1
#define KW2XDRF_SPI_CLKEN()           (SIM->SCGC6 |= (SIM_SCGC6_SPI1_MASK))
#define KW2XDRF_SPI_CLKDIS()          (SIM->SCGC6 &= ~(SIM_SCGC6_SPI1_MASK))
#define KW2XDRF_SPI_IRQ               SPI1_IRQn
#define KW2XDRF_SPI_IRQ_HANDLER       isr_spi1

void kw2xrf_spi_init(void)
{
    /* enable clocks */
    KW2XDRF_SPI_CLKEN();
    KW2XDRF_PORT_CLKEN();

    /* set pins to AF mode */
    KW2XDRF_PORT->PCR[KW2XDRF_PCS0_PIN] = PORT_PCR_MUX(KW2XDRF_PIN_AF);
    KW2XDRF_PORT->PCR[KW2XDRF_SCK_PIN] = PORT_PCR_MUX(KW2XDRF_PIN_AF);
    KW2XDRF_PORT->PCR[KW2XDRF_SOUT_PIN] = PORT_PCR_MUX(KW2XDRF_PIN_AF);
    KW2XDRF_PORT->PCR[KW2XDRF_SIN_PIN] = PORT_PCR_MUX(KW2XDRF_PIN_AF);
    /* enable pullup for sin-pin (miso) */
    KW2XDRF_PORT->PCR[KW2XDRF_SIN_PIN] |= (PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

    /* max spi clock rate for read is 9MHz, for write 16MHz */
    /* set speed for 8-bit access */
    KW2XDRF_SPI->CTAR[0] = SPI_CTAR_FMSZ(7)
                           | SPI_CTAR_PBR(0)
                           | SPI_CTAR_BR(1);
    /* set speed for 16-bit access */
    KW2XDRF_SPI->CTAR[1] = SPI_CTAR_FMSZ(15)
                           | SPI_CTAR_PBR(0)
                           | SPI_CTAR_BR(1);

    /* enable SPI */
    KW2XDRF_SPI->MCR = SPI_MCR_MSTR_MASK
                       | SPI_MCR_PCSIS(1)
                       | SPI_MCR_DOZE_MASK
                       | SPI_MCR_CLR_TXF_MASK
                       | SPI_MCR_CLR_RXF_MASK;

    KW2XDRF_SPI->RSER = (uint32_t)0;

    /* set up IRQ */
    gpio_init_int(GPIO_KW2XDRF, GPIO_NOPULL, GPIO_FALLING, &kw2xrf_rx_irq, NULL);
}

void kw2xrf_write_dreg(uint8_t addr, uint8_t value)
{
    unsigned int cpsr = disableIRQ();

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(1)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(((uint16_t)addr << 8) | value);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;
    restoreIRQ(cpsr);
}

uint8_t kw2xrf_read_dreg(uint8_t addr)
{
    unsigned int cpsr = disableIRQ();
    addr |= MKW2XDRF_REG_READ;

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(1)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA((uint16_t)addr << 8);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    restoreIRQ(cpsr);
    return (uint8_t)KW2XDRF_SPI->POPR;
}

void kw2xrf_write_iregs(uint8_t addr, uint8_t *buf, uint8_t length)
{
    uint8_t i;
    unsigned int cpsr = disableIRQ();

    while (KW2XDRF_SPI->SR & SPI_SR_TXCTR_MASK);

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(1)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_TXDATA(((uint16_t)MKW2XDM_IAR_INDEX << 8) | addr);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;

    for (i = 0; i < (length - 1); i++) {
        while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

        KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                             | SPI_PUSHR_CONT_MASK
                             | SPI_PUSHR_PCS(1)
                             | SPI_PUSHR_TXDATA(buf[i]);

        while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

        KW2XDRF_SPI->POPR;
    }

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(buf[i]);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;
    restoreIRQ(cpsr);
}

void kw2xrf_read_iregs(uint8_t addr, uint8_t *buf, uint8_t length)
{
    uint16_t iaddr = ((uint16_t)(MKW2XDM_IAR_INDEX | MKW2XDRF_REG_READ) << 8) | addr;
    uint8_t i;
    unsigned int cpsr = disableIRQ();

    while (KW2XDRF_SPI->SR & SPI_SR_TXCTR_MASK);

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(1)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_TXDATA(iaddr);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;

    for (i = 0; i < (length - 1); i++) {
        while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

        KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                             | SPI_PUSHR_CONT_MASK
                             | SPI_PUSHR_PCS(1)
                             | SPI_PUSHR_TXDATA(0);

        while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

        buf[i] = (uint8_t)KW2XDRF_SPI->POPR;
    }

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(0);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    buf[i] = (uint8_t)KW2XDRF_SPI->POPR;
    restoreIRQ(cpsr);
}

/* read / write packet buffer */

radio_packet_length_t kw2xrf_write_fifo(uint8_t *data, radio_packet_length_t length)
{
    radio_packet_length_t i;
    unsigned int cpsr = disableIRQ();

    while (KW2XDRF_SPI->SR & SPI_SR_TXCTR_MASK);

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(MKW2XDRF_BUF_WRITE);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;

    for (i = 0; i < (length - 1); i++) {
        while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

        KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                             | SPI_PUSHR_CONT_MASK
                             | SPI_PUSHR_PCS(1)
                             | SPI_PUSHR_TXDATA((uint8_t)data[i]);

        while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

        KW2XDRF_SPI->POPR;
    }

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA((uint8_t)data[i]);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;
    i++;

    restoreIRQ(cpsr);
    return i;
}

radio_packet_length_t kw2xrf_read_fifo(uint8_t *data, radio_packet_length_t length)
{
    radio_packet_length_t i;
    unsigned int cpsr = disableIRQ();

    while (KW2XDRF_SPI->SR & SPI_SR_TXCTR_MASK);

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_CONT_MASK
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(MKW2XDRF_BUF_READ);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    KW2XDRF_SPI->POPR;

    for (i = 0; i < (length - 1); i++) {
        while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

        KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                             | SPI_PUSHR_CONT_MASK
                             | SPI_PUSHR_PCS(1)
                             | SPI_PUSHR_TXDATA(0);

        while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

        data[i] = (uint8_t)KW2XDRF_SPI->POPR;
    }

    while (!(KW2XDRF_SPI->SR & SPI_SR_TFFF_MASK));

    KW2XDRF_SPI->PUSHR = SPI_PUSHR_CTAS(0)
                         | SPI_PUSHR_PCS(1)
                         | SPI_PUSHR_TXDATA(0);

    while (!(KW2XDRF_SPI->SR & SPI_SR_RXCTR_MASK));

    data[i] = (uint8_t)KW2XDRF_SPI->POPR;
    i++;

    restoreIRQ(cpsr);
    return i;
}
/** @} */
