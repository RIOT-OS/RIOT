/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           UART implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "board.h"
#include "compat_layer.h"
#include "periph_cpu.h"

#include "regs/uart.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 0
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
    /* We set the UART Receive Interrupt Mask (Bit 4) [See p979 UART 12.1]*/
    dev->UARTIMSC = UART_UARTIMSC_RXIM_BITS;
    /* Enable the IRQ */
    rp_irq_enable(uart_config[uart].irqn);
}

void _set_symbolrate(uart_t uart, uint32_t baud)
{
    assert(baud != 0);
    UART0_Type *dev = uart_config[uart].dev;
    uint32_t baud_rate_div = (8 * CPUFREQ / baud);
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

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    /* Disable the UART before changing the mode */
    atomic_clear(&dev->UARTCR, UART_UARTCR_UARTEN_BITS | UART_UARTCR_RXE_BITS |
                                   UART_UARTCR_TXE_BITS | 1 << 7);

    /* Beware of strange hardware bug: If the configuration bitmask is prepared in register and
     * transferred with a single 32 bit write (updating both parity and number of data bits at the
     * same time), the configuration change of the parity bits will not take place until after the
     * next char send out. If the configuration is updated in multiple bus accesses, it will apply
     * directly to the next char. So: Double check e.g. with tests/periph/uart_mode after touching
     * the initialization code here
     * based on Table 1035 page 976 */
    dev->UARTLCR_H = (uint32_t)data_bits << 5;

    if (stop_bits == UART_STOP_BITS_2) {
        atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_STP2_Msk);
    }

    switch (parity) {
    case UART_PARITY_NONE:
        break;
    case UART_PARITY_EVEN:
        atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_EPS_Msk | UART0_UARTLCR_H_PEN_Msk);
        break;
    case UART_PARITY_ODD:
        atomic_set(&dev->UARTLCR_H, UART0_UARTLCR_H_PEN_Msk);
        break;
    default:
        return UART_NOMODE;
    }

    atomic_set(&dev->UARTCR, UART_UARTCR_TXE_BITS | UART_UARTCR_UARTEN_BITS | UART_UARTCR_RXE_BITS);

    return UART_OK;
}

static void _reset_uart(uart_t uart)
{
    switch (uart) {
    case 0:
        /* We reset UART0 here, so we can be sure it is in a known state */
        reset_component(RESET_UART0, RESET_UART0);
        break;
    case 1:
        /* We reset UART1 here, so we can be sure it is in a known state */
        reset_component(RESET_UART1, RESET_UART1);
        break;
    default:
        break;
    }
}

void uart_init_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);

    /* We reset UART0 here, so we can be sure it is in a known state */
    _reset_uart(uart);

    /* Set the UART pins to the correct function */
    *calculate_gpio_io_ctrl_register_addr(uart_config[uart].tx_pin) = FUNCTION_SELECT_UART;
    *calculate_gpio_io_ctrl_register_addr(uart_config[uart].rx_pin) = FUNCTION_SELECT_UART;
    /* Clear the ISO bits */
    atomic_clear(
        calculate_gpio_pad_register_addr(uart_config[uart].tx_pin),
        PADS_BANK0_ISO_BITS);
    atomic_clear(
        calculate_gpio_pad_register_addr(uart_config[uart].rx_pin),
        PADS_BANK0_ISO_BITS);

    /* Set Input Enable Flag  */
    atomic_set(
        calculate_gpio_pad_register_addr(uart_config[uart].rx_pin),
        PADS_BANK0_GPIO0_IE_BITS);
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    UART0_Type *dev = uart_config[uart].dev;
    ctx[uart].rx_cb = rx_cb;
    ctx[uart].arg = arg;

    uart_init_pins(uart);

    _set_symbolrate(uart, baud);

    if (uart_mode(
            uart,
            UART_DATA_BITS_8,
            UART_PARITY_NONE,
            UART_STOP_BITS_1) != UART_OK) {
        return UART_NOMODE;
    }

    /* enable RX and IRQs, if needed */
    if (rx_cb != NULL) {
        _irq_enable(uart);
        /* clear any pending data and IRQ to avoid receiving a garbage char */
        uint32_t status = dev->UARTRIS;
        dev->UARTICR = status;
        (void)dev->UARTDR;
        atomic_set(&dev->UARTCR, UART_UARTCR_RXE_BITS);
    }

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert((unsigned)uart < UART_NUMOF);
    UART0_Type *dev = uart_config[uart].dev;

    for (size_t i = 0; i < len; i++) {
        dev->UARTDR = data[i];
        /* Wait until the TX FIFO is empty before sending the next byte */
        while (!(dev->UARTRIS & UART0_UARTRIS_TXRIS_Msk)) { }
    }
}

void uart_poweron(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    /* Get into a safe state where we know what's up */
    _reset_uart(uart);
    UART0_Type *dev = uart_config[uart].dev;
    /* Restore config from registers */
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

void uart_deinit_pins(uart_t uart)
{
    assert((unsigned)uart < UART_NUMOF);
    /* @TODO */
    /* gpio_reset_all_config(uart_config[uart].tx_pin); */
    SIO->GPIO_OE_CLR = 1LU << uart_config[uart].tx_pin;
    if (ctx[uart].rx_cb) {
        /* gpio_reset_all_config(uart_config[uart].rx_pin); */
    }
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
    _reset_uart(uart);
}

void isr_handler(uint8_t num)
{
    UART0_Type *dev = uart_config[num].dev;

    uint32_t status = dev->UARTMIS;
    atomic_set(&dev->UARTICR, status);

    if (status & UART_UARTMIS_RXMIS_BITS) {
        uint32_t data = dev->UARTDR;
        if (data & (UART0_UARTDR_BE_Msk | UART0_UARTDR_PE_Msk | UART0_UARTDR_FE_Msk)) {
            puts("[rpx0xx] uart RX error (parity, break, or framing error");
        }
        else {
            ctx[num].rx_cb(ctx[num].arg, (uint8_t)data);
        }
    }
}

/** Overwrites the WEAK_DEFAULT isr_uart0 */
void isr_uart0(void)
{
    isr_handler(0);
    rp_end_isr();
}

void isr_uart1(void)
{
    isr_handler(1);
    rp_end_isr();
}

/** @} */
