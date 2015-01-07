/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 volatiles UG (haftungsbeschränkt)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_tm4c123
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Benjamin Valentin <benjamin.valentin@volatiles.de>
 *
 * @}
 */

#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"

#include "board.h"

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

static inline int get_uart_base(uart_t uart) {
    switch(uart) {
#if UART_0_EN
        case UART_0:
        return UART0_BASE;
#endif
#if UART_1_EN
        case UART_1:
        return UART1_BASE;
#endif
        default:
        return -1;
    }
}

static inline int get_uart_num(uart_t uart) {
    switch(uart) {
#if UART_0_EN
        case UART_0:
        return 0;
#endif
#if UART_1_EN
        case UART_1:
        return 1;
#endif
        default:
        return -1;
    }
}

static inline int get_uart_irq(uart_t uart) {
    switch(uart) {
#if UART_0_EN
        case UART_0:
        return UART_0_IRQ_CHAN;
#endif
#if UART_1_EN
        case UART_1:
        return UART_1_IRQ_CHAN;
#endif
        default:
        return -1;
    }
}

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Unified interrupt handler for all UART devices
 *
 * @param uartnum       the number of the UART that triggered the ISR
 * @param uart          the UART device that triggered the ISR
 */
static inline void irq_handler(uint8_t uartnum, void* uart);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);
    if (res < 0) {
        return res;
    }

    /* remember callback addresses */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;

    /* enable receive interrupt */
    // TODO
    NVIC_SetPriority(get_uart_irq(uart), UART_IRQ_PRIO);
    NVIC_EnableIRQ(get_uart_irq(uart));
    //  UART_0_DEV->CR1 |= USART_CR1_RXNEIE;

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
    /* Enable UART */
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0 + get_uart_num(uart));

    /* Use the internal 16MHz oscillator as the UART clock source */
    ROM_UARTClockSourceSet(get_uart_base(uart), UART_CLOCK_PIOSC);

    /* Configure the UART for 115200, n, 8, 1 */
    ROM_UARTConfigSetExpClk(get_uart_base(uart), 16000000, baudrate,
                            (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_WLEN_8));

    /* Enable the UART operation */
    ROM_UARTEnable(get_uart_base(uart));

    return 0;
}

void uart_tx_begin(uart_t uart)
{
    // TODO
}

int uart_write(uart_t uart, char data)
{
    ROM_UARTCharPutNonBlocking(get_uart_base(uart), data);

    return 0;
}

int uart_read_blocking(uart_t uart, char *data)
{
    *data = ROM_UARTCharGet(get_uart_base(uart));

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    ROM_UARTCharPut(get_uart_base(uart), data);

    return 1;
}

void uart_poweron(uart_t uart)
{
    // TODO
}

void uart_poweroff(uart_t uart)
{
    // TODO
}

#if UART_0_EN
__attribute__((naked)) void UART_0_ISR(void)
{
    irq_handler(UART_0, NULL);
}
#endif

#if UART_1_EN
__attribute__((naked)) void UART_1_ISR(void)
{
    irq_handler(UART_1, NULL);
}
#endif

static inline void irq_handler(uint8_t uartnum, void *dev)
{
	// TOOD
/*
    if (dev->SR & USART_SR_RXNE) {
        char data = (char)dev->DR;
        uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
    }
    else if (dev->SR & USART_SR_TXE) {
        if (uart_config[uartnum].tx_cb(uart_config[uartnum].arg) == 0) {
            dev->CR1 &= ~(USART_CR1_TXEIE);
        }
    }
*/
    if (sched_context_switch_request) {
        thread_yield();
    }
}

#endif /* UART_NUMOF */
