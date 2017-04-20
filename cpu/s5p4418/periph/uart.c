/*
 * Copyright (C) 2017 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 */

#include <stdio.h>
#include <stdint.h>
#include "s5p4418_irq.h"
#include "s5p4418_uart.h"
#include "periph/uart.h"

typedef unsigned int uart_t;

static uart_isr_ctx_t isr_ctx[UART_NUMOF];

uint8_t uart_getc(void)
{
    /* Wait for incoming data */
    while(UART0->FR & UART_RXFE);

    return UART0->DR;
}

void uart_putc(uint8_t c)
{
    /* Wait for tx buff ready */
    while(UART0->FR & UART_TXFF);
    UART0->DR = c;
}

void uart0_irq_handler(void)
{
   // Clear UARTRXINTR
   UART0->ICR |= (1 << 4);

   // Re enable IRQ to allow reentry
   __asm__ __volatile__("cpsie i");

   /* Execute callback function */
   isr_ctx[0].rx_cb(isr_ctx[0].arg, UART0->DR);
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{

    if (uart !=0 )
    {
        printf("UART_NODEV\r\n");
        return UART_NODEV;
    }

    /* save ISR context */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg   = arg;

    /* Register uart IRQ handler */
    s5p4418_irq_register(&uart0_irq_handler, 7, 7);

    /* Disable FIFO mode */
    UART0->LCR_H &= ~(1<<4); // FEN = 0

    // RXIC = 1 => clear UARTRXINTR
    UART0->ICR |= (1 << 4);

    /* Enable RX interrupt */
    UART0->IMSC |=  (1<<4);  // RXIM = 1

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (data[i]=='\n')
        {
            uart_putc ('\r');
            uart_putc ('\n');
        }
        else
        {
            uart_putc(data[i]);
        }
    }
}

int uart_read(uart_t uart, const uint8_t *data, size_t len)
{
  uint8_t *tempbuff = (uint8_t *)data;
  int i;

  for (i = 0; i < len; i++)
  {
    uint8_t ch = uart_getc();

    if (ch == '\n')
    {
        uart_putc ('\r');
        uart_putc ('\n');
        break;
    }
    else
    {
        tempbuff[i] = ch;
        uart_putc (ch);
    }
  }

  return i;
}
