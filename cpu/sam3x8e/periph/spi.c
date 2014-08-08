/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Maxime Blanloeil <maxime.blanloeil@phelma.grenoble-inp.fr>
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

#if SPI_NUMOF

typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(spi_t dev);
static inline void irq_handler_begin(spi_t dev);

static spi_state_t config[SPI_NUMOF];

static char byte_begin = 0xab;

void spi_poweron(spi_t dev)
{

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_CLKEN(); /* Enable peripheral clock for SPI0 */
            SPI_0_PORT_CLKEN();/* Enable peripheral clock for PIOA */
            break;
#endif
    }

    return;
}

void spi_poweroff(spi_t dev)
{

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            while (!(SPI_0_DEV->SPI_SR & SPI_SR_SPIENS)); /* wait until no more busy */

            SPI_0_CLKDIS();/* Disable peripheral clock for SPI0 */
            SPI_0_PORT_CLKEN();/* Disable peripheral clock for PIOA */
            NVIC_DisableIRQ(SPI_0_IRQ);
            NVIC_DisableIRQ(SPI_0_PORT_IRQ);
            break;
#endif
    }

    return;
}

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{

    Pio *port = 0;
    Spi *spi_port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            port = SPI_0_PORT;
            spi_port = SPI_0_DEV;
            /************************* PIO-Init *************************/
            /* For details on the following operations, please read the SAM3X documentation
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. PIO description is given from
            the page 641 to the page 696 */

            /* Push-pull configuration */
            port->PIO_MDER &= ~SPI_0_MISO_PIN;
            port->PIO_MDDR |= SPI_0_MISO_PIN;
            port->PIO_MDER &= ~SPI_0_MOSI_PIN;
            port->PIO_MDDR |= SPI_0_MOSI_PIN;
            port->PIO_MDER &= ~SPI_0_SCK_PIN;
            port->PIO_MDDR |= SPI_0_SCK_PIN;
            port->PIO_MDER &= ~SPI_0_NSS_PIN;
            port->PIO_MDDR |= SPI_0_NSS_PIN;

            /* With pull-up resistors */
            port->PIO_PUDR &= ~SPI_0_MISO_PIN;
            port->PIO_PUER |= SPI_0_MISO_PIN;
            port->PIO_PUDR &= ~SPI_0_MOSI_PIN;
            port->PIO_PUER |= SPI_0_MOSI_PIN;
            port->PIO_PUDR &= ~SPI_0_SCK_PIN;
            port->PIO_PUER |= SPI_0_SCK_PIN;
            port->PIO_PUDR &= ~SPI_0_NSS_PIN;
            port->PIO_PUER |= SPI_0_NSS_PIN;

            /* Clear output */
            port->PIO_SODR &= ~SPI_0_MISO_PIN;
            port->PIO_CODR |= SPI_0_MISO_PIN;
            port->PIO_SODR &= ~SPI_0_MOSI_PIN;
            port->PIO_CODR |= SPI_0_MOSI_PIN;
            port->PIO_SODR &= ~SPI_0_SCK_PIN;
            port->PIO_CODR |= SPI_0_SCK_PIN;
            port->PIO_SODR &= ~SPI_0_NSS_PIN;
            port->PIO_CODR |= SPI_0_NSS_PIN;

            /************************* Configure PIOs to SPI0 *************************/
            /* For details on the following operations, please read the SAM3X documentation
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. Peripherals description is given from
            the page 47 to the page 56 */

            /* Peripheral Function Selection */
            port->PIO_PER &= ~SPI_0_MISO_PIN;
            port->PIO_PDR |= SPI_0_MISO_PIN;
            port->PIO_PER &= ~SPI_0_MOSI_PIN;
            port->PIO_PDR |= SPI_0_MOSI_PIN;
            port->PIO_PER &= ~SPI_0_SCK_PIN;
            port->PIO_PDR |= SPI_0_SCK_PIN;
            port->PIO_PER &= ~SPI_0_NSS_PIN;
            port->PIO_PDR |= SPI_0_NSS_PIN;

            /* Periphal A */
            port->PIO_ABSR &= ~SPI_0_MISO_PIN;
            port->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            port->PIO_ABSR &= ~SPI_0_SCK_PIN;
            port->PIO_ABSR &= ~SPI_0_NSS_PIN;

            switch (conf) {
                case SPI_CONF_FIRST_RISING:
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_SECOND_RISING:
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_FIRST_FALLING:
                    spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_SECOND_FALLING:
                    spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
                    break;

            }

            /* Chip Select Register */
            spi_port->SPI_CSR[0] = 0;
            spi_port->SPI_CSR[0] |= SPI_CSR_SCBR(speed);
            spi_port->SPI_CSR[0] |= SPI_CSR_BITS_8_BIT;
            spi_port->SPI_CSR[0] |= SPI_CSR_DLYBCT(0x10); /* This delay permits to keep the CS signal
                                                             active with consecutive transmissions */

#endif
            break;
    }

    /************************* SPI-Init *************************/
    /* Control Register */
    spi_port->SPI_CR |= SPI_CR_SPIEN;
    /* Mode Register */
    spi_port->SPI_MR = 0;
    spi_port->SPI_MR |= SPI_MR_MSTR;
    spi_port->SPI_MR &= ~SPI_MR_PS;
    spi_port->SPI_MR &= ~SPI_MR_PCS(0);
    /* Enable SPI interrupts */
    spi_port->SPI_IER = 0;
    spi_port->SPI_IER |= SPI_IER_RDRF;
    spi_port->SPI_IER |= SPI_IER_TDRE;
    spi_port->SPI_IER |= SPI_IER_TXEMPTY;
    spi_port->SPI_IER |= SPI_IER_OVRES;
    spi_port->SPI_IER |= SPI_IER_NSSR;

    return 1;

}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{

    Pio *port = 0;
    Spi *spi_port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            port = SPI_0_PORT;
            spi_port = SPI_0_DEV;
            NVIC_SetPriority(SPI_0_IRQ, SPI_0_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_0_IRQ);

            NVIC_SetPriority(SPI_0_PORT_IRQ, SPI_0_PORT_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_0_PORT_IRQ);

            /************************* PIO-Init *************************/
            /* For details on the following operations, please read the SAM3X documentation
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. PIO description is given from
            the page 641 to the page 696 */

            /* Push-pull configuration */
            port->PIO_MDER &= ~SPI_0_MISO_PIN;
            port->PIO_MDDR |= SPI_0_MISO_PIN;
            port->PIO_MDER &= ~SPI_0_MOSI_PIN;
            port->PIO_MDDR |= SPI_0_MOSI_PIN;
            port->PIO_MDER &= ~SPI_0_SCK_PIN;
            port->PIO_MDDR |= SPI_0_SCK_PIN;
            port->PIO_MDER &= ~SPI_0_NSS_PIN;
            port->PIO_MDDR |= SPI_0_NSS_PIN;

            /* With pull-up resistors */
            port->PIO_PUDR &= ~SPI_0_MISO_PIN;
            port->PIO_PUER |= SPI_0_MISO_PIN;
            port->PIO_PUDR &= ~SPI_0_MOSI_PIN;
            port->PIO_PUER |= SPI_0_MOSI_PIN;
            port->PIO_PUDR &= ~SPI_0_SCK_PIN;
            port->PIO_PUER |= SPI_0_SCK_PIN;
            port->PIO_PUDR &= ~SPI_0_NSS_PIN;
            port->PIO_PUER |= SPI_0_NSS_PIN;

            /* Clear output */
            port->PIO_SODR &= ~SPI_0_MISO_PIN;
            port->PIO_CODR |= SPI_0_MISO_PIN;
            port->PIO_SODR &= ~SPI_0_MOSI_PIN;
            port->PIO_CODR |= SPI_0_MOSI_PIN;
            port->PIO_SODR &= ~SPI_0_SCK_PIN;
            port->PIO_CODR |= SPI_0_SCK_PIN;
            port->PIO_SODR &= ~SPI_0_NSS_PIN;
            port->PIO_CODR |= SPI_0_NSS_PIN;

            /* Enable interrupt */
            port->PIO_IER |= SPI_0_NSS_PIN;
            port->PIO_IDR &= ~SPI_0_NSS_PIN;

            /************************* Configure PIOs to SPI0 *************************/
            /* For details on the following operations, please read the SAM3X documentation
            provided by Atmel on http://www.atmel.com/Images/doc11057.pdf. Peripherals description is given from
            the page 47 to the page 56 */

            /* Peripheral Function Selection */
            port->PIO_PER &= ~SPI_0_MISO_PIN;
            port->PIO_PDR |= SPI_0_MISO_PIN;
            port->PIO_PER &= ~SPI_0_MOSI_PIN;
            port->PIO_PDR |= SPI_0_MOSI_PIN;
            port->PIO_PER &= ~SPI_0_SCK_PIN;
            port->PIO_PDR |= SPI_0_SCK_PIN;
            port->PIO_PER &= ~SPI_0_NSS_PIN;
            port->PIO_PDR |= SPI_0_NSS_PIN;

            /* Periphal A */
            port->PIO_ABSR &= ~SPI_0_MISO_PIN;
            port->PIO_ABSR &= ~SPI_0_MOSI_PIN;
            port->PIO_ABSR &= ~SPI_0_SCK_PIN;
            port->PIO_ABSR &= ~SPI_0_NSS_PIN;



            switch (conf) {
                case SPI_CONF_FIRST_RISING:
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_SECOND_RISING:
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_FIRST_FALLING:
                    spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] &= ~SPI_CSR_NCPHA;
                    break;

                case SPI_CONF_SECOND_FALLING:
                    spi_port->SPI_CSR[0] |= SPI_CSR_CPOL;
                    spi_port->SPI_CSR[0] |= SPI_CSR_NCPHA;
                    break;

            }

            /* Chip Select Register */
            spi_port->SPI_CSR[0] = 0;

#endif
            break;
    }

    /************************* SPI-Init *************************/
    /* Control Register */
    spi_port->SPI_CR |= SPI_CR_SPIEN;
    /* Mode Register */
    spi_port->SPI_MR = 0;
    /* Enable SPI interrupts */
    spi_port->SPI_IER = 0;
    spi_port->SPI_IER |= SPI_IER_RDRF;
    spi_port->SPI_IER |= SPI_IER_OVRES;
    /* set callback */
    config[dev].cb = cb;

    return 0;

}

int spi_transfer_byte(spi_t dev, char out, char *in)
{

    Spi *spi_port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_port = SPI_0_DEV;
            break;
#endif
    }

    if (spi_port == 0) {
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

        if (in) {
            in[i] = in_temp;
        }

        trans_bytes++;
    }

    return trans_bytes++;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{

    Spi *spi_port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_port = SPI_0_DEV;
            break;
#endif

    }

    spi_port->SPI_TDR = SPI_TDR_TD(reset_val);
}

static inline void irq_handler_transfer(spi_t dev)
{

    Spi *spi_port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            spi_port = SPI_0_DEV;
            break;

#endif
    }

    char data = 0;

    if (spi_port->SPI_SR & SPI_SR_RDRF) {

        data = spi_port->SPI_RDR & SPI_RDR_RD_Msk;
        data = config[dev].cb(data);
        spi_port->SPI_TDR = data;

    }
    else {
        while (1) {
            for (int i = 0; i < 2000000; i++) {
                asm("nop");
            }
        }
    }

    /* see if a thread with higher priority wants to run now */
    if (sched_context_switch_request) {
        thread_yield();
    }

}

static inline void irq_handler_begin(spi_t dev)
{
    spi_transmission_begin(dev, byte_begin);
}


__attribute__((naked))
void isr_spi0(void)
{
    ISR_ENTER();
    irq_handler_transfer(SPI_0);
    ISR_EXIT();
}

#if SPI_0_EN
__attribute__((naked)) void isr_pioa(void)
{
    ISR_ENTER();

    if (PIOA->PIO_ISR & PIO_PA28A_SPI0_NPCS0) {
        irq_handler_begin(SPI_0);
    }

    ISR_EXIT();
}
#endif

#endif



