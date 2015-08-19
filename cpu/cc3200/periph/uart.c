/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Attilio Dona' <@attiliodona>
 *
 * @}
 */

#include <stddef.h>


#include "board.h"
#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

#if 0
#undef BIT
#define BIT(n) ( 1 << (n) )

#define UART_WORD_LENGTH        8

enum {
    FIFO_LEVEL_1_8TH = 0,
    FIFO_LEVEL_2_8TH = 1,
    FIFO_LEVEL_4_8TH = 2,
    FIFO_LEVEL_6_8TH = 3,
    FIFO_LEVEL_7_8TH = 4,
};

/* Bit masks for the UART Masked Interrupt Status (MIS) Register: */
#define OEMIS BIT(10) /**< UART overrun error masked status */
#define BEMIS BIT( 9) /**< UART break error masked status */
#define FEMIS BIT( 7) /**< UART framing error masked status */
#define RTMIS BIT( 6) /**< UART RX time-out masked status */
#define RXMIS BIT( 4) /**< UART RX masked interrupt status */

#define UART_CTL_HSE_VALUE    0
#define DIVFRAC_NUM_BITS      6
#define DIVFRAC_MASK          ( (1 << DIVFRAC_NUM_BITS) - 1 )

/** @brief Indicates if there are bytes available in the UART0 receive FIFO */
#define uart0_rx_avail() ( UART0->FRbits.RXFE == 0 )

/** @brief Indicates if there are bytes available in the UART1 receive FIFO */
#define uart1_rx_avail() ( UART1->FRbits.RXFE == 0 )

/** @brief Read one byte from the UART0 receive FIFO */
#define uart0_read()     ( UART0->DR )

/** @brief Read one byte from the UART1 receive FIFO */
#define uart1_read()     ( UART1->DR )
#endif

/*---------------------------------------------------------------------------*/

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    uart_tx_cb_t tx_cb;
    void *arg;
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];


/*---------------------------------------------------------------------------*/
static void reset(unsigned long uart_base)
{
	UARTDisable(uart_base);
	UARTRxErrorClear(uart_base);
	UARTEnable(uart_base);
}


/*---------------------------------------------------------------------------*/

#if UART_0_EN
void UART_0_ISR(void)
{
	MAP_UARTIntClear(UARTA0_BASE, UART_INT_RX);

    while (UARTCharsAvail(UARTA0_BASE)) {
        uart_config[0].rx_cb(uart_config[0].arg, MAP_UARTCharGet(UARTA0_BASE));
    }

    if (UARTRxErrorGet(UARTA0_BASE)) {
    	reset(UARTA0_BASE);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* UART_0_EN */

#if UART_1_EN
void UART_1_ISR(void)
{
	MAP_UARTIntClear(UARTA1_BASE, UART_INT_RX);

    while (UARTCharsAvail(UARTA1_BASE)) {
        uart_config[1].rx_cb(uart_config[1].arg, MAP_UARTCharGet(UARTA1_BASE));
    }

    if (UARTRxErrorGet(UARTA1_BASE)) {
    	reset(UARTA1_BASE);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* UART_1_EN */

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    /* initialize basic functionality */
    int res = uart_init_blocking(uart, baudrate);

    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].tx_cb = tx_cb;
    uart_config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
#if UART_0_EN
        case UART_0:
            IntPrioritySet(INT_UARTA0, UART_IRQ_PRIO);
            IntEnable(INT_UARTA0);
            break;
#endif
#if UART_1_EN
        case UART_1:
            IntPrioritySet(INT_UARTA1, UART_IRQ_PRIO);
            IntEnable(INT_UARTA1);
            break;
#endif
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate)
{
	unsigned long u;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            u = UARTA0_BASE;

            //
            // Enable Peripheral Clocks
            //
            MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

            //
            // Configure PIN_55 for UART0 UART0_TX
            //
            MAP_PinTypeUART(PIN_55, PIN_MODE_3);

            //
            // Configure PIN_57 for UART0 UART0_RX
            //
            MAP_PinTypeUART(PIN_57, PIN_MODE_3);

            MAP_UARTConfigSetExpClk(u,MAP_PRCMPeripheralClockGet(PRCM_UARTA0),
            		baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));


            break;
#endif
#if UART_1_EN
        case UART_1:
            u = UARTA1_BASE;

        	//
            // Enable Peripheral Clocks
            //
        	PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);

        	//
            // Configure PIN_07 for UART1 UART1_TX
            //
        	PinTypeUART(PIN_07, PIN_MODE_5);

        	//
            // Configure PIN_08 for UART1 UART1_RX
            //
        	PinTypeUART(PIN_08, PIN_MODE_5);

            MAP_UARTConfigSetExpClk(u,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
            		baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

            break;
#endif

        default:
            return -1;
    }

    return 0;
}

void uart_tx_begin(uart_t uart)
{

}

void uart_tx_end(uart_t uart)
{

}

int uart_write(uart_t uart, char data)
{
	unsigned long u;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            u = UARTA0_BASE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            u = UARTA1_BASE;
            break;
#endif

        default:
            return -1;
    }

    return MAP_UARTCharPutNonBlocking(u, data);;
}

int uart_read_blocking(uart_t uart, char *data)
{
    unsigned long u;

    switch (uart) {
#if UART_0_EN
        case UART_0:
            u = UARTA0_BASE;
            break;
#endif
#if UART_1_EN
        case UART_1:
            u = UARTA1_BASE;
            break;
#endif

        default:
            return -1;
    }

    *data = MAP_UARTCharGet(u);

    return 1;
}

int uart_write_blocking(uart_t uart, char data)
{
    MAP_UARTCharPut(UARTA0_BASE, data);

    return 1;
}

void uart_poweron(uart_t uart)
{

}

void uart_poweroff(uart_t uart)
{

}

#endif /* UART_NUMOF */
