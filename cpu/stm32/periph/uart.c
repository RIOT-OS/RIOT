/*
 * Copyright (C) 2014-2017 Freie Universität Berlin
 * Copyright (C) 2016 OTA keys
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hermann Lelong <hermann@otakeys.com>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "assert.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "pm_layered.h"

#if defined(CPU_LINE_STM32L4R5xx) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
#define ISR_REG     ISR
#define ISR_TXE     USART_ISR_TXE_TXFNF
#define ISR_RXNE    USART_ISR_RXNE_RXFNE
#define ISR_TC      USART_ISR_TC
#define TDR_REG     TDR
#define RDR_REG     RDR
#elif defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0) || \
      defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L4) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32F7) || \
      defined(CPU_FAM_STM32G4)
#define ISR_REG     ISR
#define ISR_TXE     USART_ISR_TXE
#define ISR_RXNE    USART_ISR_RXNE
#define ISR_TC      USART_ISR_TC
#define TDR_REG     TDR
#define RDR_REG     RDR
#else
#define ISR_REG     SR
#define ISR_TXE     USART_SR_TXE
#define ISR_RXNE    USART_SR_RXNE
#define ISR_TC      USART_SR_TC
#define TDR_REG     DR
#define RDR_REG     DR
#endif

#if defined(CPU_LINE_STM32L4R5xx) || defined(CPU_FAM_STM32G0) || \
    defined(CPU_FAM_STM32L5)
#define RXENABLE            (USART_CR1_RE | USART_CR1_RXNEIE_RXFNEIE)
#else
#define RXENABLE            (USART_CR1_RE | USART_CR1_RXNEIE)
#endif

#ifdef MODULE_PERIPH_UART_NONBLOCKING

#include "tsrb.h"
/**
 * @brief   Allocate for tx ring buffers
 */
static tsrb_t uart_tx_rb[UART_NUMOF];
static uint8_t uart_tx_rb_buf[UART_NUMOF][UART_TXBUF_SIZE];
#endif

/**
 * @brief   Allocate memory to store the callback functions
 *
 * Extend standard uart_isr_ctx_t with data_mask field. This is needed
 * in order to mask parity bit.
 */
static struct {
    uart_rx_cb_t rx_cb;   /**< data received interrupt callback */
    void *arg;            /**< argument to both callback routines */
    uint8_t data_mask;    /**< mask applied to the data register */
} isr_ctx[UART_NUMOF];

static inline USART_TypeDef *dev(uart_t uart)
{
    return uart_config[uart].dev;
}

static inline void uart_init_usart(uart_t uart, uint32_t baudrate);
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
#ifdef MODULE_PERIPH_LPUART
static inline void uart_init_lpuart(uart_t uart, uint32_t baudrate);
#endif
#endif

#ifdef MODULE_PERIPH_UART_HW_FC
static inline void uart_init_rts_pin(uart_t uart)
{
    if (uart_config[uart].rts_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].rts_pin, GPIO_OUT);
#ifdef CPU_FAM_STM32F1
        gpio_init_af(uart_config[uart].rts_pin, GPIO_AF_OUT_PP);
#else
        gpio_init_af(uart_config[uart].rts_pin, uart_config[uart].rts_af);
#endif
    }
}

static inline void uart_init_cts_pin(uart_t uart)
{
    if (uart_config[uart].cts_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].cts_pin, GPIO_IN);
#ifndef CPU_FAM_STM32F1
        gpio_init_af(uart_config[uart].cts_pin, uart_config[uart].cts_af);
#endif
    }
}
#endif

static inline void uart_init_pins(uart_t uart, uart_rx_cb_t rx_cb)
{
     /* configure TX pin */
#ifdef CPU_FAM_STM32F1
    gpio_init_af(uart_config[uart].tx_pin, GPIO_AF_OUT_PP);
#else
    gpio_init_af(uart_config[uart].tx_pin, uart_config[uart].tx_af);
#endif
    /* configure RX pin */
    if (rx_cb) {
        gpio_init(uart_config[uart].rx_pin, GPIO_IN_PU);
#ifndef CPU_FAM_STM32F1
        gpio_init_af(uart_config[uart].rx_pin, uart_config[uart].rx_af);
#endif
    }
#ifdef MODULE_PERIPH_UART_HW_FC
    uart_init_cts_pin(uart);
    uart_init_rts_pin(uart);
#endif
}

static inline void uart_enable_clock(uart_t uart)
{
#ifdef STM32_PM_STOP
    if (isr_ctx[uart].rx_cb) {
        pm_block(STM32_PM_STOP);
    }
#endif
    periph_clk_en(uart_config[uart].bus, uart_config[uart].rcc_mask);
}

static inline void uart_disable_clock(uart_t uart)
{
    periph_clk_dis(uart_config[uart].bus, uart_config[uart].rcc_mask);
#ifdef STM32_PM_STOP
    if (isr_ctx[uart].rx_cb) {
        pm_unblock(STM32_PM_STOP);
    }
#endif
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    /* save ISR context */
    isr_ctx[uart].rx_cb     = rx_cb;
    isr_ctx[uart].arg       = arg;
    isr_ctx[uart].data_mask = 0xFF;

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    /* set up the TX buffer */
    tsrb_init(&uart_tx_rb[uart], uart_tx_rb_buf[uart], UART_TXBUF_SIZE);
#endif

    uart_enable_clock(uart);

    /* reset UART configuration -> defaults to 8N1 mode */
    dev(uart)->CR1 = 0;
    dev(uart)->CR2 = 0;
    dev(uart)->CR3 = 0;

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
    switch (uart_config[uart].type) {
        case STM32_USART:
            uart_init_usart(uart, baudrate);
            break;
#ifdef MODULE_PERIPH_LPUART
        case STM32_LPUART:
            uart_init_lpuart(uart, baudrate);
            break;
#endif
        default:
            return UART_NODEV;
    }
#else
    uart_init_usart(uart, baudrate);
#endif

    /* Attach pins to enabled UART periph. Note: It is important that the UART
     * interface is configured prior to attaching the pins, as otherwise the
     * signal level flickers during initialization resulting in garbage being
     * sent. */
    uart_init_pins(uart, rx_cb);

    /* enable RX interrupt if applicable */
    if (rx_cb) {
        NVIC_EnableIRQ(uart_config[uart].irqn);
        dev(uart)->CR1 = (USART_CR1_UE | USART_CR1_TE | RXENABLE);
    }
    else {
        dev(uart)->CR1 = (USART_CR1_UE | USART_CR1_TE);
    }

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    NVIC_EnableIRQ(uart_config[uart].irqn);
#endif

#ifdef MODULE_PERIPH_UART_HW_FC
    if (uart_config[uart].cts_pin != GPIO_UNDEF) {
        dev(uart)->CR3 |= USART_CR3_CTSE;
    }
    if (uart_config[uart].rts_pin != GPIO_UNDEF) {
        dev(uart)->CR3 |= USART_CR3_RTSE;
    }
#endif

    return UART_OK;
}

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    assert(uart < UART_NUMOF);

    isr_ctx[uart].data_mask = 0xFF;

    if (parity) {
        switch (data_bits) {
            case UART_DATA_BITS_6:
                data_bits = UART_DATA_BITS_7;
                isr_ctx[uart].data_mask = 0x3F;
                break;
            case UART_DATA_BITS_7:
                data_bits = UART_DATA_BITS_8;
                isr_ctx[uart].data_mask = 0x7F;
                break;
            case UART_DATA_BITS_8:
#ifdef USART_CR1_M0
                data_bits = USART_CR1_M0;
#else
                data_bits = USART_CR1_M;
#endif
                break;
            default:
                return UART_NOMODE;
        }
    }
    if ((data_bits & UART_INVALID_MODE) || (parity & UART_INVALID_MODE)) {
        return UART_NOMODE;
    }

#ifdef USART_CR1_M1
    if (!(dev(uart)->ISR & USART_ISR_TC)) {
        return UART_INTERR;
    }
    dev(uart)->CR1 &= ~(USART_CR1_UE | USART_CR1_TE);
#endif

    dev(uart)->CR2 &= ~USART_CR2_STOP;
    dev(uart)->CR1 &= ~(USART_CR1_PS | USART_CR1_PCE | USART_CR1_M);

    dev(uart)->CR2 |= stop_bits;
    dev(uart)->CR1 |= (USART_CR1_UE | USART_CR1_TE | data_bits | parity);

    return UART_OK;
}
#endif /* MODULE_PERIPH_UART_MODECFG */

static inline void uart_init_usart(uart_t uart, uint32_t baudrate)
{
    uint16_t mantissa;
    uint8_t fraction;
    uint32_t clk;

    /* calculate and apply baudrate */
    clk = periph_apb_clk(uart_config[uart].bus) / baudrate;
    mantissa = (uint16_t)(clk / 16);
    fraction = (uint8_t)(clk - (mantissa * 16));
    dev(uart)->BRR = ((mantissa & 0x0fff) << 4) | (fraction & 0x0f);
}

#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L4) || \
    defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
    defined(CPU_FAM_STM32L5)
#ifdef CPU_FAM_STM32L5
#define RCC_CCIPR_LPUART1SEL_0  RCC_CCIPR1_LPUART1SEL_0
#define RCC_CCIPR_LPUART1SEL_1  RCC_CCIPR1_LPUART1SEL_1
#define CCIPR                   CCIPR1
#endif
#ifdef MODULE_PERIPH_LPUART
static inline void uart_init_lpuart(uart_t uart, uint32_t baudrate)
{
    uint32_t clk;

    switch (uart_config[uart].clk_src) {
        case 0:
            clk = periph_apb_clk(uart_config[uart].bus);
            break;
        case RCC_CCIPR_LPUART1SEL_0:
            clk = CLOCK_CORECLOCK;
            break;
        case (RCC_CCIPR_LPUART1SEL_0 | RCC_CCIPR_LPUART1SEL_1):
            clk = 32768;
            break;
        default: /* HSI is not supported */
            return;
    }

    RCC->CCIPR |= uart_config[uart].clk_src;

    /* LSE can only be used with baudrate <= 9600 */
    if ( (clk < (3 * baudrate)) || (clk > (4096 * baudrate))) {
        return;
    }

    /* LPUARTDIV = f_clk * 256 / baudrate */
    uint32_t brr = (uint32_t)(((uint64_t)clk << 8) / baudrate);

    dev(uart)->BRR = brr;
}
#endif /* MODULE_PERIPH_LPUART */
#endif /* STM32L0 || STM32L4 || STM32WB */

static inline void send_byte(uart_t uart, uint8_t byte)
{
    while (!(dev(uart)->ISR_REG & ISR_TXE)) {}
    dev(uart)->TDR_REG = byte;
}

#ifndef MODULE_PERIPH_UART_NONBLOCKING
static inline void wait_for_tx_complete(uart_t uart)
{
    while (!(dev(uart)->ISR_REG & ISR_TC)) {}
}
#endif

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);
#if DEVELHELP
    /* If tx is not enabled don't try to send */
    if (!(dev(uart)->CR1 & USART_CR1_TE)) {
        return;
    }
#endif
#ifdef MODULE_PERIPH_DMA
    if (!len) {
        return;
    }
    if (uart_config[uart].dma != DMA_STREAM_UNDEF) {
        if (irq_is_in()) {
            uint16_t todo = 0;
            if (dev(uart)->CR3 & USART_CR3_DMAT) {
                /* DMA transfer for UART on-going */
                todo = dma_suspend(uart_config[uart].dma);
            }
            if (todo) {
                dma_stop(uart_config[uart].dma);
                dev(uart)->CR3 &= ~USART_CR3_DMAT;
            }
            for (unsigned i = 0; i < len; i++) {
                send_byte(uart, data[i]);
            }
            if (todo > 0) {
                wait_for_tx_complete(uart);
                dev(uart)->CR3 |= USART_CR3_DMAT;
                dma_resume(uart_config[uart].dma, todo);
            }
        }
        else {
            dma_acquire(uart_config[uart].dma);
            dev(uart)->CR3 |= USART_CR3_DMAT;
            dma_transfer(uart_config[uart].dma, uart_config[uart].dma_chan, data,
                         (void *)&dev(uart)->TDR_REG, len, DMA_MEM_TO_PERIPH, DMA_INC_SRC_ADDR);

            /* make sure the function is synchronous by waiting for the transfer to
             * finish */
            wait_for_tx_complete(uart);
            dev(uart)->CR3 &= ~USART_CR3_DMAT;
            dma_release(uart_config[uart].dma);
        }
        return;
    }
#endif
#ifdef MODULE_PERIPH_UART_NONBLOCKING
    for (size_t i = 0; i < len; i++) {
        dev(uart)->CR1 |= (USART_CR1_TCIE);
        if (irq_is_in() || __get_PRIMASK()) {
            /* if ring buffer is full free up a spot */
            if (tsrb_full(&uart_tx_rb[uart])) {
                send_byte(uart, tsrb_get_one(&uart_tx_rb[uart]));
            }
            tsrb_add_one(&uart_tx_rb[uart], data[i]);
        }
        else {
            while (tsrb_add_one(&uart_tx_rb[uart], data[i]) < 0) {}
        }
    }
#else
    for (size_t i = 0; i < len; i++) {
        send_byte(uart, data[i]);
    }
    /* make sure the function is synchronous by waiting for the transfer to
     * finish */
    wait_for_tx_complete(uart);
#endif
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uart_enable_clock(uart);

    dev(uart)->CR1 |= (USART_CR1_UE);

#ifdef MODULE_PERIPH_UART_HW_FC
    /* STM32F4 errata 2.10.9: nRTS is active while RE or UE = 0
     * we should only configure nRTS pin after setting UE */
    uart_init_rts_pin(uart);
#endif
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

#ifdef MODULE_PERIPH_UART_HW_FC
    /* the uart peripheral does not put RTS high from hardware when
     * UE flag is cleared, so we need to do this manually */
    if (uart_config[uart].rts_pin != GPIO_UNDEF) {
        gpio_init(uart_config[uart].rts_pin, GPIO_OUT);
        gpio_set(uart_config[uart].rts_pin);
    }
#endif

    dev(uart)->CR1 &= ~(USART_CR1_UE);

    uart_disable_clock(uart);
}

#ifdef MODULE_PERIPH_UART_NONBLOCKING
static inline void irq_handler_tx(uart_t uart)
{
    int byte = tsrb_get_one(&uart_tx_rb[uart]);
    if (byte >= 0) {
        dev(uart)->TDR_REG = byte;
    }

    /* disable the interrupt if there are no more bytes to send */
    if (tsrb_empty(&uart_tx_rb[uart])) {
        dev(uart)->CR1 &= ~(USART_CR1_TCIE);
    }
}
#endif

static inline void irq_handler(uart_t uart)
{
    uint32_t status = dev(uart)->ISR_REG;

#ifdef MODULE_PERIPH_UART_NONBLOCKING
    if (status & ISR_TC) {
        irq_handler_tx(uart);
    }
#endif

    if (status & ISR_RXNE) {
        isr_ctx[uart].rx_cb(isr_ctx[uart].arg,
                            (uint8_t)dev(uart)->RDR_REG & isr_ctx[uart].data_mask);
    }
#if defined(USART_ISR_ORE)
    /* USART_ISR_ORE is cleared by writing 1 to ORECF */
    if (status & USART_ISR_ORE) {
        dev(uart)->ICR |= USART_ICR_ORECF;
    }
#else
    /* USART_SR_ORE is cleared by reading SR and DR sequentially */
    if (status & USART_SR_ORE) {
        dev(uart)->DR;
    }
#endif

    cortexm_isr_end();
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
