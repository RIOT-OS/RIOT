#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "periph/timer.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define PSOC6_TIMER_INTR_TC_MASK TCPWM_CNT_INTR_TC_Msk

#if defined(TCPWM_CNT_INTR_CC_MATCH_Msk)
#define PSOC6_TIMER_INTR_CC_MASK TCPWM_CNT_INTR_CC_MATCH_Msk
#else
#define PSOC6_TIMER_INTR_CC_MASK TCPWM_CNT_INTR_CC_Msk
#endif

static timer_isr_ctx_t _timer_isr_ctx[TIMER_NUMOF];

// cfg tcpwm channel in up mode
static void _counter_setup(const timer_conf_t *cfg, uint32_t chan)
{
    cfg->dev->CNT[chan].CTRL = _VAL2FLD(TCPWM_CNT_CTRL_GENERIC, 0U) |
                               _VAL2FLD(TCPWM_CNT_CTRL_ONE_SHOT, 0U) |
                               _VAL2FLD(TCPWM_CNT_CTRL_UP_DOWN_MODE, 0U) |
                               _VAL2FLD(TCPWM_CNT_CTRL_MODE, 0U);
    cfg->dev->CNT[chan].TR_CTRL0 = _VAL2FLD(TCPWM_CNT_TR_CTRL0_CAPTURE_SEL, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL0_COUNT_SEL, 1U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL0_RELOAD_SEL, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL0_STOP_SEL, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL0_START_SEL, 0U);
    cfg->dev->CNT[chan].TR_CTRL1 = _VAL2FLD(TCPWM_CNT_TR_CTRL1_CAPTURE_EDGE, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL1_COUNT_EDGE, 3U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL1_RELOAD_EDGE, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL1_STOP_EDGE, 0U) |
                                   _VAL2FLD(TCPWM_CNT_TR_CTRL1_START_EDGE, 0U);
    cfg->dev->CNT[chan].TR_CTRL2 = 0U;
    cfg->dev->CNT[chan].COUNTER = 0U;
    cfg->dev->CNT[chan].PERIOD = TIMER_0_MAX_VALUE;
    cfg->dev->CNT[chan].CC = TIMER_0_MAX_VALUE;
    cfg->dev->CNT[chan].INTR = PSOC6_TIMER_INTR_TC_MASK | PSOC6_TIMER_INTR_CC_MASK;
    cfg->dev->CNT[chan].INTR_MASK = 0U;
}

static void _alarm_clear(tim_t tim)
{
    const timer_conf_t *cfg = &timer_config[tim];

    // stop alarm chan + clr pending irq
    cfg->dev->CTRL_CLR = (1UL << cfg->alarm_chan);
    cfg->dev->CNT[cfg->alarm_chan].INTR_MASK = 0U;
    cfg->dev->CNT[cfg->alarm_chan].INTR = PSOC6_TIMER_INTR_TC_MASK |
                                          PSOC6_TIMER_INTR_CC_MASK;
}

static void _alarm_set(tim_t tim, uint32_t ticks)
{
    const timer_conf_t *cfg = &timer_config[tim];

    // re-arm one-shot alarm chan
    cfg->dev->CTRL_CLR = (1UL << cfg->alarm_chan);
    cfg->dev->CNT[cfg->alarm_chan].COUNTER = 0U;
    cfg->dev->CNT[cfg->alarm_chan].PERIOD = ticks;
    cfg->dev->CNT[cfg->alarm_chan].INTR = PSOC6_TIMER_INTR_TC_MASK |
                                          PSOC6_TIMER_INTR_CC_MASK;
    cfg->dev->CNT[cfg->alarm_chan].INTR_MASK = PSOC6_TIMER_INTR_TC_MASK;
    cfg->dev->CTRL_SET = (1UL << cfg->alarm_chan);
    cfg->dev->CMD_START = (1UL << cfg->alarm_chan);
}

static void _timer_irq_handler(tim_t tim)
{
    const timer_conf_t *cfg = &timer_config[tim];
    uint32_t intr = cfg->dev->CNT[cfg->alarm_chan].INTR_MASKED;

    // only alarm chan emits callbacks
    if ((intr & PSOC6_TIMER_INTR_TC_MASK) != 0U) {
        cfg->dev->CNT[cfg->alarm_chan].INTR = PSOC6_TIMER_INTR_TC_MASK;
        _alarm_clear(tim);
        if (_timer_isr_ctx[tim].cb) {
            _timer_isr_ctx[tim].cb(_timer_isr_ctx[tim].arg, 0);
        }
    }
    else if (intr != 0U) {
        cfg->dev->CNT[cfg->alarm_chan].INTR = intr;
    }

    cortexm_isr_end();
}

void NvicMux0_IRQHandler(void)
{
    _timer_irq_handler(TIMER_DEV(0));
}

void tcpwm_0_interrupts_0_IRQHandler(void)
{
    _timer_irq_handler(TIMER_DEV(0));
}

void tcpwm_0_interrupts_1_IRQHandler(void)
{
    _timer_irq_handler(TIMER_DEV(0));
}

// init timer + start timebase
int timer_init(tim_t tim, uint32_t freq, timer_cb_t cb, void *arg)
{
    if ((tim >= TIMER_NUMOF) || (freq == 0U) || (freq > PERI_CLK) || ((PERI_CLK % freq) != 0U)) {
        return -1;
    }

    const timer_conf_t *cfg = &timer_config[tim];
    uint32_t divider = PERI_CLK / freq;

    if ((divider == 0U) || (divider > 256U)) {
        return -1;
    }

    _timer_isr_ctx[tim].cb = cb;
    _timer_isr_ctx[tim].arg = arg;

    cfg->dev->CTRL_CLR = (1UL << cfg->timebase_chan) | (1UL << cfg->alarm_chan);
    PERI->CLOCK_CTL[cfg->pclk_timebase] =
        _VAL2FLD(CY_PERI_CLOCK_CTL_TYPE_SEL, 0U) |
        _VAL2FLD(CY_PERI_CLOCK_CTL_DIV_SEL, cfg->div8_index);
    PERI->CLOCK_CTL[cfg->pclk_alarm] =
        _VAL2FLD(CY_PERI_CLOCK_CTL_TYPE_SEL, 0U) |
        _VAL2FLD(CY_PERI_CLOCK_CTL_DIV_SEL, cfg->div8_index);
    _CLR_SET_REG32(PERI->DIV_8_CTL[cfg->div8_index], PERI_DIV_8_CTL_INT8_DIV, divider - 1U);
    PERI->DIV_CMD = PERI_DIV_CMD_ENABLE_Msk |
                    CY_PERI_DIV_CMD_PA_TYPE_SEL_Msk |
                    CY_PERI_DIV_CMD_PA_DIV_SEL_Msk |
                    _VAL2FLD(CY_PERI_DIV_CMD_TYPE_SEL, 0U) |
                    _VAL2FLD(CY_PERI_DIV_CMD_DIV_SEL, cfg->div8_index);

    _counter_setup(cfg, cfg->timebase_chan);
    _counter_setup(cfg, cfg->alarm_chan);

#if defined(CPU_CORE_CORTEX_M0PLUS)
    // route tcpwm irq via cm0 mux
    CPUSS->CM0_SYSTEM_INT_CTL[cfg->sys_int_src] =
        _VAL2FLD(CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_IDX, cfg->cm0_cpu_int_idx) |
        CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Msk;
    cpu_patch_irq_vector(cfg->irqn, NvicMux0_IRQHandler);
#endif

    NVIC_ClearPendingIRQ(cfg->irqn);
    NVIC_EnableIRQ(cfg->irqn);
    NVIC_SetPriority(cfg->irqn, 1);

    cfg->dev->CTRL_SET = (1UL << cfg->timebase_chan);
    cfg->dev->CMD_START = (1UL << cfg->timebase_chan);
    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    if ((tim >= TIMER_NUMOF) || (channel != 0)) {
        return -1;
    }

    unsigned state = irq_disable();
    _alarm_set(tim, timeout ? timeout : 1U);
    irq_restore(state);
    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    if ((tim >= TIMER_NUMOF) || (channel != 0)) {
        return -1;
    }

    unsigned int now = timer_read(tim);
    int32_t diff = (int32_t)(value - now);
    return timer_set(tim, channel, (diff <= 0) ? 0U : (unsigned int)diff);
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    (void)flags;
    return timer_set(tim, channel, value);
}

int timer_clear(tim_t tim, int channel)
{
    if ((tim >= TIMER_NUMOF) || (channel != 0)) {
        return -1;
    }

    const timer_conf_t *cfg = &timer_config[tim];
    _alarm_clear(tim);
    cfg->dev->CMD_STOP = (1UL << cfg->alarm_chan);
    return 0;
}

unsigned int timer_read(tim_t tim)
{
    if (tim >= TIMER_NUMOF) {
        return 0U;
    }

    const timer_conf_t *cfg = &timer_config[tim];
    return (unsigned int)cfg->dev->CNT[cfg->timebase_chan].COUNTER;
}

void timer_start(tim_t tim)
{
    if (tim < TIMER_NUMOF) {
        const timer_conf_t *cfg = &timer_config[tim];
        cfg->dev->CTRL_SET = (1UL << cfg->timebase_chan);
        cfg->dev->CMD_START = (1UL << cfg->timebase_chan);
    }
}

void timer_stop(tim_t tim)
{
    if (tim < TIMER_NUMOF) {
        const timer_conf_t *cfg = &timer_config[tim];
        _alarm_clear(tim);
        cfg->dev->CTRL_CLR = (1UL << cfg->timebase_chan) | (1UL << cfg->alarm_chan);
        cfg->dev->CMD_STOP = (1UL << cfg->timebase_chan) | (1UL << cfg->alarm_chan);
    }
}
