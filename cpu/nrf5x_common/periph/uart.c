/*
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Jan Wagner <mail@jwagner.eu>
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral UART interface
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "compiler_hints.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/uart.h"

#ifdef UARTE_PRESENT
#  define PSEL_RXD          PSEL.RXD
#  define PSEL_TXD          PSEL.TXD
#  define PSEL_RTS          PSEL.RTS
#  define PSEL_CTS          PSEL.CTS
#  define ENABLE_ON         UARTE_ENABLE_ENABLE_Enabled
#  define ENABLE_OFF        UARTE_ENABLE_ENABLE_Disabled
#  define UART_TYPE         NRF_UARTE_Type
#else
#  define PSEL_RXD          PSELRXD
#  define PSEL_TXD          PSELTXD
#  define PSEL_RTS          PSELRTS
#  define PSEL_CTS          PSELCTS
#  define ENABLE_ON         UART_ENABLE_ENABLE_Enabled
#  define ENABLE_OFF        UART_ENABLE_ENABLE_Disabled
#  define UART_TYPE         NRF_UART_Type
#endif

/* Unify the HW flow control config mask name across nRF51 (UART) and nRF52 and
 * newer (UARTE). The nRF54L only provides the UARTE_ prefixed variant. */
#if defined(UARTE_CONFIG_HWFC_Msk)
#  define CONFIG_HWFC_Msk UARTE_CONFIG_HWFC_Msk
#elif defined(UART_CONFIG_HWFC_Msk)
#  define CONFIG_HWFC_Msk UART_CONFIG_HWFC_Msk
#endif

/* The baudrate enumeration uses the UARTE_ prefix on families that have an
 * EasyDMA UARTE (nRF52 and newer) and the UART_ prefix on the nRF51, which has
 * no UARTE peripheral. */
#ifdef UARTE_PRESENT
#  define BAUD(x)           UARTE_BAUDRATE_BAUDRATE_Baud ## x
#else
#  define BAUD(x)           UART_BAUDRATE_BAUDRATE_Baud ## x
#endif

/* The nRF54L moved the EasyDMA tasks of the (UART)E peripheral into dedicated
 * DMA register groups. The RX/TX start and stop tasks also exist on the older
 * UART/UARTE peripherals, so these accessors are defined for all families. */
#ifdef CPU_FAM_NRF54L
#  define TASKS_RXSTART(d)  (d)->TASKS_DMA.RX.START
#  define TASKS_RXSTOP(d)   (d)->TASKS_DMA.RX.STOP
#  define TASKS_TXSTART(d)  (d)->TASKS_DMA.TX.START
#  define TASKS_TXSTOP(d)   (d)->TASKS_DMA.TX.STOP
#else
#  define TASKS_RXSTART(d)  (d)->TASKS_STARTRX
#  define TASKS_RXSTOP(d)   (d)->TASKS_STOPRX
#  define TASKS_TXSTART(d)  (d)->TASKS_STARTTX
#  define TASKS_TXSTOP(d)   (d)->TASKS_STOPTX
#endif

/* EasyDMA buffer registers, events, interrupt and short masks. On the nRF54L
 * these moved into DMA register groups, the older UARTE families keep the flat
 * RXD/TXD layout. */
#ifdef UARTE_PRESENT
#  ifdef CPU_FAM_NRF54L
#    define RXD_PTR(d)              (d)->DMA.RX.PTR
#    define RXD_MAXCNT(d)           (d)->DMA.RX.MAXCNT
#    define RXD_AMOUNT(d)           (d)->DMA.RX.AMOUNT
#    define TXD_PTR(d)              (d)->DMA.TX.PTR
#    define TXD_MAXCNT(d)           (d)->DMA.TX.MAXCNT
#    define EVENTS_RXEND(d)         (d)->EVENTS_DMA.RX.END
#    define EVENTS_TXEND(d)         (d)->EVENTS_DMA.TX.END
#    define INTEN_RXEND_Msk         UARTE_INTENSET_DMARXEND_Msk
#    define INTEN_TXEND_Msk         UARTE_INTENSET_DMATXEND_Msk
#    define SHORT_RXEND_RXSTART_Msk UARTE_SHORTS_DMA_RX_END_DMA_RX_START_Msk
#  else
#    define RXD_PTR(d)              (d)->RXD.PTR
#    define RXD_MAXCNT(d)           (d)->RXD.MAXCNT
#    define RXD_AMOUNT(d)           (d)->RXD.AMOUNT
#    define TXD_PTR(d)              (d)->TXD.PTR
#    define TXD_MAXCNT(d)           (d)->TXD.MAXCNT
#    define EVENTS_RXEND(d)         (d)->EVENTS_ENDRX
#    define EVENTS_TXEND(d)         (d)->EVENTS_ENDTX
#    define INTEN_RXEND_Msk         UARTE_INTENSET_ENDRX_Msk
#    define INTEN_TXEND_Msk         UARTE_INTENSET_ENDTX_Msk
#    define SHORT_RXEND_RXSTART_Msk UARTE_SHORTS_ENDRX_STARTRX_Msk
#  endif
#endif

/* The nRF54L UARTE has no TXSTARTED event, which the non-blocking TX path
 * relies on, so non-blocking mode is only supported on the older families. */
#if defined(MODULE_PERIPH_UART_NONBLOCKING) && !defined(CPU_FAM_NRF54L)
#  define UART_NONBLOCKING   (1)
#else
#  define UART_NONBLOCKING   (0)
#endif

/**
 * @brief Chunk size used for transferring data from ROM [in bytes]
 */
#ifndef NRF_UARTE_CHUNK_SIZE
#define NRF_UARTE_CHUNK_SIZE    (32U)
#endif

/**
 * @brief Allocate memory for the interrupt context
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];
#ifdef UARTE_PRESENT
static uint8_t rx_buf[UART_NUMOF];
#endif

#if UART_NONBLOCKING

#  include "tsrb.h"
/**
 * @brief   Allocate for tx ring buffers
 */
static uint8_t tx_buf[UART_NUMOF];
static tsrb_t uart_tx_rb[UART_NUMOF];
static uint8_t uart_tx_rb_buf[UART_NUMOF][UART_TXBUF_SIZE];
#endif

/**
 * @brief Shared IRQ Callback for UART on nRF53/nRF9160
*/
void uart_isr_handler(void *arg);

/* use an enum to count the number of UART ISR macro names defined by the
 * board */
enum {
#ifdef UART_0_ISR
    UART_0_ISR_NUM,
#endif
#ifdef UART_1_ISR
    UART_1_ISR_NUM,
#endif
    UART_ISR_NUMOF,
};

static inline void set_power(uart_t uart, bool value)
{
    UART_TYPE *dev = uart_config[uart].dev;

    if (value) {
        dev->ENABLE = ENABLE_ON;
    }
    else {
        dev->ENABLE = ENABLE_OFF;
    }

#ifndef UARTE_PRESENT
    dev->POWER = value;
#endif
}

static inline bool get_power(uart_t uart)
{
    UART_TYPE *dev = uart_config[uart].dev;
    return dev->ENABLE != ENABLE_OFF;
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
/* ensure the ISR names have been defined as needed */
#if !defined(CPU_NRF53) && !defined(CPU_NRF9160)
    static_assert(UART_NUMOF == UART_ISR_NUMOF, "Define(s) of UART ISR name(s) missing");
#endif
    if ((unsigned)uart >= UART_NUMOF) {
        return UART_NODEV;
    }

    UART_TYPE *dev = uart_config[uart].dev;

    /* remember callback addresses and argument */
    isr_ctx[uart].rx_cb = rx_cb;
    isr_ctx[uart].arg = arg;

#ifdef UARTE_CONFIG_FRAMESIZE_Msk
    /* the nRF54L UARTE must be disabled to be reconfigured and has a mandatory
     * FRAMESIZE field that has to be kept at 8 bit (no parity, 1 stop bit) */
    dev->ENABLE = ENABLE_OFF;
    dev->CONFIG = (UARTE_CONFIG_FRAMESIZE_8bit << UARTE_CONFIG_FRAMESIZE_Pos);
#else
    /* reset configuration registers */
    dev->CONFIG = 0;
#endif

    /* configure RX pin */
    if (rx_cb) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN);
        dev->PSEL_RXD = uart_config[uart].rx_pin;
    }

    /* configure TX pin */
    gpio_init(uart_config[uart].tx_pin, GPIO_OUT);
    dev->PSEL_TXD = uart_config[uart].tx_pin;

    /* enable HW-flow control if defined */
 #ifdef MODULE_PERIPH_UART_HW_FC
    /* set pin mode for RTS and CTS pins */
    if (uart_config[uart].rts_pin != GPIO_UNDEF && uart_config[uart].cts_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].rts_pin, GPIO_OUT);
        gpio_init(uart_config[uart].cts_pin, GPIO_IN);
        /* configure RTS and CTS pins to use */
        dev->PSEL_RTS = uart_config[uart].rts_pin;
        dev->PSEL_CTS = uart_config[uart].cts_pin;
        dev->CONFIG |= CONFIG_HWFC_Msk; /* enable HW flow control */
    }
    else
#endif
    {
        dev->PSEL_RTS = 0xffffffff;   /* pin disconnected */
        dev->PSEL_CTS = 0xffffffff;   /* pin disconnected */
    }

    /* select baudrate */
    switch (baudrate) {
    case 1200:
        dev->BAUDRATE = BAUD(1200);
        break;
    case 2400:
        dev->BAUDRATE = BAUD(2400);
        break;
    case 4800:
        dev->BAUDRATE = BAUD(4800);
        break;
    case 9600:
        dev->BAUDRATE = BAUD(9600);
        break;
    case 14400:
        dev->BAUDRATE = BAUD(14400);
        break;
    case 19200:
        dev->BAUDRATE = BAUD(19200);
        break;
    case 28800:
        dev->BAUDRATE = BAUD(28800);
        break;
    case 38400:
        dev->BAUDRATE = BAUD(38400);
        break;
    case 57600:
        dev->BAUDRATE = BAUD(57600);
        break;
    case 76800:
        dev->BAUDRATE = BAUD(76800);
        break;
    case 115200:
        dev->BAUDRATE = BAUD(115200);
        break;
    case 230400:
        dev->BAUDRATE = BAUD(230400);
        break;
    case 250000:
        dev->BAUDRATE = BAUD(250000);
        break;
    case 460800:
        dev->BAUDRATE = BAUD(460800);
        break;
    case 921600:
        dev->BAUDRATE = BAUD(921600);
        break;
    case 1000000:
        dev->BAUDRATE = BAUD(1M);
        break;
    default:
        return UART_NOBAUD;
    }

    /* enable the UART device */
    set_power(uart, true);

#if UART_NONBLOCKING
    /* set up the TX buffer */
    tsrb_init(&uart_tx_rb[uart], uart_tx_rb_buf[uart], UART_TXBUF_SIZE);
#endif

    if (rx_cb) {
#ifdef UARTE_PRESENT
        RXD_MAXCNT(dev) = 1;
        RXD_PTR(dev) = (uint32_t)&rx_buf[uart];
        EVENTS_RXEND(dev) = 0;
        dev->INTENSET = INTEN_RXEND_Msk;
        dev->SHORTS |= SHORT_RXEND_RXSTART_Msk;
        TASKS_RXSTART(dev) = 1;
#else
        dev->INTENSET = UART_INTENSET_RXDRDY_Msk;
        TASKS_RXSTART(dev) = 1;
#endif
    }

    if (rx_cb || UART_NONBLOCKING) {
#if  defined(CPU_NRF53) || defined(CPU_NRF9160)
        shared_irq_register_uart(dev, uart_isr_handler, (void *)(uintptr_t)uart);
#else
        NVIC_EnableIRQ(uart_config[uart].irqn);
#endif
    }
    return UART_OK;
}

void uart_poweron(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    set_power(uart, true);
    if (isr_ctx[uart].rx_cb) {
        TASKS_RXSTART(uart_config[uart].dev) = 1;
    }
}

void uart_poweroff(uart_t uart)
{
    assume((unsigned)uart < UART_NUMOF);

    TASKS_RXSTOP(uart_config[uart].dev) = 1;
    set_power(uart, false);
}

/* Unify macro names across nRF51 (UART) and nRF52 and newer (UARTE) */
#if defined(UARTE_CONFIG_PARITY_Msk)
#  define CONFIG_PARITY_Msk UARTE_CONFIG_PARITY_Msk
#elif defined(UART_CONFIG_PARITY_Msk)
#  define CONFIG_PARITY_Msk UART_CONFIG_PARITY_Msk
#endif

#if defined(UARTE_CONFIG_STOP_Msk)
#  define CONFIG_STOP_Msk UARTE_CONFIG_STOP_Msk
#elif defined(UART_CONFIG_STOP_Msk)
#  define CONFIG_STOP_Msk UART_CONFIG_STOP_Msk
#endif

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assume((unsigned)uart < UART_NUMOF);
    /* Not all nRF5x MCUs support 2 stop bits, but the vendor header files
     * reflect the feature set. */
    switch (stop_bits) {
    case UART_STOP_BITS_1:
#ifdef CONFIG_STOP_Msk
    case UART_STOP_BITS_2:
#endif
        break;
    default:
        return UART_NOMODE;
    }

    if (data_bits != UART_DATA_BITS_8) {
        return UART_NOMODE;
    }

    if ((parity != UART_PARITY_NONE) && (parity != UART_PARITY_EVEN)) {
        return UART_NOMODE;
    }

    /* Do not modify hardware flow control (nor the mandatory frame size on
     * families that expose it, e.g. the nRF54L) */
    uint32_t keep = CONFIG_HWFC_Msk;
#ifdef UARTE_CONFIG_FRAMESIZE_Msk
    keep |= UARTE_CONFIG_FRAMESIZE_Msk;
#endif
    uint32_t conf = uart_config[uart].dev->CONFIG & keep;

#ifdef CONFIG_STOP_Msk
    if (stop_bits == UART_STOP_BITS_2) {
        conf |= UARTE_CONFIG_STOP_Msk;
    }
#endif

    if (parity == UART_PARITY_EVEN) {
        conf |= CONFIG_PARITY_Msk;
    }

    uart_config[uart].dev->CONFIG = conf;
    return UART_OK;
}

/* UART with EasyDMA */
#ifdef UARTE_PRESENT
static void _write_buf(uart_t uart, const uint8_t *data, size_t len)
{
    UART_TYPE *dev = uart_config[uart].dev;

    EVENTS_TXEND(dev) = 0;
    if (UART_NONBLOCKING) {
        dev->INTENSET = INTEN_TXEND_Msk;
    }
    /* set data to transfer to DMA TX pointer */
    TXD_PTR(dev) = (uint32_t)data;
    TXD_MAXCNT(dev) = len;
    /* start transmission */
    TASKS_TXSTART(dev) = 1;
    /* wait for the end of transmission */
    if (!UART_NONBLOCKING) {
        while (EVENTS_TXEND(dev) == 0) {}
        TASKS_TXSTOP(dev) = 1;
    }
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assume((unsigned)uart < UART_NUMOF);
    if (!get_power(uart)) {
        /* Device is powered down. Writing anyway would deadlock */
        return;
    }
#if UART_NONBLOCKING
    for (size_t i = 0; i < len; i++) {
        /* in IRQ or interrupts disabled */
        if (irq_is_in() || __get_PRIMASK()) {
            if (tsrb_full(&uart_tx_rb[uart])) {
                /* wait for end of ongoing transmission */
                if (uart_config[uart].dev->EVENTS_TXSTARTED) {
                    while (uart_config[uart].dev->EVENTS_ENDTX == 0) {}
                    uart_config[uart].dev->EVENTS_TXSTARTED = 0;
                }
                /* free one spot in buffer */
                tx_buf[uart] = tsrb_get_one(&uart_tx_rb[uart]);
                _write_buf(uart, &tx_buf[uart], 1);
            }
            tsrb_add_one(&uart_tx_rb[uart], data[i]);
        }
        else {
            /* if no transmission is ongoing and ring buffer is full
               free up a spot in the buffer by sending one byte */
            if (!uart_config[uart].dev->EVENTS_TXSTARTED && tsrb_full(&uart_tx_rb[uart])) {
                tx_buf[uart] = tsrb_get_one(&uart_tx_rb[uart]);
                _write_buf(uart, &tx_buf[uart], 1);
            }
            while (tsrb_add_one(&uart_tx_rb[uart], data[i]) < 0) {}
        }
    }
    /* if no transmission is ongoing bootstrap the transmission process
       by setting a single byte to be written */
    if (!uart_config[uart].dev->EVENTS_TXSTARTED) {
        if (!tsrb_empty(&uart_tx_rb[uart])) {
            tx_buf[uart] = tsrb_get_one(&uart_tx_rb[uart]);
            _write_buf(uart, &tx_buf[uart], 1);
        }
    }
#else
    /* EasyDMA can only transfer data from RAM (see ref. manual, sec. 6.34.1).
     * So if the given `data` buffer resides in ROM, we need to copy it to RAM
     * before being able to transfer it. To make sure the stack does not
     * overflow, we do this chunk-wise. */
    if (!((uint32_t)data & RAM_MASK)) {
        size_t pos = 0;
        while (pos < len) {
            uint8_t tmp[NRF_UARTE_CHUNK_SIZE];
            size_t off = len - pos;
            off = (off > NRF_UARTE_CHUNK_SIZE) ? NRF_UARTE_CHUNK_SIZE : off;
            memcpy(tmp, data + pos, off);
            _write_buf(uart, tmp, off);
            pos += off;
        }
    }
    else {
        _write_buf(uart, data, len);
    }
#endif
}

static void irq_handler(uart_t uart)
{
    if (EVENTS_RXEND(uart_config[uart].dev)) {
        EVENTS_RXEND(uart_config[uart].dev) = 0;

        /* make sure we actually received new data */
        if (RXD_AMOUNT(uart_config[uart].dev) != 0) {
            /* Process received byte */
            isr_ctx[uart].rx_cb(isr_ctx[uart].arg, rx_buf[uart]);
        }
    }

#if UART_NONBLOCKING
    if (uart_config[uart].dev->EVENTS_ENDTX) {
        /* reset flags and idsable ISR on EVENTS_ENDTX */
        uart_config[uart].dev->EVENTS_ENDTX = 0;
        uart_config[uart].dev->EVENTS_TXSTARTED = 0;
        uart_config[uart].dev->INTENCLR = UARTE_INTENSET_ENDTX_Msk;
        if (tsrb_empty(&uart_tx_rb[uart])) {
            uart_config[uart].dev->TASKS_STOPTX = 1;
        } else {
            tx_buf[uart] = tsrb_get_one(&uart_tx_rb[uart]);
            _write_buf(uart, &tx_buf[uart], 1);
        }
    }
#endif

    cortexm_isr_end();
}

#else /* UART without EasyDMA*/

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assume((unsigned)uart < UART_NUMOF);

    uart_config[uart].dev->TASKS_STARTTX = 1;

    for (size_t i = 0; i < len; i++) {
        /* This section of the function is not thread safe:
            - another thread may mess up with the uart at the same time.
           In order to avoid an infinite loop in the interrupted thread,
           the TXRDY flag must be cleared before writing the data to be
           sent and not after. This way, the higher priority thread will
           exit this function with the TXRDY flag set, then the interrupted
           thread may have not transmitted his data but will still exit the
           while loop.
        */
        /* reset ready flag */
        uart_config[uart].dev->EVENTS_TXDRDY = 0;
        /* write data into transmit register */
        uart_config[uart].dev->TXD = data[i];
        /* wait for any transmission to be done */
        while (uart_config[uart].dev->EVENTS_TXDRDY == 0) {}
    }

    uart_config[uart].dev->TASKS_STOPTX = 1;
}

static void irq_handler(uart_t uart)
{
    if (uart_config[uart].dev->EVENTS_RXDRDY == 1) {
        uart_config[uart].dev->EVENTS_RXDRDY = 0;
        uint8_t byte = (uint8_t)(uart_config[uart].dev->RXD & 0xff);
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg, byte);
    }

    cortexm_isr_end();
}

#endif

#if defined(CPU_NRF53) || defined(CPU_NRF9160)
void uart_isr_handler(void *arg)
{
    uart_t uart = (uart_t)(uintptr_t)arg;

    irq_handler(uart);
}
#else
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

#endif /* def CPU_NRF53 || CPU_NRF9160 */
