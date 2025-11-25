/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_uart
 *
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * This implementation only supports blocking writing using busy-wait.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "gpio_mux.h"
#include "flexcomm.h"

#include <stdlib.h>

#include "vendor/drivers/fsl_clock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Runtime UART configuration with user callback function for RX.
 */
static uart_isr_ctx_t config[UART_NUMOF];

/**
 * @brief The device number (in the UART_NUMOF range) registered for each
 *        flexcomm port.
 */
static uart_t uart_dev_from_flexcomm[FSL_FEATURE_SOC_FLEXCOMM_COUNT] = {
    [0 ... FSL_FEATURE_SOC_FLEXCOMM_COUNT - 1] = UART_NUMOF
};

/**
 * @brief Limit values for the USART OSRVAL, BRGVAL and MULTx values.
 * @{
 */
#define UART_OSRVAL_MAX 15u
#define UART_BRGVAL_MAX 0xffffu
#define UART_MULTX_MAX 255u
/** @} */

/**
 * @brief The maximum UART frequency divisor possible.
 *
 * This will limit the lower end of the baudrate allowed.
 */
#define UART_MAX_DIVISOR ((UART_OSRVAL_MAX + 1) * (UART_BRGVAL_MAX + 1) * \
                          (UART_MULTX_MAX + 256))

static int _uart_set_baudrate(USART_Type *dev, uint8_t flexcomm_num,
                              uint32_t baudrate)
{
    assert(flexcomm_num < 2);
    /* The FLEXCOMM clock for the FLEXCOMM0 and FLEXCOMM1 is based on the
     * kCLOCK_BusClk clock frequency with an optional divisor, using the
     * following formula:
     *   flexcomm freq := bus freq / (1 + (MULTx / 256))
     * where MULTx is a value between 0 and 255 and is set with
     * CLOCK_SetClkDiv(). Only the FLEXCOMM0 and FLEXCOMM1 support USART
     * function, so the MULTx is always available.
     * The UART baudrate is then:
     *   uart baudrate := flexcomm freq / ((OSRVAL + 1) * (BRGVAL + 1))
     * where OSRVAL (oversample selection value) is a number between 4 and 15
     * (the larger the better) and BRGVAL is between 0 and 0xffff.
     * Combining and expanding the previous expression:
     *   bus freq * 256 / baudrate = (256 + MULTx) * (OSRVAL + 1) * (BRGVAL + 1)
     * so we need to find those the values that minimize the error and maximize
     * OSRVAL.
     */
    if (baudrate == 0) {
        return UART_NOBAUD;
    }
    uint32_t bus_freq = CLOCK_GetFreq(kCLOCK_BusClk);
    uint32_t target;
    {
        /* The remainder of this division is unavoidable frequency error at the
         * current clock frequency so we can discard it now. We add 127 to round
         * up or down to the nearest target value. */
        const uint64_t target64 = (((uint64_t)(bus_freq) << 8ull) + 127u) /
                                  baudrate;
        if (target64 > UART_MAX_DIVISOR) {
            return UART_NOBAUD;
        }
        /* At this point we know the target value fits in 32-bit since
         * UART_MAX_DIVISOR fits in 32-bit. */
        target = target64;
    }

    uint32_t best_osrval = 0;
    uint32_t best_multx = 0;
    uint32_t best_brgval = 0;
    uint32_t best_error = UINT_MAX;

    /* To simplify the math, let's assume we need to pick 3 values A, B and C
     * such that A * B * C is as close as a possible to a given target T. In
     * other words, we need to minimize the error |T - A * B * C|.
     * To do that, we scan over all possible values of A and B (about 2000
     * possibilities) and compute the error value taking C as the following:
     *   C := floor((T + A * B / 2) / (A * B))
     * To compute the error we can avoid some multiplications if we consider
     * that we can decompose a number N as "floor(N / M) * M + remainder(N, M)"
     * taking N = T + A * B / 2 and M = A * B we get the error:
     *   |T - A * B * C| = |T + A * B / 2 - A * B * C - A * B / 2|
     *   = |remainder(T + A * B / 2, A * B) - A * B / 2|
     */
    for (uint8_t osrval_p1 = UART_OSRVAL_MAX + 1; osrval_p1 > 8; osrval_p1--) {
        /* Initial value of (OSRVAL + 1) * (256 + MULTx) */
        uint32_t m = osrval_p1 * 256;
        for (uint32_t multx_p256 = 256;
             multx_p256 < 256 + UART_MULTX_MAX && best_error != 0 &&
             m / 2 <= target;
             multx_p256++, m += osrval_p1) {
            uint32_t error = (target + m / 2) % m;
            error = abs((int32_t)error - (int32_t)(m / 2));
            if (error < best_error) {
                /* Only in this case we need to do the division as well. */
                uint32_t brgval_p1 = (target + m / 2) / m;
                if (brgval_p1 > (UART_BRGVAL_MAX + 1)) {
                    continue;
                }
                best_osrval = osrval_p1 - 1;
                best_multx = multx_p256 - 256;
                best_brgval = brgval_p1 - 1;
                best_error = error;
            }
        }
        if (best_error == 0) {
            break;
        }
    }

    if (best_osrval == 0) {
        return UART_NOBAUD;
    }

    CLOCK_SetClkDiv(flexcomm_num ? kCLOCK_DivFrg1 : kCLOCK_DivFrg0, best_multx);
    dev->BRG = best_brgval;
    dev->OSR = best_osrval;
    return UART_OK;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    const uart_conf_t *uart_conf = uart_config + uart;
    USART_Type *const dev = uart_conf->dev;

    int flexcomm_num = flexcomm_init((FLEXCOMM_Type *)dev, FLEXCOMM_ID_UART);
    if (flexcomm_num < 0) {
        return UART_INTERR;
    }
    uart_dev_from_flexcomm[flexcomm_num] = uart;

    int ret = _uart_set_baudrate(dev, flexcomm_num, baudrate);
    if (ret != UART_OK) {
        return ret;
    }

    /* remember callback addresses */
    config[uart].rx_cb = rx_cb;
    config[uart].arg = arg;

    /* Interrupt trigger and level for RX and TX disabled by default. */
    dev->FIFOTRIG = 0;

    /* Enable RX side. */
    if (rx_cb != NULL && gpio_is_valid(uart_conf->rx_pin)) {
        /* Trigger RX interrupt when there is at least 1 byte (RXLVL = 0). */
        dev->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK | USART_FIFOTRIG_RXLVL(0);

        /* Enable RX interrupt. */
        dev->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK |
                        USART_FIFOCFG_ENABLERX_MASK;
        dev->FIFOINTENSET = USART_FIFOINTENSET_RXLVL_MASK;
        /* flexcomm_num is the same as the USART instance number in the
         * USART_IRQS array. */
        const uint8_t usart_irqn[] = USART_IRQS;
        NVIC_EnableIRQ(usart_irqn[flexcomm_num]);
    }
    else {
        dev->FIFOCFG &= ~USART_FIFOCFG_ENABLERX_MASK;
        uart_conf->dev->FIFOINTENCLR = USART_FIFOINTENSET_RXLVL_MASK;
    }

    /* Enable TX side. */
    if (gpio_is_valid(uart_conf->tx_pin)) {
        dev->FIFOCFG |= USART_FIFOCFG_EMPTYTX_MASK |
                        USART_FIFOCFG_ENABLETX_MASK;
    }
    else {
        dev->FIFOCFG &= ~USART_FIFOCFG_ENABLETX_MASK;
    }

    /* Configure RX and TX pins. RX/TX pins are always in function 4.
     * GPIO_UNDEF are ignored. */
    gpio_init_mux(uart_conf->rx_pin, 4);
    gpio_init_mux(uart_conf->tx_pin, 4);
    /* This call also enables the UART. */
    return uart_mode(uart, UART_DATA_BITS_8, UART_PARITY_NONE,
                     UART_STOP_BITS_1);
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    if ((data_bits & UART_INVALID_MODE) || (parity & UART_INVALID_MODE)) {
        return UART_NOMODE;
    }
    /* Setup mode and enable USART. The values of the uart_data_bits_t,
     * uart_parity_t and uart_stop_bits_t enums were selected to match the
     * fields in this registers so there's no need to do any conversion. */
    uart_config[uart].dev->CFG = USART_CFG_PARITYSEL(parity)
                                 | USART_CFG_STOPLEN(stop_bits)
                                 | USART_CFG_DATALEN(data_bits)
                                 | USART_CFG_LOOP(0) | USART_CFG_ENABLE_MASK;
    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    USART_Type *dev = uart_config[uart].dev;

    /* If the TX side or the whole uart mode was not enabled during init or at
     * all yet we can only ignore this transmission. This allows DEBUG messages
     * to be ignored without hanging here before the uart is initialized. */
    if (!(dev->FIFOCFG & USART_FIFOCFG_ENABLETX_MASK) ||
        !(dev->CFG & USART_CFG_ENABLE_MASK)) {
        return;
    }

    for (; len; len--) {
        while (!(dev->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK)) {}
        dev->FIFOWR = *(data++);
    }
    /* Wait until we flush out all the bytes. */
    while (!(dev->STAT & USART_STAT_TXIDLE_MASK)) {}
}

void uart_poweron(uart_t uart)
{
    USART_Type *dev = uart_config[uart].dev;

    dev->CFG |= USART_CFG_ENABLE_MASK;
}

void uart_poweroff(uart_t uart)
{
    USART_Type *dev = uart_config[uart].dev;

    while (!(dev->STAT & USART_STAT_TXIDLE_MASK)) {}
    dev->CFG &= ~USART_CFG_ENABLE_MASK;
}

void isr_flexcomm_uart(USART_Type *dev, uint32_t flexcomm_num)
{
    uart_t uart = uart_dev_from_flexcomm[flexcomm_num];

    while (dev->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK) {
        /* Reading from FIFORD may clear the FIFOSTAT RXNOTEMPTY if we read all
         * the bytes. */
        uint8_t data = dev->FIFORD;
        if (uart < UART_NUMOF && config[uart].rx_cb != NULL) {
            config[uart].rx_cb(config[uart].arg, data);
        }
    }

    if (dev->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK) {
        /* This is a USART FIFO RX overrun.
         * Note: writing a 1 to the FIFOSTAT flag clears it. */
        dev->FIFOSTAT = USART_FIFOSTAT_RXERR_MASK;
        /* TODO: Signal an error to the application. */
    }

    cortexm_isr_end();
}
