/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation.
 *
 * @author      Juergen Fitschen <me@jue.yt>
 * @}
 */

#include <assert.h>

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"

#include "pm_layered.h"
#include "bitarithm.h"

#include "em_eusart.h"

/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t isr_ctx[UART_NUMOF];

/**
 * @brief   Marks blocked_pm_mode entry valid
 */
#define PM_MODE_SHALL_BLOCK BIT7
#define PM_MODE_BLOCKED BIT6

/**
 * @brief   Mask for storing the pm mode
 */
#define PM_MODE_MASK (BIT6 - 1)

#if IS_ACTIVE(MODULE_PM_LAYERED)
/**
 * @brief   Keep track of currently blocked blocked_pm_mode
 */
static uint8_t blocked_pm_mode[UART_NUMOF];

static inline void _clear_pm_mode(uart_t dev)
{
    /* unblock previously blocked mode */
    if (blocked_pm_mode[dev] & PM_MODE_BLOCKED) {
        pm_unblock(blocked_pm_mode[dev] & PM_MODE_MASK);
        CLRBIT(blocked_pm_mode[dev], PM_MODE_BLOCKED);
    }
}

static inline void _set_pm_mode(uart_t dev)
{
    /* Check if we shall block any pm mode at all */
    if (!(blocked_pm_mode[dev] & PM_MODE_SHALL_BLOCK)) {
        return;
    }

    /* block minimum required pm mode */
    if (!(blocked_pm_mode[dev] & PM_MODE_BLOCKED)) {
        pm_block(blocked_pm_mode[dev] & PM_MODE_MASK);
        SETBIT(blocked_pm_mode[dev], PM_MODE_BLOCKED);
    }
}

static inline void _setup_pm_mode(uart_t dev, uint8_t mode)
{
    assert(!(blocked_pm_mode[dev] & PM_MODE_BLOCKED));
    blocked_pm_mode[dev] = mode;
}
#else /* !IS_ACTIVE(MODULE_PM_LAYERED) */
static inline void _clear_pm_mode(uart_t dev) { (void) dev; }
static inline void _set_pm_mode(uart_t dev) { (void) dev; }
static inline void _setup_pm_mode(uart_t dev, uint8_t mode) { (void) dev; (void) mode; }
#endif /* !IS_ACTIVE(MODULE_PM_LAYERED) */

static void _uart_enable(uart_t dev)
{
    EUSART_TypeDef *uart = uart_config[dev].dev;

    /* enable tx */
    EUSART_Enable_TypeDef enable = eusartEnableTx;

    /* enable rx if needed */
    if (isr_ctx[dev].rx_cb) {
        enable |= eusartEnableRx;
    }

    EUSART_Enable(uart, enable);
}

static void _uart_disable(uart_t dev)
{
    EUSART_TypeDef *uart = uart_config[dev].dev;

    /* disable tx and rx */
    EUSART_Enable(uart, eusartDisable);
}

#define GET_PIN(x) (x & 0xf)
#define GET_PORT(x) (x >> 4)

int uart_init(uart_t dev, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* check if device is valid */
    if (dev >= UART_NUMOF) {
        return -1;
    }

    /* save interrupt callback context */
    isr_ctx[dev].rx_cb = rx_cb;
    isr_ctx[dev].arg = arg;

    EUSART_TypeDef *uart = (EUSART_TypeDef *) uart_config[dev].dev;

    /* select and enable clock */
    CMU_ClockEnable(uart_config[dev].cmu, true);

    /* initialize the pins */
    gpio_init(uart_config[dev].rx_pin, GPIO_IN_PU);
    gpio_init(uart_config[dev].tx_pin, GPIO_OUT | 1); /* 1 for high */

    /* reset and initialize peripheral */
    /* the clock source defines whether the periph is initialized in LF or HF mode */
    CMU_Select_TypeDef src = CMU_ClockSelectGet(uart_config[dev].cmu);
    if ((src == cmuSelect_LFRCO) ||
        (src == cmuSelect_ULFRCO) ||
        (src == cmuSelect_LFXO)) {
        EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_LF;
        init.enable = eusartDisable;
        init.baudrate = baudrate;
        EUSART_UartInitLf(uart, &init);
    } else {
        EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;
        init.enable = eusartDisable;
        init.baudrate = baudrate;
        EUSART_UartInitHf(uart, &init);
    }

    /* route pings to eusart periph */
    GPIO->EUSARTROUTE[EUSART_NUM(uart)].TXROUTE =
        (GET_PORT(uart_config[dev].tx_pin) << _GPIO_EUSART_TXROUTE_PORT_SHIFT) |
        (GET_PIN(uart_config[dev].tx_pin) << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[EUSART_NUM(uart)].RXROUTE =
        (GET_PORT(uart_config[dev].rx_pin) << _GPIO_EUSART_RXROUTE_PORT_SHIFT) |
        (GET_PIN(uart_config[dev].rx_pin) << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[EUSART_NUM(uart)].ROUTEEN =
        (GPIO_EUSART_ROUTEEN_RXPEN | GPIO_EUSART_ROUTEEN_TXPEN);

    /* clear previously set pm_mode */
    _clear_pm_mode(dev);
    _setup_pm_mode(dev, 0);

    /* enable the interrupt */
    if (rx_cb) {
        /* If we want to receive data from the UART,
         * we have to make sure to stick with the right pm mode! */
        switch (src) {
            case cmuSelect_ULFRCO:
                /* NOP ... EM3 is the lowest available pm mode */
                break;
            case cmuSelect_LFRCO:
            case cmuSelect_LFXO:
                /* We must stay in EM2 for LFXO / LFRCO -> block EM3*/
                _setup_pm_mode(dev, PM_MODE_SHALL_BLOCK | EFM32_PM_MODE_EM3);
                break;
            default:
                /* We must stay in EM0/1 for all other clock -> block EM2*/
                _setup_pm_mode(dev, PM_MODE_SHALL_BLOCK | EFM32_PM_MODE_EM2);
        }

        /* Enable RX FIFO Level IRQ */
        /* CFG1->RXFIW is set to one frame after reset */
        EUSART_IntEnable(uart, EUSART_IEN_RXFL);
        NVIC_ClearPendingIRQ(uart_config[dev].irq);
        NVIC_EnableIRQ(uart_config[dev].irq);
    }

    uart_poweron(dev);

    return 0;
}

#ifdef MODULE_PERIPH_UART_MODECFG
int uart_mode(uart_t dev, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits)
{
    EUSART_TypeDef *uart = uart_config[dev].dev;

    if (data_bits >= UART_MODE_UNSUPPORTED || parity >= UART_MODE_UNSUPPORTED) {
        return UART_NOMODE;
    }

    /* uart must be disabled to alter modes */
    _uart_disable(dev);

    uart->FRAMECFG = (data_bits << _EUSART_FRAMECFG_DATABITS_SHIFT)
                   | (parity << _EUSART_FRAMECFG_PARITY_SHIFT)
                   | (stop_bits << _EUSART_FRAMECFG_STOPBITS_SHIFT);

    _uart_enable(dev);

    return UART_OK;
}
#endif

void uart_poweron(uart_t dev)
{
    CMU_ClockEnable(uart_config[dev].cmu, true);

    _uart_enable(dev);

    /* block power mode */
    _set_pm_mode(dev);
}

void uart_poweroff(uart_t dev)
{
    /* unblock power mode */
    _clear_pm_mode(dev);

    _uart_disable(dev);

    CMU_ClockEnable(uart_config[dev].cmu, false);
}

void uart_write(uart_t dev, const uint8_t *data, size_t len)
{
    EUSART_TypeDef *uart = uart_config[dev].dev;

    /* When the system goes into EM2 or EM3, all EUSARTs located in PD1 will be
     * disabled automatically. We can detect this by querying the transmitter
     * enable bit and re-enable the EUSART on-the-fly. */
    if (!(uart->STATUS & EUSART_STATUS_TXENS)) {
        uart_poweron(dev);
    }

    while (len--) {
        EUSART_Tx(uart, *(data++));
    }

    /* spin until transmission is complete */
    while (!(EUSART_StatusGet(uart) & EUSART_STATUS_TXC)) {}
}

static void rx_irq(uart_t dev)
{
    EUSART_TypeDef *uart = uart_config[dev].dev;

    if (EUSART_IntGetEnabled(uart) & EUSART_IF_RXFL) {
        while (EUSART_StatusGet(uart) & EUSART_STATUS_RXFL) {
            uint8_t c = EUSART_Rx(uart);
            if (isr_ctx[dev].rx_cb) {
                isr_ctx[dev].rx_cb(isr_ctx[dev].arg, c);
            }
        }
        EUSART_IntClear(uart, EUSART_IF_RXFL);
    }

    cortexm_isr_end();
}

#ifdef UART_0_ISR_RX
void UART_0_ISR_RX(void)
{
    rx_irq(0);
}
#endif

#ifdef UART_1_ISR_RX
void UART_1_ISR_RX(void)
{
    rx_irq(1);
}
#endif

#ifdef UART_2_ISR_RX
void UART_2_ISR_RX(void)
{
    rx_irq(2);
}
#endif

#ifdef UART_3_ISR_RX
void UART_3_ISR_RX(void)
{
    rx_irq(3);
}
#endif

#ifdef UART_4_ISR_RX
void UART_4_ISR_RX(void)
{
    rx_irq(4);
}
#endif
