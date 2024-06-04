#include "irq.h"
#include "macros/math.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph_cpu.h"

const msp430_usci_uart_params_t usci_a0_as_uart = {
    .usci_params = {
        .id = MSP430_USCI_ID_A0,
        .dev = MSP430_USCI_B_FROM_USCI_A(&USCI_A0),
        .interrupt_flag = &UC0IFG,
        .interrupt_enable = &UC0IE,
        .tx_irq_mask = UCA0TXIE,
        .rx_irq_mask = UCA0RXIE,
    },
    .txd = GPIO_PIN(P3, 4),
    .rxd = GPIO_PIN(P3, 5),
};

const msp430_usci_uart_params_t usci_a1_as_uart = {
    .usci_params = {
        .id = MSP430_USCI_ID_A1,
        .dev = MSP430_USCI_B_FROM_USCI_A(&USCI_A1),
        .interrupt_flag = &UC1IFG,
        .interrupt_enable = &UC1IE,
        .tx_irq_mask = UCA1TXIE,
        .rx_irq_mask = UCA1RXIE,
    },
    .txd = GPIO_PIN(P3, 6),
    .rxd = GPIO_PIN(P3, 7),
};

const msp430_usci_spi_params_t usci_a0_as_spi = {
    .usci_params = {
        .id = MSP430_USCI_ID_A0,
        .dev = MSP430_USCI_B_FROM_USCI_A(&USCI_A0),
        .interrupt_flag = &UC0IFG,
        .interrupt_enable = &UC0IE,
        .tx_irq_mask = UCA0TXIE,
        .rx_irq_mask = UCA0RXIE,
    },
    .mosi = GPIO_PIN(P3, 4),
    .miso = GPIO_PIN(P3, 5),
    .sck = GPIO_PIN(P3, 0),
};

const msp430_usci_spi_params_t usci_a1_as_spi = {
    .usci_params = {
        .id = MSP430_USCI_ID_A1,
        .dev = MSP430_USCI_B_FROM_USCI_A(&USCI_A1),
        .interrupt_flag = &UC1IFG,
        .interrupt_enable = &UC1IE,
        .tx_irq_mask = UCA1TXIE,
        .rx_irq_mask = UCA1RXIE,
    },
    .mosi = GPIO_PIN(P3, 7),
    .miso = GPIO_PIN(P3, 6),
    .sck = GPIO_PIN(P5, 0),
};

const msp430_usci_spi_params_t usci_b0_as_spi = {
    .usci_params = {
        .id = MSP430_USCI_ID_B0,
        .dev = &USCI_B0,
        .interrupt_flag = &UC0IFG,
        .interrupt_enable = &UC0IE,
        .tx_irq_mask = UCB0TXIE,
        .rx_irq_mask = UCB0RXIE,
    },
    .mosi = GPIO_PIN(P3, 1),
    .miso = GPIO_PIN(P3, 2),
    .sck = GPIO_PIN(P3, 3),
};

const msp430_usci_spi_params_t usci_b1_as_spi = {
    .usci_params = {
        .id = MSP430_USCI_ID_B1,
        .dev = &USCI_B1,
        .interrupt_flag = &UC1IFG,
        .interrupt_enable = &UC1IE,
        .tx_irq_mask = UCB1TXIE,
        .rx_irq_mask = UCB1RXIE,
    },
    .mosi = GPIO_PIN(P5, 1),
    .miso = GPIO_PIN(P5, 2),
    .sck = GPIO_PIN(P5, 3),
};

static mutex_t _usci_locks[MSP430_USCI_ID_NUMOF] = {
    MUTEX_INIT,
    MUTEX_INIT,
    MUTEX_INIT,
    MUTEX_INIT,
};

static uint8_t _auxiliary_clock_acquired;

void msp430_usci_acquire(const msp430_usci_params_t *params,
                         const msp430_usci_conf_t *conf)
{
    assume((unsigned)params->id < MSP430_USCI_ID_NUMOF);

    mutex_lock(&_usci_locks[params->id]);
    msp430_usci_b_t *dev = params->dev;

    /* We only need to acquire the auxiliary (low frequency) clock domain, as
     * the subsystem main clock (SMCLK) will be acquired on-demand when activity
     * is detected on RXD, as per datasheet:
     *
     * > The USCI module provides automatic clock activation for SMCLK for use
     * > with low-power modes.
     */
    switch (conf->prescaler.clk_source) {
    case USCI_CLK_AUX:
        msp430_clock_acquire(MSP430_CLOCK_AUXILIARY);
        _auxiliary_clock_acquired |= 1U << params->id;
        break;
    default:
        _auxiliary_clock_acquired &= ~(1U << params->id);
        break;
    }

    /* put device in disabled/reset state */
    dev->CTL1 = UCSWRST;

    /* apply given configuration */
    dev->CTL0 = conf->ctl0;
    dev->CTL1 = conf->prescaler.clk_source | UCSWRST;
    dev->BR0 = conf->prescaler.br0;
    dev->BR1 = conf->prescaler.br1;
    dev->MCTL = conf->prescaler.mctl;

    /* disable USCI IRQs and clear any spurious IRQ flags */
    uint8_t clear_irq_mask = ~(params->tx_irq_mask | params->rx_irq_mask);
    unsigned irq_mask = irq_disable();
    *params->interrupt_flag &= clear_irq_mask;
    *params->interrupt_enable &= clear_irq_mask;
    irq_restore(irq_mask);
}

void msp430_usci_release(const msp430_usci_params_t *params)
{
    assume(params->id < MSP430_USCI_ID_NUMOF);

    msp430_usci_b_t *dev = params->dev;

    /* Disable USCI */
    dev->CTL0 = UCSWRST;

    /* disable USCI IRQs and clear any spurious IRQ flags */
    uint8_t clear_irq_mask = ~(params->tx_irq_mask | params->rx_irq_mask);
    unsigned irq_mask = irq_disable();
    *params->interrupt_enable &= clear_irq_mask;
    *params->interrupt_flag &= clear_irq_mask;

    if (_auxiliary_clock_acquired & (1U << params->id)) {
        msp430_clock_release(MSP430_CLOCK_AUXILIARY);
    }
    irq_restore(irq_mask);

    /* Release mutex */
    mutex_unlock(&_usci_locks[params->id]);
}

msp430_usci_prescaler_t msp430_usci_prescale(uint32_t target_hz)
{
    msp430_usci_prescaler_t result = {
        .mctl = 0,
        .clk_source = USCI_CLK_SUBMAIN,
    };

    /* If a watch crystal is used for the auxiliary clock, allow using the
     * auxiliary clock to be used as clock source for well-known
     * symbol rates, so that enabling low power modes is possible while
     * UART RX is active */
    if ((clock_params.lfxt1_frequency == 32768)
            && (clock_params.auxiliary_clock_divier == AUXILIARY_CLOCK_DIVIDE_BY_1)) {
        assert(msp430_auxiliary_clock_freq() == 32768);
        result.clk_source = USCI_CLK_AUX;
        /* The datasheet gives a formula that is used to estimate BRS, but
         * for optimal accuracy "a detailed error calculation must be performed
         * for each bit for each UCBRSx setting". We take the pre-calculated
         * optimal values from the datasheet here. The idea is that if the
         * clock source is slow ticking, the extra bit timing accuracy may
         * be needed. Otherwise the estimation will be good enough.
         */
        switch (target_hz) {
        case 1200:
            result.mctl = 2U << UCBRS_Pos;
            result.br0 = 27;
            return result;
        case 2400:
            result.mctl = 6U << UCBRS_Pos;
            result.br0 = 13;
            return result;
        case 4800:
            result.mctl = 7U << UCBRS_Pos;
            result.br0 = 6;
            return result;
        case 9600:
            result.mctl = 3U << UCBRS_Pos;
            result.br0 = 3;
            return result;
        }
    }

    /* Otherwise, we compute BR and estimate BRS. We shift left by 7 to avoid
     * floating point arithmetic. (7 is the largest shit amount for which
     * clock frequencies with two-digit values in MHz don't exceed the 32 bit
     * value range.) */
    uint32_t tmp = DIV_ROUND(msp430_submain_clock_freq() << 7, target_hz);
    /* BR is the integral part */
    uint16_t br = tmp >> 7;
    /* BRS is the fractional part multiplied by 8. We combine the multiplication
     * by 8 (left-shift by 3) with the right-shift by 7 here to a right-shift
     * by 4. */
    uint8_t brs = (tmp & 0x7f) >> 4;
    result.clk_source = USCI_CLK_SUBMAIN;

    result.br0 = (uint8_t)br;
    result.br1 = (uint8_t)(br >> 8);
    result.mctl = brs << UCBRS_Pos;

    return result;
}
