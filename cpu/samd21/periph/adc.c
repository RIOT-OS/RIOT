/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samr21
 * @{
 *
 * @file
 * @brief       ADC driver implementation
 *
 * @author      Rane Balslev (SAMR21) <ranebalslev@gmail.com> 
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"
#define ENABLE_DEBUG    (1)
#include "debug.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

/* Prototypes */
bool adc_syncing(Adc*);
void adc_clear_status(Adc*, uint32_t);
bool adc_get_status(Adc*);
void adc_configure_with_resolution(Adc*, int);

int adc_init(adc_t dev, adc_precision_t precision)
{
    adc_poweron(dev);
    Adc *adc = 0;
    switch (dev) {
    #if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;

            switch (precision) 
            {
                case ADC_RES_8BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_8BIT); 
                    break;
                case ADC_RES_10BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_10BIT); 
                    break;
                case ADC_RES_12BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_12BIT); 
                    break;
                case ADC_RES_16BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_16BIT);            
                    break;
                default:
                    return -1;
            }
            while(adc_syncing(adc));
            /* Enable bandgap if internal ref 1 volt */
            if(ADC_0_REF_DEFAULT == ADC_0_REF_INT_1V)
            {
                SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;                    
            }
            /* Enable */
            adc->CTRLA.reg |= ADC_CTRLA_ENABLE;
        break;
    #endif
        default:
            return -1;
    }
    return 0;
}

int adc_sample(adc_t dev, int channel)
{
    uint16_t result;
    Adc *adc = 0;
    switch (dev)
    {
    #if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;
            /* Starts the ADC conversion */
            while(adc_syncing(adc));
            adc->SWTRIG.reg |= ADC_SWTRIG_START;
        break;
    #endif
        default:
            return -1;
    }
    while(adc_syncing(adc));
    result = adc->RESULT.reg;
    /*CLEAR RESET FLAG */
    adc_clear_status(adc, ADC_0_STATUS_RESULT_READY);
    while(!adc_get_status(adc)){} /* MUST NOT BLOCK */
    return result;
}

void adc_poweron(adc_t dev) 
{
    switch (dev) 
    {
    #if ADC_0_EN
        case ADC_0:
            /* Setup generic clock mask for adc */
            PM->APBCMASK.reg |= PM_APBCMASK_ADC;
            /* Setup generic clock channel for adc */                             
            GCLK->CLKCTRL.reg |= (ADC_GCLK_ID << GCLK_CLKCTRL_GEN_Pos);
            /* Enable generic clock channel */
            *((uint8_t*)&GCLK->CLKCTRL.reg) |= ADC_GCLK_ID;
            /* enable generic clock */
            GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
            break;
    #endif
        default:
            break;    
    }
}

void adc_poweroff(adc_t dev)
{
    Adc *adc = 0;
    
    switch (dev) 
    {
    #if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;
            while(adc_syncing(adc));
            /* Disable */
            adc->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
            break;
    #endif
        default:
            break;
    }
}

int adc_map(adc_t dev, int value, int min, int max)
{
    DEBUG("adc_map Not implemented!\n");
    return 0;
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    DEBUG("adc_mapf Not implemented!\n");
    return 0.0;
}

bool adc_syncing(Adc* adc)
{
    if(adc->STATUS.reg & ADC_STATUS_SYNCBUSY)
        return true;
    return false;
}

/* Configure ADC with defined Resolution */
void adc_configure_with_resolution(Adc* adc, int precision)
{
    /* Set RUN_IN_STANDBY */
    adc->CTRLA.reg = (ADC_0_RUN_IN_STANDBY << ADC_CTRLA_RUNSTDBY_Pos);
    /* Set Voltage Reference */
    adc->REFCTRL.reg = (ADC_0_REF_COM_EN << ADC_REFCTRL_REFCOMP_Pos) | ADC_0_REF_DEFAULT;
    /* Set the accumlation and devide result */
    adc->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(ADC_0_DIV_RES_DEFAULT) | ADC_0_ACCUM_DEFAULT;
    /* Set Sample length */
    adc->SAMPCTRL.reg = (ADC_0_SAMPLE_LENGTH << ADC_SAMPCTRL_SAMPLEN_Pos);
    while(adc_syncing(adc));    
    /* If external vref. Pin setup */
    if(ADC_0_REF_DEFAULT == ADC_0_REF_EXT_B)
    {
        ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_REF_DEFAULT);
        ADC_0_PORT.PINCFG[ADC_0_REF_DEFAULT].bit.INEN = true;
        ADC_0_PORT.PINCFG[ADC_0_REF_DEFAULT].bit.PULLEN = false;
    }
    while(adc_syncing(adc));
    /* Configure CTRLB Register HERE IS THE RESOLUTION SET!*/
    adc->CTRLB.reg = 
        ADC_0_PRESCALER |
        precision | 
        (ADC_0_CORRECTION_EN << ADC_CTRLB_CORREN_Pos) | 
        (ADC_0_FREE_RUNNING << ADC_CTRLB_FREERUN_Pos) | 
        (ADC_0_LEFT_ADJUST << ADC_CTRLB_LEFTADJ_Pos) |
        (ADC_0_DIFFERENTIAL_MODE << ADC_CTRLB_DIFFMODE_Pos);        
    while(adc_syncing(adc));
    /* Configure Window Mode Register */
    adc->WINCTRL.reg = ADC_0_WINDOW_MODE;
    while(adc_syncing(adc)); 
    /* Configure PIN SCAN MODE & positive & Negative Input Pins */
    adc->INPUTCTRL.reg = 
        ADC_0_GAIN_FACTOR_DEFAULT |
        (ADC_0_PIN_SCAN_OFFSET_START << ADC_INPUTCTRL_INPUTOFFSET_Pos) |
        (ADC_0_PIN_SCAN_INPUT_TO_SCAN << ADC_INPUTCTRL_INPUTSCAN_Pos) |
        ADC_0_NEG_INPUT |
        ADC_0_POS_INPUT;
    /* Configure event action */
    adc->EVCTRL.reg = ADC_0_EVENT_ACTION;
    /* Disable all interrupts */
    adc->INTENCLR.reg =
        (1 << ADC_INTENCLR_SYNCRDY_Pos) | (1 << ADC_INTENCLR_WINMON_Pos) |
        (1 << ADC_INTENCLR_OVERRUN_Pos) | (1 << ADC_INTENCLR_RESRDY_Pos);
    /* Load the fixed device calibration constants*/
    adc->CALIB.reg =
        ADC_CALIB_BIAS_CAL(
            (*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos)
        ) |
        ADC_CALIB_LINEARITY_CAL(
            (*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos)
        );
}

/* Clear interrupt status flags */
void adc_clear_status(Adc* adc, uint32_t status_flag)
{
    uint32_t interrupt_flags = 0;

    if(status_flag & ADC_0_STATUS_RESULT_READY)
        interrupt_flags |= ADC_INTFLAG_RESRDY;
    if(status_flag & ADC_0_STATUS_WINDOW)
        interrupt_flags |= ADC_INTFLAG_WINMON;
    if(status_flag & ADC_0_STATUS_OVERRUN)
        interrupt_flags |= ADC_INTFLAG_OVERRUN;
    /* Clear interrupt flag*/
    adc->INTFLAG.reg = interrupt_flags;
}

/* Get ADC status */
bool adc_get_status(Adc* adc)
{
    uint32_t interrupt_flags = adc->INTFLAG.reg;
    uint32_t status_flags = 0;

    if(interrupt_flags & ADC_INTFLAG_RESRDY)
        status_flags |= ADC_0_STATUS_RESULT_READY;
    if(interrupt_flags & ADC_INTFLAG_WINMON)
        status_flags |= ADC_0_STATUS_WINDOW;
    if(interrupt_flags & ADC_INTFLAG_OVERRUN)
    {
        status_flags |= ADC_0_STATUS_OVERRUN;
        //adc_clear_status(adc, ADC_0_STATUS_OVERRUN); // TODO:
    }
    if(status_flags > 0)
        return false;
    else
        return true;
}

#endif /* ADC_NUMOF */
