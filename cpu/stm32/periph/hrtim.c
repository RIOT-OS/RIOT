/*
 * Copyright (c) 2020 LAAS-CNRS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_hrtim
 * @{
 *
 * @file
 * @brief       Low-level HRTIM driver implementation
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 *
 * @}
 */

#include "periph/hrtim.h"
#include "cpu.h"
#include "assert.h"
#include "periph/gpio.h"

#include "xtimer.h"

static inline HRTIM_TypeDef *dev(hrtim_t hrtim)
{
    return hrtim_config[hrtim].dev;
}

static inline void _clk_init(hrtim_t hrtim)
{
    /* 1. selection of the high-speed PLL output */
    RCC->CFGR3 |= hrtim_config[hrtim].rcc_sw_mask;
    /* 2. clock enable for the registers mapped on the APB2 bus */
    periph_clk_en(hrtim_config[hrtim].bus, hrtim_config[hrtim].rcc_mask);

    /* HRTIM DLL initialization */
    /* DLL calibration: periodic calibration enabled, period set to 14μs */
    dev(hrtim)->sCommonRegs.DLLCR = HRTIM_DLLCR_CALRTE_1
                                    | HRTIM_DLLCR_CALRTE_0
                                    | HRTIM_DLLCR_CALEN;

    /* Check DLL end of calibration flag */
    while(!(dev(hrtim)->sCommonRegs.ISR & HRTIM_IER_DLLRDY)) {}
}

static inline uint32_t _period_ckpsc(hrtim_t hrtim, uint32_t freq,
                                        uint16_t *per, uint8_t *ckpsc)
{
    uint32_t f_hrtim = periph_apb_clk(hrtim_config[hrtim].bus) * 2;

    /* t_hrck = f_hrck / freq but f_hrck = (f_hrtim * 32) which is too
     * big for an uint32 so we will firstly divide f_hrtim then also add
     * the modulo to preserve accuracy. */
    uint32_t period = (f_hrtim / freq) * 32 + (f_hrtim % freq) * 32 / freq;

    /* period = t_hrck / 2^ckpsc so bits over 15 position directly give
     * the needed prescaller */
    *ckpsc = (bitarithm_msb(period) > 15)
           ? bitarithm_msb(period) - 15 : 0;
    period /= (1 << *ckpsc);

    /* From the f334 reference manual :
     * The period and compare values must be within a lower and an upper
     * limit related to the high-resolution implementation and listed in
     * Table 82:
     * • The minimum value must be greater than or equal to 3 periods of
     * the f HRTIM clock
     * • The maximum value must be less than or equal to 0xFFFF - 1
     * periods of the f HRTIM clock */
    uint16_t min_period = *ckpsc < 5 ? (96 >> *ckpsc) : 0x3;
    uint16_t max_period = *ckpsc < 4 ? (0xFFFF - (32 >> *ckpsc)) : 0xFFFD;

    /* Adjust prescaler and period if needed */
    if (period > (uint32_t)max_period) {
        *ckpsc = *ckpsc + 1;
        period /= 2;
    }
    /* Verify parameters */
    assert( (*ckpsc <= 7)
            && (hrtim < HRTIM_NUMOF)
            && (period >= min_period)
            && (period <= max_period));

    /* Note: with period = max_period (48MHz frequency for f344) it
     * theorically not possible to obtain PWM because this limitation
     * also concern compare value, but it is probably possible to
     * workaround this limitation and manage a duty cycle using the
     * dead-time generator which have a 868ps resolution... */

    *per = (uint16_t)period;

    /* Compute and return the effective frequency */
    uint32_t frequency = (  (f_hrtim / period) * 32
                            + (f_hrtim % period) * 32 / period
                         )  / (1 << *ckpsc);
    return frequency;
}

uint16_t hrtim_init(hrtim_t hrtim, uint32_t *freq, uint16_t dt)
{
    /* Master timer initialization */
    uint16_t period = hrtim_init_mstr(hrtim, freq);

    /* Slave timers initialization */
    for (unsigned tu = 0; tu < HRTIM_STU_NUMOF; tu++) {
        hrtim_init_tu(hrtim, tu, freq);

        /* Set the dead time
         * Note: this must be done before enable counter */
        hrtim_pwm_dt(hrtim, tu, dt);

        /* Enable counter */
        hrtim_cnt_en(hrtim, (1 << (HRTIM_MCR_TACEN_Pos + tu)));

        /* Setup outputs */
        hrtim_cmpl_pwm_out(hrtim, tu);

        /* Reset on master timer period event */
        hrtim_rst_evt_en(hrtim, tu, RST_MSTPER);
    }

    return period;
}

void hrtim_pwm_set(hrtim_t hrtim, hrtim_tu_t tu, uint16_t value, uint16_t shift)
{
    static uint16_t prev_value[HRTIM_STU_NUMOF];
    static uint16_t prev_shift[HRTIM_STU_NUMOF];

    if (value != prev_value[tu]) {
        prev_value[tu] = value;

        /* Disable outputs when duty cycle is 0 */
        if (value == 0) {
            dev(hrtim)->sCommonRegs.ODISR |= ((OUT1 | OUT2) << (tu * 2));
            return;
        }
        /* Set comparator for duty cycle */
        dev(hrtim)->sTimerxRegs[tu].CMP1xR = value;

        /* Enable outputs */
        dev(hrtim)->sCommonRegs.OENR |= ((OUT1 | OUT2) << (tu * 2));
    }
    if (shift != prev_shift[tu]) {
        prev_shift[tu] = shift;

        /* Set reset comparator for phase positioning */
        if (shift) {
            dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTPER;
            switch (tu) {
                /* Timer A is the phase shift reference so it can't be phase
                 * shifted */
                case TIMB: /* Timer B reset on master cmp1 */
                    dev(hrtim)->sMasterRegs.MCMP1R = shift;
                    dev(hrtim)->sTimerxRegs[tu].RSTxR |= RST_MSTCMP1;
                    break;
                case TIMC: /* Timer C reset on master cmp2 */
                    dev(hrtim)->sMasterRegs.MCMP2R = shift;
                    dev(hrtim)->sTimerxRegs[tu].RSTxR |= RST_MSTCMP2;
                    break;
                case TIMD: /* Timer D reset on master cmp3 */
                    dev(hrtim)->sMasterRegs.MCMP3R = shift;
                    dev(hrtim)->sTimerxRegs[tu].RSTxR |= RST_MSTCMP3;
                    break;
                case TIME: /* Timer E reset on master cmp4 */
                    dev(hrtim)->sMasterRegs.MCMP4R = shift;
                    dev(hrtim)->sTimerxRegs[tu].RSTxR |= RST_MSTCMP4;
                    break;
    #if (HRTIM_STU_NUMOF == 6)
                case TIMF: /* Timer F reset on timerA cmp2 */
                    dev(hrtim)->sTimerxRegs[0].CMP2xR = shift;
                    dev(hrtim)->sTimerxRegs[tu].RSTxR |= RSTF_TACMP2;
                    break;
    #endif
                default:
                    break;
            }
        }
        else if(dev(hrtim)->sTimerxRegs[tu].PERxR == dev(hrtim)->sMasterRegs.MPER
                && (dev(hrtim)->sTimerxRegs[tu].TIMxCR & HRTIM_TIMCR_CK_PSC_Msk)
                    == (dev(hrtim)->sMasterRegs.MCR & HRTIM_MCR_CK_PSC_Msk)){
            /* shift == 0 and timing unit run at the same frequency as master */
            switch (tu) {
                case TIMB:
                    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTCMP1;
                    break;
                case TIMC:
                    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTCMP2;
                    break;
                case TIMD:
                    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTCMP3;
                    break;
                case TIME:
                    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTCMP4;
                    break;
    #if (HRTIM_STU_NUMOF == 6)
                case TIMF:
                    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RSTF_TACMP2;
                    break;
    #endif
                default:
                    break;
            }
            dev(hrtim)->sTimerxRegs[tu].RSTxR |= RST_MSTPER;
        }
        else {
            /* timing unit do not run at the same frequency as master so
             * phase positioning is not applicable */
            dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~RST_MSTPER;
        }
    }
}

uint16_t hrtim_init_mstr(hrtim_t hrtim, uint32_t *freq)
{
    uint16_t period;
    uint8_t ckpsc;

    /* HRTIM clock initialization */
    _clk_init(hrtim);

    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
    *freq = _period_ckpsc(hrtim, *freq, &period, &ckpsc);
    /* master timer prescaler init */
    dev(hrtim)->sMasterRegs.MCR &= ~HRTIM_MCR_CK_PSC_Msk;
    dev(hrtim)->sMasterRegs.MCR |= (ckpsc << HRTIM_MCR_CK_PSC_Pos);

    /* Master timer initialization */
    /* continuous mode, preload enabled on repetition event */
    dev(hrtim)->sMasterRegs.MCR |= (HRTIM_MCR_CONT | HRTIM_MCR_PREEN
                                | HRTIM_MCR_MREPU);

    /* Enable counter */
    dev(hrtim)->sMasterRegs.MCR |= (1 << HRTIM_MCR_MCEN_Pos);

    /* Configure the PWM frequency by setting the period registers */
    dev(hrtim)->sMasterRegs.MPER = period;

    return period;
}

uint16_t hrtim_init_tu(hrtim_t hrtim, hrtim_tu_t tu, uint32_t *freq)
{
    uint16_t period;
    uint8_t ckpsc;

    /* Outputs initialization */
    hrtim_out_dis(hrtim, tu, OUT1 | OUT2);
    hrtim_tu_conf_t tu_conf = hrtim_config[hrtim].tu[tu];
    gpio_init(tu_conf.pin[0], GPIO_OUT);
    gpio_init(tu_conf.pin[1], GPIO_OUT);
    gpio_init_af(tu_conf.pin[0], tu_conf.af);
    gpio_init_af(tu_conf.pin[1], tu_conf.af);

    /* At start-up, it is mandatory to initialize first the prescaler
     * bitfields before writing the compare and period registers. */
    *freq = _period_ckpsc(hrtim, *freq, &period, &ckpsc);
    dev(hrtim)->sTimerxRegs[tu].TIMxCR &= ~HRTIM_TIMCR_CK_PSC_Msk;
    dev(hrtim)->sTimerxRegs[tu].TIMxCR |= (ckpsc << HRTIM_TIMCR_CK_PSC_Pos);

    /* timer initialization */
    /* continuous mode, preload enabled on repetition event */
    dev(hrtim)->sTimerxRegs[tu].TIMxCR |= (HRTIM_TIMCR_CONT
                                      | HRTIM_TIMCR_PREEN
                                      | HRTIM_TIMCR_TREPU);

    /* Configure the PWM frequency by setting the period registers */
    dev(hrtim)->sTimerxRegs[tu].PERxR = period;

    return period;
}

void hrtim_set_cb_set(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb)
{
    if (out == OUT1) {
        dev(hrtim)->sTimerxRegs[tu].SETx1R |= cb;
    }
    else { /* OUT2 */
        dev(hrtim)->sTimerxRegs[tu].SETx2R |= cb;
    }
}

void hrtim_set_cb_unset(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb)
{
    if (out == OUT1) {
        dev(hrtim)->sTimerxRegs[tu].SETx1R &= ~cb;
    }
    else { /* OUT2 */
        dev(hrtim)->sTimerxRegs[tu].SETx2R &= ~cb;
    }
}

void hrtim_rst_cb_set(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out,
                        hrtim_cb_t cb)
{
    if (out == OUT1) {
        dev(hrtim)->sTimerxRegs[tu].RSTx1R |= cb;
    }
    else { /* OUT2 */
        dev(hrtim)->sTimerxRegs[tu].RSTx2R |= cb;
    }
}

void hrtim_rst_cb_unset(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out,
                            hrtim_cb_t cb)
{
    if (out == OUT1) {
        dev(hrtim)->sTimerxRegs[tu].RSTx1R &= ~cb;
    }
    else { /* OUT2 */
        dev(hrtim)->sTimerxRegs[tu].RSTx2R &= ~cb;
    }
}

void hrtim_cmpl_pwm_out(hrtim_t hrtim, hrtim_tu_t tu)
{
    dev(hrtim)->sTimerxRegs[tu].SETx1R = PER;
    dev(hrtim)->sTimerxRegs[tu].RSTx1R = CMP1;
    dev(hrtim)->sTimerxRegs[tu].SETx2R = CMP1;
    dev(hrtim)->sTimerxRegs[tu].RSTx2R = PER;
}

void hrtim_period_set(hrtim_t hrtim, hrtim_tu_t tu, uint16_t value)
{
    if (tu == MSTR) {
        dev(hrtim)->sMasterRegs.MPER = value;
    }
    else {
        dev(hrtim)->sTimerxRegs[tu].PERxR = value;
    }
}

void hrtim_cmp_set(hrtim_t hrtim, hrtim_tu_t tu, hrtim_cmp_t cmp,
                    uint16_t value)
{
    if (tu == MSTR) {
        switch (cmp) {
            case CMP1xR:
                dev(hrtim)->sMasterRegs.MCMP1R = value;
                break;
            case CMP2xR:
                dev(hrtim)->sMasterRegs.MCMP2R = value;
                break;
            case CMP3xR:
                dev(hrtim)->sMasterRegs.MCMP3R = value;
                break;
            case CMP4xR:
                dev(hrtim)->sMasterRegs.MCMP4R = value;
                break;
            default:
                break;
        }
    }
    else {
        switch (cmp) {
            case CMP1xR:
                dev(hrtim)->sTimerxRegs[tu].CMP1xR = value;
                break;
            case CMP2xR:
                dev(hrtim)->sTimerxRegs[tu].CMP2xR = value;
                break;
            case CMP3xR:
                dev(hrtim)->sTimerxRegs[tu].CMP3xR = value;
                break;
            case CMP4xR:
                dev(hrtim)->sTimerxRegs[tu].CMP4xR = value;
                break;
            default:
                break;
        }
    }
}

void hrtim_cnt_en(hrtim_t hrtim, hrtim_cen_t cen)
{
    dev(hrtim)->sMasterRegs.MCR |= cen;
}

void hrtim_cnt_dis(hrtim_t hrtim, hrtim_cen_t cen)
{
    dev(hrtim)->sMasterRegs.MCR &= ~cen;
}

void hrtim_rst_evt_en(hrtim_t hrtim, hrtim_tu_t tu, hrtim_rst_evt_t evt)
{
    dev(hrtim)->sTimerxRegs[tu].RSTxR |= evt;
}

void hrtim_rst_evt_dis(hrtim_t hrtim, hrtim_tu_t tu, hrtim_rst_evt_t evt)
{
    dev(hrtim)->sTimerxRegs[tu].RSTxR &= ~evt;
}

void hrtim_out_en(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out)
{
    dev(hrtim)->sCommonRegs.OENR |= (out << (tu * 2));
}

void hrtim_out_dis(hrtim_t hrtim, hrtim_tu_t tu, hrtim_out_t out)
{
    dev(hrtim)->sCommonRegs.ODISR |= (out << (tu * 2));
}

void hrtim_pwm_dt(hrtim_t hrtim, hrtim_tu_t tu, uint16_t ns)
{
    uint32_t ps = ns * 1000;
    /* t_dtg = (2^dtpsc) * (t_hrtim / 8)
     *       = (2^dtpsc) / (f_hrtim * 8) */
    uint32_t f_hrtim = periph_apb_clk(hrtim_config[hrtim].bus) * 2;
    uint8_t dtpsc = 0;
    uint32_t t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
    uint16_t dt = ps / t_dtg_ps;
    while (dt > 511 && dtpsc < 7) {
        dtpsc++;
        t_dtg_ps = (1 << dtpsc) * 1000000 / ((f_hrtim * 8) / 1000000);
        dt = ps / t_dtg_ps;
    }
    if (dt > 511) {
        dt = 511;
    }
    dev(hrtim)->sTimerxRegs[tu].DTxR &= ~(HRTIM_DTR_DTPRSC_Msk
                                        | HRTIM_DTR_DTF_Msk
                                        | HRTIM_DTR_DTR);
    dev(hrtim)->sTimerxRegs[tu].DTxR |= (dtpsc << HRTIM_DTR_DTPRSC_Pos);
    dev(hrtim)->sTimerxRegs[tu].DTxR |= (dt << HRTIM_DTR_DTF_Pos);
    dev(hrtim)->sTimerxRegs[tu].DTxR |= (dt << HRTIM_DTR_DTR_Pos);
    dev(hrtim)->sTimerxRegs[tu].OUTxR |= HRTIM_OUTR_DTEN; /* Note: This
    * parameter cannot be changed once the timer is operating (TxEN bit
    * set) or if its outputs are enabled and set/reset by another timer. */
}
