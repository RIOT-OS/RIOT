/*
 * mc1322x-adc.h - Structure definition for registers of the
 * Analog to Digital Converter module of the mc1322x MCU
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This source code is licensed under the GNU Lesser General Public License,
 * Version 2.  See the file LICENSE for more details.
 *
 * This file is part of RIOT.
 */

#ifndef MC1322X_ADC_H
#define MC1322X_ADC_H

#include <stdint.h>
#include "adc_legacy.h"

#define ADC_BASE (0x8000D000)

/* Structure-based register definitions */
/* ADC registers are all 16-bit wide with 16-bit access only */
struct ADC_struct {
    union {
        uint16_t COMP[8];
        struct {
            uint16_t COMP_0;
            uint16_t COMP_1;
            uint16_t COMP_2;
            uint16_t COMP_3;
            uint16_t COMP_4;
            uint16_t COMP_5;
            uint16_t COMP_6;
            uint16_t COMP_7;
        };
    };
    uint16_t BAT_COMP_OVER;
    uint16_t BAT_COMP_UNDER;
    union {
        uint16_t SEQ_1;
        struct ADC_SEQ_1 {
            uint16_t CH0: 1;
            uint16_t CH1: 1;
            uint16_t CH2: 1;
            uint16_t CH3: 1;
            uint16_t CH4: 1;
            uint16_t CH5: 1;
            uint16_t CH6: 1;
            uint16_t CH7: 1;
            uint16_t BATT: 1;
            uint16_t : 6;
            uint16_t SEQ_MODE: 1;
        } SEQ_1bits;
    };
    union {
        uint16_t SEQ_2;
        struct ADC_SEQ_2 {
            uint16_t CH0: 1;
            uint16_t CH1: 1;
            uint16_t CH2: 1;
            uint16_t CH3: 1;
            uint16_t CH4: 1;
            uint16_t CH5: 1;
            uint16_t CH6: 1;
            uint16_t CH7: 1;
            uint16_t : 7;
            uint16_t SEQ_MODE: 1;
        } SEQ_2bits;
    };
    union {
        uint16_t CONTROL;
        struct ADC_CONTROL {
            uint16_t ON: 1;
            uint16_t TIMER1_ON: 1;
            uint16_t TIMER2_ON: 1;
            uint16_t SOFT_RESET: 1;
            uint16_t AD1_VREFHL_EN: 1;
            uint16_t AD2_VREFHL_EN: 1;
            uint16_t : 6;
            uint16_t COMPARE_IRQ_MASK: 1;
            uint16_t SEQ1_IRQ_MASK: 1;
            uint16_t SEQ2_IRQ_MASK: 1;
            uint16_t FIFO_IRQ_MASK: 1;
        } CONTROLbits;
    };
    uint16_t TRIGGERS;
    uint16_t PRESCALE;
    uint16_t reserved1;
    uint16_t FIFO_READ;
    uint16_t FIFO_CONTROL;
    union {
        uint16_t FIFO_STATUS;
        struct ADC_FIFO_STATUS {
            uint16_t LEVEL: 4;
            uint16_t FULL: 1;
            uint16_t EMPTY: 1;
            uint16_t : 10;
        } FIFO_STATUSbits;
    };
    uint16_t reserved2[5];
    uint16_t SR_1_HIGH;
    uint16_t SR_1_LOW;
    uint16_t SR_2_HIGH;
    uint16_t SR_2_LOW;
    uint16_t ON_TIME;
    uint16_t CONVERT_TIME;
    uint16_t CLOCK_DIVIDER;
    uint16_t reserved3;
    union {
        uint16_t OVERRIDE;
        struct ADC_OVERRIDE {
            uint16_t MUX1: 4;
            uint16_t MUX2: 4;
            uint16_t AD1_ON: 1;
            uint16_t AD2_ON: 1;
            uint16_t : 6;
        } OVERRIDEbits;
    };
    uint16_t IRQ;
    uint16_t MODE;
    uint16_t RESULT_1;
    uint16_t RESULT_2;
};

static volatile struct ADC_struct *const ADC = (void *)(ADC_BASE);

/* function prototypes */
void adc_setup_channel(uint8_t channel);
void adc_flush(void);
void adc_service(uint16_t *channels_read);

#endif /* MC1322X_ADC_H */
