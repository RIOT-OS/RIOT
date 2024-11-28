/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 FreshTemp, LLC.
 *               2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dylan Laduranty <dylanladuranty@gmail.com>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include "bitfield.h"
#include "modules.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined (CPU_COMMON_SAML1X) || defined (CPU_COMMON_SAMD5X)
#define UART_HAS_TX_ISR
#endif

/* default to fractional baud rate calculation */
#if !defined(CONFIG_SAM0_UART_BAUD_FRAC) && defined(SERCOM_USART_BAUD_FRAC_BAUD)
/* SAML21 has no fractional baud rate on SERCOM5 */
#if defined(CPU_SAML21)
#define CONFIG_SAM0_UART_BAUD_FRAC  0
#else
#define CONFIG_SAM0_UART_BAUD_FRAC  1
#endif
#endif

/* SAMD20 defines no generic macro */
#ifdef SERCOM_USART_CTRLA_TXPO_PAD0
#undef SERCOM_USART_CTRLA_TXPO
#define SERCOM_USART_CTRLA_TXPO(n) ((n) << SERCOM_USART_CTRLA_TXPO_Pos)
#endif

/**
 * @brief   Allocate memory to store the callback functions & buffers
 */
#ifdef MODULE_PERIPH_UART_NONBLOCKING
#include "tsrb.h"
static tsrb_t uart_tx_rb[UART_NUMOF];
static uint8_t uart_tx_rb_buf[UART_NUMOF][UART_TXBUF_SIZE];
#endif
static uart_isr_ctx_t uart_ctx[UART_NUMOF];
static uint32_t symbol_rates[UART_NUMOF];
static BITFIELD(_enabled, UART_NUMOF) = { 0 };

static void uart_isr(void *arg);

static bool _is_txondemand(uart_t uart)
{
    if (!IS_USED(MODULE_PERIPH_UART_TX_ONDEMAND)) {
        return false;
    }

    return uart_config[uart].flags & UART_FLAG_TX_ONDEMAND;
}

gpio_t uart_pin_cts(uart_t uart)
{
#ifdef MODULE_PERIPH_UART_HW_FC
    if (uart_config[uart].tx_pad == UART_PAD_TX_0_RTS_2_CTS_3) {
        return uart_config[uart].cts_pin;
    }
#endif
    (void)uart;
    return GPIO_UNDEF;
}

gpio_t uart_pin_rts(uart_t uart)
{
#ifdef MODULE_PERIPH_UART_HW_FC
    if (uart_config[uart].tx_pad == UART_PAD_TX_0_RTS_2_CTS_3) {
        return uart_config[uart].rts_pin;
    }
#endif
    (void)uart;
    return GPIO_UNDEF;
}

/**
 * @brief   Get the pointer to the base register of the given UART
 *
 * @param[in] uart      UART interface number
 *
 * @return              base register address
 */
static SercomUsart *_dev(uart_t uart)
{
    return uart_config[uart].dev;
}

MAYBE_UNUSED
static void _syncbusy(SercomUsart *dev)
{
#ifdef SERCOM_USART_SYNCBUSY_MASK
    while (dev->SYNCBUSY.reg) {}
#else
    while (dev->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY) {}
#endif
}

static uint16_t _calculate_baud(uint32_t baudrate, uint32_t f_src)
{
#if IS_ACTIVE(CONFIG_SAM0_UART_BAUD_FRAC)
    /* Asynchronous Fractional */
    /* BAUD + FP / 8 = f_src / (S * f_baud)       */
    /* BAUD * 8 + FP = (8 * f_src) / (S * f_baud) */
    /* S * (BAUD + 8 * FP) = (8 * f_src) / f_baud */
    uint32_t baud = (f_src * 8) / baudrate;
    SERCOM_USART_BAUD_Type baudreg = { .FRAC = {
        .FP = (baud >> 4) & 0x7, /* baud / 16 */
        .BAUD = baud >> 7, /* baud / (8 * 16) */
    } };
    return baudreg.reg;
#else
    /* Asynchronous Arithmetic */
    /* BAUD = 2^16     * (2^0 - 2^4 * f_baud / f_src)     */
    /*      = 2^(16-n) * (2^n - 2^(n+4) * f_baud / f_src) */
    /*      = 2^(20-n) * (2^(n-4) - 2^n * f_baud / f_src) */

    /* 2^n * f_baud < 2^32 -> find the next power of 2 */
    uint8_t pow = __builtin_clz(baudrate);

    /* 2^n * f_baud */
    baudrate <<= pow;

    /* (2^(n-4) - 2^n * f_baud / f_src) */
    uint32_t tmp = (1 << (pow - 4)) - baudrate / f_src;
    uint32_t rem = baudrate % f_src;

    uint8_t scale = 20 - pow;
    return (tmp << scale) - (rem << scale) / f_src;
#endif
}

static void _attach_pins(uart_t uart)
{
    if (gpio_is_valid(uart_config[uart].rx_pin)) {
        gpio_init_mux(uart_config[uart].rx_pin, uart_config[uart].mux);
    }

    if (gpio_is_valid(uart_config[uart].tx_pin) &&
            !_is_txondemand(uart)) {
        gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);
    }

    /* If RTS/CTS needed, enable them */
    /* Ensure RTS is defined */
    if (gpio_is_valid(uart_pin_rts(uart))) {
        gpio_init_mux(uart_pin_rts(uart), uart_config[uart].mux);
    }
    /* Ensure CTS is defined */
    if (gpio_is_valid(uart_pin_cts(uart))) {
        gpio_init_mux(uart_pin_cts(uart), uart_config[uart].mux);
    }
}

static void _detach_pins(uart_t uart)
{
    if (gpio_is_valid(uart_config[uart].rx_pin)) {
        gpio_disable_mux(uart_config[uart].rx_pin);
    }

    if (gpio_is_valid(uart_config[uart].tx_pin)
            && !_is_txondemand(uart)) {
        gpio_disable_mux(uart_config[uart].tx_pin);
    }

    /* If RTS/CTS needed, enable them */
    /* Ensure RTS is defined */
    if (gpio_is_valid(uart_pin_rts(uart))) {
        gpio_disable_mux(uart_pin_rts(uart));
    }
    /* Ensure CTS is defined */
    if (gpio_is_valid(uart_pin_cts(uart))) {
        gpio_disable_mux(uart_pin_cts(uart));
    }
}

void uart_enable_tx(uart_t uart)
{
    if (gpio_is_valid(uart_config[uart].tx_pin)) {
        gpio_init_mux(uart_config[uart].tx_pin, uart_config[uart].mux);
    }
}

void uart_disable_tx(uart_t uart)
{
    if (gpio_is_valid(uart_config[uart].tx_pin)) {
        gpio_disable_mux(uart_config[uart].tx_pin);
    }
}

static void _configure_pins(uart_t uart)
{
    /* configure RX pin */
    if (gpio_is_valid(uart_config[uart].rx_pin)) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN);
    }

    /* configure TX pin */
    if (gpio_is_valid(uart_config[uart].tx_pin)) {
        gpio_set(uart_config[uart].tx_pin);
        gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    }

    /* If RTS/CTS needed, enable them */
    /* Ensure RTS is defined */
    if (gpio_is_valid(uart_pin_rts(uart))) {
        gpio_init(uart_pin_rts(uart), GPIO_OUT);
    }
    /* Ensure CTS is defined */
    if (gpio_is_valid(uart_pin_cts(uart))) {
        gpio_init(uart_pin_cts(uart), GPIO_IN);
    }
}

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg)
{
    DEBUG("[uart] init %u\n", (unsigned)uart);
    /* if UART is still on, we disable it first */
    if (bf_isset(_enabled, uart)) {
        uart_poweroff(uart);
    }

    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    symbol_rates[uart] = baud;
    uart_ctx[uart] = (uart_isr_ctx_t){ .rx_cb = rx_cb, .arg = arg };

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    /* set up the TX buffer */
    tsrb_init(&uart_tx_rb[uart], uart_tx_rb_buf[uart], UART_TXBUF_SIZE);
#endif

    /* configure pins */
    _configure_pins(uart);

    uart_poweron(uart);

    return UART_OK;
}

void uart_init_pins(uart_t uart)
{
    _configure_pins(uart);

    uart_poweron(uart);
}

void uart_deinit_pins(uart_t uart)
{
    uart_poweroff(uart);

    /* de-configure RX pin */
    if (uart_config[uart].rx_pin != GPIO_UNDEF) {
        gpio_disable_mux(uart_config[uart].rx_pin);
    }

    /* de-configure TX pin */
    if (uart_config[uart].tx_pin != GPIO_UNDEF) {
        gpio_disable_mux(uart_config[uart].tx_pin);
    }

    /* If RTS/CTS needed, enable them */
    /* Ensure RTS is defined */
    if (gpio_is_valid(uart_pin_rts(uart))) {
        gpio_disable_mux(uart_pin_rts(uart));
    }
    /* Ensure CTS is defined */
    if (gpio_is_valid(uart_pin_cts(uart))) {
        gpio_disable_mux(uart_pin_cts(uart));
    }
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if (!gpio_is_valid(uart_config[uart].tx_pin)) {
        return;
    }

    if (!(_dev(uart)->CTRLA.reg & SERCOM_USART_CTRLA_ENABLE)) {
        return;
    }

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    for (const void* end = data + len; data != end; ++data) {
        if (irq_is_in() || __get_PRIMASK()) {
            /* if ring buffer is full free up a spot */
            if (tsrb_full(&uart_tx_rb[uart])) {
                while (!(_dev(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {}
                _dev(uart)->DATA.reg = tsrb_get_one(&uart_tx_rb[uart]);
            }
            tsrb_add_one(&uart_tx_rb[uart], *data);
        }
        else {
            while (tsrb_add_one(&uart_tx_rb[uart], *data) < 0) {}
        }
        /* check and enable DRE IRQs atomically */
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_UART_PM_BLOCK)
        unsigned state = irq_disable();
        /* tsrb_add_one() is blocking the thread. It may happen that
         * the corresponding ISR has turned off DRE IRQs and, thus,
         * unblocked the corresponding power mode. */
        if (!(_dev(uart)->INTENSET.reg & SERCOM_USART_INTENSET_DRE)) {
            pm_block(SAM0_UART_PM_BLOCK);
        }
        _dev(uart)->INTENSET.reg = SERCOM_USART_INTENSET_DRE;
        irq_restore(state);
#else
        _dev(uart)->INTENSET.reg = SERCOM_USART_INTENSET_DRE;
#endif
    }
#else
    for (const void* end = data + len; data != end; ++data) {
        while (!(_dev(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)) {}
        _dev(uart)->DATA.reg = *data;
    }
    while (!(_dev(uart)->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)) {}
#endif
}

void uart_poweron(uart_t uart)
{
    sercom_t sercom = sercom_id(uart_config[uart].dev);
    DEBUG("[uart] powering on #%u on SERCOM%u\n", (unsigned)uart, (unsigned)sercom);
    if (bf_isset(_enabled, uart)) {
        /* already on, nothing to do */
        return;
    }

    bf_set_atomic(_enabled, uart);

    uint32_t f_src = sam0_gclk_freq(uart_config[uart].gclk_src);

#if IS_ACTIVE(CONFIG_SAM0_UART_BAUD_FRAC)
    uint32_t sampr;
    /* constraint: f_baud ≤ f_src / S */
    if (symbol_rates[uart] * 16 > f_src) {
        /* 8x oversampling */
        sampr = SERCOM_USART_CTRLA_SAMPR(0x3);
        f_src <<= 1;
    }
    else {
        /* 16x oversampling */
        sampr = SERCOM_USART_CTRLA_SAMPR(0x1);
    }
#endif

    /* set asynchronous mode w/o parity, LSB first, TX and RX pad as specified
     * by the board in the periph_conf.h, x16 sampling and use internal clock */
    uint32_t ctrla = SERCOM_USART_CTRLA_DORD
#if IS_ACTIVE(CONFIG_SAM0_UART_BAUD_FRAC)
    /* enable Asynchronous Fractional mode */
               | sampr
#endif
               | SERCOM_USART_CTRLA_TXPO(uart_config[uart].tx_pad)
               | SERCOM_USART_CTRLA_RXPO(uart_config[uart].rx_pad)
               | SERCOM_USART_CTRLA_MODE(0x1);

    /* Set run in standby mode if enabled */
    if (uart_config[uart].flags & UART_FLAG_RUN_STANDBY) {
        ctrla |= SERCOM_USART_CTRLA_RUNSTDBY;
    }

    /* calculate symbol rate */
    uint16_t baud = _calculate_baud(symbol_rates[uart], f_src);

    uint32_t ctrlb = 0;
    /* enable transmitter, and configure 8N1 mode */
    if (gpio_is_valid(uart_config[uart].tx_pin)) {
        ctrlb = SERCOM_USART_CTRLB_TXEN;
    }

    sercom_irq_cb_t irq_cb = NULL;

    /* enable receiver and RX interrupt if configured */
    if ((uart_ctx[uart].rx_cb) && (gpio_is_valid(uart_config[uart].rx_pin))) {
        irq_cb = uart_isr;
        ctrlb |= SERCOM_USART_CTRLB_RXEN;

        /* set wakeup receive from sleep if enabled */
        if (uart_config[uart].flags & UART_FLAG_WAKEUP) {
            ctrlb |= SERCOM_USART_CTRLB_SFDE;
        }
    }

    if (IS_USED(MODULE_PERIPH_UART_NONBLOCKING)) {
        irq_cb = uart_isr;
    }

    sercom_acquire(sercom, uart_config[uart].gclk_src, irq_cb, (void *)(uintptr_t)uart);

    _attach_pins(uart);

    SercomUsart *dev = _dev(uart);
    dev->CTRLA.reg = ctrla;
    dev->CTRLB.reg = ctrlb;
    dev->BAUD.reg = baud;
    /* enable RX IRQ, if needed */
    if ((uart_ctx[uart].rx_cb) && (gpio_is_valid(uart_config[uart].rx_pin))) {
        dev->INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    }

    /* and finally enable the device and the IRQs as needed */
    dev->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    _syncbusy(dev);
}

void uart_poweroff(uart_t uart)
{
    sercom_t sercom = sercom_id(uart_config[uart].dev);
    DEBUG("[uart] powering off #%u on SERCOM%u\n", (unsigned)uart, (unsigned)sercom);
    /* If we would release a SERCOM that we do not own, we could e.g. disable
     * the SERCOM in the middle of an SPI or I2C transfer */
    if (!bf_isset(_enabled, uart)) {
        return;
    }

    bf_unset_atomic(_enabled, uart);

    _detach_pins(uart);
    sercom_release(sercom);
}

#ifdef MODULE_PERIPH_UART_COLLISION
bool uart_collision_detected(uart_t uart)
{
    /* In case of collision, the CTRLB register
     * will be in sync during disabling of TX,
     * then the flag will be set.
     */
    _syncbusy(_dev(uart));

    bool collision = _dev(uart)->STATUS.reg & SERCOM_USART_STATUS_COLL;
    _dev(uart)->STATUS.reg = SERCOM_USART_STATUS_COLL;
    return collision;
}

void uart_collision_detect_enable(uart_t uart)
{
    /* CTRLB is enable protected */
    _dev(uart)->CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
    _syncbusy(_dev(uart));

    /* clear stale collision flag */
    _dev(uart)->STATUS.reg = SERCOM_USART_STATUS_COLL;

    /* enable collision detection */
    _dev(uart)->CTRLB.reg |= SERCOM_USART_CTRLB_COLDEN;

    /* disable RX interrupt */
    _dev(uart)->INTENCLR.reg = SERCOM_USART_INTENCLR_RXC;

    /* re-enable UART */
    _dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;

    /* wait for config to be applied */
    _syncbusy(_dev(uart));
}

static void _drain_rxbuf(SercomUsart *dev)
{
    /* clear readback bytes from receive buffer */
    while (dev->INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) {
        dev->DATA.reg;
    }
}

void uart_collision_detect_disable(uart_t uart)
{
    uint32_t ctrlb = _dev(uart)->CTRLB.reg;

    /* re-enable TX after collision */
    ctrlb |= SERCOM_USART_CTRLB_TXEN;

    /* disable collision detection */
    ctrlb &= ~SERCOM_USART_CTRLB_COLDEN;

    /* CTRLB is enable protected */
    _dev(uart)->CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
    _syncbusy(_dev(uart));

    _dev(uart)->CTRLB.reg = ctrlb;

    /* re-enable UART */
    _dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;

    /* wait for config to be applied */
    _syncbusy(_dev(uart));

    /* clear bytes from RX buffer */
    _drain_rxbuf(_dev(uart));

    /* re-enable RX complete IRQ */
    if (uart_ctx[uart].rx_cb) {
        _dev(uart)->INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    }
}
#endif

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    if (uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    if (stop_bits != UART_STOP_BITS_1 && stop_bits != UART_STOP_BITS_2) {
        return UART_NOMODE;
    }

    if (parity != UART_PARITY_NONE && parity != UART_PARITY_EVEN &&
            parity != UART_PARITY_ODD) {
        return UART_NOMODE;
    }

    /* Disable UART first to remove write protect */
    _dev(uart)->CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;
    _syncbusy(_dev(uart));

    uint32_t ctrlb = _dev(uart)->CTRLB.reg;

    if (parity == UART_PARITY_NONE) {
        _dev(uart)->CTRLA.reg &= ~SERCOM_USART_CTRLA_FORM_Msk;
    }
    else {
        _dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_FORM(1);
        if (parity == UART_PARITY_ODD) {
            ctrlb |= SERCOM_USART_CTRLB_PMODE;
        }
        else {
            ctrlb &= ~SERCOM_USART_CTRLB_PMODE;
        }
    }

    if (stop_bits == UART_STOP_BITS_1) {
        ctrlb &= ~SERCOM_USART_CTRLB_SBMODE;
    }
    else {
        ctrlb |= SERCOM_USART_CTRLB_SBMODE;
    }

    _dev(uart)->CTRLB.reg = ((ctrlb & ~SERCOM_USART_CTRLB_CHSIZE_Msk) |
                            SERCOM_USART_CTRLB_CHSIZE(data_bits));

    /* Enable UART again */
    _dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    _syncbusy(_dev(uart));

    return UART_OK;
}
#endif /* MODULE_PERIPH_UART_MODECFG */

#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
void uart_rxstart_irq_configure(uart_t uart, uart_rxstart_cb_t cb, void *arg)
{
    /* CTRLB is enable-proteced */
    _dev(uart)->CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE;

    /* set start of frame detection enable */
    _dev(uart)->CTRLB.reg |= SERCOM_USART_CTRLB_SFDE;

    uart_ctx[uart].rxs_cb  = cb;
    uart_ctx[uart].rxs_arg = arg;

    /* enable UART again */
    _dev(uart)->CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

void uart_rxstart_irq_enable(uart_t uart)
{
    /* clear stale interrupt flag */
    _dev(uart)->INTFLAG.reg  = SERCOM_USART_INTFLAG_RXS;

    /* enable interrupt */
    _dev(uart)->INTENSET.reg = SERCOM_USART_INTENSET_RXS;
}

void uart_rxstart_irq_disable(uart_t uart)
{
    _dev(uart)->INTENCLR.reg = SERCOM_USART_INTENCLR_RXS;
}
#endif /* MODULE_PERIPH_UART_RXSTART_IRQ */

#ifdef MODULE_PERIPH_UART_NONBLOCKING
static void irq_handler_tx(unsigned uartnum)
{
    /* workaround for saml1x */
    int c = tsrb_get_one(&uart_tx_rb[uartnum]);
    if (c >= 0) {
        _dev(uartnum)->DATA.reg = c;
    }

    /* disable the interrupt if there are no more bytes to send */
    if (tsrb_empty(&uart_tx_rb[uartnum])) {
        _dev(uartnum)->INTENCLR.reg = SERCOM_USART_INTENSET_DRE;
#if IS_ACTIVE(MODULE_PM_LAYERED) && defined(SAM0_UART_PM_BLOCK)
        /* we really should be in IRQ context! */
        assert(irq_is_in());
        pm_unblock(SAM0_UART_PM_BLOCK);
#endif
    }
}
#endif

static void uart_isr(void *arg)
{
    uart_t uart = (uintptr_t)arg;
    SercomUsart *dev = _dev(uart);
    uint32_t status = dev->INTFLAG.reg;
    /* TXC is used by uart_write() */
    dev->INTFLAG.reg = status & ~SERCOM_USART_INTFLAG_TXC;

#if MODULE_PERIPH_UART_NONBLOCKING
    if ((status & SERCOM_USART_INTFLAG_DRE) &&
        (dev->INTENSET.reg & SERCOM_USART_INTENSET_DRE)) {
        irq_handler_tx(uart);
    }
#endif

#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
    if ((status & SERCOM_USART_INTFLAG_RXS) &&
        (dev->INTENSET.reg & SERCOM_USART_INTENSET_RXS)) {
        uart_ctx[uart].rxs_cb(uart_ctx[uart].rxs_arg);
    }
#endif

    if (status & SERCOM_USART_INTFLAG_RXC) {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uart].rx_cb(uart_ctx[uart].arg, dev->DATA.reg);
    }

}
