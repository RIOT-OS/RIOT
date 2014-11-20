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

/**
 * @brief Initialization of a given ADC device
 *
 * The ADC will be initialized in synchronous, blocking mode, so no callbacks for finished
 * conversions are required as conversions are presumed to be very fast (somewhere in the
 * range of some us).
 *
 * @param[in] dev           the device to initialize
 * @param[in] precision     the precision to use for conversion
 *
 * @return                  0 on success
 * @return                  -1 on precision not available
 */
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
                case ADC_RES_6BIT: // Not possible!
                    return -1;
                    break;
                case ADC_RES_8BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_8BIT); 
                    break;
                case ADC_RES_10BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_10BIT); 
                    break;
                case ADC_RES_12BIT:
                    adc_configure_with_resolution(adc, ADC_0_RES_12BIT); 
                    break;
                case ADC_RES_14BIT: // Not possible!
                    return -1;
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

/**
 * @brief Start a new conversion by using the given channel.
 *
 * If a conversion on any channel on the given ADC core is in progress, it is aborted.
 *
 * @param[in] dev           the ADC device to use for the conversion
 * @param[in] channel       the channel to convert from
 *
 * @return                  the converted value
 * @return                  -1 on invalid channel
 */
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
    }
    while(adc_syncing(adc));
    result = adc->RESULT.reg;
    //CLEAR RESET FLAG
    adc_clear_status(adc, ADC_0_STATUS_RESULT_READY);
    while(!adc_get_status(adc)){} /* MUST NOT BLOCK */
    return result;
}

/**
 * @brief Enable the power for the given ADC device
 *
 * @param[in] dev           the ADC device to power up
 */
void adc_poweron(adc_t dev) 
{
    switch (dev) 
    {
    #if ADC_0_EN
        case ADC_0:
            /* Setup generic clock mask for adc */
            PM->APBCMASK.reg |= PM_APBCMASK_ADC;
            /* Setup generic clock channel for adc */ 
            //system_gclk_chan_disable(ADC_GCLK_ID); //TODO:                            
            GCLK->CLKCTRL.reg |= (ADC_GCLK_ID << GCLK_CLKCTRL_GEN_Pos);
            /* Enable generic clock channel */
            // Need a "mutex" ???  TODO:  
            // Select requested generator channel
            *((uint8_t*)&GCLK->CLKCTRL.reg) |= ADC_GCLK_ID;
            // enable generic clock
            GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
            //release mutex TODO:
            break;
    #endif
        default:
            break;    
    }
}

/**
 * @brief Disable the power for the given ADC device
 *
 * @param[in] dev           the ADC device to power down
 */
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

/**
 * @brief Helper function to map a converted value to the given integer range.
 *
 * This function is useful for converting sampled ADC values into their physical representation.
 *
 * The min value is asserted to be smaller than the max value.
 *
 * @param[in] dev           the ADC device to read the precision value from (as input interval)
 * @param[in] value         the value to map
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
int adc_map(adc_t dev, int value, int min, int max)
{
    DEBUG("adc_map Not implemented!\n");
    return 0;
}

/**
 * @brief Helper function to map a converted value to the given float range
 *
 * @see adc_map
 *
 * @param[in] dev           the ADC device to read the precision value from (as input interval)
 * @param[in] value         the value to map
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
float adc_mapf(adc_t dev, int value, float min, float max)
{
    DEBUG("adc_mapf Not implemented!\n");
    return 0.0;
}

/* ADC syncing */
bool adc_syncing(Adc* adc)
{
    if(adc->STATUS.reg & ADC_STATUS_SYNCBUSY)
        return true;
    return false;
}

/* Configure ADC with defined Resolution */
void adc_configure_with_resolution(Adc* adc, int precision)
{
    adc->CTRLA.reg = (ADC_0_RUN_IN_STANDBY << ADC_CTRLA_RUNSTDBY_Pos); // Set RUN_IN_STANDBY register to default
    adc->REFCTRL.reg = (ADC_0_REF_COM_EN << ADC_REFCTRL_REFCOMP_Pos) | ADC_0_REF_DEFAULT; // Set Reference config register to default
    adc->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(ADC_0_DIV_RES_DEFAULT) | ADC_0_ACCUM_DEFAULT; // Set the accumaltion and devide results register
    adc->SAMPCTRL.reg = (ADC_0_SAMPLE_LENGTH << ADC_SAMPCTRL_SAMPLEN_Pos); // Set Sample length register with default value
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ } 

    /* Configure CTRLB Register*/  // <---- HERE IS THE RESOLUTION SET!
    adc->CTRLB.reg = 
        ADC_0_PRESCALER | 
        /*ADC_0_RES_16BIT*/
        precision | 
        (ADC_0_CORRECTION_EN << ADC_CTRLB_CORREN_Pos) | 
        (ADC_0_FREE_RUNNING << ADC_CTRLB_FREERUN_Pos) | 
        (ADC_0_LEFT_ADJUST << ADC_CTRLB_LEFTADJ_Pos) |
        (ADC_0_DIFFERENTIAL_MODE << ADC_CTRLB_DIFFMODE_Pos);        
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ }

    /* Configure Window Mode Register */
    adc->WINCTRL.reg = ADC_0_WINDOW_MODE;
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ }

    //Perhaps lower&upper window must be set, even though it's disabled???

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
