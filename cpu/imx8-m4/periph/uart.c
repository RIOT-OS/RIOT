/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_imx8-m4
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Dylan Laduranty <dylanladuranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph/uart.h"
#include "periph/gpio.h"
#include "clk_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SRC_CLK     80000000U


/**
 * @brief   Allocate memory to store the callback functions
 */
static uart_isr_ctx_t uart_ctx[UART_NUMOF];

static inline UART_Type *dev(uart_t dev)
{
    return uart_config[dev].dev;
}

static inline int uart_id(uart_t inst)
{
    uint32_t addr = (uint32_t)dev(inst);
    switch(addr) {
        case UART1_BASE:
                    return 1;
        case UART2_BASE:
                    return 2;
        case UART3_BASE:
                    return 3;
        case UART4_BASE:
                    return 4;
        default:
                    return -1;
    }
}
 int _uart_set_baudrate(uart_t uart, uint32_t baudrate)
{
    uint32_t numerator = 0u;
    uint32_t denominator = 0U;
    uint32_t divisor = 0U;
    uint32_t refFreqDiv = 0U;
    uint32_t divider = 1U;
    uint64_t baudDiff = 0U;
    uint64_t tempNumerator = 0U;
    uint32_t tempDenominator = 0u;

    /* get the approximately maximum divisor */
    numerator = SRC_CLK;
    denominator = baudrate << 4;
    divisor = 1;

    while (denominator != 0)
    {
        divisor = denominator;
        denominator = numerator % denominator;
        numerator = divisor;
    }

    numerator = SRC_CLK / divisor;
    denominator = (baudrate << 4) / divisor;

    /* numerator ranges from 1 ~ 7 * 64k */
    /* denominator ranges from 1 ~ 64k */
    if ((numerator > (UART_UBIR_INC_MASK * 7)) || (denominator > UART_UBIR_INC_MASK))
    {
        uint32_t m = (numerator - 1) / (UART_UBIR_INC_MASK * 7) + 1;
        uint32_t n = (denominator - 1) / UART_UBIR_INC_MASK + 1;
        uint32_t max = m > n ? m : n;
        numerator /= max;
        denominator /= max;
        if (0 == numerator)
        {
            numerator = 1;
        }
        if (0 == denominator)
        {
            denominator = 1;
        }
    }
    divider = (numerator - 1) / UART_UBIR_INC_MASK + 1;

    switch (divider)
    {
        case 1:
            refFreqDiv = 0x05;
            break;
        case 2:
            refFreqDiv = 0x04;
            break;
        case 3:
            refFreqDiv = 0x03;
            break;
        case 4:
            refFreqDiv = 0x02;
            break;
        case 5:
            refFreqDiv = 0x01;
            break;
        case 6:
            refFreqDiv = 0x00;
            break;
        case 7:
            refFreqDiv = 0x06;
            break;
        default:
            refFreqDiv = 0x05;
            break;
    }
    /* Compare the difference between baudRate_Bps and calculated baud rate.
     * Baud Rate = Ref Freq / (16 * (UBMR + 1)/(UBIR+1)).
     * baudDiff = (srcClock_Hz/divider)/( 16 * ((numerator / divider)/ denominator).
     */
    tempNumerator = SRC_CLK;
    tempDenominator = (numerator << 4);
    divisor = 1;
    /* get the approximately maximum divisor */
    while (tempDenominator != 0)
    {
        divisor = tempDenominator;
        tempDenominator = tempNumerator % tempDenominator;
        tempNumerator = divisor;
    }
    tempNumerator = SRC_CLK / divisor;
    tempDenominator = (numerator << 4) / divisor;
    baudDiff = (tempNumerator * denominator) / tempDenominator;
    baudDiff = (baudDiff >= baudrate) ? (baudDiff - baudrate) : (baudrate - baudDiff);

    if (baudDiff < (baudrate / 100) * 3)
    {
        dev(uart)->UFCR &= ~UART_UFCR_RFDIV_MASK;
        dev(uart)->UFCR |= UART_UFCR_RFDIV(refFreqDiv);
        dev(uart)->UBIR = UART_UBIR_INC(denominator - 1);
        dev(uart)->UBMR = UART_UBMR_MOD(numerator / divider - 1);
        dev(uart)->ONEMS = UART_ONEMS_ONEMS(SRC_CLK / (1000 * divider));

        return 0;
    }
    else
    {
        return -1;
    }
}

static inline void _gpio_config(gpio_t pin, uint32_t val)
{
    if((pin & 0xff) == 24) {
        (*(uint32_t*) 0x303304A4) = val;
    }
    else {
         (*(uint32_t*) 0x303304A8) = val;
    }
}

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    (void) rx_cb;
    (void) arg;
    (void) uart;
    (void) baudrate;

    /* Enable uart clock */
    ccm_set_root_clk(CCM_ROOT_CLK_UART2, 1, 1, 1);
    ccm_set_ccgr(CCM_CCGR_UART2, CLK_ENABLE_ALL_DOMAINS);
    ccm_en_root_clk(CCM_ROOT_CLK_UART2, 1);

    /* Reset peripheral */
    dev(uart)->UCR1 &= ~UART_UCR1_UARTEN_MASK;
    dev(uart)->UCR2 &= ~UART_UCR2_SRST_MASK;
    while ((dev(uart)->UCR2 & UART_UCR2_SRST_MASK) == 0) {}

    /* Set pins mux */
    gpio_init_mux(uart_config[uart].rx_pin, 0);
    gpio_init_mux(uart_config[uart].tx_pin, 0);

    _gpio_config(uart_config[uart].rx_pin, IOMUXC_SW_PAD_CTL_PAD_DSE(6U) | 
                                           IOMUXC_SW_PAD_CTL_PAD_SRE(1U) | 
                                           IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);

    _gpio_config(uart_config[uart].tx_pin, IOMUXC_SW_PAD_CTL_PAD_DSE(6U) |
                                           IOMUXC_SW_PAD_CTL_PAD_SRE(1U) |
                                           IOMUXC_SW_PAD_CTL_PAD_PUE_MASK);
    /* Set UART parameters */
    dev(uart)->UCR2 = UART_UCR2_WS(1) |
                       UART_UCR2_STPB(0) |
                      /* (((0) << UART_UCR2_PROE_SHIFT) & (UART_UCR2_PREN_MASK |
                       UART_UCR2_PROE_MASK)) |*/
                       UART_UCR2_TXEN(1) |
                       UART_UCR2_RXEN(1) |
                       UART_UCR2_IRTS_MASK;

    /* IMX8 datasheet states
    NOTE: In this chip, UARTs are used in MUXED mode, so that this bit
    should always be set.*/
    dev(uart)->UCR3 |= UART_UCR3_RXDMUXSEL_MASK;

    /* Disable baudrate autodetection */
    dev(uart)->UCR1 &= ~UART_UCR1_ADBR_MASK;

    /* Set baudrate */
    _uart_set_baudrate(uart, baudrate);
    /* Enable UART peripheral */
    dev(uart)->UCR1 |= UART_UCR1_UARTEN_MASK;

    /* Enable RX interrupt */
    if ((rx_cb) && (uart_config[uart].rx_pin != GPIO_UNDEF)) {
        uart_ctx[uart].rx_cb = rx_cb;
        uart_ctx[uart].arg = arg;
        dev(uart)->UCR1 |= UART_UCR1_RRDYEN_MASK;
        NVIC_EnableIRQ(UART1_IRQn + uart_id(uart) - 1);
    }
     return UART_OK;
}



void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    for(uint16_t i=0; i < len; i++) {
        while (!(dev(uart)->USR2 & UART_USR2_TXDC_MASK)) {}
        dev(uart)->UTXD = data[i] & UART_UTXD_TX_DATA_MASK;
    }
}

void uart_poweron(uart_t uart)
{
    (void)uart;
}

void uart_poweroff(uart_t uart)
{
    (void)uart;
}
static inline void irq_handler(unsigned uartnum)
{
    /* Frame error */
    if (dev(uartnum)->USR1 & UART_USR1_FRAMERR_MASK)
    {
        dev(uartnum)->USR1 |= UART_USR1_FRAMERR_MASK;
    }
    /* Handle received bytes */
    if (dev(uartnum)->USR1 & UART_USR1_RRDY_MASK)
    {
        /* interrupt flag is cleared by reading the data register */
        uart_ctx[uartnum].rx_cb(uart_ctx[uartnum].arg,
                                (uint8_t)(dev(uartnum)->URXD & UART_URXD_RX_DATA_MASK));
    }
    cortexm_isr_end();
}

#ifdef UART_0_ISR
void UART_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef UART_1_ISR
void UART_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef UART_2_ISR
void UART_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef UART_3_ISR
void UART_3_ISR(void)
{
    irq_handler(3);
}
#endif



