/*
 * Copyright(C) 2016,2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

 /**
  * @ingroup     cpu_mips_pic32_common
  * @ingroup     drivers_periph_uart
  * @{
  *
  * @file
  * @brief       Peripheral UART driver implementation
  *
  * @}
  */
#include <assert.h>
#include "periph/uart.h"
#include "board.h"

#define UxMODE(U)    (U.regs[0x00/4])
#define UxMODECLR(U) (U.regs[0x04/4])
#define UxMODESET(U) (U.regs[0x08/4])
#define UxSTA(U)     (U.regs[0x10/4])
#define UxSTACLR(U)  (U.regs[0x14/4])
#define UxSTASET(U)  (U.regs[0x18/4])
#define UxTXREG(U)   (U.regs[0x20/4])
#define UxRXREG(U)   (U.regs[0x30/4])
#define UxBRG(U)     (U.regs[0x40/4])
#define REGS_SPACING (_UART2_BASE_ADDRESS - _UART1_BASE_ADDRESS)

/* PERIPHERAL_CLOCK must be defined in board file */

typedef struct PIC32_UART_tag {
    volatile uint32_t *regs;
    uint32_t clock;
} PIC32_UART_T;

/* pic uarts are numbered 1 to 6 */
static PIC32_UART_T pic_uart[UART_NUMOF + 1];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    (void)rx_cb;
    (void)arg;

    assert(uart <= UART_NUMOF && uart != 0); /*No uart 0 on pic32*/

    /* Pin Mux should be setup in board file */

    pic_uart[uart].regs =
        (volatile uint32_t *)(_UART1_BASE_ADDRESS + (uart - 1) * REGS_SPACING);
    pic_uart[uart].clock = PERIPHERAL_CLOCK;

    UxBRG(pic_uart[uart])= (pic_uart[uart].clock / (16 * baudrate)) - 1;
    UxSTA(pic_uart[uart])= 0;
    UxMODE(pic_uart[uart])= _U1MODE_ON_MASK;
    UxSTASET(pic_uart[uart])= _U1STA_URXEN_MASK;
    UxSTASET(pic_uart[uart])= _U1STA_UTXEN_MASK;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart <= UART_NUMOF && uart != 0);

    while(len--) {
        while(UxSTA(pic_uart[uart])& _U1STA_UTXBF_MASK) {}
        UxTXREG(pic_uart[uart]) = *data++;
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart <= UART_NUMOF && uart != 0);

    UxMODESET(pic_uart[uart])= _U1MODE_ON_MASK;

}

void uart_poweroff(uart_t uart)
{
    assert(uart <= UART_NUMOF && uart != 0);

    UxMODECLR(pic_uart[uart])= _U1MODE_ON_MASK;
}
