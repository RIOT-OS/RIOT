/*
 * adc.c - implementation of the Analog to Digital Converter module of the mc1322x MCU
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "mc1322x-adc.h"
#include "gpio.h"

#define ADC_CLOCK_DIVIDER_300kHz 0x50

#ifndef REF_OSC
#define REF_OSC 24000000UL          /* reference osc. frequency */
#endif

#define ADC_PRESCALE_CLK 1000000    /* targeted prescale clk */
#define ADC_PRESCALE_VALUE ((REF_OSC / ADC_PRESCALE_CLK)-1)

#define ADC_CONVERT_TIME 23         /* function of prescale clk. has to be >= 20us */

#define ADC_NUM_CHANS 8

/**
 * Initializes the ADC module.
 *
 * Analog Clk set to 300kHz
 * Prescale Clk set to 1MHz
 * Convert Time set to 23us
 * Not using timers or IRQs
 */
void adc_init(void)
{
    /* configure frequencies */
    ADC->CLOCK_DIVIDER = ADC_CLOCK_DIVIDER_300kHz;
    ADC->PRESCALE = ADC_PRESCALE_VALUE;

    /* power on */
    ADC->CONTROLbits.ON = 0x1;

    /* ON-TIME must be >= 10us */
    ADC->ON_TIME = 0xa;

    /* should be >= 20us (6 ADC clks) */
    ADC->CONVERT_TIME = ADC_CONVERT_TIME;

    /* automated mode */
    ADC->MODE = 0x0;

    /* don't use IRQs */
    ADC->FIFO_CONTROL = 0x0;

    /* disable all input channels */
    ADC->SEQ_1 = 0x0;
    /* sequence using convert time */
    ADC->SEQ_1bits.SEQ_MODE = 0x0;
    /* enable battery reference voltage channel */
    ADC->SEQ_1bits.BATT = 0x1;

    /* disable all input channels */
    ADC->SEQ_2 = 0x0;
    /* sequence using convert time */
    ADC->SEQ_2bits.SEQ_MODE = 0x0;
}

/**
 * Set up a given channel 0...7 for ADC usage.
 *
 * \param channel The channel to set up
 */
void adc_setup_channel(uint8_t channel)
{
    switch (channel) {
        case 0:
            ADC->SEQ_1bits.CH0 = 0x1;
            GPIO->FUNC_SEL.ADC0 = 0x1;
            GPIO->PAD_DIR.ADC0 = 0x0;
            GPIO->PAD_KEEP.ADC0 = 0x0;
            GPIO->PAD_PU_EN.ADC0 = 0;
            break;

        case 1:
            ADC->SEQ_1bits.CH1 = 0x1;
            GPIO->FUNC_SEL.ADC1 = 0x1;
            GPIO->PAD_DIR.ADC1 = 0x0;
            GPIO->PAD_KEEP.ADC1 = 0x0;
            GPIO->PAD_PU_EN.ADC1 = 0;
            break;

        case 2:
            ADC->SEQ_1bits.CH2 = 0x1;
            GPIO->FUNC_SEL.ADC2 = 0x1;
            GPIO->PAD_DIR.ADC2 = 0x0;
            GPIO->PAD_KEEP.ADC2 = 0x0;
            GPIO->PAD_PU_EN.ADC2 = 0;
            break;

        case 3:
            ADC->SEQ_1bits.CH3 = 0x1;
            GPIO->FUNC_SEL.ADC3 = 0x1;
            GPIO->PAD_DIR.ADC3 = 0x0;
            GPIO->PAD_KEEP.ADC3 = 0x0;
            GPIO->PAD_PU_EN.ADC3 = 0;
            break;

        case 4:
            ADC->SEQ_1bits.CH4 = 0x1;
            GPIO->FUNC_SEL.ADC4 = 0x1;
            GPIO->PAD_DIR.ADC4 = 0x0;
            GPIO->PAD_KEEP.ADC4 = 0x0;
            GPIO->PAD_PU_EN.ADC4 = 0;
            break;

        case 5:
            ADC->SEQ_1bits.CH5 = 0x1;
            GPIO->FUNC_SEL.ADC5 = 0x1;
            GPIO->PAD_DIR.ADC5 = 0x0;
            GPIO->PAD_KEEP.ADC5 = 0x0;
            GPIO->PAD_PU_EN.ADC5 = 0;
            break;

        case 6:
            ADC->SEQ_1bits.CH6 = 0x1;
            GPIO->FUNC_SEL.ADC6 = 0x1;
            GPIO->PAD_DIR.ADC6 = 0x0;
            GPIO->PAD_KEEP.ADC6 = 0x0;
            GPIO->PAD_PU_EN.ADC6 = 0;
            break;

        case 7:
            ADC->SEQ_1bits.CH7 = 0x1;
            GPIO->FUNC_SEL.ADC7 = 0x1;
            GPIO->PAD_DIR.ADC7 = 0x0;
            GPIO->PAD_KEEP.ADC7 = 0x0;
            GPIO->PAD_PU_EN.ADC7 = 0;
            break;
    }
}

/**
 * Read from the ADC FIFO
 * Reads a 16 bit value from the ADC FIFO.
 * Bits 15:12 contain the channel the value origin.
 * Bits 11:0  contain the actual measured value.
 *
 * \return 16 Bits containing the channel and the measurement.
 */
uint16_t adc_read(void)
{
    /* wait for ADC result */
    while (ADC->FIFO_STATUSbits.EMPTY) {
        continue;
    }

    /* upper 4 bits contain channel number */
    return ADC->FIFO_READ;
}

/**
 * Flushes any measured values from the ADC FIFO until FIFO is empty.
 */
void adc_flush(void)
{
    while (!ADC->FIFO_STATUSbits.EMPTY) {
        ADC->FIFO_READ;
    }
}

/**
 * When using several channels simultaniously this function can read
 * values from the ADC FIFO and store them in an array sorted by the
 * channel number.
 *
 * \param channels_read An array of 8 uint16_t the measured values get
 *                      stored into. The user could use ADC_NUM_CHANS
 *                      to asure compliancy.
 */
void adc_service(uint16_t *channels_read)
{
    while (!ADC->FIFO_STATUSbits.EMPTY) {
        uint16_t tmp = ADC->FIFO_READ;

        if ((tmp >> 12) < ADC_NUM_CHANS) {
            channels_read[tmp >> 12] = tmp & 0x0fff;
        }
    }
}
