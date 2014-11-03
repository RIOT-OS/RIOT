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


/* guard in case that no ADC device is defined */
#if ADC_NUMOF

enum adc_system_clock_apb_bus 
{
    //SYSTEM_CLOCK_APB_APBA, //NOT USED BY ADC
    //SYSTEM_CLOCK_APB_APBB, //NOT USED BY ADC
    SYSTEM_CLOCK_APB_APBC,
};

/* Prototypes */
void adc_system_apb_clock_set_mask(const enum adc_system_clock_apb_bus, const uint32_t);
void adc_system_gclk_chan_set_config(const uint8_t, uint8_t);
void adc_system_gclk_chan_enable(const uint8_t);
void adc_gclk(void);
bool adc_syncing(Adc*);
void adc_system_voltage_ref_enable(void);
void adc_enable(Adc*);
void adc_disable(Adc*);
int adc_config_setup(Adc*, adc_precision_t);
void adc_start_conversion(Adc*);
void adc_clear_status(Adc*, uint32_t);
bool adc_get_status(Adc*);
void adc_configure_with_resolution(Adc*, int);

/* ADC Init */
int adc_init(adc_t dev, adc_precision_t precision)
{
    adc_poweron(dev);
    Adc *adc = 0;
    switch (dev) {
        #if ADC_0_EN
            case ADC_0:
                adc = ADC_0_DEV;
                int response = adc_config_setup(adc, precision);
                if(response == -1)
                {
                    return response;
                }
                adc_enable(adc);
            break;
        #endif
    }
    return 0;
}

/* ADC Sample */
int adc_sample(adc_t dev, int channel)
{
    uint16_t result;

     Adc *adc = 0;
    switch (dev) {
        #if ADC_0_EN
            case ADC_0:
                adc = ADC_0_DEV;
                adc_start_conversion(adc);
            break;
        #endif
    }
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ }

    result = adc->RESULT.reg;

    //CLEAR RESET FLAG
    adc_clear_status(adc, ADC_0_STATUS_RESULT_READY);

    while(!adc_get_status(adc)){} // MUST NOT BLOCK

    return result;
}

void adc_poweron(adc_t dev)
{
    switch (dev) 
    {
    #if ADC_0_EN
        case ADC_0:            
            adc_system_apb_clock_set_mask(SYSTEM_CLOCK_APB_APBC, PM_APBCMASK_ADC);
            adc_gclk();
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
            adc_disable(adc); // perhaps theres a need to clear the gclk channel as well???
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
    return 0.0;
}

/* Setup generic clock mask for adc */
void adc_system_apb_clock_set_mask(const enum adc_system_clock_apb_bus bus, const uint32_t mask)
{
    switch(bus)
    {
        case SYSTEM_CLOCK_APB_APBC:
            PM->APBCMASK.reg |= mask;
            break;
        default:
            return;
            break;
    }
}

/* Setup generic clock channel for adc */
void adc_system_gclk_chan_set_config(const uint8_t channel, uint8_t generator)
{
    uint32_t clkctrlconfig = (channel << GCLK_CLKCTRL_GEN_Pos);
    
    //system_gclk_chan_disable(channel); //TODO:

    // write new configuration
    GCLK->CLKCTRL.reg = clkctrlconfig;
}

/* Enable generic clock channel */
void adc_system_gclk_chan_enable(const uint8_t channel)
{
    //cpu_irq_enter_critical(); // TODO: Need a "lock" ???
    
    // Select requested generator channel
    *((uint8_t*)&GCLK->CLKCTRL.reg) = channel;

    // enable generic clock
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;

    //cpu_irq_leave_critical(); //TODO:
}

/* Setup gclk */
void adc_gclk(void)
{
    adc_system_gclk_chan_set_config(ADC_GCLK_ID, ADC_0_CLK_SOURCE);
    adc_system_gclk_chan_enable(ADC_GCLK_ID);
}

/* ADC syncing */
bool adc_syncing(Adc* adc)
{
    if(adc->STATUS.reg & ADC_STATUS_SYNCBUSY)
        return true;
    return false;
}

/* VREF internal reference 1 volt */
void adc_system_voltage_ref_enable(void)
{
    SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
}

/* ADC Enable */
void adc_enable(Adc* adc)
{
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ }

    /* Enable bandgap if internal ref 1 volt */
    if(ADC_0_REF_DEFAULT == ADC_0_REF_INT_1V)
        adc_system_voltage_ref_enable();

    /* Enable */
    adc->CTRLA.reg |= ADC_CTRLA_ENABLE;
}

/* ADC Disable */
void adc_disable(Adc* adc)
{
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ } 

    /* Disable */
    adc->CTRLA.reg &= ~ADC_CTRLA_ENABLE;
} 

/* Config Setup */
int adc_config_setup(Adc* adc, adc_precision_t precision)
{
    switch (precision) {
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
        }
    return 0;
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

/* Starts the ADC conversion */
void adc_start_conversion(Adc* adc)
{
    while(adc_syncing(adc)) {/*wait for the ADC to become ready!*/ }
    adc->SWTRIG.reg |= ADC_SWTRIG_START;
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
