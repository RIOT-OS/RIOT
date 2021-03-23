/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#define DAC_VAL(in) (in >> (16 - DAC_RES_BITS))

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
    while (DAC->STATUS.bit.SYNCBUSY) {}
#endif
}

#ifdef DAC_DACCTRL_CCTRL_Msk
static uint32_t _get_CCTRL(uint32_t freq)
{
    if (freq < 1200000) {
        return DAC_DACCTRL_CCTRL_CC100K;
    }

    if (freq < 6000000) {
        return DAC_DACCTRL_CCTRL_CC1M;
    }

    if (freq < 12000000) {
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
    DAC->CTRLA.bit.ENABLE = 0;
    _sync();

#ifdef DAC_DACCTRL_ENABLE
    DAC->DACCTRL[line].reg = DAC_DACCTRL_ENABLE
                           | _get_CCTRL(sam0_gclk_freq(DAC_CLOCK));
#endif

    /* Set Reference Voltage & enable Output if needed */
    DAC->CTRLB.reg = DAC_VREF
#ifdef DAC_CTRLB_EOEN
                   | DAC_CTRLB_EOEN
#endif
                   ;

    DAC->CTRLA.bit.ENABLE = 1;
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
    /* DAC has multiple outputs */
    const uint32_t mask = (1 << (DAC_SYNCBUSY_DATA_Pos + line));
    while (DAC->SYNCBUSY.reg & mask) {}

    DAC->DATA[line].reg = DAC_VAL(value);
#else
    /* DAC has only one output */
    (void) line;

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
