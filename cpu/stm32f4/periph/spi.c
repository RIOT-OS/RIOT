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

#include "cpu.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "stm32f407xx.h"


typedef struct {
    char(*cb)(char data);
} spi_state_t;

static inline void irq_handler_transfer(spi_t dev);
static inline void irq_handler_begin(spi_t dev);

static spi_state_t config[SPI_NUMOF];

static char cb_delay = 0xb8;
static char byte_begin = 0xab;


int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{

    uint8_t speed_devider;

    GPIO_TypeDef *port = 0;
    SPI_TypeDef *spi_port = 0;

    switch (speed) {
        case 0:
            speed_devider = 7; // makes 656 kHz
            break;

        case 1:
            speed_devider = 7; // makes 656 kHz
            break;

        case 2:
            speed_devider = 6; // makes 1.3 MHz
            break;

        case 3:
            speed_devider = 5; // makes 2.6 MHz
            break;

        case 4:
            speed_devider = 4; // makes 5.3 MHz
            break;

        /*	    case 5: speed_devider = 3; // makes 11 MHz
                        break;

        	    case 6: speed_devider = 2; // makes 21 MHz
                        break;

        	    case 7: speed_devider = 1; // makes 42 MHz
                        break;

                case 8: speed_devider = 0; // makes 84 MHz
                        break;

        */
        default:
            speed_devider = 7;
    }


    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            port = SPI_0_PORT; /* = GPIOA */
            spi_port = SPI_0_DEV;

            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode */
            port->MODER &= ~(2 << (2 * SPI_0_SCK_GPIO));
            port->MODER |= (2 << (2 * SPI_0_SCK_GPIO));
            port->MODER &= ~(2 << (2 * SPI_0_MISO_GPIO));
            port->MODER |= (2 << (2 * SPI_0_MISO_GPIO));
            port->MODER &= ~(2 << (2 * SPI_0_MOSI_GPIO));
            port->MODER |= (2 << (2 * SPI_0_MOSI_GPIO));

            /* Set speed */
            port->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_SCK_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_MISO_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_MOSI_GPIO));

            /* Set to push-pull configuration */
            port->OTYPER &= ~(1 << SPI_0_SCK_GPIO);
            port->OTYPER |= (0 << SPI_0_SCK_GPIO);
            port->OTYPER &= ~(1 << SPI_0_MISO_GPIO);
            port->OTYPER |= (0 << SPI_0_MISO_GPIO);
            port->OTYPER &= ~(1 << SPI_0_MOSI_GPIO);
            port->OTYPER |= (0 << SPI_0_MOSI_GPIO);

            /* Configure push-pull resistors */
            port->PUPDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_SCK_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_MISO_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_MOSI_GPIO));

            /* Configure GPIOs to SPI0 (static) */
#if (SPI_0_SCK_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_SCK_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_SCK_GPIO));

#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_SCK_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_SCK_GPIO - 8)));
#endif

#if (SPI_0_MISO_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_MISO_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_MISO_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_MISO_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_MISO_GPIO - 8)));
#endif

#if (SPI_0_MOSI_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_MOSI_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_MOSI_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_MOSI_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_MOSI_GPIO - 8)));
#endif
            break;
#endif
#if SPI_1_EN

        case SPI_1:
            port = SPI_1_PORT;
            spi_port = SPI_1_DEV;
            /************************* GPIO-Init *************************/

            /* Set GPIOs to AF mode */
            port->MODER &= ~(2 << (2 * SPI_1_SCK_GPIO));
            port->MODER |= (2 << (2 * SPI_1_SCK_GPIO));
            port->MODER &= ~(2 << (2 * SPI_1_MISO_GPIO));
            port->MODER |= (2 << (2 * SPI_1_MISO_GPIO));
            port->MODER &= ~(2 << (2 * SPI_1_MOSI_GPIO));
            port->MODER |= (2 << (2 * SPI_1_MOSI_GPIO));

            /* Set speed */
            port->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_SCK_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_MISO_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_MOSI_GPIO));

            /* Set to push-pull configuration */
            port->OTYPER &= ~(1 << SPI_1_SCK_GPIO);
            port->OTYPER |= (0 << SPI_1_SCK_GPIO);
            port->OTYPER &= ~(1 << SPI_1_MISO_GPIO);
            port->OTYPER |= (0 << SPI_1_MISO_GPIO);
            port->OTYPER &= ~(1 << SPI_1_MOSI_GPIO);
            port->OTYPER |= (0 << SPI_1_MOSI_GPIO);

            /* Configure push-pull resistors */
            port->PUPDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_SCK_GPIO));
            port->PUPDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_MISO_GPIO));
            port->PUPDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_MOSI_GPIO));

            /* Configure GPIOs to SPI1 (static) */
#if (SPI_1_SCK_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_SCK_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_SCK_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_SCK_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_SCK_GPIO - 8)));
#endif

#if (SPI_1_MISO_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_MISO_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_MISO_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_MISO_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_MISO_GPIO - 8)));
#endif

#if (SPI_1_MOSI_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_MOSI_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_MOSI_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_MOSI_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_MOSI_GPIO - 8)));
#endif
            break;
#endif

            return -1;
    }

    if (spi_port == 0) {
        return -1;
    }

    /**************** SPI-Init *****************/
    spi_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);/* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */

    spi_port->CR1 = 0;
    spi_port->CR2 = 0;

    spi_port->CR2 |= (SPI_CR2_SSOE);  /* 1: enable master = disable multimaster */

    spi_port->CR1 |= (SPI_CR1_SPE); /* SPI enable */
    spi_port->CR1 |= (speed_devider << 3);  /* Define serial clock baud rate. 001 leads to f_PCLK/4 */
    spi_port->CR1 |= (SPI_CR1_MSTR);  /* 1: master configuration */
    spi_port->CR1 |= (conf);

    return 0;
}



int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{

    SPI_TypeDef *spi_port = 0;
    GPIO_TypeDef *port = 0;

    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            port = SPI_0_PORT;
            spi_port = SPI_0_DEV;
            NVIC_SetPriority(SPI_0_IRQ_HANDLER, SPI_0_IRQ_PRIO); /* set SPI interrupt priority */
            NVIC_EnableIRQ(SPI_0_IRQ_HANDLER); /* set SPI interrupt priority */

            NVIC_SetPriority(EXTI4_IRQn, SPI_0_IRQ_PRIO);
            NVIC_EnableIRQ(EXTI4_IRQn);

            // connect PA4 to interrupt. EXTICR[1] (so the second) for pins 4..7, ~0xf = 0000 for PA pin
            SYSCFG->EXTICR[1] &= ~(0xf); // this should be in periph_cong.h!!!!!!!!!!!!!!!!!!!!!

            EXTI->IMR |= (1 << 4);// 1: interrupt request grom line 4 not masked

            // configure the active edges (here both) : maybe later via flags or so
            EXTI->RTSR &= ~(1 << 4); // 1: rising trigger disabled
            EXTI->FTSR |= (1 << 4);  // 1: falling trigger enabled


            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode (not especially input or output) */
            port->MODER &= ~(2 << (2 * SPI_0_NSS_GPIO));
            port->MODER |= (2 << (2 * SPI_0_NSS_GPIO));
            port->MODER &= ~(2 << (2 * SPI_0_SCK_GPIO));
            port->MODER |= (2 << (2 * SPI_0_SCK_GPIO));
            port->MODER &= ~(2 << (2 * SPI_0_MISO_GPIO));
            port->MODER |= (2 << (2 * SPI_0_MISO_GPIO));
            port->MODER &= ~(2 << (2 * SPI_0_MOSI_GPIO));
            port->MODER |= (2 << (2 * SPI_0_MOSI_GPIO));

            /* Set speed */
            port->OSPEEDR &= ~(3 << (2 * SPI_0_NSS_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_NSS_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_SCK_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_MISO_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_0_MOSI_GPIO));

            /* Set to push-pull configuration (not open drain) */
            port->OTYPER &= ~(1 << SPI_0_NSS_GPIO);
            port->OTYPER |= (0 << SPI_0_NSS_GPIO);
            port->OTYPER &= ~(1 << SPI_0_SCK_GPIO);
            port->OTYPER |= (0 << SPI_0_SCK_GPIO);
            port->OTYPER &= ~(1 << SPI_0_MISO_GPIO);
            port->OTYPER |= (0 << SPI_0_MISO_GPIO);
            port->OTYPER &= ~(1 << SPI_0_MOSI_GPIO);
            port->OTYPER |= (0 << SPI_0_MOSI_GPIO);

            /* Configure push-pull resistors */
            port->PUPDR &= ~(3 << (2 * SPI_0_NSS_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_NSS_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_0_SCK_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_SCK_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_0_MISO_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_MISO_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_0_MOSI_GPIO));
            port->PUPDR |= (0 << (2 * SPI_0_MOSI_GPIO));


            /* Configure GPIOs to SPI0 (static) */
#if (SPI_0_NSS_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_NSS_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_NSS_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_NSS_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_NSS_GPIO - 8)));
#endif

#if (SPI_0_SCK_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_SCK_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_SCK_GPIO));

#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_SCK_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_SCK_GPIO - 8)));
#endif

#if (SPI_0_MISO_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_MISO_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_MISO_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_MISO_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_MISO_GPIO - 8)));
#endif

#if (SPI_0_MOSI_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_0_MOSI_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_0_MOSI_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_0_MOSI_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_0_MOSI_GPIO - 8)));
#endif

            break;
#endif
#if SPI_1_EN

        case SPI_1:
            port = SPI_1_PORT;
            spi_port = SPI_1_DEV;
            NVIC_SetPriority(SPI_1_IRQ_HANDLER, SPI_1_IRQ_PRIO);
            NVIC_EnableIRQ(SPI_1_IRQ_HANDLER);


            /***************** GPIO-Init *****************/
            /* Set GPIOs to AF mode (not especially input or output) */
            port->MODER &= ~(2 << (2 * SPI_1_NSS_GPIO));
            port->MODER |= (2 << (2 * SPI_1_NSS_GPIO));
            port->MODER &= ~(2 << (2 * SPI_1_SCK_GPIO));
            port->MODER |= (2 << (2 * SPI_1_SCK_GPIO));
            port->MODER &= ~(2 << (2 * SPI_1_MISO_GPIO));
            port->MODER |= (2 << (2 * SPI_1_MISO_GPIO));
            port->MODER &= ~(2 << (2 * SPI_1_MOSI_GPIO));
            port->MODER |= (2 << (2 * SPI_1_MOSI_GPIO));

            /* Set speed */
            port->OSPEEDR &= ~(3 << (2 * SPI_1_NSS_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_NSS_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_SCK_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_MISO_GPIO));
            port->OSPEEDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
            port->OSPEEDR |= (3 << (2 * SPI_1_MOSI_GPIO));

            /* Set to push-pull configuration (not open drain) */
            port->OTYPER &= ~(1 << SPI_1_NSS_GPIO);
            port->OTYPER |= (0 << SPI_1_NSS_GPIO);
            port->OTYPER &= ~(1 << SPI_1_SCK_GPIO);
            port->OTYPER |= (0 << SPI_1_SCK_GPIO);
            port->OTYPER &= ~(1 << SPI_1_MISO_GPIO);
            port->OTYPER |= (0 << SPI_1_MISO_GPIO);
            port->OTYPER &= ~(1 << SPI_1_MOSI_GPIO);
            port->OTYPER |= (0 << SPI_1_MOSI_GPIO);

            /* Configure push-pull resistors */
            port->PUPDR &= ~(3 << (2 * SPI_1_NSS_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_NSS_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_1_SCK_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_SCK_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_1_MISO_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_MISO_GPIO));

            port->PUPDR &= ~(3 << (2 * SPI_1_MOSI_GPIO));
            port->PUPDR |= (0 << (2 * SPI_1_MOSI_GPIO));

            /* Configure GPIOs to SPI1 (static) */
#if (SPI_1_NSS_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_NSS_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_NSS_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_NSS_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_NSS_GPIO - 8)));
#endif

#if (SPI_1_SCK_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_SCK_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_SCK_GPIO));

#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_SCK_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_SCK_GPIO - 8)));
#endif

#if (SPI_1_MISO_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_MISO_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_MISO_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_MISO_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_MISO_GPIO - 8)));
#endif

#if (SPI_1_MOSI_GPIO < 8)
            port->AFR[0] &= ~(5 << (4 * SPI_1_MOSI_GPIO));
            port->AFR[0] |= (5 << (4 * SPI_1_MOSI_GPIO));
#else
            port->AFR[1] &= ~(5 << (4 * (SPI_1_MOSI_GPIO - 8)));
            port->AFR[1] |= (5 << (4 * (SPI_1_MOSI_GPIO - 8)));
#endif
            break;
#endif

            return -1;
    }

    if (spi_port == 0) {
        return -1;
    }

    /***************** SPI-Init *****************/
    spi_port->I2SCFGR &= ~(SPI_I2SCFGR_I2SMOD);

    spi_port->CR1 = 0;
    spi_port->CR2 = 0;

    /* enable RXNEIE flag to enable rx buffer not empty interrupt */
    spi_port->CR2 |= (SPI_CR2_RXNEIE); /*1:not masked */

    spi_port->CR1 |= (SPI_CR1_SPE); /* SPI enable */
    spi_port->CR1 |= (conf);

    /* set callback */
    config[dev].cb = cb;

    return 0;
}


int spi_transfer_byte(spi_t dev, char out, char *in)
{


    SPI_TypeDef *spi_port = 0;

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

    if (spi_port == 0) {
        return -1;
    }

    while (!(spi_port->SR & SPI_SR_TXE));

    spi_port->DR = out;




    while (!(spi_port->SR & SPI_SR_RXNE));

    *in = spi_port->DR;

    return 1;
}


int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{

    int i, trans_ret, trans_bytes = 0;
    char out_temp, in_temp;


    for (i = 0; i < length; i++) {

        if (out) {
            out_temp = out[i];
        }
        else {
            out_temp = 0;
        }

        trans_ret = spi_transfer_byte(dev, out_temp, &in_temp);

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

    char ret_status; /* many devices give back status at first transmission */
    int trans_ret;

    trans_ret = spi_transfer_byte(dev, reg, &ret_status);

    if (trans_ret < 0) {
        return -1;
    }

    return spi_transfer_byte(dev, out, in);

}


int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{

    int trans_ret;
    char ret_status;

    trans_ret = spi_transfer_byte(dev, reg, &ret_status);

    /* TODO */


    return trans_ret;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{

    SPI_TypeDef *spi_port = 0;
    char placeholder;

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

    }

    //while ( !(spi_port->SR & SPI_SR_TXE));
    spi_port->DR = reset_val;

    /*while ( !(spi_port->SR & SPI_SR_RXNE) );
     placeholder = spi_port->DR;*/

}


void spi_poweron(spi_t dev)
{


    switch (dev) {
#if SPI_0_EN

        case SPI_0:
            SPI_0_PORT_CLKEN(); /* Enable peripheral clock for GPIO port A */
            SPI_0_CLKEN(); /* Enable peripheral clock for SPI1 */
            break;
#endif
#if SPI_1_EN

        case SPI_1:
            SPI_1_PORT_CLKEN();
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
            while (SPI_0_DEV->SR & SPI_SR_BSY); /* wait until no more busy */

            SPI_0_CLKDIS();
            SPI_0_PORT_CLKDIS();
            break;
#endif
#if SPI_1_EN

        case SPI_1:
            while (SPI_1_DEV->SR & SPI_SR_BSY); /* wait until no more busy */

            SPI_1_CLKDIS();
            SPI_1_PORT_CLKDIS();
            break;
#endif

    }
}


static inline void irq_handler_transfer(spi_t dev)
{

    char cb = 0;

    spi_transfer_byte(dev, cb_delay, &cb);
    config[dev].cb(cb);
    /* return byte of callback is transferred to master in next transmission cycle */
    cb_delay = cb;
}

static inline void irq_handler_begin(spi_t dev)
{

    spi_transmission_begin(dev, byte_begin);
}

__attribute__((naked))
void isr_spi1(void)
{
    ISR_ENTER();
    /* Interrupt is cleared by reading operation in irq_handler */
    irq_handler_transfer(SPI_0);

}
__attribute__((naked))
void isr_spi2(void)
{
    ISR_ENTER();
    irq_handler_transfer(SPI_1);
    ISR_EXIT();
}

#if SPI_0_EN
__attribute__((naked))
void isr_exti4(void)
{
    ISR_ENTER();

    if (EXTI->PR & EXTI_PR_PR4) {// for interrupt by gpio flanks
        EXTI->PR |= EXTI_PR_PR4;
        irq_handler_begin(SPI_0);
    }

    ISR_EXIT();
}
#endif
