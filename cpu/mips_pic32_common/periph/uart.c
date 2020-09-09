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

#include "cpu.h"
#include "eic.h"
#include "sched.h"
#include "thread.h"
#include "assert.h"
#include "periph/uart.h"
#include "periph/gpio.h"

#define UxMODE(U)    ((U)[0x00/4])
#define UxMODECLR(U) ((U)[0x04/4])
#define UxMODESET(U) ((U)[0x08/4])
#define UxSTA(U)     ((U)[0x10/4])
#define UxSTACLR(U)  ((U)[0x14/4])
#define UxSTASET(U)  ((U)[0x18/4])
#define UxTXREG(U)   ((U)[0x20/4])
#define UxRXREG(U)   ((U)[0x30/4])
#define UxBRG(U)     ((U)[0x40/4])

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

static void irq_handler(uart_t uart)
{
    uint32_t status = UxSTA(uart_config[uart].base);

    if (status & _U1STA_URXDA_MASK) {
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, UxRXREG(uart_config[uart].base));
    }
    if (status & _U1STA_OERR_MASK) {
        UxSTA(uart_config[uart].base) &= ~_U1STA_OERR_MASK;
    }
#ifdef CPU_FAM_PIC32MX
    eic_clear_flag(uart_config[uart].irq);
#else
    eic_clear_flag(uart_config[uart].vector);
#endif

    mips32r2_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(UART_DEV(0));
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(UART_DEV(1));
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(UART_DEV(2));
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(UART_DEV(3));
}
#endif

#ifdef UART_4_ISR
void UART_4_ISR(void)
{
    irq_handler(UART_DEV(4));
}
#endif

#ifdef UART_5_ISR
void UART_5_ISR(void)
{
    irq_handler(UART_DEV(5));
}
#endif

static void uart_init_pins(uart_t uart, uart_rx_cb_t rx_cb)
{
     /* configure TX pin */
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    /* set TX pin high to avoid garbage during further initialization */
    gpio_set(uart_config[uart].tx_pin);
    *(uart_config[uart].tx_mux_reg) = uart_config[uart].tx_af;

    /* configure RX pin */
    if (rx_cb) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN_PU);
        *(uart_config[uart].rx_mux_reg) = uart_config[uart].rx_af;
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    uart_init_pins(uart, rx_cb);

    UxBRG(uart_config[uart].base) = (uart_config[uart].clock / (16 * baudrate)) - 1;
    UxSTA(uart_config[uart].base) = 0;
    if (rx_cb) {
        /* register callbacks */
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
        UxSTASET(uart_config[uart].base) = _U1STA_URXEN_MASK;

        switch (uart) {
#ifdef UART_0_ISR
        case UART_DEV(0): set_external_isr_cb(uart_config[uart].vector, UART_0_ISR); break;
#endif
#ifdef UART_1_ISR
        case UART_DEV(1): set_external_isr_cb(uart_config[uart].vector, UART_1_ISR); break;
#endif
#ifdef UART_2_ISR
        case UART_DEV(2): set_external_isr_cb(uart_config[uart].vector, UART_2_ISR); break;
#endif
#ifdef UART_3_ISR
        case UART_DEV(3): set_external_isr_cb(uart_config[uart].vector, UART_3_ISR); break;
#endif
#ifdef UART_4_ISR
        case UART_DEV(4): set_external_isr_cb(uart_config[uart].vector, UART_4_ISR); break;
#endif
#ifdef UART_5_ISR
        case UART_DEV(5): set_external_isr_cb(uart_config[uart].vector, UART_5_ISR); break;
#endif
        }
        eic_configure_priority(uart_config[uart].vector, 1, 0);

#ifdef CPU_FAM_PIC32MX
        eic_enable(uart_config[uart].irq);
#else
        eic_enable(uart_config[uart].vector);
#endif

    }
    UxSTASET(uart_config[uart].base) = _U1STA_UTXEN_MASK;
    UxMODE(uart_config[uart].base) = _U1MODE_ON_MASK;

    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    while(len--) {
        while(UxSTA(uart_config[uart].base)& _U1STA_UTXBF_MASK) {}
        UxTXREG(uart_config[uart].base) = *data++;
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    UxMODESET(uart_config[uart].base)= _U1MODE_ON_MASK;

}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    UxMODECLR(uart_config[uart].base)= _U1MODE_ON_MASK;
}
