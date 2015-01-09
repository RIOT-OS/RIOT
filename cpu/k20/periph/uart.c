/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       UART driver implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */


#include <stdlib.h>
#include <stdint.h>
#include <board.h>
#include <periph/uart.h>
#include <sched.h>
#include <thread.h>

#include "MK20DZ10.h"
#include "kinetis_sim.h"
#include "k20_uart.h"
#include "k20_interrupts.h"


void isr_uart_status(uart_t uartid);

typedef struct {
    bool blocking;
    bool transmitting;
    uart_rx_cb_t rx_callback;
    uart_tx_cb_t tx_callback;
    void *callback_data;
} uart_config_t;

static k20_uart0_output_t _k20_uart0_output = K20_UART0_OUTPUT_PTD_6_7;

uart_config_t config[UART_NUMOF];


void k20_uart0_select_output(k20_uart0_output_t output)
{
    _k20_uart0_output = output;
}

static UART_MemMapPtr _uart_get_base(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
    case UART_0:
        return UART0_BASE_PTR;
#endif
#if UART_1_EN
    case UART_1:
        return UART1_BASE_PTR;
#endif
#if UART_2_EN
    case UART_2:
        return UART2_BASE_PTR;
#endif
    default:
        return NULL;
    }
}

static int _uart_get_status_irqn(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
    case UART_0:
        return UART0_Status_IRQn;
#endif
#if UART_1_EN
    case UART_1:
        return UART1_Status_IRQn;
#endif
#if UART_2_EN
    case UART_2:
        return UART2_Status_IRQn;
#endif
    default:
        return 0;
    }
}

static int _uart_get_error_irqn(uart_t uart)
{
    switch (uart) {
#if UART_0_EN
    case UART_0:
        return UART0_Error_IRQn;
#endif
#if UART_1_EN
    case UART_1:
        return UART1_Error_IRQn;
#endif
#if UART_2_EN
    case UART_2:
        return UART2_Error_IRQn;
#endif
    default:
        return 0;
    }
}

static void _uart_clockgate_enable(uart_t uartid)
{
#if UART_0_EN
    if (uartid == UART_0) {
        switch (_k20_uart0_output) {
        case K20_UART0_OUTPUT_PTD_6_7:
            kinetis_clock_gate_enable(K20_CGATE_PORTD);
            break;
        case K20_UART0_OUTPUT_PTA_1_2:
            kinetis_clock_gate_enable(K20_CGATE_PORTA);
            break;
        case K20_UART0_OUTPUT_PTB_16_17:
            kinetis_clock_gate_enable(K20_CGATE_PORTB);
            break;
        }

        kinetis_clock_gate_enable(K20_CGATE_UART0);
    }
#endif
#if UART_1_EN
    if (uartid == UART_1) {
        kinetis_clock_gate_enable(K20_CGATE_PORTC);
        kinetis_clock_gate_enable(K20_CGATE_UART1);
    }
#endif
#if UART_2_EN
    if (uartid == UART_2) {
        kinetis_clock_gate_enable(K20_CGATE_PORTD);
        kinetis_clock_gate_enable(K20_CGATE_UART2);
    }
#endif
}

static void _uart_clockgate_disable(uart_t uartid)
{
#if UART_0_EN
    if (uartid == UART_0) {
        kinetis_clock_gate_disable(K20_CGATE_UART0);
    }
#endif
#if UART_1_EN
    if (uartid == UART_1) {
        kinetis_clock_gate_disable(K20_CGATE_UART1);
    }
#endif
#if UART_2_EN
    if (uartid == UART_2) {
        kinetis_clock_gate_disable(K20_CGATE_UART2);
    }
#endif
}

void uart_poweron(uart_t uartid)
{
    _uart_clockgate_enable(uartid);

    /* Enable the Interrupts */
    if (!config[uartid].blocking) {
        int status_irqn = _uart_get_status_irqn(uartid);
        NVIC_EnableIRQ(status_irqn);
        NVIC_SetPriority(status_irqn, UART_IRQ_PRIO);

        int error_irqn = _uart_get_error_irqn(uartid);
        NVIC_EnableIRQ(error_irqn);

        #ifdef UART_ERROR_IRQ_PRIO
        int error_irq_prio = UART_ERROR_IRQ_PRIO;
        #else
        /**
         *  Add 1 to the UART priority. The error interrupt should be
         *  more important than the status interrupt
         */
        int error_irq_prio = UART_IRQ_PRIO + 1;
        #endif

        NVIC_SetPriority(error_irqn, error_irq_prio);
    }

    UART_MemMapPtr uart = _uart_get_base(uartid);
    uart->C2 |= (1 << UART_C2_TE_SHIFT)   /* Enable UART Transmitter */
             | (1 << UART_C2_RE_SHIFT);   /* Enable UART Receiver */
}

void uart_poweroff(uart_t uartid)
{
    _uart_clockgate_disable(uartid);

    /* Disable the interrupts */
    if (!config[uartid].blocking) {
        NVIC_DisableIRQ(_uart_get_status_irqn(uartid));
        NVIC_DisableIRQ(_uart_get_error_irqn(uartid));
    }

    UART_MemMapPtr uart = _uart_get_base(uartid);
    uart->C2 &= ~((1 << UART_C2_TE_SHIFT)    /* Disable UART Transmitter */
                 |(1 << UART_C2_RE_SHIFT));  /* Disable UART Receiver */

    _uart_clockgate_disable(uartid);
}

static int _uart_configure(uart_t uartid, uint32_t baudrate, bool blocking)
{
    /* Configure the UART interface */
    /* TODO: UART 2 runs with bus clock. This code assumes it runs with CPU clock */
    UART_MemMapPtr uart = _uart_get_base(uartid);

    uint32_t clk = F_CPU;
    uint32_t divider = clk / (16 * baudrate);

    /* SBR is 13 bits */
    uint32_t sbr = divider & 0x1FFF;

    if (sbr != divider) {
        /* Invalid baud rate */
        return -1;
    }

    /**
     * The below calculation will yield a fine adjust value rounded to the nearest
     * configurable fraction.
     */
    //~ brfa = (((64ull * clk) / (baudrate * 16) + 1) / 2) % 32;
    uint8_t brfa = ((((4 * clk) / baudrate) + 1) / 2) % 32;

    /* Setup pins */
    _uart_clockgate_enable(uartid);

    if (uartid == 0) {
        switch (_k20_uart0_output) {
        case K20_UART0_OUTPUT_PTD_6_7:
            PORTD_BASE_PTR->PCR[6] = (3 << PORT_PCR_MUX_SHIFT);
            PORTD_BASE_PTR->PCR[7] = (3 << PORT_PCR_MUX_SHIFT);
            break;
        case K20_UART0_OUTPUT_PTA_1_2:
            PORTA_BASE_PTR->PCR[1] = (2 << PORT_PCR_MUX_SHIFT);
            PORTA_BASE_PTR->PCR[2] = (2 << PORT_PCR_MUX_SHIFT);
            break;
        case K20_UART0_OUTPUT_PTB_16_17:
            PORTB_BASE_PTR->PCR[16] = (3 << PORT_PCR_MUX_SHIFT);
            PORTB_BASE_PTR->PCR[17] = (3 << PORT_PCR_MUX_SHIFT);
            break;
        }
    } else if (uartid == 1) {
        PORTC_BASE_PTR->PCR[3] = (3 << PORT_PCR_MUX_SHIFT);
        PORTC_BASE_PTR->PCR[4] = (3 << PORT_PCR_MUX_SHIFT);
    } else if (uartid == 2) {
        PORTD_BASE_PTR->PCR[2] = (3 << PORT_PCR_MUX_SHIFT);
        PORTD_BASE_PTR->PCR[3] = (3 << PORT_PCR_MUX_SHIFT);
    }

    /* Setup FIFOs. The MCU only supports 1-byte FIFOs for UART 2 & 3 */
    if (uart == UART0_BASE_PTR) {
        uart->PFIFO = (1 << UART_PFIFO_TXFE_SHIFT) /* Enable TX FIFO */
                    | (1 << UART_PFIFO_RXFE_SHIFT);/* Enable RX FIFO */

        uart->TWFIFO = 7;
    }
    else {
        uart->PFIFO = (1 << UART_PFIFO_TXFE_SHIFT) /* Enable TX FIFO */
                    | (1 << UART_PFIFO_RXFE_SHIFT);/* Enable RX FIFO */

        uart->TWFIFO = 0;
    }

    uart->RWFIFO = 1; /* Assert a new byte when the FIFO is 1 */

    uart->CFIFO = (1 << UART_CFIFO_TXFLUSH_SHIFT) /* Flush TX FIFO */
                | (1 << UART_CFIFO_RXFLUSH_SHIFT) /* Flush RX FIFO */
                | ((int)(!blocking) << UART_CFIFO_TXOFE_SHIFT)   /* Enable TX FIFO overflow interrupt */
                | (0 << UART_CFIFO_RXUFE_SHIFT);  /* Disable RX FIFO underflow interrupt */


    /* "To update the 13-bit baud rate setting, SBR[12:0], first write to BDH
       to buffer the high half of the new value and then write to BDL." */
    uart->BDH = (0 << UART_BDH_LBKDIE_SHIFT)   /* No line break interrupt */
              | (1 << UART_BDH_RXEDGIE_SHIFT)  /* Receive input interrupt */
              | ((sbr >> 8) & UART_BDH_SBR_MASK);

    uart->BDL = (sbr & UART_BDL_SBR_MASK);

    uart->C1 = (0 << UART_C1_LOOPS_SHIFT)    /* No loopback mode */
             | (0 << UART_C1_UARTSWAI_SHIFT) /* Clock continues in Wait mode */
             | (0 << UART_C1_RSRC_SHIFT)     /* Loop back mode is ignored */
             | (0 << UART_C1_M_SHIFT)        /* 8 data bits */
             | (0 << UART_C1_WAKE_SHIFT)     /* Idle line wakeup */
             | (0 << UART_C1_ILT_SHIFT)      /* Idle line type: after start bit */
             | (0 << UART_C1_PE_SHIFT)       /* No parity */
             | (0 << UART_C1_PT_SHIFT);      /* Ignored bc. no parity */

    uart->C2 = (0 << UART_C2_TIE_SHIFT)  /* No transmitter interrupt */
             | (0 << UART_C2_TCIE_SHIFT) /* Transmission complete interrupt */
             | ((int)(!blocking) << UART_C2_RIE_SHIFT)  /* Receiver full interrupt */
             | (0 << UART_C2_ILIE_SHIFT) /* No idle line interrupt */
             | (0 << UART_C2_TE_SHIFT)   /* Enable UART Transmitter */
             | (0 << UART_C2_RE_SHIFT)   /* Enable UART Receiver */
             | (0 << UART_C2_RWU_SHIFT)  /* Wakeup Control: Normal function */
             | (0 << UART_C2_SBK_SHIFT); /* No queue break characters */

    uart->C3 = (0 << UART_C3_R8_SHIFT)    /* Received bit 8: ignored */
             | (0 << UART_C3_T8_SHIFT)    /* Transmit bit 8: ignored */
             | (0 << UART_C3_TXDIR_SHIFT) /* Single wire mode: ignored */
             | (0 << UART_C3_TXINV_SHIFT) /* No transmit data inversion */
             | (0 << UART_C3_ORIE_SHIFT)  /* Overrun error interrupt enable */
             | (0 << UART_C3_NEIE_SHIFT)  /* Noise flag interrupt enable */
             | (0 << UART_C3_FEIE_SHIFT)  /* Framing error interrupt enable */
             | (0 << UART_C3_PEIE_SHIFT); /* Parity error interrupt enable */

    uart->C4 = (0 << UART_C4_MAEN1_SHIFT) /* Match address default behavior */
             | (0 << UART_C4_MAEN2_SHIFT) /* Match address default behavior */
             | (0 << UART_C4_M10_SHIFT)   /* No 10 bit mode */
             | UART_C4_BRFA(brfa);        /* Baud rate fine adjust value */

    uart_poweron(uartid);

    return 0;
}

int uart_init(uart_t uartid, uint32_t baudrate, uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if (uart == NULL) {
        return -2;
    }

    /* Save the callback information */
    config[uartid].blocking = false;
    config[uartid].rx_callback = rx_cb;
    config[uartid].tx_callback = tx_cb;
    config[uartid].callback_data = arg;

    return _uart_configure(uartid, baudrate, false);
}

int uart_init_blocking(uart_t uartid, uint32_t baudrate)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if (uart == NULL) {
        return -2;
    }

    config[uartid].blocking = true;
    return _uart_configure(uartid, baudrate, true);
}

static void _uart_tx_end(uart_t uartid)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if(uart && !config[uartid].blocking) {
        config[uartid].transmitting = false;

        /* Disable Transmission complete interrupt */
        uart->C2 &= ~(1 << UART_C2_TCIE_SHIFT);
    }
}

void uart_tx_begin(uart_t uartid)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if(uart && !config[uartid].blocking) {
        config[uartid].transmitting = true;

        /* Enable Transmission complete interrupt */
        uart->C2 |= (1 << UART_C2_TCIE_SHIFT);

        /**
         * Call the interrupt handler yourself the first time
         * It will take care of it when nothing is to do (still transmitting)
         */
        isr_uart_status(uartid);
    }
}

int uart_write_blocking(uart_t uartid, char data)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if (uart) {
        while(!(uart->S1 & UART_S1_TDRE_MASK));
        uart->D = data;

        return 1;
    }

    return -1;
}

int uart_write(uart_t uartid, char data)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);
    if (uart) {
        uart->D = data;

        return 1;
    }

    return -1;
}

int uart_read_blocking(uart_t uartid, char *data)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);

    if(uart) {
        while(!(uart->S1 & UART_S1_RDRF_MASK));
        *data = uart->D;

        return 1;
    }

    return -1;
}

/**
 *  @brief      Combined UART IRQ status handler
 *
 *  @param[in]  uartid  The UART id of the source IRQ
 *  @{
 */
void isr_uart_status(uart_t uartid)
{
    UART_MemMapPtr uart = _uart_get_base(uartid);

    bool blocking = config[uartid].blocking;

    if (!blocking) {
        if (config[uartid].transmitting && config[uartid].tx_callback != NULL) {
            int send_more_data = 1;

            while(send_more_data == 1 && uart->S1 & UART_S1_TDRE_MASK) {
                /* Transmit FIFO not full */
                /* Transmit a byte */
                send_more_data = config[uartid].tx_callback(config[uartid].callback_data);
            }

            if (send_more_data == 0) {
                _uart_tx_end(uartid);
            }
        }

        if ((uart->RCFIFO & UART_RCFIFO_RXCOUNT_MASK) > 0) {
            /* Receive data register full */
            do {
                if ((uart->RCFIFO & UART_RCFIFO_RXCOUNT_MASK) == 1) {
                    /* Last read, clear the flag */
                    uart->S1;
                }

                /* Read a byte */
                uint8_t data = uart->D;

                /* Tell the callback */
                if(config[uartid].rx_callback != NULL) {
                    config[uartid].rx_callback(config[uartid].callback_data, data);
                }
            } while((uart->RCFIFO & UART_RCFIFO_RXCOUNT_MASK) > 0);

            /* Clear received input interrupt */
            uart->S2 |= UART_S2_RXEDGIF_MASK;
        }
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}
/** @} */

/**
 *  @brief      Combined UART IRQ error handler
 *
 *  @param[in]  uartid  The UART id of the source IRQ
 *  @TODO       Implement error handling
 *  @{
 */
void isr_uart_error(uart_t uartid)
{
    return;
}
/** @} */

#if UART_0_EN
void isr_uart0_status(void)
{
    isr_uart_status(UART_0);
}

void isr_uart0_error(void)
{
    isr_uart_error(UART_0);
}
#endif

#if UART_1_EN
void isr_uart1_status(void)
{
    isr_uart_status(UART_1);
}

void isr_uart1_error(void)
{
    isr_uart_error(UART_1);
}
#endif

#if UART_2_EN
void isr_uart2_status(void)
{
    isr_uart_status(UART_2);
}

void isr_uart2_error(void)
{
    isr_uart_error(UART_2);
}
#endif

