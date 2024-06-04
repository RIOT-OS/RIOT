#include "macros/math.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph_cpu.h"

const msp430_usart_uart_params_t usart0_as_uart = {
    .usart_params = {
        .num = 0,
        .dev = &USART_0,
        .sfr = &USART_0_SFR,
        .tx_irq_mask = UTXE0,
        .rx_irq_mask = URXE0,
    },
    .tx_enable_mask = UTXE0,
    .rxtx_enable_mask = URXE0 | UTXE0,
    .txd = GPIO_PIN(P3, 4),
    .rxd = GPIO_PIN(P3, 5),
};

const msp430_usart_uart_params_t usart1_as_uart = {
    .usart_params = {
        .num = 1,
        .dev = &USART_1,
        .sfr = &USART_1_SFR,
        .tx_irq_mask = UTXE1,
        .rx_irq_mask = URXE1,
    },
    .tx_enable_mask = UTXE1,
    .rxtx_enable_mask = URXE1 | UTXE1,
    .txd = GPIO_PIN(P3, 6),
    .rxd = GPIO_PIN(P3, 7),
};

const msp430_usart_spi_params_t usart0_as_spi = {
    .usart_params = {
        .num = 0,
        .dev = &USART_0,
        .sfr = &USART_0_SFR,
        .tx_irq_mask = UTXE0,
        .rx_irq_mask = URXE0,
    },
    .enable_mask = USPIE0,
    .mosi = GPIO_PIN(P3, 1),
    .miso = GPIO_PIN(P3, 2),
    .sck = GPIO_PIN(P3, 3),
};

const msp430_usart_spi_params_t usart1_as_spi = {
    .usart_params = {
        .num = 1,
        .dev = &USART_1,
        .sfr = &USART_1_SFR,
        .tx_irq_mask = UTXE1,
        .rx_irq_mask = URXE1,
    },
    .enable_mask = USPIE1,
    .mosi = GPIO_PIN(P5, 1),
    .miso = GPIO_PIN(P5, 2),
    .sck = GPIO_PIN(P5, 3),
};

static mutex_t usart_locks[USART_NUMOF] = {
    MUTEX_INIT,
    MUTEX_INIT,
};

/* store the clock acquired by each USART, so it can be release again */
static msp430_usart_clk_t _clocks_acquired[USART_NUMOF];

void msp430_usart_acquire(const msp430_usart_params_t *params,
                          const msp430_usart_conf_t *conf,
                          uint8_t enable_mask)
{
    assume(params->num < USART_NUMOF);

    mutex_lock(&usart_locks[params->num]);
    msp430_usart_t *dev = params->dev;
    msp430_usart_sfr_t *sfr = params->sfr;

    _clocks_acquired[params->num] = conf->prescaler.clk_source;
    switch (_clocks_acquired[params->num]) {
    case USART_CLK_SUBMAIN:
        msp430_clock_acquire(MSP430_CLOCK_SUBMAIN);
        break;
    case USART_CLK_AUX:
        msp430_clock_acquire(MSP430_CLOCK_AUXILIARY);
        break;
    default:
        /* external clock from GPIO, safe to disable internal clocks */
        break;
    }

    /* first, make sure USART is off before reconfiguring it */
    sfr->ME = 0;
    /* reset USART */
    dev->CTL = SWRST;

    /* apply given configuration */
    dev->CTL = conf->ctl | SWRST;
    dev->MCTL = conf->prescaler.mctl;
    dev->TCTL = conf->prescaler.clk_source;
    dev->BR0 = conf->prescaler.br0;
    dev->BR1 = conf->prescaler.br1;

    /* disable USART IRQs and clear any spurious IRQ flags */
    sfr->IE = 0;
    sfr->IFG = 0;
    /* enable USART as specified */
    sfr->ME = enable_mask;
}

void msp430_usart_release(const msp430_usart_params_t *params)
{
    assume(params->num < USART_NUMOF);

    msp430_usart_sfr_t *sfr = params->sfr;

    /* Disable USART */
    sfr->ME = 0;
    /* disable USART IRQs and clear any spurious IRQ flags */
    sfr->IE = 0;
    sfr->IFG = 0;

    switch (_clocks_acquired[params->num]) {
    case USART_CLK_SUBMAIN:
        msp430_clock_release(MSP430_CLOCK_SUBMAIN);
        break;
    case USART_CLK_AUX:
        msp430_clock_release(MSP430_CLOCK_AUXILIARY);
        break;
    default:
        /* external clock from GPIO, not managed here */
        break;
    }

    /* Release mutex */
    mutex_unlock(&usart_locks[params->num]);
}

msp430_usart_prescaler_t msp430_usart_prescale(uint32_t clock, uint16_t min_br)
{
    msp430_usart_prescaler_t result = { .mctl = 0 };
    uint32_t clk_hz;

    /* If a watch crystal is used for the auxiliary clock, allow using the
     * auxiliary clock to be used as clock source for well-known
     * symbol rates, so that enabling low power modes is possible while
     * UART RX is active */
    if ((clock_params.lfxt1_frequency == 32768)
            && (clock_params.auxiliary_clock_divier == AUXILIARY_CLOCK_DIVIDE_BY_1)) {
        clk_hz = msp430_auxiliary_clock_freq();
        assume(clk_hz == 32768);
        result.clk_source = USART_CLK_AUX;
        /* Rather than calculating the correct modulation control register
         * values, just hard-code it for four well-known symbol rates. If the
         * symbol rate is something else, we go for the high frequency
         * subsystem main clock, where bit timings are easier to hit even
         * without fine-tuning it via the modulation control register */
        switch (clock) {
        case 9600:
            result.mctl = 0x4a;
            break;
        case 4800:
            result.mctl = 0x6f;
            break;
        case 2400:
            result.mctl = 0x6b;
            break;
        case 1200:
            result.mctl = 0x03;
            break;
        default:
            clk_hz = msp430_submain_clock_freq();
            result.clk_source = USART_CLK_SUBMAIN;
        }
    }
    else {
        clk_hz = msp430_submain_clock_freq();
        result.clk_source = USART_CLK_SUBMAIN;
    }

    uint16_t br = DIV_ROUND(clk_hz, clock);

    if (br < min_br) {
        br = min_br;
    }

    result.br0 = (uint8_t)br;
    result.br1 = (uint8_t)(br >> 8);
    return result;
}
