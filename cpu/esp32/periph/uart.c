/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "esp_common.h"

#include "cpu.h"
#include "irq_arch.h"
#include "log.h"
#include "sched.h"
#include "thread.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "gpio_arch.h"
#include "driver/periph_ctrl.h"
#include "esp/common_macros.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/rtc.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "xtensa/xtensa_api.h"

#undef  UART_CLK_FREQ
#define UART_CLK_FREQ   rtc_clk_apb_freq_get() /* APB_CLK is used */

struct uart_hw_t {
    uart_dev_t* regs;       /* pointer to register data struct of the UART device */
    uint8_t  pin_txd;       /* TxD pin */
    uint8_t  pin_rxd;       /* RxD pin */
    uint8_t  signal_txd;    /* TxD signal from the controller */
    uint8_t  signal_rxd;    /* RxD signal to the controller */
    uint32_t baudrate;      /* used baudrate */
    bool     used;          /* indicates whether UART is used */
    uint8_t  int_src;       /* peripheral interrupt source used by the UART device */
    uart_isr_ctx_t isr_ctx; /* callback functions */
};

/* hardware ressources */
static struct uart_hw_t __uarts[] = {
    {
        .regs = &UART0,
        .pin_txd = GPIO1,
        .pin_rxd = GPIO3,
        .signal_txd = U0TXD_OUT_IDX,
        .signal_rxd = U0RXD_IN_IDX,
        .baudrate = STDIO_UART_BAUDRATE,
        .used = false,
        .int_src = ETS_UART0_INTR_SOURCE
    },
    #if defined(UART1_TXD) && defined(UART1_RXD)
    {   .regs = &UART1,
        .pin_txd = UART1_TXD,
        .pin_rxd = UART1_RXD,
        .signal_txd = U1TXD_OUT_IDX,
        .signal_rxd = U1RXD_IN_IDX,
        .baudrate = STDIO_UART_BAUDRATE,
        .used = false,
        .int_src = ETS_UART1_INTR_SOURCE
    },
    #endif
    #if defined(UART2_TXD) && defined(UART2_RXD)
    {   .regs = &UART2,
        .pin_txd = UART2_TXD,
        .pin_rxd = UART2_RXD,
        .signal_txd = U2TXD_OUT_IDX,
        .signal_rxd = U2RXD_IN_IDX,
        .baudrate = STDIO_UART_BAUDRATE,
        .used = false,
        .int_src = ETS_UART2_INTR_SOURCE
    }
    #endif
};

/* declaration of external functions */
extern void uart_div_modify(uint8_t uart_no, uint32_t div);

/* forward declaration of internal functions */
static uint8_t IRAM __uart_rx_one_char (uart_t uart);
static void __uart_tx_one_char(uart_t uart, uint8_t data);
static void __uart_intr_enable (uart_t uart);
static void IRAM __uart_intr_handler (void *para);

void __uart_config (uart_t uart)
{
    CHECK_PARAM (uart < UART_NUMOF);

    /* setup the baudrate */
    if (uart == UART_DEV(0) || uart == UART_DEV(1)) {
        /* for UART0 and UART1, we can us the ROM function */
        uart_div_modify(uart, (UART_CLK_FREQ << 4) / __uarts[uart].baudrate);
    }
    else {
        /* for UART2, we have to control it by registers */
        __uarts[uart].regs->conf0.tick_ref_always_on = 1; /* use APB_CLK */
        /* compute and set the integral and the decimal part */
        uint32_t clk = (UART_CLK_FREQ << 4) / __uarts[uart].baudrate;
        __uarts[uart].regs->clk_div.div_int  = clk >> 4;
        __uarts[uart].regs->clk_div.div_frag = clk & 0xf;
    }

    /* set 8 data bits */
    __uarts[uart].regs->conf0.bit_num = 3;
    /* reset the FIFOs */
    __uarts[uart].regs->conf0.rxfifo_rst = 1;
    __uarts[uart].regs->conf0.rxfifo_rst = 0;
    __uarts[uart].regs->conf0.txfifo_rst = 1;
    __uarts[uart].regs->conf0.txfifo_rst = 0;

    if (__uarts[uart].isr_ctx.rx_cb) {
        /* since reading can only be done byte by byte, we set
           UART_RXFIFO_FULL_THRHD interrupt level to 1 byte */
        __uarts[uart].regs->conf1.rxfifo_full_thrhd = 1;

        /* enable the RX FIFO FULL interrupt */
        __uart_intr_enable (uart);

        /* route all UART interrupt sources to same the CPU interrupt */
        intr_matrix_set(PRO_CPU_NUM, __uarts[uart].int_src, CPU_INUM_UART);

        /* we have to enable therefore the CPU interrupt here */
        xt_set_interrupt_handler(CPU_INUM_UART, __uart_intr_handler, NULL);
        xt_ints_on(BIT(CPU_INUM_UART));
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    DEBUG("%s uart=%d, rate=%d, rx_cb=%p, arg=%p\n", __func__, uart, baudrate, rx_cb, arg);

    CHECK_PARAM_RET (uart < UART_NUMOF, -1);

    /* UART1 and UART2 have configurable pins */
    if (uart == UART_DEV(1) || uart == UART_DEV(2)) {

        /* reset the pins when they were already used as UART pins */
        if (gpio_get_pin_usage(__uarts[uart].pin_txd) == _UART) {
            gpio_set_pin_usage(__uarts[uart].pin_txd, _GPIO);
        }
        if (gpio_get_pin_usage(__uarts[uart].pin_rxd) == _UART) {
            gpio_set_pin_usage(__uarts[uart].pin_rxd, _GPIO);
        }

        /* try to initialize the pins as GPIOs first */
        if (gpio_init (__uarts[uart].pin_txd, GPIO_OUT) ||
            gpio_init (__uarts[uart].pin_rxd, GPIO_IN)) {
            return -1;
        }

        /* store the usage type in GPIO table */
        gpio_set_pin_usage(__uarts[uart].pin_txd, _UART);
        gpio_set_pin_usage(__uarts[uart].pin_rxd, _UART);

        /* connect TxD pin to the TxD output signal through the GPIO matrix */
        GPIO.func_out_sel_cfg[__uarts[uart].pin_txd].func_sel = __uarts[uart].signal_txd;

        /* connect RxD input signal to the RxD pin through the GPIO matrix */
        GPIO.func_in_sel_cfg[__uarts[uart].signal_rxd].sig_in_sel = 1;
        GPIO.func_in_sel_cfg[__uarts[uart].signal_rxd].sig_in_inv = 0;
        GPIO.func_in_sel_cfg[__uarts[uart].signal_rxd].func_sel = __uarts[uart].pin_rxd;
    }
    __uarts[uart].baudrate = baudrate;

    /* register interrupt context */
    __uarts[uart].isr_ctx.rx_cb = rx_cb;
    __uarts[uart].isr_ctx.arg   = arg;

    /* enable and configure the according UART module */
    uart_poweron(uart);

    return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    CHECK_PARAM (uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        __uart_tx_one_char(uart, data[i]);
    }
}

void uart_poweron (uart_t uart)
{
    switch (uart) {
        #if UART_NUMOF
        case 0:  periph_module_enable(PERIPH_UART0_MODULE);
                 __uart_config(uart);
                 break;
        #endif
        #if UART_NUMOF > 1
        case 1:  periph_module_enable(PERIPH_UART1_MODULE);
                 __uart_config(uart);
                 break;
        #endif
        #if UART_NUMOF > 2
        case 2:  periph_module_enable(PERIPH_UART2_MODULE);
                 __uart_config(uart);
                 break;
        #endif
        default: break;
    }
}

void uart_poweroff (uart_t uart)
{
    switch (uart) {
        #if UART_NUMOF
        case 0: periph_module_disable(PERIPH_UART0_MODULE); break;
        #endif
        #if UART_NUMOF > 1
        case 1: periph_module_disable(PERIPH_UART1_MODULE); break;
        #endif
        #if UART_NUMOF > 2
        case 2: periph_module_disable(PERIPH_UART2_MODULE); break;
        #endif
        default: break;
    }
}

void IRAM __uart_intr_handler (void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter ();

    /* UART0, UART1, UART2 peripheral interrupt sources are routed to the same
       interrupt, so we have to use the status to distinguish interruptees */
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        if (__uarts[uart].used) {
            DEBUG("%s uart=%d int_st=%08x\n", __func__,
                  uart, __uarts[uart].regs->int_st.val);

            if (__uarts[uart].used && __uarts[uart].regs->int_st.rxfifo_full) {
                /* read one byte of data */
                uint8_t data = __uart_rx_one_char (uart);
                /* if registered, call the RX callback function */
                if (__uarts[uart].isr_ctx.rx_cb) {
                    __uarts[uart].isr_ctx.rx_cb(__uarts[uart].isr_ctx.arg, data);
                }
                /* clear interrupt flag */
                __uarts[uart].regs->int_clr.rxfifo_full = 1;
            }

            /* TODO handle other types of interrupts, for the moment just clear them */
            __uarts[uart].regs->int_clr.val = ~0x0;
        }
    }

    irq_isr_exit ();
}

/* RX/TX FIFO capacity is 128 byte */
#define UART_FIFO_MAX 127

/* receive one data byte with wait */
static uint8_t IRAM __uart_rx_one_char (uart_t uart)
{
    /* wait until at least von byte is in RX FIFO */
    while (!__uarts[uart].regs->status.rxfifo_cnt) {}

    /* read the lowest byte from RX FIFO register */
    return __uarts[uart].regs->fifo.rw_byte;
}

/* send one data byte with wait */
static void __uart_tx_one_char(uart_t uart, uint8_t data)
{
    /* wait until at least one byte is avaiable in the TX FIFO */
    while (__uarts[uart].regs->status.txfifo_cnt >= UART_FIFO_MAX) {}

    /* send the byte by placing it in the TX FIFO using MPU */
    WRITE_PERI_REG(UART_FIFO_AHB_REG(uart), data);
}

static void __uart_intr_enable(uart_t uart)
{
    __uarts[uart].regs->int_ena.rxfifo_full = 1;
    __uarts[uart].regs->int_clr.rxfifo_full = 1;
    __uarts[uart].used = true;

    DEBUG("%s %08x\n", __func__, __uarts[uart].regs->int_ena.val);
}

/* systemwide UART initializations */
void uart_system_init (void)
{
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        /* reset all UART interrupt status registers */
        __uarts[uart].regs->int_clr.val = ~0;
    }
}

void uart_print_config(void)
{
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        ets_printf("\tUART_DEV(%d)\ttxd=%d rxd=%d\n", uart,
                   __uarts[uart].pin_txd, __uarts[uart].pin_rxd);
    }
}

int uart_set_baudrate(uart_t uart, uint32_t baudrate)
{
    DEBUG("%s uart=%d, rate=%d\n", __func__, uart, baudrate);

    CHECK_PARAM_RET (uart < UART_NUMOF, -1);

    /* use APB_CLK */
    __uarts[uart].regs->conf0.tick_ref_always_on = 1;
    /* compute and set the integral and the decimal part */
    uint32_t clk = (UART_CLK_FREQ << 4) / baudrate;
    __uarts[uart].regs->clk_div.div_int  = clk >> 4;
    __uarts[uart].regs->clk_div.div_frag = clk & 0xf;

    return UART_OK;
}
