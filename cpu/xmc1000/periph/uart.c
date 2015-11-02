/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include "periph_conf.h"

#include "periph/gating.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph/usic.h"

#include "sched.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

/**
 * @brief Allocate memory to store the rx callback.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    usic_fdr_t fdr = { 0 };
    usic_brg_t brg = { 0 };

    /* STEP value: (944/1024) * 32Mhz = 29.5Mhz (16 x 1.8432 Mhz) */
    fdr.field.step = 944;
    /* DM: fractional divider mode */
    fdr.field.dm = 2;

    /* CTQIN: values below relate to f[PDIV] */
    brg.field.ctqsel  = USIC_CTQIN_PDIV;
    /* DCTQ: denominator for time quanta counter (8 pulses) */
    brg.field.dctq    = 7;
    /* PCTQ: pre-divider for time quanta counter (1) */
    brg.field.pctq    = 0;
    /* PDIV: divider factor to generate f[PDIV] */
    brg.field.pdiv    = ((((DCO1_FREQUENCY / 2) / 1024) * fdr.field.step) /
                         (baudrate * (brg.field.dctq + 1)));

    /* register receive callback */
    uart_ctx[uart].rx_cb = rx_cb;
    uart_ctx[uart].arg = uart_instance[uart].usic;

    /* enable IRQ (isr_usic5) */
    NVIC_EnableIRQ(USIC0_5_IRQn);

    /* setup & start the USIC channel */
    usic_init((usic_channel_t *)&uart_instance[uart],
              &_xmc_usic_asc_controls,
              brg, fdr, USIC_MODE_ASC);

    return 0;
}


void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    USIC_CH_TypeDef *usic = uart_instance[uart].usic;

    for (int i = 0; i < len; i++) {
        while (usic->TCSR & USIC_CH_TCSR_TDV_Msk) {
            /* wait for transmission to be ready */
        }

        usic->TBUF[0] = data[i];
    }
}

void uart_poweron(uart_t uart)
{
    USIC_CH_TypeDef *usic = uart_instance[uart].usic;

    /* clear clock */
    GATING_CLEAR(USIC0);

    /* enable module & set normal mode 0 */
    usic->KSCFG = ((usic->KSCFG & ~(USIC_CH_KSCFG_NOMCFG_Msk | USIC_CH_KSCFG_MODEN_Pos)) |
                   (USIC_CH_KSCFG_BPNOM_Msk | USIC_CH_KSCFG_BPMODEN_Msk));
}

void uart_poweroff(uart_t uart)
{
    USIC_CH_TypeDef *usic = uart_instance[uart].usic;

    /* disable BUSY reporting for receive */
    usic->PCR &= ~USIC_CH_PCR_ASCMode_RSTEN_Msk;
    /* enable BUSY reporting for transmit */
    usic->PCR |=  USIC_CH_PCR_ASCMode_TSTEN_Msk;
    /* request stop mode 1 */
    usic->KSCFG |= ((0x3 << USIC_CH_KSCFG_NOMCFG_Pos) |
                    USIC_CH_KSCFG_BPNOM_Msk);
    /* wait for transmissions to finish */
    while (usic->PSR & USIC_CH_PSR_ASCMode_BUSY_Msk) ;
    /* disable BUSY reporting for transmit */
    usic->PCR &= ~USIC_CH_PCR_ASCMode_TSTEN_Msk;
    /* disable USIC module */
    usic->KSCFG |= ((0 << USIC_CH_KSCFG_MODEN_Pos) |
                    USIC_CH_KSCFG_BPMODEN_Msk);
    /* gate clock */
    GATING_SET(USIC0);
}

#if UART_0_EN
void isr_usic5(void)
{
    USIC_CH_TypeDef *usic = uart_instance[UART_DEV(0)].usic;

    if (usic->PSR & USIC_CH_PSR_RIF_Msk) {
        char data = (char)usic->RBUF;
        uart_ctx[0].rx_cb(uart_ctx[0].arg, data);
        usic->PSR |= USIC_CH_PSR_RIF_Msk;
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#endif /* UART_NUMOF */
