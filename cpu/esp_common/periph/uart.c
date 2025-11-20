/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "uart_ll.h"

#ifdef MODULE_ESP_QEMU
#include "esp/uart_regs.h"
#endif /* MODULE_ESP_QEMU */

#define UART0           uart0
#define UART1           uart1
#define CPU_INUM_UART   ETS_UART_INUM

#else /* defined(CPU_ESP8266) */

#include "esp_cpu.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "esp_rom_uart.h"
#include "hal/clk_tree_ll.h"
#include "hal/uart_ll.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/periph_defs.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"
#include "soc/uart_pins.h"

#undef  UART_CLK_FREQ

#if CPU_FAM_ESP32 || CPU_FAM_ESP32S2 || CPU_FAM_ESP32S3 || CPU_FAM_ESP32C3
/* UART_CLK_FREQ corresponds to APB_CLK_FREQ for ESP32, ESP32-S2, ESP32-S3,
 * ESP32-C2 and ESP32-C3, which is a fixed frequency of 80 MHz. However,
 * this only applies to CPU clock frequencies of 80 MHz and above.
 * For lower CPU clock frequencies, the APB clock corresponds to the CPU clock
 * frequency. Therefore, we need to determine the actual UART clock frequency
 * from the actual APB clock frequency. */
#  define UART_CLK_FREQ rtc_clk_apb_freq_get() /* APB_CLK is used */
#elif CPU_FAM_ESP32C6
#  define UART_CLK_FREQ (CLK_LL_PLL_80M_FREQ_MHZ * MHZ)  /* PLL_F80M_CLK is used */
#elif CPU_FAM_ESP32H2
#  define UART_CLK_FREQ (CLK_LL_PLL_48M_FREQ_MHZ * MHZ)  /* PLL_F48M_CLK is used */
#else
#  error "Platform implementation is missing"
#endif

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

        esp_rom_output_tx_wait_idle(uart);
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
        uart_ll_clr_intsts_mask(_uarts[uart].regs, UART_LL_INTR_MASK);
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
            uint32_t int_st = uart_ll_get_intsts_mask(_uarts[uart].regs);
            DEBUG("%s uart=%d int_st=%08"PRIx32"\n", __func__, uart, int_st);

            if (_uarts[uart].used && (int_st & UART_RXFIFO_FULL_INT_ST)) {
                /* read one byte of data */
                uint8_t data = _uart_rx_one_char(uart);
                /* if registered, call the RX callback function */
                if (_uarts[uart].isr_ctx.rx_cb) {
                    _uarts[uart].isr_ctx.rx_cb(_uarts[uart].isr_ctx.arg, data);
                }
                /* clear interrupt flag */
                uart_ll_clr_intsts_mask(_uarts[uart].regs, UART_RXFIFO_FULL_INT_ST);
            }

            /* TODO handle other types of interrupts, for the moment just clear them */
            uart_ll_clr_intsts_mask(_uarts[uart].regs, UART_LL_INTR_MASK);
        }
    }

    irq_isr_exit();
}

/* receive one data byte with wait */
static uint8_t IRAM _uart_rx_one_char(uart_t uart)
{
#if defined(CPU_ESP8266) && defined(MODULE_ESP_QEMU)
    /* wait until at least von byte is in RX FIFO */
    while (!FIELD2VAL(UART_STATUS_RXFIFO_COUNT, UART(uart).STATUS)) {}

    /* read the lowest byte from RX FIFO register */
    return UART(uart).FIFO & 0xff; /* only bit 0 ... 7 */
#else
    /* wait until at least von byte is in RX FIFO */
    while (uart_ll_get_rxfifo_len(_uarts[uart].regs) == 0) {}

    uint8_t data;
    uart_ll_read_rxfifo(_uarts[uart].regs, &data, 1);

    return data;
#endif
}

/* send one data byte with wait */
static void _uart_tx_one_char(uart_t uart, uint8_t data)
{
    /* wait until at least one byte is available in the TX FIFO */
    while (!uart_ll_get_txfifo_len(_uarts[uart].regs)) {}

    /* send the byte by placing it in the TX FIFO using MPU */
#if defined(CPU_ESP8266) && defined(MODULE_ESP_QEMU)
    UART(uart).FIFO = data;
#else
    uart_ll_write_txfifo(_uarts[uart].regs, &data, 1);
#endif
}

static void _uart_intr_enable(uart_t uart)
{
    uart_ll_ena_intr_mask(_uarts[uart].regs, UART_RXFIFO_FULL_INT_RAW);
    uart_ll_clr_intsts_mask(_uarts[uart].regs, UART_RXFIFO_FULL_INT_RAW);

    _uarts[uart].used = true;

    DEBUG("%s %08"PRIx32"\n", __func__, uart_ll_get_intr_ena_status(_uarts[uart].regs));
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
    uart_ll_rxfifo_rst(_uarts[uart].regs);
    uart_ll_txfifo_rst(_uarts[uart].regs);

    if (_uarts[uart].isr_ctx.rx_cb) {
        /* since reading can only be done byte by byte, we set
           UART_RXFIFO_FULL_THRHD interrupt level to 1 byte */
        uart_ll_set_rxfifo_full_thr(_uarts[uart].regs, 1);

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
        esp_cpu_intr_set_handler(CPU_INUM_UART, _uart_intr_handler, NULL);
        esp_cpu_intr_enable(BIT(CPU_INUM_UART));
#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
        /* set interrupt level */
        esp_cpu_intr_set_priority(CPU_INUM_UART, 1);
#endif
#endif /* CPU_ESP8266 */
    }
}

static int _uart_set_baudrate(uart_t uart, uint32_t baudrate)
{
    DEBUG("%s uart=%d, rate=%" PRIu32 "\n", __func__, uart, baudrate);

    assert(uart < UART_NUMOF);

    /* wait until TX FIFO is empty */
    while (uart_ll_get_txfifo_len(_uarts[uart].regs) < UART_LL_FIFO_DEF_LEN) { }

    critical_enter();

    _uarts[uart].baudrate = baudrate;

    uart_ll_set_sclk(_uarts[uart].regs, (soc_module_clk_t)UART_SCLK_DEFAULT);
    uart_ll_set_baudrate(_uarts[uart].regs, _uarts[uart].baudrate, UART_CLK_FREQ);

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
        case UART_DATA_BITS_5: break;
        case UART_DATA_BITS_6: break;
        case UART_DATA_BITS_7: break;
        case UART_DATA_BITS_8: break;
        default: LOG_TAG_ERROR("uart", "invalid number of data bits\n");
                 critical_exit();
                 return UART_NOMODE;
    }
    uart_ll_set_data_bit_num(_uarts[uart].regs, (uart_word_length_t)data_bits);

    /* store changed number of data bits in configuration */
    _uarts[uart].data = data_bits;

    /* set number of stop bits */
    switch (stop_bits) {
        case UART_STOP_BITS_1: break;
        case UART_STOP_BITS_2: break;
        default: LOG_TAG_ERROR("uart", "invalid number of stop bits\n");
                 critical_exit();
                 return UART_NOMODE;
    }
    uart_ll_set_stop_bits(_uarts[uart].regs, stop_bits);

    /* store changed number of stop bits in configuration */
    _uarts[uart].stop = stop_bits;

    /* set parity mode */
    switch (parity) {
        case UART_PARITY_NONE: break;
        case UART_PARITY_EVEN: break;
        case UART_PARITY_ODD: break;
        default: LOG_TAG_ERROR("uart", "invalid or unsupported parity mode\n");
                 critical_exit();
                 return UART_NOMODE;
    }
    uart_ll_set_parity(_uarts[uart].regs, parity);

    /* store changed parity in configuration */
    _uarts[uart].parity = parity;

    critical_exit();

    return UART_OK;
}
