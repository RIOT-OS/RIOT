
#include "sensor_driver.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include "unistd.h"
#include "configuration.h"
#include "periph/adc.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "vtimer.h"
#define ENABLE_DEBUG    (0)
#include "debug.h"

void activateRelayCtrl(void)
{
    gpio_init_out(GPIO_0, GPIO_NOPULL);
    gpio_set(GPIO_0);
    usleep(RelayCtrlDelay);
};

void deactivateRelayCtrl(void)
{
    gpio_clear(GPIO_0);
};

void adcSetup(void)
{
    adc_t dev;
    dev = ADC_0; 
    adc_precision_t pre;
    pre = ADC_RES_16BIT;
    int response = adc_init(dev, pre);
    if(response == -1)
        DEBUG("ADC init failed");
};

void i2cSetup(void)
{
    DEBUG("\nNot implemented!\n");
};

int getAdcValue(void)
{
    adc_t dev;
    dev = ADC_0;
    int result = 0;
    uint32_t resultAccum = 0;
    uint16_t count = 0;  

    activateRelayCtrl();
    while( count < accumulateNum)
    {
        result = adc_sample(dev, 0);       
        resultAccum += result;
        count++;
    } 
    deactivateRelayCtrl();

    resultAccum = (uint32_t)(resultAccum/accumulateNum); 

    return resultAccum;
};

int getI2cValue(uint8_t adr)
{
    DEBUG("\nNot implemented!\n");
    return 34;
}

uint32_t getWeightValueFromAdcInput(uint32_t adcValue)
{
    if(adcValue == 0)
    {
        DEBUG("ADC value == 0\n");
    }

    double voltStepRatio = (AdcReferenceVoltage/(double)AdcResolution);
    double resultVoltage = (double)(((double)adcValue)*voltStepRatio);
    double resultVoltageGainCorr = (resultVoltage/gainCorrection);
    double resultVoltageGainFactor = (resultVoltageGainCorr/gainFactor);
    double resultVoltageMicroVolt = (resultVoltageGainFactor/microVoltPrGram);
    uint32_t result = (uint32_t)resultVoltageMicroVolt;

    return result;
};

double getBatteryValueFromAdcInput(int adcValue)
{
    DEBUG("\nNot implemented!\n");
    return 0.0;
};

void setWeightTara(void)
{
    DEBUG("\nNot implemented!\n");
}