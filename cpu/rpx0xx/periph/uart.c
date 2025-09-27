/*
 * SPDX-FileCopyrightText: 2021 Nick Weiler, Justus Krebs, Franz Freitag
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       UART driver implementation for the RP2040
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "assert.h"
#include "bitarithm.h"
#include "board.h"
#include "io_reg.h"
#include "irq.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG    0
#include "debug.h"

static uart_isr_ctx_t ctx[UART_NUMOF];

/* back up values of registers used during uart_poweroff() / uart_poweron() */
static uint32_t uartibrd;
static uint32_t uartfbrd;
static uint32_t uartlcr_h;
static uint32_t uartcr;

void _irq_enable(uart_t uart)
{
    UART0_Type *dev = uart_config[uart].dev;
    dev->UARTIMSC = UART0_UARTIMSC_RXIM_Msk;
    NVIC_EnableIRQ(uart_config[uart].irqn);
}

void _set_symbolrate(uart_t uart, uint32_t baud)
{
    assert(baud != 0);
    UART0_Type *dev = uart_config[uart].dev;
    uint32_t baud_rate_div = (8 * CLOCK_PERIPH / baud);
    uint32_t baud_ibrd = baud_rate_div >> 7;
    uint32_t baud_fbrd;

    if (baud_ibrd == 0) {
        baud_ibrd = 1;
        baud_fbrd = 0;
    }
    else if (baud_ibrd >= 65535) {
        baud_ibrd = 65535;
        baud_fbrd = 0;
    }
    else {
        baud_fbrd = ((baud_rate_div & 0x7f) + 1) / 2;
    }

    dev->UARTIBRD = baud_ibrd;
    dev->UARTFBRD = baud_fbrd;
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t uart_parity,
              uart_stop_bits_t stop_bits)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    io_reg_atomic_clear(&dev->UARTCR,
                        UART0_UARTCR_UARTEN_Msk | UART0_UARTCR_TXE_Msk | UART0_UARTCR_RXE_Msk);

    /* Beware of strange hardware bug: If the configuration bitmask is prepared in register and
     * transferred with a single 32 bit write (updating both parity and number of data bits at the
     * same time), the configuration change of the parity bits will not take place until after the
     * next char send out. If the configuration is updated in multiple bus accesses, it will apply
     * directly to the next char. So: Double check e.g. with tests/periph/uart_mode after touching
     * the initialization code here */
    dev->UARTLCR_H = (uint32_t)data_bits << UART0_UARTLCR_H_WLEN_Pos;

    if (stop_bits == UART_STOP_BITS_2) {
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_STP2_Msk);
    }

    switch (uart_parity) {
    case UART_PARITY_NONE:
        break;
    case UART_PARITY_EVEN:
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_EPS_Msk | UART0_UARTLCR_H_PEN_Msk);
        break;
    case UART_PARITY_ODD:
        io_reg_atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_PEN_Msk);
        break;
    default:
        return UART_NOMODE;
    }

    io_reg_atomic_set(&dev->UARTCR,
                      UART0_UARTCR_UARTEN_Msk | UART0_UARTCR_TXE_Msk | UART0_UARTCR_RXE_Msk);

    return UART_OK;
}

void uart_init_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    /* not reusing gpio_init() here to not send a char of garbage upon init. Also, we
     * can use a lower drive strength here, hopefully limiting the current a bit when TX is
     * connected to the recipients TX by accident. */
    const gpio_pad_ctrl_t tx_pad_config = {
        .drive_strength = DRIVE_STRENGTH_2MA,
    };
    const gpio_io_ctrl_t tx_io_config = {
        .function_select = FUNCTION_SELECT_UART,
    };
    const gpio_pad_ctrl_t rx_pad_config = {
        .pull_up_enable = 1,
        .input_enable = 1,
        .schmitt_trig_enable = 1,
    };
    const gpio_io_ctrl_t rx_io_config = {
        .function_select = FUNCTION_SELECT_UART,
    };
    gpio_set_pad_config(uart_config[uart].tx_pin, tx_pad_config);
    gpio_set_io_config(uart_config[uart].tx_pin, tx_io_config);
    if (ctx[uart].rx_cb) {
        gpio_set_pad_config(uart_config[uart].rx_pin, rx_pad_config);
        gpio_set_io_config(uart_config[uart].rx_pin, rx_io_config);
    }
}

void uart_deinit_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    gpio_reset_all_config(uart_config[uart].tx_pin);
    SIO->GPIO_OE_CLR = 1LU << uart_config[uart].tx_pin;
    if (ctx[uart].rx_cb) {
        gpio_reset_all_config(uart_config[uart].rx_pin);
    }
}

static void _poweron(uart_t uart)
{
    uint32_t reset_bit_mask = (uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk;
    periph_reset(reset_bit_mask);
    periph_reset_done(reset_bit_mask);
}

void uart_poweron(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    _poweron(uart);
    UART0_Type *dev = uart_config[uart].dev;
    /* restore configuration registers */
    dev->UARTIBRD = uartibrd;
    dev->UARTFBRD = uartfbrd;
    dev->UARTLCR_H = uartlcr_h;
    dev->UARTCR = uartcr;
    /* restore IRQs, if needed */
    if (ctx[uart].rx_cb != NULL) {
        _irq_enable(uart);
    }
    uart_init_pins(uart);
}

void uart_poweroff(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;
    /* backup configuration registers */
    uartibrd = dev->UARTIBRD;
    uartfbrd = dev->UARTFBRD;
    uartlcr_h = dev->UARTLCR_H;
    uartcr = dev->UARTCR;
    /* disconnect GPIOs and power off peripheral */
    uart_deinit_pins(uart);
    periph_reset((uart) ? RESETS_RESET_uart1_Msk : RESETS_RESET_uart0_Msk);
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    UART0_Type *dev = uart_config[uart].dev;
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    _poweron(uart);
    uart_init_pins(uart);

    /* Beware: Changes to the symbol rate only take affect after touching the UARTLCR_H register,
     * which is done in uart_mode(). */
    _set_symbolrate(uart, baud);

    if (uart_mode(uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1) != UART_OK) {
        return UART_NOMODE;
    }

    /* enable RX and IRQs, if needed */
    if (rx_cb != NULL) {
        _irq_enable(uart);
        /* clear any pending data and IRQ to avoid receiving a garbage char */
        uint32_t status = dev->UARTRIS;
        dev->UARTICR = status;
        (void)dev->UARTDR;
        io_reg_atomic_set(&dev->UARTCR, UART0_UARTCR_RXE_Msk);
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        dev->UARTDR = data[i];
        while (!(dev->UARTRIS & UART0_UARTRIS_TXRIS_Msk)) { }
    }
}

gpio_t uart_pin_rx(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    return uart_config[uart].rx_pin;
}

gpio_t uart_pin_tx(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    return uart_config[uart].tx_pin;
}

void isr_handler(uint8_t num)
{
    UART0_Type *dev = uart_config[num].dev;

    uint32_t status = dev->UARTMIS;
    dev->UARTICR = status;

    if (status & UART0_UARTMIS_RXMIS_Msk) {
        uint32_t data = dev->UARTDR;
        if (data & (UART0_UARTDR_BE_Msk | UART0_UARTDR_PE_Msk | UART0_UARTDR_FE_Msk)) {
            DEBUG_PUTS("[rpx0xx] uart RX error (parity, break, or framing error");
        }
        else {
            ctx[num].rx_cb(ctx[num].arg, (uint8_t)data);
        }
    }
}

void isr_uart0(void)
{
    isr_handler(0);
    cortexm_isr_end();
}

void isr_uart1(void)
{
    isr_handler(1);
    cortexm_isr_end();
}
