/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
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

#include <assert.h>

#include "esp_common.h"
#include "cpu.h"
#include "irq_arch.h"
#include "log.h"
#include "sched.h"
#include "thread.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "stdio_uart.h"

#include "esp/common_macros.h"
#include "rom/ets_sys.h"
#include "gpio_arch.h"

#if __xtensa__
#include "xtensa/xtensa_api.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(CPU_ESP8266)

#include "esp/iomux_regs.h"
#include "esp8266/uart_struct.h"

#ifdef MODULE_ESP_QEMU
#include "esp/uart_regs.h"
#endif /* MODULE_ESP_QEMU */

#define UART0           uart0
#define UART1           uart1
#define CPU_INUM_UART   ETS_UART_INUM

#else /* defined(CPU_ESP8266) */

#include "driver/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "esp_rom_uart.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/periph_defs.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "soc/uart_pins.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"

#undef  UART_CLK_FREQ
#define UART_CLK_FREQ   rtc_clk_apb_freq_get() /* APB_CLK is used */

#endif /* defined(CPU_ESP8266) */

struct uart_hw_t {
    uart_dev_t* regs;       /* pointer to register data struct of the UART device */
    bool     used;          /* indicates whether UART is used */
    uint32_t baudrate;      /* used baudrate */
    uart_data_bits_t data;  /* used data bits */
    uart_stop_bits_t stop;  /* used stop bits */
    uart_parity_t  parity;  /* used parity bits */
    uart_isr_ctx_t isr_ctx; /* callback functions */
#if !defined(CPU_ESP8266)
    uint8_t  mod;           /* peripheral hardware module of the UART interface */
    uint8_t  signal_txd;    /* TxD signal from the controller */
    uint8_t  signal_rxd;    /* RxD signal to the controller */
    uint8_t  int_src;       /* peripheral interrupt source used by the UART device */
#endif
};

/* hardware resources */
static struct uart_hw_t _uarts[] = {
    {
        .regs = &UART0,
        .used = false,
        .baudrate = STDIO_UART_BAUDRATE,
        .data = UART_DATA_BITS_8,
        .stop = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
#if !defined(CPU_ESP8266)
        .mod = PERIPH_UART0_MODULE,
        .signal_txd = U0TXD_OUT_IDX,
        .signal_rxd = U0RXD_IN_IDX,
        .int_src = ETS_UART0_INTR_SOURCE
#endif /* !defined CPU_ESP8266 */
    },

#if !defined(CPU_ESP8266)
#if defined(UART1_TXD) && defined(UART1_RXD) && (SOC_UART_NUM > 1)
    {
        .regs = &UART1,
        .used = false,
        .baudrate = STDIO_UART_BAUDRATE,
        .data = UART_DATA_BITS_8,
        .stop = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .mod = PERIPH_UART1_MODULE,
        .signal_txd = U1TXD_OUT_IDX,
        .signal_rxd = U1RXD_IN_IDX,
        .int_src = ETS_UART1_INTR_SOURCE
    },
#endif /* defined(UART1_TXD) && defined(UART1_RXD) && (SOC_UART_NUM > 1) */

#if defined(UART2_TXD) && defined(UART2_RXD) && (SOC_UART_NUM > 2)
    {
        .regs = &UART2,
        .used = false,
        .baudrate = STDIO_UART_BAUDRATE,
        .data = UART_DATA_BITS_8,
        .stop = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .mod = PERIPH_UART2_MODULE,
        .signal_txd = U2TXD_OUT_IDX,
        .signal_rxd = U2RXD_IN_IDX,
        .int_src = ETS_UART2_INTR_SOURCE
    },
#endif /* defined(UART2_TXD) && defined(UART2_RXD) && (SOC_UART_NUM > 2) */
#endif /* !defined(CPU_ESP8266) */
};

/* declaration of external functions */
extern void uart_div_modify(uint8_t uart_no, uint32_t div);

/* forward declaration of internal functions */
static int _uart_set_baudrate(uart_t uart, uint32_t baudrate);
static int _uart_set_mode(uart_t uart, uart_data_bits_t data_bits,
                          uart_parity_t parity, uart_stop_bits_t stop_bits);
static uint8_t IRAM _uart_rx_one_char(uart_t uart);
static void _uart_tx_one_char(uart_t uart, uint8_t data);
static void _uart_intr_enable(uart_t uart);
static void _uart_config(uart_t uart);
static void IRAM _uart_intr_handler(void *para);

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    DEBUG("%s uart=%d, rate=%" PRIu32 ", rx_cb=%p, arg=%p\n", __func__,
          uart, baudrate, rx_cb, arg);

    assert(uart < UART_NUMOF_MAX);
    assert(uart < UART_NUMOF);

#ifndef CPU_ESP8266
    assert(uart_config[uart].txd != GPIO_UNDEF);
    assert(uart_config[uart].rxd != GPIO_UNDEF);

    /* don't reinitialize the pins if they are already configured as UART pins */
    if ((gpio_get_pin_usage(uart_config[uart].txd) != _UART) ||
        (gpio_get_pin_usage(uart_config[uart].rxd) != _UART)) {

        /* try to initialize the pins where the TX line is set and temporarily
         * configured as a pull-up open-drain output before configuring it as
         * a push-pull output to avoid a several msec long LOW pulse resulting
         * in some garbage */
        gpio_set(uart_config[uart].txd);
        if (gpio_init(uart_config[uart].txd, GPIO_OD_PU) ||
            gpio_init(uart_config[uart].txd, GPIO_OUT) ||
            gpio_init(uart_config[uart].rxd, GPIO_IN_PU)) {
            return -1;
        }

        /* store the usage type in GPIO table */
        gpio_set_pin_usage(uart_config[uart].txd, _UART);
        gpio_set_pin_usage(uart_config[uart].rxd, _UART);

        esp_rom_uart_tx_wait_idle(uart);
        esp_rom_gpio_connect_out_signal(uart_config[uart].txd,
                                        _uarts[uart].signal_txd, false, false);
        esp_rom_gpio_connect_in_signal(uart_config[uart].rxd,
                                       _uarts[uart].signal_rxd, false);
    }
#endif /* CPU_ESP8266 */

    _uarts[uart].baudrate = baudrate;

    /* register interrupt context */
    _uarts[uart].isr_ctx.rx_cb = rx_cb;
    _uarts[uart].isr_ctx.arg   = arg;

    /* enable and configure the according UART module */
    uart_poweron(uart);

    return UART_OK;
}

#if MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    return _uart_set_mode(uart, data_bits, parity, stop_bits);
}
#endif

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        _uart_tx_one_char(uart, data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

#ifndef CPU_ESP8266
    periph_module_enable(_uarts[uart].mod);
#endif
    _uart_config(uart);
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

#ifndef CPU_ESP8266
    periph_module_disable(_uarts[uart].mod);
#endif
}

/* systemwide UART initializations */
void uart_system_init(void)
{
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        /* reset all UART interrupt status registers */
        _uarts[uart].regs->int_clr.val = ~0;
    }
#ifndef CPU_ESP8266
    /* reset the pin usage of the default UART0 pins to GPIO to allow
     * reinitialization and usage for other purposes */
    gpio_set_pin_usage(U0TXD_GPIO_NUM, _GPIO);
    gpio_set_pin_usage(U0RXD_GPIO_NUM, _GPIO);
#if defined(CONFIG_CONSOLE_UART_TX) && defined(CONFIG_CONSOLE_UART_TX)
    /* reset the pin usage of the UART pins used by the bootloader to
     * _GPIO to be able to use them later for other purposes */
    gpio_set_pin_usage(CONFIG_CONSOLE_UART_TX, _GPIO);
    gpio_set_pin_usage(CONFIG_CONSOLE_UART_RX, _GPIO);
#endif
#endif
}

void uart_print_config(void)
{
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        printf("\tUART_DEV(%" PRIu32 ")\ttxd=%d rxd=%d\n", (uint32_t)uart,
               uart_config[uart].txd, uart_config[uart].rxd);
    }
}

static void IRAM _uart_intr_handler(void *arg)
{
    /* to satisfy the compiler */
    (void)arg;

    irq_isr_enter();

    /* UART0, UART1, UART2 peripheral interrupt sources are routed to the same
       interrupt, so we have to use the status to distinguish interruptees */
    for (unsigned uart = 0; uart < UART_NUMOF; uart++) {
        if (_uarts[uart].used) {
            DEBUG("%s uart=%d int_st=%08x\n", __func__,
                  uart, (unsigned)_uarts[uart].regs->int_st.val);

#ifdef CPU_FAM_ESP32S3
            if (_uarts[uart].used && _uarts[uart].regs->int_st.rxfifo_full_int_st) {
#else
            if (_uarts[uart].used && _uarts[uart].regs->int_st.rxfifo_full) {
#endif
                /* read one byte of data */
                uint8_t data = _uart_rx_one_char(uart);
                /* if registered, call the RX callback function */
                if (_uarts[uart].isr_ctx.rx_cb) {
                    _uarts[uart].isr_ctx.rx_cb(_uarts[uart].isr_ctx.arg, data);
                }
                /* clear interrupt flag */
#ifdef CPU_FAM_ESP32S3
                _uarts[uart].regs->int_clr.rxfifo_full_int_clr = 1;
#else
                _uarts[uart].regs->int_clr.rxfifo_full = 1;
#endif
            }

            /* TODO handle other types of interrupts, for the moment just clear them */
            _uarts[uart].regs->int_clr.val = ~0x0;
        }
    }

    irq_isr_exit();
}

/* RX/TX FIFO capacity is 128 byte */
#define UART_FIFO_MAX 128

/* receive one data byte with wait */
static uint8_t IRAM _uart_rx_one_char(uart_t uart)
{
#if defined(MODULE_ESP_QEMU) && defined(CPU_ESP8266)
    /* wait until at least von byte is in RX FIFO */
    while (!FIELD2VAL(UART_STATUS_RXFIFO_COUNT, UART(uart).STATUS)) {}

    /* read the lowest byte from RX FIFO register */
    return UART(uart).FIFO & 0xff; /* only bit 0 ... 7 */
#else
    /* wait until at least von byte is in RX FIFO */
    while (!_uarts[uart].regs->status.rxfifo_cnt) {}

#if defined(CPU_FAM_ESP32) || defined(CPU_ESP8266)
    /* read the lowest byte from RX FIFO register */
    return _uarts[uart].regs->fifo.rw_byte;
#elif defined(CPU_FAM_ESP32S3)
    /* read the lowest byte from RX FIFO register */
    return _uarts[uart].regs->fifo.rxfifo_rd_byte;
#elif defined(CPU_FAM_ESP32S2)
    return READ_PERI_REG(UART_FIFO_AHB_REG(uart));
#else
    /* read the lowest byte from RX FIFO register */
    return _uarts[uart].regs->ahb_fifo.rw_byte;
#endif
#endif

}

/* send one data byte with wait */
static void _uart_tx_one_char(uart_t uart, uint8_t data)
{
    /* wait until at least one byte is available in the TX FIFO */
    while (_uarts[uart].regs->status.txfifo_cnt >= UART_FIFO_MAX) {}

    /* send the byte by placing it in the TX FIFO using MPU */
#ifdef CPU_ESP8266
#ifdef MODULE_ESP_QEMU
    UART(uart).FIFO = data;
#else /* MODULE_ESP_QEMU */
    _uarts[uart].regs->fifo.rw_byte = data;
#endif /* MODULE_ESP_QEMU */
#else /* CPU_ESP8266 */
    WRITE_PERI_REG(UART_FIFO_AHB_REG(uart), data);
#endif /* CPU_ESP8266 */
}

static void _uart_intr_enable(uart_t uart)
{
#ifdef CPU_FAM_ESP32S3
    _uarts[uart].regs->int_ena.rxfifo_full_int_ena = 1;
    _uarts[uart].regs->int_clr.rxfifo_full_int_clr = 1;
#else
    _uarts[uart].regs->int_ena.rxfifo_full = 1;
    _uarts[uart].regs->int_clr.rxfifo_full = 1;
#endif
    _uarts[uart].used = true;

    DEBUG("%s %08x\n", __func__, (unsigned)_uarts[uart].regs->int_ena.val);
}

static void _uart_config(uart_t uart)
{
    assert(uart < UART_NUMOF);

    /* setup the baudrate */
    if (_uart_set_baudrate(uart, _uarts[uart].baudrate) != UART_OK) {
        return;
    }

    /* set number of data bits, stop bits and parity mode */
    if (_uart_set_mode(uart, _uarts[uart].data, _uarts[uart].parity,
                             _uarts[uart].stop) != UART_OK) {
        return;
    }

    /* reset the FIFOs */
    _uarts[uart].regs->conf0.rxfifo_rst = 1;
    _uarts[uart].regs->conf0.rxfifo_rst = 0;
    _uarts[uart].regs->conf0.txfifo_rst = 1;
    _uarts[uart].regs->conf0.txfifo_rst = 0;

    if (_uarts[uart].isr_ctx.rx_cb) {
        /* since reading can only be done byte by byte, we set
           UART_RXFIFO_FULL_THRHD interrupt level to 1 byte */
        _uarts[uart].regs->conf1.rxfifo_full_thrhd = 1;

        /* enable the RX FIFO FULL interrupt */
        _uart_intr_enable(uart);

#ifdef CPU_ESP8266
        /* we have to enable therefore the CPU interrupt here */
        xt_set_interrupt_handler(CPU_INUM_UART, _uart_intr_handler, NULL);
        xt_ints_on(BIT(CPU_INUM_UART));
#else /* CPU_ESP8266 */
        /* route all UART interrupt sources to same the CPU interrupt */
        intr_matrix_set(PRO_CPU_NUM, _uarts[uart].int_src, CPU_INUM_UART);
        /* we have to enable therefore the CPU interrupt here */
        intr_cntrl_ll_set_int_handler(CPU_INUM_UART, _uart_intr_handler, NULL);
        intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_UART));
#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
        /* set interrupt level */
        intr_cntrl_ll_set_int_level(CPU_INUM_UART, 1);
#endif
#endif /* CPU_ESP8266 */
    }
}

static int _uart_set_baudrate(uart_t uart, uint32_t baudrate)
{
    DEBUG("%s uart=%d, rate=%" PRIu32 "\n", __func__, uart, baudrate);

    assert(uart < UART_NUMOF);

    /* wait until TX FIFO is empty */
    while (_uarts[uart].regs->status.txfifo_cnt != 0) { }

    critical_enter();

    _uarts[uart].baudrate = baudrate;

#ifdef CPU_ESP8266

    /* compute and set clock divider */
    uint32_t clk_div = UART_CLK_FREQ / _uarts[uart].baudrate;
    _uarts[uart].regs->clk_div.val = clk_div & 0xFFFFF;

#else

/* TODO look for an HAL/LL API function */
#ifdef CPU_FAM_ESP32
    /* use APB_CLK */
    _uarts[uart].regs->conf0.tick_ref_always_on = 1;
#endif
#if defined(CPU_FAM_ESP32C3) || defined(CPU_FAM_ESP32S3)
    _uarts[uart].regs->clk_conf.sclk_sel = 1; /* APB clock used instead of XTAL */
#endif
    /* compute and set the integral and the decimal part */
    uint32_t clk_div = (UART_CLK_FREQ << 4) / _uarts[uart].baudrate;
#ifdef CPU_FAM_ESP32S3
    _uarts[uart].regs->clkdiv.clkdiv  = clk_div >> 4;
    _uarts[uart].regs->clkdiv.clkdiv_frag = clk_div & 0xf;
#else
    _uarts[uart].regs->clk_div.div_int  = clk_div >> 4;
    _uarts[uart].regs->clk_div.div_frag = clk_div & 0xf;
#endif /* CPU_FAM_ESP32S3 */

#endif

    critical_exit();

    return UART_OK;
}

static int _uart_set_mode(uart_t uart, uart_data_bits_t data_bits,
                          uart_parity_t parity, uart_stop_bits_t stop_bits)
{
    DEBUG("%s uart=%d, data_bits=%d parity=%d stop_bits=%d\n", __func__,
          uart, data_bits, parity, stop_bits);

    assert(uart < UART_NUMOF);

    critical_enter();

    /* set number of data bits */
    switch (data_bits) {
        case UART_DATA_BITS_5: _uarts[uart].regs->conf0.bit_num = 0; break;
        case UART_DATA_BITS_6: _uarts[uart].regs->conf0.bit_num = 1; break;
        case UART_DATA_BITS_7: _uarts[uart].regs->conf0.bit_num = 2; break;
        case UART_DATA_BITS_8: _uarts[uart].regs->conf0.bit_num = 3; break;
        default: LOG_TAG_ERROR("uart", "invalid number of data bits\n");
                 critical_exit();
                 return UART_NOMODE;
    }
    /* store changed number of data bits in configuration */
    _uarts[uart].data = data_bits;

    /* set number of stop bits */
#ifdef CPU_ESP8266
    switch (stop_bits) {
        case UART_STOP_BITS_1: _uarts[uart].regs->conf0.stop_bit_num = 1; break;
        case UART_STOP_BITS_2: _uarts[uart].regs->conf0.stop_bit_num = 3; break;
        default: LOG_TAG_ERROR("uart", "invalid number of stop bits\n");
                 critical_exit();
                 return UART_NOMODE;
    }
#else
    /* workaround for hardware bug when stop bits are set to 2-bit mode. */
    switch (stop_bits) {
        case UART_STOP_BITS_1: _uarts[uart].regs->conf0.stop_bit_num = 1;
                               _uarts[uart].regs->rs485_conf.dl1_en = 0;
                               break;
        case UART_STOP_BITS_2: _uarts[uart].regs->conf0.stop_bit_num = 1;
                               _uarts[uart].regs->rs485_conf.dl1_en = 1;
                               break;
        default: LOG_TAG_ERROR("uart", "invalid number of stop bits\n");
                 critical_exit();
                 return UART_NOMODE;
    }
#endif

    /* store changed number of stop bits in configuration */
    _uarts[uart].stop = stop_bits;

    /* set parity mode */
    switch (parity) {
        case UART_PARITY_NONE: _uarts[uart].regs->conf0.parity_en = 0;
                               break;
        case UART_PARITY_EVEN: _uarts[uart].regs->conf0.parity = 0;
                               _uarts[uart].regs->conf0.parity_en = 1;
                               break;
        case UART_PARITY_ODD: _uarts[uart].regs->conf0.parity = 1;
                              _uarts[uart].regs->conf0.parity_en = 1;
                              break;
        default: LOG_TAG_ERROR("uart", "invalid or unsupported parity mode\n");
                 critical_exit();
                 return UART_NOMODE;
    }
    /* store changed parity in configuration */
    _uarts[uart].parity = parity;

    critical_exit();

    return UART_OK;
}
