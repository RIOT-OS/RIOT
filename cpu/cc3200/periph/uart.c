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
 * @author      Attilio Dona'
 *
 * @}
 */

#include <stddef.h>

#include "board.h"
#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/uart.h"
#include "periph_conf.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF

/*---------------------------------------------------------------------------*/

/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    void *arg;
} uart_conf_t;

int uart_init_blocking(uart_t uart, uint32_t baudrate);

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];

/*---------------------------------------------------------------------------*/
static void reset(unsigned long uart_base) {
    MAP_UARTDisable(uart_base);
    MAP_UARTRxErrorClear(uart_base);
    MAP_UARTEnable(uart_base);
    MAP_UARTFIFODisable(uart_base);
}

/*---------------------------------------------------------------------------*/

#if UART_0_EN
void isr_uart0(void) {

    MAP_UARTIntClear(UARTA0_BASE,
    UART_INT_RX | UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE);

    if (MAP_UARTRxErrorGet(UARTA0_BASE)) {
        reset(UARTA0_BASE);
    } else {
        long data;

        data = MAP_UARTCharGetNonBlocking(UARTA0_BASE);
        if (data != -1) {
            uart_config[0].rx_cb(uart_config[0].arg, data);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* UART_0_EN */

#if UART_1_EN
void isr_uart1(void)
{
    MAP_UARTIntClear(UARTA1_BASE, UART_INT_RX|UART_INT_OE|UART_INT_BE|UART_INT_PE|UART_INT_FE);

    if (UARTRxErrorGet(UARTA1_BASE)) {
        reset(UARTA1_BASE);
    } else {
        long data;

        data = MAP_UARTCharGetNonBlocking(UARTA1_BASE);
        if (data != -1) {
            uart_config[1].rx_cb(uart_config[1].arg, data);
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif /* UART_1_EN */

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg) {
    /* initialize basic functionality */
    int res = uart_init_blocking(uart, baudrate);

    if (res != 0) {
        return res;
    }

    /* register callbacks */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    /* configure interrupts and enable RX interrupt */
    switch (uart) {
#if UART_0_EN
    case UART_0:
        MAP_UARTIntEnable(UARTA0_BASE,
        UART_INT_RX | UART_INT_OE | UART_INT_BE | UART_INT_PE | UART_INT_FE);
        MAP_IntPrioritySet(INT_UARTA0, UART_IRQ_PRIO);
        MAP_IntEnable(INT_UARTA0);
        break;
#endif
#if UART_1_EN
        case UART_1:
        MAP_UARTIntEnable(UARTA1_BASE, UART_INT_RX|UART_INT_OE|UART_INT_BE|UART_INT_PE|UART_INT_FE);
        MAP_IntPrioritySet(INT_UARTA1, UART_IRQ_PRIO);
        MAP_IntEnable(INT_UARTA1);
        break;
#endif
    }

    return 0;
}

int uart_init_blocking(uart_t uart, uint32_t baudrate) {

    switch (uart) {
#if UART_0_EN
    case UART_0:

        MAP_PRCMPeripheralReset(PRCM_UARTA0);

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

        MAP_UARTConfigSetExpClk(UARTA0_BASE, MAP_PRCMPeripheralClockGet(PRCM_UARTA0),
                baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                UART_CONFIG_PAR_NONE));

        reset(UARTA0_BASE);

        break;
#endif
#if UART_1_EN
        case UART_1:

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

        MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(PRCM_UARTA1),
                baudrate, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                        UART_CONFIG_PAR_NONE));
        reset(UARTA1_BASE);

        break;
#endif

    default:
        return -1;
    }

    return 0;
}

void uart_tx_begin(uart_t uart) {

}

void uart_tx_end(uart_t uart) {

}

void uart_write(uart_t uart, const uint8_t *data, size_t len) {
    unsigned long u;

#ifdef CUSTOM_WRITE_R
    uart_remote(data, len);
#endif

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
        return;
    }

    for(size_t i=0; i<len; i++) {
    	MAP_UARTCharPut(u, data[i]);;
    }

}

int uart_read_blocking(uart_t uart, char *data) {
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

/**
 * @brief Get the Command string from UART
 *
 * @param  pucBuffer is the command store to which command will be populated
 * @param  ucBufLen is the length of buffer store available
 *
 * @return Length of the bytes received. -1 if buffer length exceeded.
 *
 **/
int uart_read_line(uart_t uart, char *pcBuffer, unsigned int uiBufLen) {
    char cChar;
    int iLen = 0;
    unsigned long CONSOLE;

    switch (uart) {
#if UART_0_EN
    case UART_0:
        CONSOLE = UARTA0_BASE;
        break;
#endif
#if UART_1_EN
        case UART_1:
        CONSOLE = UARTA1_BASE;
        break;
#endif

    default:
        return -1;
    }

    //
    // Wait to receive a character over UART
    //
    while (MAP_UARTCharsAvail(CONSOLE) == false) {
        xtimer_usleep(MSEC_TO_TICKS(1));
    }
    cChar = MAP_UARTCharGetNonBlocking(CONSOLE);

    //
    // Echo the received character
    //
    MAP_UARTCharPut(CONSOLE, cChar);
    iLen = 0;

    //
    // Checking the end of Command
    //
    while ((cChar != '\r') && (cChar != '\n')) {
        //
        // Handling overflow of buffer
        //
        if (iLen >= uiBufLen) {
            return -1;
        }

        //
        // Copying Data from UART into a buffer
        //
        if (cChar != '\b') {
            *(pcBuffer + iLen) = cChar;
            iLen++;
        } else {
            //
            // Deleting last character when you hit backspace
            //
            if (iLen) {
                iLen--;
            }
        }
        //
        // Wait to receive a character over UART
        //
        while (MAP_UARTCharsAvail(CONSOLE) == false) {
            xtimer_usleep(MSEC_TO_TICKS(1));
        }
        cChar = MAP_UARTCharGetNonBlocking(CONSOLE);
        //
        // Echo the received character
        //
        MAP_UARTCharPut(CONSOLE, cChar);
    }

    *(pcBuffer + iLen) = '\0';

    MAP_UARTCharPut(CONSOLE, '\n');
    MAP_UARTCharPut(CONSOLE, '\r');

    return iLen;
}

int uart_write_blocking(uart_t uart, char data) {
    MAP_UARTCharPut(UARTA0_BASE, data);

    return 1;
}

void uart_poweron(uart_t uart) {

}

void uart_poweroff(uart_t uart) {

}

#endif /* UART_NUMOF */
