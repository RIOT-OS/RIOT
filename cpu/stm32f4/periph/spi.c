/*
 * Copyright (C) 2014 University of Applied Sciences
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


#include <stdlib.h>
 #include <stdio.h>
#include <stdint.h>

#include "cpu.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "stm32f407xx.h"

typedef struct {
    char (*cb)(unsigned int seq, char data);
} spi_state_t;

static inline void irq_handler(spi_t dev);

static spi_state_t config[SPI_NUMOF];

static char cb_delay = 0xb8;


/* ###################################### Init Master ###################################### */

int spi_init_master(spi_t dev, spi_conf_t conf, uint32_t speed){

	GPIO_TypeDef *port;
	SPI_TypeDef *SPI_port;


    switch (dev) {
#if SPI_0_EN
        case SPI_0:
			port = SPI_0_PORT; /* = GPIOA */
			SPI_port = SPI_0_DEV;
            /************************* GPIO-Init *************************/

			/* Set GPIOs to AF mode */
			port->MODER &= ~(2 << (2 * SPI_0_SCK_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_SCK_GPIO));
			port->MODER &= ~(2 << (2 * SPI_0_MISO_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_MISO_GPIO));
			port->MODER &= ~(2 << (2 * SPI_0_MOSI_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_MOSI_GPIO));

			/* Set speed */
			port->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_SCK_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_MISO_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_MOSI_GPIO));

			/* Set to push-pull configuration */
			port->OTYPER &= ~(1 << SPI_0_SCK_GPIO);
			port->OTYPER |=  (0 << SPI_0_SCK_GPIO);
			port->OTYPER &= ~(1 << SPI_0_MISO_GPIO);
			port->OTYPER |=  (0 << SPI_0_MISO_GPIO);
			port->OTYPER &= ~(1 << SPI_0_MOSI_GPIO);
			port->OTYPER |=  (0 << SPI_0_MOSI_GPIO);

			/* Configure push-pull resistors */
			port->PUPDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_SCK_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_MISO_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_MOSI_GPIO));

			/* Configure GPIOs to SPI1 (static) */
			port->AFR[0] &= ~(5 << (4 * SPI_0_SCK_GPIO)); /* 5 (=101): value for AF5, 4: word length, 5: pin no 5 */
			port->AFR[0] |=  (5 << (4 * SPI_0_SCK_GPIO));
			port->AFR[0] &= ~(5 << (4 * SPI_0_MISO_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_MISO_GPIO));
			port->AFR[0] &= ~(5 << (4 * SPI_0_MOSI_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_MOSI_GPIO));
            break;
#endif
#if SPI_1_EN
        case SPI_1:
        	port = SPI_1_PORT;
            SPI_port = SPI_1_DEV;
            /************************* GPIO-Init *************************/

			/* Set GPIOs to AF mode */
			port->MODER &= ~(2 << (2 * SPI_1_SCK_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_SCK_GPIO));
			port->MODER &= ~(2 << (2 * SPI_1_MISO_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_MISO_GPIO));
			port->MODER &= ~(2 << (2 * SPI_1_MOSI_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_MOSI_GPIO));

			/* Set speed */
			port->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_SCK_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_MISO_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_MOSI_GPIO));

			/* Set to push-pull configuration */
			port->OTYPER &= ~(1 << SPI_1_SCK_GPIO);
			port->OTYPER |=  (0 << SPI_1_SCK_GPIO);
			port->OTYPER &= ~(1 << SPI_1_MISO_GPIO);
			port->OTYPER |=  (0 << SPI_1_MISO_GPIO);
			port->OTYPER &= ~(1 << SPI_1_MOSI_GPIO);
			port->OTYPER |=  (0 << SPI_1_MOSI_GPIO);

			/* Configure push-pull resistors */
			port->PUPDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_SCK_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_MISO_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_MOSI_GPIO));

			/* Configure GPIOs to SPI1 (static) */
			port->AFR[1] &= ~(5 << (4 * (SPI_1_SCK_GPIO - 8))); /* 5 (=101): value for AF5, 4: word length, 5: pin no 5 */
			port->AFR[1] |=  (5 << (4 * (SPI_1_SCK_GPIO - 8)));
			port->AFR[1] &= ~(5 << (4 * (SPI_1_MISO_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_MISO_GPIO - 8)));
			port->AFR[1] &= ~(5 << (4 * (SPI_1_MOSI_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_MOSI_GPIO - 8)));
            break;
#endif

        case SPI_UNDEFINED:
        default:
            return -1;
    }

    /************************* SPI-Init *************************/

				SPI_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);/* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    			SPI_port->CR1 &= ~(SPI_CR1_BIDIMODE); /* 0: 2-line bidirectional data mode */
    			SPI_port->CR1 &= ~(SPI_CR1_BIDIOE); /* just relevant if BIDIMODE ist unidirect */
    			SPI_port->CR1 &= ~(SPI_CR1_CRCEN); /* 0: disable CRC */
    			SPI_port->CR1 &= ~(SPI_CR1_CRCNEXT); /* 0: no CRC phase */
    			SPI_port->CR1 &= ~(SPI_CR1_DFF); /* 0: 8bit datamode */
    			SPI_port->CR1 &= ~(SPI_CR1_RXONLY); /* 0: full duplex mode */

    			SPI_port->CR1 &= ~(SPI_CR1_SSM);/* 0: software slave management disabled */

    			SPI_port->CR2 &= ~(SPI_CR2_SSOE);	/* 0: enable multimaster */
    			SPI_port->CR2 |=  (SPI_CR2_SSOE); /* 1: enable master = disable multimaster */

    			SPI_port->CR1 &= ~(SPI_CR1_LSBFIRST); /* 0: MSB first */
    			SPI_port->CR1 &= ~(SPI_CR1_BR);
    			SPI_port->CR1 |=  (speed << 3); /* Define serial clock baud rate. 001 leads to f_PCLK/4 */
    			SPI_port->CR1 &= ~(SPI_CR1_MSTR);
    			SPI_port->CR1 |=  (SPI_CR1_MSTR); /* 1: master configuration */

    			/* select motorola frame format to use CPHA and CPOL (TI protocol doesn't allow this) */
    			SPI_port->CR2 &= ~(1 << 4); /* "FRF" bit does not exist in stm32f4xx.h */
    			SPI_port->CR1 &= ~(conf);	/* no shift because CPOL&CPHA are at first position of CR1-reguster */
    			SPI_port->CR1 |=  (conf);
	
	return 0;
}


/* ###################################### Init Slave ###################################### */

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(unsigned int seq, char data)){

	SPI_TypeDef *SPI_port;
	GPIO_TypeDef *port;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
			port = SPI_0_PORT;
			SPI_port = SPI_0_DEV;
			NVIC_SetPriority(SPI_0_IRQ_HANDLER, SPI_0_IRQ_PRIO); /* set SPI interrupt priority */
			NVIC_EnableIRQ(SPI_0_IRQ_HANDLER); /* set SPI interrupt priority */


            /************************* GPIO-Init *************************/

			/* Set GPIOs to AF mode (not especially input or output) */
			port->MODER &= ~(2 << (2 * SPI_0_NSS_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_NSS_GPIO));
			port->MODER &= ~(2 << (2 * SPI_0_SCK_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_SCK_GPIO));
			port->MODER &= ~(2 << (2 * SPI_0_MISO_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_MISO_GPIO));
			port->MODER &= ~(2 << (2 * SPI_0_MOSI_GPIO));
			port->MODER |=  (2 << (2 * SPI_0_MOSI_GPIO));

			/* Set speed */
			port->OSPEEDR &= ~(3 << (2 * SPI_0_NSS_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_NSS_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_SCK_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_MISO_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_0_MOSI_GPIO));

			/* Set to push-pull configuration (not open drain) */
			port->OTYPER &= ~(1 << SPI_0_NSS_GPIO);
			port->OTYPER |=  (0 << SPI_0_NSS_GPIO);
			port->OTYPER &= ~(1 << SPI_0_SCK_GPIO);
			port->OTYPER |=  (0 << SPI_0_SCK_GPIO);
			port->OTYPER &= ~(1 << SPI_0_MISO_GPIO);
			port->OTYPER |=  (0 << SPI_0_MISO_GPIO);
			port->OTYPER &= ~(1 << SPI_0_MOSI_GPIO);
			port->OTYPER |=  (0 << SPI_0_MOSI_GPIO);

			/* Configure push-pull resistors */
			port->PUPDR &= ~(3 << (2 * SPI_0_NSS_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_NSS_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_SCK_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_MISO_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_0_MOSI_GPIO));

			/* Configure GPIOs to SPI1 (static) */
			port->AFR[0] &= ~(5 << (4 * SPI_0_NSS_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_NSS_GPIO));
			port->AFR[0] &= ~(5 << (4 * SPI_0_SCK_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_SCK_GPIO));
			port->AFR[0] &= ~(5 << (4 * SPI_0_MISO_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_MISO_GPIO));
			port->AFR[0] &= ~(5 << (4 * SPI_0_MOSI_GPIO));
			port->AFR[0] |=  (5 << (4 * SPI_0_MOSI_GPIO));
            break;
#endif
#if SPI_1_EN
        case SPI_1:
        	port = SPI_1_PORT;
            SPI_port = SPI_1_DEV;
			NVIC_SetPriority(SPI_1_IRQ_HANDLER, SPI_1_IRQ_PRIO);
			NVIC_EnableIRQ(SPI_1_IRQ_HANDLER);

            /************************* GPIO-Init *************************/

			/* Set GPIOs to AF mode (not especially input or output) */
			port->MODER &= ~(2 << (2 * SPI_1_NSS_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_NSS_GPIO));
			port->MODER &= ~(2 << (2 * SPI_1_SCK_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_SCK_GPIO));
			port->MODER &= ~(2 << (2 * SPI_1_MISO_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_MISO_GPIO));
			port->MODER &= ~(2 << (2 * SPI_1_MOSI_GPIO));
			port->MODER |=  (2 << (2 * SPI_1_MOSI_GPIO));

			/* Set speed */
			port->OSPEEDR &= ~(3 << (2 * SPI_1_NSS_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_NSS_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_SCK_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_MISO_GPIO));
			port->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
			port->OSPEEDR |=  (3 << (2 * SPI_1_MOSI_GPIO));

			/* Set to push-pull configuration (not open drain) */
			port->OTYPER &= ~(1 << SPI_1_NSS_GPIO);
			port->OTYPER |=  (0 << SPI_1_NSS_GPIO);
			port->OTYPER &= ~(1 << SPI_1_SCK_GPIO);
			port->OTYPER |=  (0 << SPI_1_SCK_GPIO);
			port->OTYPER &= ~(1 << SPI_1_MISO_GPIO);
			port->OTYPER |=  (0 << SPI_1_MISO_GPIO);
			port->OTYPER &= ~(1 << SPI_1_MOSI_GPIO);
			port->OTYPER |=  (0 << SPI_1_MOSI_GPIO);

			/* Configure push-pull resistors */
			port->PUPDR &= ~(3 << (2 * SPI_1_NSS_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_NSS_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_SCK_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_MISO_GPIO));

			port->PUPDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
			port->PUPDR |=  (0 << (2 * SPI_1_MOSI_GPIO));

			/* Configure GPIOs to SPI1 (static) */
			port->AFR[1] &= ~(5 << (4 * (SPI_1_NSS_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_NSS_GPIO - 8)));
			port->AFR[1] &= ~(5 << (4 * (SPI_1_SCK_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_SCK_GPIO - 8)));
			port->AFR[1] &= ~(5 << (4 * (SPI_1_MISO_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_MISO_GPIO - 8)));
			port->AFR[1] &= ~(5 << (4 * (SPI_1_MOSI_GPIO - 8)));
			port->AFR[1] |=  (5 << (4 * (SPI_1_MOSI_GPIO - 8)));
            break;
#endif

        case SPI_UNDEFINED:
        default:
            return -1;
    }

    /************************* SPI-Init *************************/
    		SPI_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);
			SPI_port->CR1 &= ~(SPI_CR1_BIDIMODE);
			SPI_port->CR1 &= ~(SPI_CR1_BIDIOE);
			SPI_port->CR1 &= ~(SPI_CR1_CRCEN);
			SPI_port->CR1 &= ~(SPI_CR1_CRCNEXT);
			SPI_port->CR1 &= ~(SPI_CR1_DFF);
			SPI_port->CR1 &= ~(SPI_CR1_RXONLY);

			SPI_port->CR1 &= ~(SPI_CR1_SSM);  /* 1: Software slave management disabled */
			SPI_port->CR2 &= ~(SPI_CR2_SSOE);	/* 0: nss pin as input at slave */

			SPI_port->CR1 &= ~(SPI_CR1_LSBFIRST);
			SPI_port->CR1 &= ~(SPI_CR1_BR);
			SPI_port->CR1 &= ~(SPI_CR1_MSTR); /* 0: slave configuration */

			SPI_port->CR2 &= ~(1 << 4);
			SPI_port->CR1 &= ~(conf);
			SPI_port->CR1 |=  (conf);

			/* enable RXNEIE flag to enable rx buffer not empty interrupt */
			SPI_port->CR2 |= (SPI_CR2_RXNEIE); /*1:not masked */

    /* set callback */
    config[dev].cb = cb;
	
	return 0;
}

/* ###################################### Transfer Byte ###################################### */

int spi_transfer_byte(spi_t dev, char out, char *in){


    SPI_TypeDef *SPI_port;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_port = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_port = SPI_1_DEV;
            break;
#endif

        default:
            return -1;
    }


	while( !(SPI_port->SR & SPI_SR_TXE));
	SPI_port->DR = out;



	while( !(SPI_port->SR & SPI_SR_RXNE) );
	*in = SPI_port->DR;

	return 1;
}



/* ###################################### Transfer ByteS ###################################### */

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length){

	int i, trans_ret, trans_bytes = 0;
	char out_temp, in_temp;


	for (i=0; i < length; i++){

		if(out){
			out_temp = out[i];
		}
		else{
			out_temp = 0;
		}

		trans_ret = spi_transfer_byte(dev, out_temp, &in_temp);

		if(trans_ret < 0)
		{
			return -1;
		}

		if(in){
			in[i] = in_temp;
		}
		trans_bytes++;
	}

	return trans_bytes++;
}

/* ###################################### Transfer Register ###################################### */

int spi_transfer_reg(spi_t dev, uint8_t reg, char *out, char *in){

	int trans_ret, trans_bytes = 0;
	char ret_status;

	trans_ret = spi_transfer_byte(dev, reg, &ret_status);

	if(trans_ret < 0)
			{
				return -1;
			}

	trans_ret = spi_transfer_byte(dev, *out, in);

	if(trans_ret < 0)
			{
				return -1;
			}
	trans_bytes++;

	return trans_bytes;
}

/* ###################################### Transfer RegisterS ###################################### */

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length){

	int trans_ret;
	char ret_status;

	trans_ret = spi_transfer_byte(dev, reg, &ret_status);

/* TODO */


	return trans_ret;
}

/* ###################################### SPI PowerOFF ###################################### */

int spi_poweron(spi_t dev){

	SPI_TypeDef *SPI_port;

switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_port = SPI_0_DEV;
            SPI_0_PORT_CLKEN(); /* Enable peripheral clock for GPIO port A */
            SPI_0_CLKEN(); /* Enable peripheral clock for SPI1 */
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_port = SPI_1_DEV;
            SPI_1_PORT_CLKEN();
            SPI_1_CLKEN();
            break;
#endif

        case SPI_UNDEFINED:
        default:
            return -1;
    }
	
	SPI_port->CR1 |=  (SPI_CR1_SPE);/* SPI enable */

	return 0;
}


/* ###################################### SPI PowerOFF ###################################### */

int spi_poweroff(spi_t dev){

	SPI_TypeDef *SPI_port;

switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_port = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_port = SPI_1_DEV;
            break;
#endif

        case SPI_UNDEFINED:
        default:
            return -1;
    }

	while( SPI_port->SR & SPI_SR_BSY );/* wait until no more busy */
	SPI_port->CR1 &=  ~(SPI_CR1_SPE);/* SPI dis */
	SPI_0_CLKDIS();
	SPI_1_CLKDIS();

	return 0;
}


/* ###################################### IRQ ###################################### */

static inline void irq_handler(spi_t dev)
{

	char cb = 0;
	unsigned int seq = 0;
	SPI_TypeDef *SPI_port;

switch (dev) {
#if SPI_0_EN
		case SPI_0:
			SPI_port = SPI_0_DEV;
			break;
#endif
#if SPI_1_EN
		case SPI_1:
			SPI_port = SPI_1_DEV;
			break;
#endif
        case SPI_UNDEFINED:
        	break;
    }

		while( !(SPI_port->SR & SPI_SR_TXE));
		SPI_port->DR = cb_delay;

		while( !(SPI_port->SR & SPI_SR_RXNE) );
		cb = SPI_port->DR;

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

