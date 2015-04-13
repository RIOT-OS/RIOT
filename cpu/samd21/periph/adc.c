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
#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

/* Prototypes */
bool adc_syncing(Adc*);
void adc_clear_status(Adc*, uint32_t);
uint32_t adc_get_status(Adc*);
int adc_configure_with_resolution(Adc*, uint32_t);

int adc_init(adc_t dev, adc_precision_t precision)
{
    //adc_poweron(dev);
    Adc *adc = 0;
    int init = 0;
    switch (dev) {
    #if ADC_0_EN
        case ADC_0:
            adc = ADC_0_DEV;
            while(adc_syncing(adc));
            /*Disable adc before init*/
            adc->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
            switch (precision) 
            {
                case ADC_RES_8BIT:
                    init = adc_configure_with_resolution(adc, ADC_0_RES_8BIT); 
                    break;
                case ADC_RES_10BIT:
                    init = adc_configure_with_resolution(adc, ADC_0_RES_10BIT); 
                    break;
                case ADC_RES_12BIT:
                    init = adc_configure_with_resolution(adc, ADC_0_RES_12BIT); 
                    break;
                case ADC_RES_16BIT:
                    init = adc_configure_with_resolution(adc, ADC_0_RES_16BIT);
                    DEBUG("Init switch DONE\n");            
                    break;
                default:
                    return -1;
            }
            while(adc_syncing(adc));
            if(init == -1)
                return -1;
            /* Enable bandgap if internal ref 1 volt */
            if(ADC_0_REF_DEFAULT == ADC_0_REF_INT_1V)
            {
                //SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;                    
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
    int result = 0;
    Adc *adc = 0;
    int posChanInput = 0;
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
            DEBUG("case default\n");
            return -1;
    }
    // adc->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
    // switch(channel)
    // {
    //     case 1:
    //         /* Pin Muxing */
    //         ADC_0_PORT.PINCFG[ ADC_0_CH1 ].bit.PMUXEN = 1;
    //         ADC_0_PORT.PMUX[ ADC_0_CH1 / 2].bit.PMUXO = 1;

    //         /* ADC_0_POS_INPUT Input */
    //         ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_CH1);
    //         ADC_0_PORT.PINCFG[ADC_0_CH1].bit.INEN = true;
    //         ADC_0_PORT.PINCFG[ADC_0_CH1].bit.PULLEN = false; 

    //         if(ADC_0_NEG_INPUT != ADC_INPUTCTRL_MUXNEG_GND)
    //         {
    //             ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_NEG_INPUT);
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.INEN = true;
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.PULLEN = false;
    //         }
    //         posChanInput = ADC_0_CH1;
    //         break;
    //     case 2:
    //         /* Pin Muxing */
    //         ADC_0_PORT.PINCFG[ ADC_0_CH2 ].bit.PMUXEN = 1;
    //         ADC_0_PORT.PMUX[ ADC_0_CH2 / 2].bit.PMUXE = 1;

    //         /* ADC_0_POS_INPUT Input */
    //         ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_CH2);
    //         ADC_0_PORT.PINCFG[ADC_0_CH2].bit.INEN = true;
    //         ADC_0_PORT.PINCFG[ADC_0_CH2].bit.PULLEN = false; 

    //         if(ADC_0_NEG_INPUT != ADC_INPUTCTRL_MUXNEG_GND)
    //         {
    //             ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_NEG_INPUT);
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.INEN = true;
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.PULLEN = false;
    //         }
    //         posChanInput = ADC_0_CH2; 
    //         break;
    //     case 3:
    //         /* Pin Muxing */
    //         ADC_0_PORT.PINCFG[ ADC_0_CH3 ].bit.PMUXEN = 1;
    //         ADC_0_PORT.PMUX[ ADC_0_CH3 / 2].bit.PMUXO = 1;

    //         /* ADC_0_POS_INPUT Input */
    //         ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_CH3);
    //         ADC_0_PORT.PINCFG[ADC_0_CH3].bit.INEN = true;
    //         ADC_0_PORT.PINCFG[ADC_0_CH3].bit.PULLEN = false; 

    //         if(ADC_0_NEG_INPUT != ADC_INPUTCTRL_MUXNEG_GND)
    //         {
    //             ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_NEG_INPUT);
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.INEN = true;
    //             ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.PULLEN = false;
    //         }
    //         posChanInput = ADC_0_CH3; 
    //         break;
    //         default:
    //             DEBUG("Invalid channel\n");
    //             return -1;
    // }
    // /* Configure PIN SCAN MODE & positive & Negative Input Pins */
    // adc->INPUTCTRL.reg = 
    //     ADC_0_GAIN_FACTOR_DEFAULT |
    //     (ADC_0_PIN_SCAN_OFFSET_START << ADC_INPUTCTRL_INPUTOFFSET_Pos) |
    //     (ADC_0_PIN_SCAN_INPUT_TO_SCAN << ADC_INPUTCTRL_INPUTSCAN_Pos) |
    //     ADC_0_NEG_INPUT |
    //     posChanInput;
    // while(adc_syncing(adc));
    // adc->CTRLA.reg |= ADC_CTRLA_ENABLE;
    //while(adc_syncing(adc));
    /* MUST NOT BLOCK */
    while(!(adc_get_status(adc) & ADC_0_STATUS_RESULT_READY))
    {
        DEBUG("STATUS NOT READY\n");
    }
    while(adc_syncing(adc));
    result = adc->RESULT.reg;
    /*CLEAR RESET FLAG */
    adc_clear_status(adc, ADC_0_STATUS_RESULT_READY);
    if (adc_get_status(adc) & ADC_0_STATUS_OVERRUN) 
    {
        adc_clear_status(adc, ADC_0_STATUS_OVERRUN);
        DEBUG("OVERRUN\n");
        return -1;
    }
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
            /*PM->APBCMASK.reg &= ~PM_APBCMASK_ADC;*/ 
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
    if(adc->STATUS.reg & ADC_STATUS_SYNCBUSY){
        return true;
    }
    return false;
}

/* Configure ADC with defined Resolution */
int adc_configure_with_resolution(Adc* adc, uint32_t precision)
{
    adc_poweron(ADC_0);
    uint8_t  divideResult = ADC_0_DIV_RES_DEFAULT;
    uint32_t resolution = ADC_0_RES_16BIT;
    uint32_t accumulate = ADC_0_ACCUM_DEFAULT;
    if(adc->CTRLA.reg & ADC_CTRLA_SWRST)
        return -1;
    if(adc->CTRLA.reg & ADC_CTRLA_ENABLE)
        return -1;
    
    /* GCLK Setup */
    GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN
                          | GCLK_CLKCTRL_GEN_GCLK0 
                          | (ADC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));

    /* Pin Muxing */
    ADC_0_PORT.PINCFG[ ADC_0_POS_INPUT ].bit.PMUXEN = 1;
    ADC_0_PORT.PMUX[ ADC_0_POS_INPUT / 2].bit.PMUXE = 1;

    /* ADC_0_POS_INPUT Input */
    ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_POS_INPUT);
    ADC_0_PORT.PINCFG[ADC_0_POS_INPUT].bit.INEN = true;
    ADC_0_PORT.PINCFG[ADC_0_POS_INPUT].bit.PULLEN = false; 

    if(ADC_0_NEG_INPUT != ADC_INPUTCTRL_MUXNEG_GND)
    {
        ADC_0_PORT.DIRCLR.reg = (1 << ADC_0_NEG_INPUT);
        ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.INEN = true;
        ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.PULLEN = false;
    } 

    /* Set RUN_IN_STANDBY */
    adc->CTRLA.reg = (ADC_0_RUN_IN_STANDBY << ADC_CTRLA_RUNSTDBY_Pos);

    /* Set Voltage Reference */
    adc->REFCTRL.reg = (ADC_0_REF_COM_EN << ADC_REFCTRL_REFCOMP_Pos) | ADC_0_REF_DEFAULT;

    switch (precision)
    {
    case ADC_0_RES_8BIT:
        resolution = ADC_0_RES_8BIT;
        break;
    case ADC_0_RES_10BIT:
        resolution = ADC_0_RES_10BIT;
        break;
    case ADC_0_RES_12BIT:
        resolution = ADC_0_RES_12BIT;
        break;
    case ADC_0_RES_16BIT:
        divideResult = ADC_0_DIV_RES_DISABLE;
        switch(ADC_0_ACCUM_DEFAULT)
        {
            case ADC_0_ACCUM_512:
                accumulate = ADC_0_ACCUM_512;
                break;
            case ADC_0_ACCUM_1024:
                accumulate = ADC_0_ACCUM_1024;
                break;
            default:
                accumulate = ADC_0_ACCUM_256;
                break;
        }        
        resolution = ADC_0_RES_16BIT;
        break;
    default:
        DEBUG("Invalid precision");        
        return -1;
    }

    /* Set the accumlation and devide result */
    adc->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(divideResult) | accumulate;

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

    /* Configure CTRLB Register HERE IS THE RESOLUTION SET!*/
    adc->CTRLB.reg = 
        ADC_0_PRESCALER |
        resolution | 
        (ADC_0_CORRECTION_EN << ADC_CTRLB_CORREN_Pos) | 
        (ADC_0_FREE_RUNNING << ADC_CTRLB_FREERUN_Pos) | 
        (ADC_0_LEFT_ADJUST << ADC_CTRLB_LEFTADJ_Pos) |
        (ADC_0_DIFFERENTIAL_MODE << ADC_CTRLB_DIFFMODE_Pos);        


    /* Configure Window Mode Register */
    /*adc->WINCTRL.reg = ADC_0_WINDOW_MODE;
    while(adc_syncing(adc));*/

    /* Configure lower threshold */
    adc->WINLT.reg = ADC_0_WINDOW_LOWER << ADC_WINLT_WINLT_Pos;
    while(adc_syncing(adc));

    /* Configure lower threshold */
    adc->WINUT.reg = ADC_0_WINDOW_HIGHER << ADC_WINUT_WINUT_Pos;
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

    if (ADC_0_CORRECTION_EN)
    {
        if (ADC_0_GAIN_CORRECTION > ADC_GAINCORR_GAINCORR_Msk) 
        {
            DEBUG("Invalid gain correction\n");
            return -1;
        } 
        else 
        {
            /* Set gain correction value */
            adc->GAINCORR.reg = (ADC_0_GAIN_CORRECTION << ADC_GAINCORR_GAINCORR_Pos);
        }
        if (ADC_0_OFFSET_CORRECTION > 2047 || ADC_0_OFFSET_CORRECTION < -2048) 
        {
            DEBUG("Invalid offset correction\n");
            return -1;
        } 
        else 
        {
            /* Set offset correction value */
            adc->OFFSETCORR.reg = (ADC_0_OFFSET_CORRECTION << ADC_OFFSETCORR_OFFSETCORR_Pos);
        }
    }

    /* Disable all interrupts */
    adc->INTENCLR.reg =
        (1 << ADC_INTENCLR_SYNCRDY_Pos) | (1 << ADC_INTENCLR_WINMON_Pos) |
        (1 << ADC_INTENCLR_OVERRUN_Pos) | (1 << ADC_INTENCLR_RESRDY_Pos);

   
    /* Load the fixed device calibration constants*/
    adc->CALIB.reg = (ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos))
                    |
                    ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos)));
    DEBUG("\n\nADC CALIB BIAS: %u\n", bias);                    
    DEBUG("\n\nADC CALIB LINEARITY: %u\n\n", ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos))); 
    return 1;
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
uint32_t adc_get_status(Adc* adc)
{
    uint32_t interrupt_flags = adc->INTFLAG.reg;
    uint32_t status_flags = 0;

    if(interrupt_flags & ADC_INTFLAG_RESRDY)
        status_flags |= ADC_0_STATUS_RESULT_READY;
    if(interrupt_flags & ADC_INTFLAG_WINMON)
        status_flags |= ADC_0_STATUS_WINDOW;
    if(interrupt_flags & ADC_INTFLAG_OVERRUN)
        status_flags |= ADC_0_STATUS_OVERRUN;
    return status_flags;
}

#endif /* ADC_NUMOF */
