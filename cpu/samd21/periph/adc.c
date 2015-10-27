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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Mark Solters <msolters@driblet.io>
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

/*  Declare ADC configuration object  */
adc_conf_t adc_config;

/* Prototypes */
bool adc_syncing(Adc*);
void adc_clear_status(Adc*, uint32_t);
uint32_t adc_get_status(Adc*);

int adc_init(adc_t dev, adc_precision_t precision)
{
  //Adc *adc = 0;
  int init = 0;
  switch (dev) {
#if ADC_0_EN
    case ADC_0:
      //  Define ADC device.
      //adc = ADC_0_DEV;

      //  Initialize adc_config defaults.
      if (!adc_config.prescaler)
        adc_config.prescaler = ADC_0_PRESCALER;
      if (!adc_config.gain)
        adc_config.gain = ADC_0_GAIN_FACTOR_DEFAULT;
      if (!adc_config.accumulate)
        adc_config.accumulate = ADC_0_ACCUM_DEFAULT;
      if (!adc_config.divide)
        adc_config.divide = ADC_0_DIV_RES_DEFAULT;
      if (!adc_config.differential_mode)
        adc_config.differential_mode = ADC_0_DIFFERENTIAL_MODE;
      if (!adc_config.run_in_standby)
        adc_config.run_in_standby = ADC_0_RUN_IN_STANDBY;
      if (!adc_config.precision)
      {
        switch (precision)
        {
          case ADC_RES_8BIT:
            adc_config.precision = ADC_0_RES_8BIT;
            break;
          case ADC_RES_10BIT:
            adc_config.precision = ADC_0_RES_10BIT;
            break;
          case ADC_RES_12BIT:
            adc_config.precision = ADC_0_RES_12BIT;
            break;
          case ADC_RES_16BIT:
            adc_config.precision = ADC_0_RES_16BIT;
          default:
            DEBUG("Invalid precision");
            return -1;
        }
      }

      //  Configure ADC Module with adc_config.
      init = adc_configure(dev);
      if(init == -1)
        return -1;

      break;
#endif
    default:
      return -1;
  }
  return 0;
}

int adc_sample(adc_t dev, int channel)
{
  if (dev != ADC_0) {
      return -2;
  }
  if (channel >= ADC_0_CHANNELS) {
      return -1;
  }

  // Select the inputs.
  if (adc_config.differential_mode)
  {
    /*  In differential mode, use ADC_0_POS_INPUT & ADC_0_NEG_INPUT
     */
    ADC->INPUTCTRL.reg = adc_config.gain
                          /*| (ADC_0_PIN_SCAN_OFFSET_START << ADC_INPUTCTRL_INPUTOFFSET_Pos)
                          | (ADC_0_PIN_SCAN_INPUT_TO_SCAN << ADC_INPUTCTRL_INPUTSCAN_Pos)*/
                          | ADC_0_NEG_INPUT
                          | ADC_0_POS_INPUT;
  } else
  {
    /*  Otherwise, use the positive input corresponding to the provided arg
     *  "channel", and use GND as the negative input.
     */
    ADC->INPUTCTRL.reg = adc_config.gain
                          | adc_channels[channel].muxpos
                          | ADC_INPUTCTRL_MUXNEG_IOGND;
  }

  // Wait for sync.
  while (ADC->STATUS.reg & ADC_STATUS_SYNCBUSY);

  // Start the conversion.
  ADC->SWTRIG.reg = ADC_SWTRIG_START;

  // Wait for the result.
  while (!(ADC->INTFLAG.reg & ADC_INTFLAG_RESRDY));

  // Read & return result.
  return (int)ADC->RESULT.reg;
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
      break;
#endif
    default:
      break;
  }
}

int adc_map(adc_t dev, int value, int min, int max)
{
  return (int)adc_mapf(dev, value, (float)min, (float)max);
}

float adc_mapf(adc_t dev, int value, float min, float max)
{
  int resolution_bits;
  switch (adc_config.precision)
  {
    case ADC_0_RES_8BIT:
      resolution_bits = 8;
      break;
    case ADC_0_RES_10BIT:
      resolution_bits = 10;
      break;
    case ADC_0_RES_12BIT:
      resolution_bits = 12;
      break;
    case ADC_0_RES_16BIT:
      resolution_bits = 16;
      break;
    default:
      return -1;
  }
  return ((max - min) / ((float)(1 << resolution_bits) - 1)) * value;
}

bool adc_syncing(Adc* adc)
{
  if(adc->STATUS.reg & ADC_STATUS_SYNCBUSY){
    return true;
  }
  return false;
}

adc_conf_t* adc_get_configuration(adc_t dev)
{
  return &adc_config;
}

int adc_test(void)
{
  printf("%lu", adc_config.accumulate);
  return 0;
}

/* Configure ADC with parameters set in adc_conf */
int adc_configure(adc_t dev)
{

  //  Define ADC device corresponding to dev.
  Adc *adc = 0;
  switch (dev)
  {
#if ADC_0_EN
    case ADC_0:
      adc = ADC_0_DEV;
      break;
#endif
    default:
      break;
  }

  //  Wait for sync.
  while(adc_syncing(adc));

  //  Disable ADC Module before init.
  adc->CTRLA.reg &= ~ADC_CTRLA_ENABLE;

  /* Power On */
  adc_poweron(ADC_0);

  if(adc->CTRLA.reg & ADC_CTRLA_SWRST)
    return -1;
  if(adc->CTRLA.reg & ADC_CTRLA_ENABLE)
    return -1;

  /* GCLK Setup*/
  GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN
                        | GCLK_CLKCTRL_GEN_GCLK0
                        | (ADC_GCLK_ID << GCLK_CLKCTRL_ID_Pos)));

  /* Pin Muxing */
  if (adc_config.differential_mode)
  {
    /*
    //  Configure ADC_0_POS_INPUT and ADC_0_NEG_INPUT
    ADC_0_PORT.PINCFG[ ADC_0_POS_INPUT ].bit.PMUXEN = 1;
    ADC_0_PORT.PMUX[ ADC_0_POS_INPUT / 2].bit.PMUXE = 1;

    //  ADC_0_POS_INPUT Input
    ADC_0_PORT.DIRCLR.reg = (1UL << ADC_0_POS_INPUT);
    ADC_0_PORT.PINCFG[ADC_0_POS_INPUT].bit.INEN = true;
    ADC_0_PORT.PINCFG[ADC_0_POS_INPUT].bit.PULLEN = false;

    //  ADC_0_NEG_INPUT Input
    if(ADC_0_NEG_INPUT != ADC_INPUTCTRL_MUXNEG_GND)
    {
      ADC_0_PORT.DIRCLR.reg = (1UL << ADC_0_NEG_INPUT);
      ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.INEN = true;
      ADC_0_PORT.PINCFG[ADC_0_NEG_INPUT].bit.PULLEN = false;
    }
    */
  } else
  {
    /*  Configure all positive inputs enumerated in adc_channels  */
    for (int i = 0; i < ADC_0_CHANNELS; i++) {
      PortGroup *port = adc_channels[i].port;
      int pin = adc_channels[i].pin;
      port->DIRCLR.reg = (1 << pin);
      port->PINCFG[pin].reg = (PORT_PINCFG_PMUXEN | PORT_PINCFG_INEN);
      port->PMUX[pin >> 1].reg = ~(0xf << (4 * (pin & 0x1)));
      port->PMUX[pin >> 1].reg =  (1 << (4 * (pin & 0x1)));
    }
  }

  /* Set RUN_IN_STANDBY */
  adc->CTRLA.reg = (adc_config.run_in_standby << ADC_CTRLA_RUNSTDBY_Pos);

  /* Set Voltage Reference */
  adc->REFCTRL.reg = (ADC_0_REF_COM_EN << ADC_REFCTRL_REFCOMP_Pos) | ADC_0_REF_DEFAULT;

  /* Configure Hardware Averaging */
  uint32_t resolution = adc_config.precision;
  uint32_t accumulate = adc_config.accumulate;
  uint8_t  divideResult = adc_config.divide;
  if (ADC_0_RES_16BIT == resolution)
  {
    // In the subcase of 16-bit resolution,
    // handle the edge case of overflow-by-
    // division.
    divideResult = ADC_0_DIV_RES_DISABLE;
    switch(accumulate)
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
  }

  /* Set the accumulation and divide result */
  adc->AVGCTRL.reg = ADC_AVGCTRL_ADJRES(divideResult) | accumulate;

  /* Set Sample length */
  adc->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(ADC_0_SAMPLE_LENGTH);//(ADC_0_SAMPLE_LENGTH << ADC_SAMPCTRL_SAMPLEN_Pos);
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
      adc_config.prescaler |
      resolution |
      (ADC_0_CORRECTION_EN << ADC_CTRLB_CORREN_Pos) |
      (ADC_0_FREE_RUNNING << ADC_CTRLB_FREERUN_Pos) |
      (ADC_0_LEFT_ADJUST << ADC_CTRLB_LEFTADJ_Pos) |
      (adc_config.differential_mode << ADC_CTRLB_DIFFMODE_Pos);


  /* Configure Window Mode Register */
  /*adc->WINCTRL.reg = ADC_0_WINDOW_MODE;
  while(adc_syncing(adc));*/

  /* Configure lower threshold */
  adc->WINLT.reg = ADC_0_WINDOW_LOWER << ADC_WINLT_WINLT_Pos;
  while(adc_syncing(adc));

  /* Configure lower threshold */
  adc->WINUT.reg = ADC_0_WINDOW_HIGHER << ADC_WINUT_WINUT_Pos;
  while(adc_syncing(adc));

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
  adc->CALIB.reg = ADC_CALIB_BIAS_CAL((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR >> ADC_FUSES_BIASCAL_Pos))
                  | ADC_CALIB_LINEARITY_CAL((*(uint64_t*)ADC_FUSES_LINEARITY_0_ADDR >> ADC_FUSES_LINEARITY_0_Pos));

  while(adc_syncing(adc));

  //  Enable bandgap if VREF is internal 1V.
  if(ADC_0_REF_DEFAULT == ADC_0_REF_INT_1V)
  {
    SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
  }

  //  Enable ADC Module.
  adc->CTRLA.reg |= ADC_CTRLA_ENABLE;

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
