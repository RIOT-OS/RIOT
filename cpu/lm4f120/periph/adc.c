/*
 * Copyright (C) 2015 Marc Poulhiès
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
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "periph/adc.h"
#include "periph_conf.h"

/* guard in case that no ADC device is defined */
#if ADC_NUMOF

struct adc_gpio_cfg_s {
  unsigned long gpio_base;
  unsigned long gpio_sysctl;
  unsigned short gpio_pin;
};

static const struct adc_gpio_cfg_s adc0_gpio[ADC_MAX_CHANNELS] = {
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_3}, // AIN0
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_2}, // AIN1
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_1}, // AIN2
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_0}, // AIN3
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_3}, // AIN4
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_2}, // AIN5
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_1}, // AIN6
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOD, GPIO_PIN_0}, // AIN7
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_5}, // AIN8
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOE, GPIO_PIN_4}, // AIN9
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOB, GPIO_PIN_4}, // AIN10
  {GPIO_PORTE_BASE, SYSCTL_PERIPH_GPIOB, GPIO_PIN_5}, // AIN11
};

#define ADC_0_SYSCTL SYSCTL_PERIPH_ADC0
#define ADC_0_GPIO adc0_gpio
//#define ADC_0_GPIO_PIN GPIO_PIN_3
#define ADC_0_BASE ADC0_BASE
#define ADC_0_CHANNEL ADC_CTL_CH0
#define ADC_0_SEQ 3

typedef struct {
    uint8_t precision;
} adc_config_t;


adc_config_t config[ADC_NUMOF];

int adc_init(adc_t dev, adc_precision_t precision)
{
    unsigned long adc_base;
    unsigned long adc_seq;
    switch (dev) {
#if ADC_0_EN
    case ADC_0:
      adc_base = ADC_0_BASE;
      adc_seq = ADC_0_SEQ;
      break;
#endif
    }
    ROM_SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS);

    adc_poweron(dev);

    ROM_ADCHardwareOversampleConfigure(adc_base, 64);
    
    ROM_ADCSequenceDisable(adc_base, adc_seq);

    return 0;
}

int adc_sample(adc_t dev, int channel)
{
  int value[2];
  unsigned long adc_base;
  unsigned long adc_seq;
  unsigned long adc_gpio_port = 0;
  unsigned long adc_gpio_pin = 0;
  unsigned long adc_gpio_sysctl = 0;

  if (channel >= ADC_MAX_CHANNELS){
    return -1;
  }

  switch (dev) {
#if ADC_0_EN
    case ADC_0:
      adc_base = ADC_0_BASE;
      adc_seq = ADC_0_SEQ;

      adc_gpio_port = ADC_0_GPIO[channel].gpio_base;
      adc_gpio_sysctl = ADC_0_GPIO[channel].gpio_sysctl;
      adc_gpio_pin = ADC_0_GPIO[channel].gpio_pin;
      break;
#endif
    }
    ROM_SysCtlPeripheralEnable(adc_gpio_sysctl);
    ROM_GPIOPinTypeADC(adc_gpio_port, adc_gpio_pin);

    ROM_ADCSequenceDisable(adc_base, adc_seq);
    ROM_ADCIntClear(adc_base, adc_seq);

    ROM_ADCSequenceConfigure(adc_base, adc_seq, ADC_TRIGGER_PROCESSOR, 0);

    ROM_ADCSequenceStepConfigure(adc_base, adc_seq, 0, channel | ADC_CTL_IE | ADC_CTL_END);
    ROM_ADCSequenceEnable(adc_base, adc_seq);

    ROM_ADCIntClear(adc_base, adc_seq);

    ROM_ADCProcessorTrigger(adc_base, adc_seq);
    while(!ROM_ADCIntStatus(adc_base, adc_seq, false)) {
    }
    ROM_ADCIntClear(adc_base, adc_seq);
    ROM_ADCSequenceDataGet(adc_base, adc_seq, (unsigned long*) value);

    return value[0];
}

void adc_poweron(adc_t dev)
{
  unsigned long adc_sysctl;
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
	  adc_sysctl = ADC_0_SYSCTL;
	  break;
#endif
    }
    ROM_SysCtlPeripheralEnable(adc_sysctl);
}

void adc_poweroff(adc_t dev)
{
  unsigned long adc_sysctl;
    switch (dev) {
#if ADC_0_EN
        case ADC_0:
	  adc_sysctl = ADC_0_SYSCTL;
	  break;
#endif
    }
    ROM_SysCtlPeripheralDisable(adc_sysctl);
}

int adc_map(adc_t dev, int value, int min, int max)
{
    return 0;
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
    return 0.0;
}

#endif /* ADC_NUMOF */
