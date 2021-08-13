/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_adc
 *
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * This driver supports most of the ADC capabilities of the ADC block in this
 * CPU, but there are some functions left out of this driver:
 *
 *  * The software decimation is fixed to 32 samples. This could be configured
 *    per line instead with many more filtering options.
 *
 *  * There's no amplification support (PGA) other than 1x. The main issue with
 *    supporting this configuration even at build time only is that it also
 *    requires to adjust the signal's common mode voltage level so that it
 *    matches roughly Vcc/2, leaving enough dynamic range in the ADC. This is
 *    maybe beyond the scope of this interface, but it could be configured from
 *    the board if needed. See "PGA output VCM" section in the user manual for
 *    details.
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "bitarithm.h"
#include "cpu.h"
#include "mutex.h"
#include "xtimer.h"

#include "gpio_mux.h"
#include "periph/adc.h"

#include "vendor/drivers/fsl_clock.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef QN908X_ADC_CLOCK
#define QN908X_ADC_CLOCK ADC_CLOCK_500K
#endif

/* Value of the ADC_CTRL_CLKSEL field which is actually CLKSEL in the lower two
 * bits (0 for 4 MHz source and 1 for 32 KHz source) and the divisor exponent
 * in the bits 2 to 4 (0 for 128 divisor and 7 for 1 divisor). */
static const uint8_t adc_ctrl_clksel[] = {
    28,  /* 4 MHz*/
    24,  /* 2 MHz*/
    20,  /* 1 MHz*/
    16,  /* 500 KHz*/
    12,  /* 250 KHz*/
    8,   /* 125 KHz*/
    4,   /* 62.5 KHz*/
    0,   /* 31.25 KHz*/
    1,   /* 32 KHz or 32.768 KHz from the low speed source. */
};

/* ADC oversample clock cycle in microseconds (used for delay). */
static const uint8_t adc_clock_cycle_us[] = {
    1,    /* 4 MHz*/
    1,    /* 2 MHz*/
    1,    /* 1 MHz*/
    2,    /* 500 KHz*/
    4,    /* 250 KHz*/
    8,    /* 125 KHz*/
    16,   /* 62.5 KHz*/
    32,   /* 31.25 KHz*/
    32,   /* 32 KHz or 32.768 KHz from the low speed source. */
};

/* Mutex for the public interface. This guards against two threads calling the
 * ADC at the same time. */
static mutex_t adc_lock = MUTEX_INIT;

/* Flag telling whether the ADC block was initialized. We only initialize the
 * ADC block once, although some pin initialization is done per ADC line. */
static bool adc_init_done = false;

/* Mutex to lock the @ref adc_sample function waiting for the interrupt to
 * retrieve the value. */
static mutex_t adc_sample_isr_lock = MUTEX_INIT_LOCKED;

/* Pointer to the variable that should hold the pending ADC result. This is
 * used by the ISR to store the value. */
static volatile int32_t* adc_data_ptr = NULL;

/* Factory calibration actual voltage of the internal 1.2V bandgap source. */
static uint32_t adc_bandgap_calib_mv;

/**
 * @brief Get the 1.2V bandgap reference voltage in mV.
 */
static uint32_t _adc_bandgap_calib_mv(void) {
    uint32_t calib = *(uint32_t*)FSL_FEATURE_FLASH_ADDR_OF_BANDGAP_VOL;
    DEBUG("[adc] bandgap calib = %" PRIu32 " mV\n", calib);
    /* Check that the bandgap calibration value stored in the Flash Information
     * Page (Section 7.3.1.1 in the User Manual) makes sense and use a
     * reasonable default otherwise. */
    if (calib < 1214 || calib > 1228) {
        calib = 1222;
    }
    return calib;
}

/* Extract the channel number from the adc_conf_t. */
#define ADC_CONF_T_CHANNEL_NUM(conf) (((conf) >> 9u) & 0x1fu)

/* Mask to and against the adc_conf_t to get the channel enum value. */
#define ADC_CONF_T_CHANNEL_MASK (0x1fu << 9u)

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -1;
    }
    mutex_lock(&adc_lock);
    if (!adc_init_done) {
        DEBUG("[adc] ADC block init\n");
        adc_bandgap_calib_mv = _adc_bandgap_calib_mv();

        /* Power ON the ADC by clearing the disable (DIS) bits. */
        SYSCON->PMU_CTRL1 &= ~(SYSCON_PMU_CTRL1_ADC_BUF_DIS_MASK | /* PGA */
                               SYSCON_PMU_CTRL1_ADC_BG_DIS_MASK |
                               SYSCON_PMU_CTRL1_ADC_DIS_MASK |
                               SYSCON_PMU_CTRL1_ADC_VCM_DIS_MASK | /* VINN */
                               SYSCON_PMU_CTRL1_ADC_VREF_DIS_MASK);
        /* Need to wait 100 us before the ADC can be used for sampling. We could
         * in theory avoid this wait since it is only needed before adc_sample()
         * is called but it is short enough that it is safer to include it. */
        xtimer_usleep(100u);

        /* Enable the ADC clock so we can use the ADC. */
        CLOCK_EnableClock(kCLOCK_Adc);
        /* Reset the ADC. */
        SYSCON->RST_SW_SET = SYSCON_RST_SW_SET_SET_ADC_RST_MASK;
        SYSCON->RST_SW_CLR = SYSCON_RST_SW_CLR_CLR_ADC_RST_MASK;

        /* Enabled the ADC module: single conversion mode, scan disabled, window
         * mode disabled, 32-bit signed data format, software trigger.
         */
        ADC->CTRL = ADC_CTRL_CLKSEL(adc_ctrl_clksel[QN908X_ADC_CLOCK]) |
                    ADC_CTRL_CONV_MODE(1) | ADC_CTRL_SCAN_EN(0) |
                    ADC_CTRL_CH_IDX_EN(0) |
                    ADC_CTRL_DATA_FORMAT(1) | /* DATA is sign extended. */
                    ADC_CTRL_SIG_INV_EN(0) |
#ifdef BOARD_HAS_ADC_PA06_CAP
                    ADC_CTRL_VREFO_EN(1) |
#endif /* BOARD_HAS_ADC_PA06_CAP */
                    ADC_CTRL_TRIGGER(35 /* software trigger */);

#ifdef BOARD_HAS_ADC_PA06_CAP
        /* Use PA06 as the ADC_EX_CAP signal. */
        gpio_init_mux(GPIO_PIN(PORT_A, 6), 1);
#endif /* BOARD_HAS_ADC_PA06_CAP */

        /* Always use CFG[0] to sample all channels. */
        ADC->CH_CFG = 0u;

        /* Enable the PGA chopper. */
        ADC->BG_BF |= ADC_BG_BF_PGA_CHOP_EN_MASK;

        /* Enable interrupts. */
        ADC->INTEN = ADC_INTEN_DAT_RDY_INTEN_MASK | ADC_INTEN_ADC_INTEN_MASK;

        NVIC_EnableIRQ(ADC_IRQn);

        adc_init_done = true;
    }
    const adc_conf_t conf = adc_config[line];
    DEBUG("[adc] ADC line %u init: ch=%u\n", line, ADC_CONF_T_CHANNEL_NUM(conf));

    uint32_t func1_pins = 0;
    /* ADC pins are all function 1. */
    switch (conf & ADC_CONF_T_CHANNEL_MASK) {
        case ADC_CHANNEL_ADC0_ADC1:
            func1_pins = (1u << 0) | (1u << 1);
            break;
        case ADC_CHANNEL_ADC2_ADC3:
            func1_pins = (1u << 4) | (1u << 5);
            break;
        case ADC_CHANNEL_ADC4_ADC5:
            func1_pins = (1u << 8) | (1u << 9);
            break;
        case ADC_CHANNEL_ADC6_ADC7:
            func1_pins = (1u << 10) | (1u << 11);
            break;
        case ADC_CHANNEL_ADC0_VINN:
            func1_pins = 1u << 0;
            break;
        case ADC_CHANNEL_ADC1_VINN:
            func1_pins = 1u << 1;
            break;
        case ADC_CHANNEL_ADC2_VINN:
            func1_pins = 1u << 4;
            break;
        case ADC_CHANNEL_ADC3_VINN:
            func1_pins = 1u << 5;
            break;
        case ADC_CHANNEL_ADC4_VINN:
            func1_pins = 1u << 8;
            break;
        case ADC_CHANNEL_ADC5_VINN:
            func1_pins = 1u << 9;
            break;
        case ADC_CHANNEL_ADC6_VINN:
            func1_pins = 1u << 10;
            break;
        case ADC_CHANNEL_ADC7_VINN:
            func1_pins = 1u << 11;
            break;
        case ADC_CHANNEL_TEMP:
            /* Enable the internal temperature source. */
            ADC->BG_BF |= ADC_BG_BF_TEMP_EN_MASK;
            break;
        case ADC_CHANNEL_VCC4_VINN:
            /* Enable the Vcc/4 source. */
            SYSCON->ANA_EN |= SYSCON_ANA_EN_BAT_MON_EN_MASK;
            break;
        default:
            break;
    }

    const uint32_t vref_sel = conf & ADC_CTRL_VREF_SEL_MASK;
    if (ADC_CTRL_VREF_SEL(1) == vref_sel || ADC_CTRL_VREF_SEL(2) == vref_sel) {
        /* ADC_VREFI (A7 pin) used as Vref input. */
        gpio_init_mux(GPIO_PIN(PORT_A, 7), 1);
    }
    while (func1_pins) {
        uint8_t pin;
        func1_pins = bitarithm_test_and_clear(func1_pins, &pin);
        gpio_init_mux(GPIO_PIN(PORT_A, pin), 1);
    }
    mutex_unlock(&adc_lock);
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res) {
    if (line >= ADC_NUMOF) {
        return -1;
    }
    mutex_lock(&adc_lock);
    const adc_conf_t conf = adc_config[line];

    /* Enable the ADC and set the Vref selection. */
    ADC->CTRL = (ADC->CTRL & ~ADC_CTRL_VREF_SEL_MASK) |
        ADC_CTRL_ENABLE_MASK | (conf & ADC_CTRL_VREF_SEL_MASK) ;
    /* The ADC needs one ADC clock cycle before it can be triggered after we
     * enabled it. */

    /* Configure the channel parameters. We always use CFG[0]. */
    ADC->CH_SEL = 1u << ADC_CONF_T_CHANNEL_NUM(conf);
    ADC->CFG[0] =
        ADC_CFG_PGA_GAIN(0) | /* PGA gain = x1 */
        /* PGA_VINN, ADC_GAIN and VREF_GAIN directly from the config, flipping
         * 0x40 to default ADC_GAIN to x1.0, flipping 0x30 to default Vinn to
         * Vss and flipping 0x08 to default to PGA disabled (bypass enabled). */
        ((conf ^ 0x78) & (ADC_CFG_PGA_BP_MASK | ADC_CFG_PGA_VINN_MASK |
                          ADC_CFG_ADC_GAIN_MASK)) |
        ADC_CFG_PGA_VCM_EN(0) |       /* Vcm control disabled. */
        /* TODO: Allow the board to configure the decimation. */
        ADC_CFG_DOWN_SAMPLE_RATE(1) | /* down sample 32 */
        ADC_CFG_DS_DATA_STABLE(7) |
        ADC_CFG_SCAN_INTV(4); /* Switching ADC source every 32 clock cycles. */

    /* Need to wait for one ADC cycle before it can be started. */
    xtimer_usleep(adc_clock_cycle_us[QN908X_ADC_CLOCK]);

    /* Configure the destination of the ADC value read from the interrupt. */
    volatile int32_t adc_data = 0;
    adc_data_ptr = &adc_data;

    /* Triggers the sample event. When done the interrupt will fire and release
     * the mutex. */
    ADC->CTRL |= ADC_CTRL_SW_START_MASK;
    /* Wait for the interrupt to return a value in adc_data. */
    mutex_lock(&adc_sample_isr_lock);
    mutex_unlock(&adc_lock);

    int32_t ret = -1;
    /* adc_data has 23-bit signed number, with the sign extended to an int32_t.
     */
    if (res <= ADC_RES_MAX) {
        ret = adc_data >> (22 - res);
    } else if (res == ADC_RES_UV &&
               (conf & ADC_CTRL_VREF_SEL_MASK) == ADC_VREF_1V2) {
        /* Returning in uV is only supported when using Vref as the internal
         * 1.2v. */
        uint32_t vref = adc_bandgap_calib_mv;
        if (conf & ADC_VREF_GAIN_X15) {
            vref = vref + vref / 2;
        }
        /* adc_data is a signed 23 bit number and the Vref is at most 1.8v, so
         * an unsigned 11 bit number. To make this fit in signed 32-bit
         * arithmetic we need to drop some bits from the adc_data. Then we
         * convert the result to uV dividing by (1 << 20) which is now the full
         * scale of (adc_data >> 2) and multiplying by 1000. The last two
         * operations can be done as a single division still in 32-bit integer
         * space. */
        ret = ((adc_data >> 2) * (int32_t)vref) / ((1 << 20) / 1000);
    }
    DEBUG("[adc] sample line %u: ch=%u res=%u conf=0x%.4" PRIx16
          " adc=0x%.8" PRIx32 " ret=%" PRIi32 "\n",
          line, ADC_CONF_T_CHANNEL_NUM(conf), (unsigned)res, conf, adc_data,
          ret);

    return ret;
}

void isr_adc(void)
{
    if (ADC->INT & ADC_INT_DAT_RDY_INT_MASK) {
        uint32_t data = ADC->DATA;
        /* The DAT_RDY_INT bit clears automatically when reading the data. */
        if (adc_data_ptr) {
            *adc_data_ptr = data;
        }
        adc_data_ptr = NULL;
        mutex_unlock(&adc_sample_isr_lock);
    }

    cortexm_isr_end();
}
