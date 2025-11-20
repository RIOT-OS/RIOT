/*
 * SPDX-FileCopyrightText: 2020 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/dac.h"
#include "periph/gpio.h"
#include "macros/units.h"

#define DAC_VAL(in) (in >> (16 - DAC_RES_BITS))

#ifndef CONFIG_SAM0_DAC_REFRESH
#define CONFIG_SAM0_DAC_REFRESH 2
#endif

#ifndef CONFIG_SAM0_DAC_RUN_ON_STANDBY
#define CONFIG_SAM0_DAC_RUN_ON_STANDBY 0
#endif

static void _dac_init_clock(dac_t line)
{
    sam0_gclk_enable(DAC_CLOCK);

    /* GCLK Setup */
#ifdef GCLK_PCHCTRL_CHEN
    GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                   | GCLK_PCHCTRL_GEN(DAC_CLOCK);
#else
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(DAC_CLOCK)
                      | GCLK_CLKCTRL_ID(DAC_GCLK_ID);
#endif

    dac_poweron(line);
}

static inline bool _ext_vref(void)
{
#ifdef DAC_CTRLB_REFSEL_VREFP
    return DAC_VREF == DAC_CTRLB_REFSEL_VREFP;
#endif
#ifdef DAC_CTRLB_REFSEL_VREFPU
    return (DAC_VREF == DAC_CTRLB_REFSEL_VREFPU) ||
           (DAC_VREF == DAC_CTRLB_REFSEL_VREFPB);
#endif
}

static inline void _sync(void)
{
#ifdef DAC_SYNCBUSY_MASK
    while (DAC->SYNCBUSY.reg) {}
#else
    while (DAC->STATUS.reg & DAC_STATUS_SYNCBUSY) {}
#endif
}

#ifdef DAC_DACCTRL_CCTRL_Msk
static uint32_t _get_CCTRL(uint32_t freq)
{
    if (freq <= KHZ(1200)) {
        return DAC_DACCTRL_CCTRL_CC100K;
    }

    if (freq <= MHZ(6)) {
        return DAC_DACCTRL_CCTRL_CC1M;
    }

    if (freq <= MHZ(12)) {
        return DAC_DACCTRL_CCTRL_CC12M;
    }

    assert(0);
    return 0;
}
#endif

int8_t dac_init(dac_t line)
{
    switch (line) {
    case 0:
        /* DAC0 is always connected to PA2 */
        gpio_init(GPIO_PIN(PA, 2), GPIO_OUT);
        gpio_init_mux(GPIO_PIN(PA, 2), GPIO_MUX_B);
        break;
#ifdef PIN_PA05B_DAC_VOUT1
    case 1:
        /* DAC1 is always connected to PA5 */
        gpio_init(GPIO_PIN(PA, 5), GPIO_OUT);
        gpio_init_mux(GPIO_PIN(PA, 5), GPIO_MUX_B);
        break;
#endif
    default:
        return DAC_NOLINE;
    }

    if (_ext_vref()) {
        /* PA3 is external reference voltage */
        gpio_init_mux(GPIO_PIN(PA, 3), GPIO_MUX_B);
    }

    _dac_init_clock(line);

    /* Settings can only be changed when DAC is disabled */
    DAC->CTRLA.reg &= ~DAC_CTRLA_ENABLE;
    _sync();

#ifdef DAC_DACCTRL_ENABLE
    DAC->DACCTRL[line].reg = DAC_DACCTRL_ENABLE
                           | _get_CCTRL(sam0_gclk_freq(DAC_CLOCK))
#endif
#if CONFIG_SAM0_DAC_RUN_ON_STANDBY && defined(DAC_DACCTRL_RUNSTDBY)
                           | DAC_DACCTRL_RUNSTDBY
#endif
                           ;

#ifdef DAC_DACCTRL_REFRESH
    /** The DAC can only maintain its output on the desired value for approximately 100 μs.
     *  For static voltages the conversion must be refreshed periodically (see e.g.
     *  '47.6.9.3 Conversion Refresh' in the SAM D5xE5x family data sheet).
     *
     *  Note: T_REFRESH = REFRESH * T_OSCULP32K
     */
    static_assert(CONFIG_SAM0_DAC_REFRESH != 1, "DACCTRLx.REFRESH = 1 is reserved");

    DAC->DACCTRL[line].bit.REFRESH = CONFIG_SAM0_DAC_REFRESH;
#endif

    /* Set Reference Voltage & enable Output if needed */
    DAC->CTRLB.reg = DAC_VREF
#ifdef DAC_CTRLB_EOEN
                   | DAC_CTRLB_EOEN
#endif
                   ;

    DAC->CTRLA.reg = DAC_CTRLA_ENABLE
#if CONFIG_SAM0_DAC_RUN_ON_STANDBY && defined(DAC_CTRLA_RUNSTDBY)
                   | DAC_CTRLA_RUNSTDBY
#endif
                   ;
    _sync();

#ifdef DAC_STATUS_READY
    /* wait for DAC startup */
    const uint32_t mask = 1 << (DAC_STATUS_READY_Pos + line);
    while (!(DAC->STATUS.reg & mask)) {}
#endif

    return DAC_OK;
}

void dac_set(dac_t line, uint16_t value)
{
#ifdef DAC_SYNCBUSY_DATA1
    const uint32_t mask = (1 << (DAC_INTFLAG_EMPTY_Pos + line));
    while (!(DAC->INTFLAG.reg & mask)) {}

    /* DAC has multiple outputs */
    DAC->DATA[line].reg = DAC_VAL(value);
#else
    /* DAC has only one output */
    (void)line;

    _sync();
    DAC->DATA.reg = DAC_VAL(value);
#endif
}

void dac_poweron(dac_t line)
{
    (void) line;

#ifdef PM_APBCMASK_DAC
     PM->APBCMASK.reg |= PM_APBCMASK_DAC;
#endif
#ifdef MCLK_APBCMASK_DAC
     MCLK->APBCMASK.reg |= MCLK_APBCMASK_DAC;
#endif
#ifdef MCLK_APBDMASK_DAC
     MCLK->APBDMASK.reg |= MCLK_APBDMASK_DAC;
#endif
}

void dac_poweroff(dac_t line)
{
    (void) line;

#ifdef PM_APBCMASK_DAC
     PM->APBCMASK.reg &= ~PM_APBCMASK_DAC;
#endif
#ifdef MCLK_APBCMASK_DAC
     MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_DAC;
#endif
#ifdef MCLK_APBDMASK_DAC
     MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_DAC;
#endif
}
