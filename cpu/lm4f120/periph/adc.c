/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

typedef struct {
    int max_value;
} adc_config_t;

adc_config_t adc_config[ADC_NUMOF];

int adc_init(adc_t dev, adc_precision_t precision)
{
    adc_poweron(dev);
	// ADC0 is used with AIN0 on port E3.
	// ADC1 is used with AIN1 on port E2.
	// GPIO port E needs to be enabled so that these pinds can be used
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Set the ADC to 125KSPS.
	// This requires less power and produce longer samping time,
	// creating accurate conversions
	ROM_SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);

	switch (precision) {
        case ADC_RES_6BIT:
        case ADC_RES_8BIT:
        case ADC_RES_10BIT:
#if ADC_0_EN
			ROM_ADCResolutionSet(ADC0_BASE, ADC_RES_10BIT_S);
#endif	
#if ADC_1_EN
			ROM_ADCResolutionSet(ADC1_BASE, ADC_RES_10BIT_S);
#endif	
            adc_config[dev].max_value = 0x3ff;
            break;
        case ADC_RES_12BIT:
#if ADC_0_EN
			ROM_ADCResolutionSet(ADC0_BASE, ADC_RES_12BIT_S);
#endif	
#if ADC_1_EN
			ROM_ADCResolutionSet(ADC1_BASE, ADC_RES_12BIT_S);
#endif	
            adc_config[dev].max_value = 0xfff;
            break;
        case ADC_RES_14BIT:
        case ADC_RES_16BIT:
            break;
    }
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
			// Select the Analog ADC Function for these pins.
            ROM_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
			
			// Before configuring the sequencer, we need to disable ita to prevent errorneous execution
			ROM_ADCSequenceDisable(ADC0_BASE, 3);

			// Enable Sample Sequence 3 with a Software Start (Processor signal trigger).
			// The software writes an 8 (SS3) to ADC_PSSI_R to initiate a conversion on sequencer 3.
			// Sequence 3 will do a single sample when the processor sends a signal to start the conversion.
			ROM_ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

			// Configure step 0 on sequence 3. 
			// Sample channel 0 (ADC_CTL_CH0) in single-ended mode and configure the interrupt flag.
			// (ADC_CTL_IE) to be set to enable Interrupt.
			ROM_ADCSequenceStepConfigure(ADC0_BASE, 3, 0, 
					ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

			// Clear the interrupt status flag. This is done to make sure
			// the interrupt flag is cleared before we sample.
			ROM_ADCIntClear(ADC0_BASE, 3);

			// Since sample sequence 3 is now configured, it must be enabled.
			ROM_ADCSequenceEnable(ADC0_BASE, 3);

            break;
#endif
#if ADC_1_EN
        case ADC_1:
			// Select the Analog ADC Function for these pins.
            ROM_GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
			
			// Before configuring the sequencer, we need to disable ita to prevent errorneous execution
			ROM_ADCSequenceDisable(ADC1_BASE, 3);

			// Enable Sample Sequence 3 with a Software Start (Processor signal trigger).
			// The software writes an 8 (SS3) to ADC_PSSI_R to initiate a conversion on sequencer 3.
			// Sequence 3 will do a single sample when the processor sends a signal to start the conversion.
			ROM_ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

			// Configure step 0 on sequence 3. 
			// Sample channel 0 (ADC_CTL_CH1) in single-ended mode and configure the interrupt flag.
			// (ADC_CTL_IE) to be set to enable Interrupt.
			ROM_ADCSequenceStepConfigure(ADC1_BASE, 3, 0, 
					ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

			// Clear the interrupt status flag. This is done to make sure
			// the interrupt flag is cleared before we sample.
			ROM_ADCIntClear(ADC1_BASE, 3);

			// Since sample sequence 3 is now configured, it must be enabled.
			ROM_ADCSequenceEnable(ADC1_BASE, 3);

            break;
#endif
        default:
            return -1;
    }
    return 0;
}

int adc_sample(adc_t dev, int channel)
{
	unsigned long ulADC_val=0;

    switch (dev) {
#if ADC_0_EN
        case ADC_0:
			// Trigger the ADC conversion
			ROM_ADCProcessorTrigger(ADC0_BASE, 3);

			// Wait for conversion to be completed.
			while(!ROM_ADCIntStatus(ADC0_BASE, 3, false));

			// Read ADC value.
			ROM_ADCSequenceDataGet(ADC0_BASE, 3, &ulADC_val);

			// Clear the ADC interrupt flag
			ROM_ADCIntClear(ADC0_BASE, 3);
			break;
#endif
#if ADC_1_EN
        case ADC_1:
			// Trigger the ADC conversion
			ROM_ADCProcessorTrigger(ADC1_BASE, 3);

			// Wait for conversion to be completed.
			while(!ROM_ADCIntStatus(ADC1_BASE, 3, false));

			// Read ADC value.
			ROM_ADCSequenceDataGet(ADC1_BASE, 3, &ulADC_val);

			// Clear the ADC interrupt flag
			ROM_ADCIntClear(ADC1_BASE, 3);
			break;
#endif
		default:
			return -1;
    }

    /* return result */
    return ((int)ulADC_val);
}

void adc_poweron(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
			// The ADC0 Peripheral must be enabled for use
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
            break;
#endif
#if ADC_1_EN
        case ADC_1:
			ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
            break;
#endif
    }
}

void adc_poweroff(adc_t dev)
{
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
			ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_ADC0);
            break;
#endif
#if ADC_1_EN
        case ADC_1:
			ROM_SysCtlPeripheralDisable(SYSCTL_PERIPH_ADC0);
            break;
#endif
    }
}

int adc_map(adc_t dev, int value, int min, int max)
{
    return (int)adc_mapf(dev, value, (float)min, (float)max);
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return ((max - min) / ((float)adc_config[dev].max_value)) * value;
}

#endif /* ADC_NUMOF */
