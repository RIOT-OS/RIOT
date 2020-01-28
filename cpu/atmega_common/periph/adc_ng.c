/*
 * Copyright (C) 2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_periph
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation for ATmega family
 *
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <assert.h>
#include <avr/sleep.h>
#include <errno.h>
#include <stdint.h>

#include "adc_ng.h"
#include "bitarithm.h"
#include "cpu.h"
#include "irq.h"
#include "kernel_defines.h"
#include "periph_conf.h"
#include "xfa.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* ADC clock device divides core clock down to:
 *
 *    CLOCK_CORECLOCK / (1 << ADC_CLOCK_DIV)
 *
 * The ADC always generates 10 bits output, but with ADC clocks higher than
 * 200 kHz the noise level rises reducing the number of meaningful bits.
 * With a clock between 50 kHz and 200 kHz and low noise mode, the highest
 * effective resolution is achieved.
 *
 * In this article the effective resolution of the ADC depending on the ADC
 * clock is measured: http://www.openmusiclabs.com/learning/digital/atmega-adc/
 *
 * This driver implements a 10 bit resolution with an ADC clock of (just below)
 * 200 kHz and a 9 bit resolution with an ADC clock of (just below) 1MHz.
 * Note: The article also tried ADC clocks as high as 4 MHz, but according to
 * the data sheet the ADC clock must not exceed 1 MHz. So let's be better safe
 * than sorry.
 */
#if CLOCK_CORECLOCK > 12800000U
#define ADC_10BIT_CLOCK_DIV         (7U)
#elif CLOCK_CORELOCK > 6400000U
#define ADC_10BIT_CLOCK_DIV         (6U)
#elif CLOCK_CORECLOCK > 3200000U
#define ADC_10BIT_CLOCK_DIV         (5U)
#elif CLOCK_CORECLOCK > 1600000U
#define ADC_10BIT_CLOCK_DIV         (4U)
#elif CLOCK_CORECLOCK > 800000U
#define ADC_10BIT_CLOCK_DIV         (3U)
#elif CLOCK_CORECLOCK > 400000U
#define ADC_10BIT_CLOCK_DIV         (2U)
#else
#define ADC_10BIT_CLOCK_DIV         (1U)
#endif

#if CLOCK_CORECLOCK > 16000000U
#define ADC_9BIT_CLOCK_DIV          (5U)
#elif CLOCK_CORELOCK > 8000000U
#define ADC_9BIT_CLOCK_DIV          (4U)
#elif CLOCK_CORECLOCK > 4000000U
#define ADC_9BIT_CLOCK_DIV          (3U)
#elif CLOCK_CORECLOCK > 2000000U
#define ADC_9BIT_CLOCK_DIV          (2U)
#else
#define ADC_9BIT_CLOCK_DIV          (1U)
#endif

#if defined(CPU_ATMEGA2560)
/* The ATmega2560 has 16 single-ended ADC channels */
#define CHAN_NUMOF          (16U)
#else
/* All other supported ATmegas have 8 single-ended ADC channels, except for
 * the ATmega32U4. But the ADC channels of the ATmega32U4 are strangely
 * mapped to pins, so we don't support channels 8-13 to keep the code readable
 */
#define CHAN_NUMOF          (8U)
#endif

#if defined(CPU_ATMEGA328P)
/* ATmega328P has 4 ADC multiplexer bits, all fit in the ADMUX register */
#define MUX_WIDTH           (4U)
#define ADMUX_MUX_MASK      (0x0f)
#elif defined(CPU_ATMEGA1284P)
/* ATmega1284P has 5 ADC multiplexer bits, all fit in the ADMUX register */
#define MUX_WIDTH           (5U)
#define ADMUX_MUX_MASK      (0x1f)
#else
/* ATmega32U4, 128RFA1, 256RFR2, 1281, and 2560 have 6 ADC multiplexer bits.
 * The most significant bit (MUX5) is stored in ADCSRB rather than in ADMUX.
 */
#define MUX_WIDTH           (6U)
#define ADMUX_MUX_MASK      (0x1f)
#endif

/* On ATmega128RFA1 and ATmega256RFR2 AVCC the internal 1.8V regulator is used
 * as reference
 */
#if defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2)
#define AVCC                (1800U)
#elif !defined(AVCC)
/* AVCC can be specified in periph_conf.h - otherwise it will just be
 * detected on boot. */
#define AVCC                (0U)
#endif /* AVCC */

#if defined(CPU_ATMEGA32U4)
#define NTC_CHANNEL         (39U)
#elif defined(CPU_ATMEGA328P)
#define NTC_CHANNEL         (8U)
#endif

/* This value for the ADMUX selects the 1.1V internal reference as input for
 * every support ATmega, after dropping additional bits. (E.g. the ATmega328P
 * only supports 4 bits ADMUX and the value 0x0e (== 0x1e & 0x0f) selects the
 * 1.1V reference.)
 */
#define REF_1V1_CHANNEL     (0x1e)

#define FLAG_RSHIFT         (0x01)
#define FLAG_NOISE_CANCEL   (0x02)
#define FLAG_ENTROPY        (0x04)

static int _init(void *handle, uint8_t chan, uint8_t res, uint8_t ref);
static void _off(void *handle);
static int _single(void *handle, int32_t *dest);

static uint8_t flags = 0;

/* Supported voltage references... */
static int16_t refs[] = {
#if defined(CPU_ATMEGA32U4)
        2560,
        AVCC,
#elif defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2)
        1500,
        1600,
        AVCC,
#elif defined(CPU_ATMEGA328P)
        1100,
        AVCC,
#elif defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA2560)
        1100,
        2560,
        AVCC,
#else
#error "Missing reference voltage configuration"
#endif
    0 /* <-- terminating zero element */
};
/* ...and corresponding configuration bits */
static const uint8_t ref_confs[] = {
#if defined(CPU_ATMEGA32U4)
    /* ATmega32U4: See Table 24-3 in the data sheet */
    (1 << REFS1) | (1 << REFS0),
    (1 << REFS0),
#elif defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2)
    /* ATmega128RFA1: See Table 27-10 in the data sheet */
    /* ATmega256RFR2: See Table 27-11 in the data sheet */
    (1 << REFS1),
    (1 << REFS1) | (1 << REFS0),
    (1 << REFS0),
#elif defined(CPU_ATMEGA328P)
    /* ATmega328P: See Table 24-3 in the data sheet */
    (1 << REFS1) | (1 << REFS0),
    (1 << REFS0),
#elif defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA1284P) || defined(CPU_ATMEGA2560)
    /* ATmega1281: See Table 26-3 in the data sheet */
    /* ATmega1284P: See Table 23-3 in the data sheet */
    /* ATmega2560: See Table 26-3 in the data sheet */
    (1 << REFS1),
    (1 << REFS1) | (1 << REFS0),
    (1 << REFS0),
#endif
};

#ifdef NTC_CHANNEL
static adc_ng_ntc_t int_ntc = {
    .coefficient = 10533,
    .offset = 361,
};
#endif

adc_ng_driver_t periph_adc_ng_driver = {
    .init = _init,
    .off = _off,
    .single = _single,
    .res_supported = BIT8 | BIT9,
    .refs = refs,
    /* For all ATmegas, the lowest reference voltage (idx 0) can be select
     * as input */
    .ref_input_idx = 0,
    /* And the last is always VCC */
    .ref_vcc_idx = ARRAY_SIZE(ref_confs) - 1,
    .highest_single_ended_channel = CHAN_NUMOF - 1,
#ifdef NTC_CHANNEL
    .entropy_bits = 1,
    .ntc = &int_ntc
#endif
};

XFA_CONST(adc_ng_backends, 0) adc_ng_backend_t periph_adc_ng_backend = {
    .driver = &periph_adc_ng_driver,
    .handle = NULL,
};
XFA(adc_ng_states, 0) adc_ng_state_t periph_adc_ng_state;

ISR(ADC_vect)
{
    /* nothing to do: After ISR finishes, execution continues after the point
     * ADC noise cancel mode was started
     */
}

static void setup_admux(uint8_t chan, uint8_t ref)
{
    DEBUG("ref = %x, ref_conf = %x\n", ref, ref_confs[ref]);
    if (chan == ADC_NG_CHAN_FIXED_REF) {
        /* Magic channel that selects the smallest fixed voltage reference as
         * input. (Useful to deduce AVCC, when AVCC is reference.)
         * (For the ATmega328P the correct value would be 0x0e, but the
         * leading bits are dropped later on anyway.)
         */
        chan = REF_1V1_CHANNEL;
    }
#ifdef NTC_CHANNEL
    else if (chan == ADC_NG_CHAN_ENTROPY) {
        chan = NTC_CHANNEL;
        /* Always use the smallest reference voltage, as small noise on the
         * input voltage will have bigger impact on the sample there */
        ref = 0;
    }
    else if (chan == ADC_NG_CHAN_NTC) {
        chan = NTC_CHANNEL;
    }
#endif /* NTC_CHANNEL */
    else if (chan < 8) {
        /* Disable corresponding Digital input */
        DIDR0 |= (1 << chan);
        /* Set ADC-pin as input */
#if defined(CPU_ATMEGA328P)
        DDRC &= ~(1 << chan);
        PORTC &= ~(1 << chan);
#elif defined(CPU_ATMEGA1284P)
        DDRA &= ~(1 << chan);
        PORTA &= ~(1 << chan);
#elif defined(CPU_ATMEGA32U4) || defined(CPU_ATMEGA1281) || defined(CPU_ATMEGA2560)
        DDRF  &= ~(1 << chan);
        PORTF &= ~(1 << chan);
#endif /* CPU_ATMEGA328P */
    }
#if defined(CPU_ATMEGA2560)
    else if (chan < 16) {
        /* Disable corresponding Digital input */
        DIDR2 |= (1 << (chan - 8));
        /* Set ADC-pin as input */
        DDRK  &= ~(1 << (chan - 8));
        PORTK &= ~(1 << (chan - 8));
        /* single ended channels ADC8 - ADC15 have are mapped from 0x20 */
        chan &= 0x07;
        chan |= 0x20;
    }
#endif

    /* Set reference configuration and lower 5 bits of the channel mux */
    ADMUX = ref_confs[ref] | (chan & ADMUX_MUX_MASK);

#if MUX_WIDTH == 6U
    /* Set upper bits of the channel mux, if present */
    if (chan & 0x20) {
        ADCSRB |= (1 << MUX5);
    }
    else {
        ADCSRB &= ~(1 << MUX5);
    }
#endif
}

static int _init(void *handle, uint8_t chan, uint8_t res, uint8_t ref)
{
    (void)handle;
    assert(ref < ARRAY_SIZE(ref_confs));

    if (
        (chan >= CHAN_NUMOF) && (chan != ADC_NG_CHAN_FIXED_REF)
#ifdef NTC_CHANNEL
        && (chan != ADC_NG_CHAN_ENTROPY) && (chan != ADC_NG_CHAN_NTC)
#endif
    ) {
        return -ENXIO;
    }

#ifdef CPU_ATMEGA32U4
    /* The ATmega32U4 is missing pins for PF2 and PF3, which would have been
     * ADC channel 2 and 3 if present */
    if ((chan == 2) || (chan == 3)) {
        return -ENXIO;
    }
#endif

    unsigned state = irq_disable();
    if (ADCSRA & (1 << ADEN)) {
        irq_restore(state);
        return -EALREADY;
    }

    /* enable ADC and set ADC clock corresponding to the effective resolution
     * requested */
#ifdef NTC_CHANNEL
    if (chan == ADC_NG_CHAN_ENTROPY) {
        /* Ignore requested resolution according to doc and freely choose
         * setting providing highest entropy per conversion: Go with fastest
         * ADC clock (most noisy), do not use noise cancel mode, do not
         * discard noisy last bit.
         */
        ADCSRA = (1 << ADEN) | ADC_10BIT_CLOCK_DIV;
        flags = FLAG_ENTROPY | FLAG_NOISE_CANCEL;
    }
    else
#endif /* NTC_CHANNEL */
    if (res == 10) {
        ADCSRA = (1 << ADEN) | ADC_10BIT_CLOCK_DIV;
        /* Reduce noise by turning CPU during ADC conversion off */
        flags = FLAG_NOISE_CANCEL;
    }
    else {
        ADCSRA =  (1 << ADEN) | ADC_9BIT_CLOCK_DIV;
        /* Result has to be shifted to the right by one to discard the noisy
         * last bit. Perform ADC conversion with CPU active for better
         * conversion speed
         */
        flags = FLAG_RSHIFT | FLAG_NOISE_CANCEL;
    }

    irq_restore(state);

    setup_admux(chan, ref);

    if (ref < ARRAY_SIZE(ref_confs) - 1) {
        /* All but the last voltage reference are constant references.
         * According to the data sheet, the first measurement with a constant
         * reference is inaccurate as the reference needs some time to
         * stabilize. As workaround we'll just take one sample to through
         * away.
         */
        int32_t trash;
        /* through away first measurement to let reference voltage settle */
        _single(NULL, &trash);
    }
    else if (chan == ADC_NG_CHAN_FIXED_REF) {
        /* There seems to be no warning in the data sheet, but using the
         * constant reference as input has shown to be widely unreliable
         * until it finally stabilizes after a couple of measurements. No idea
         * why it takes much longer to stabilize when used as input rather than
         * as reference...
         */
        int32_t trash;
        for (uint8_t i = 0; i < 8; i++) {
            _single(NULL, &trash);
        }
    }

    return 0;
}

static void _off(void *handle)
{
    (void)handle;
    ADCSRA &= ~(1 << ADEN);
}

static int _single(void *handle, int32_t *dest)
{
    (void)handle;
    assert(dest != NULL);
    assert(ADCSRA & (1 << ADEN));

    DEBUG("ADMUX = 0x%x, ADCSRA = 0x%x, ADCSRB = 0x%x\n", ADMUX, ADCSRA, ADCSRB);

    if (flags & FLAG_NOISE_CANCEL) {
        /* Wait for TX to complete */
        while (avr8_is_uart_tx_pending()) {}
        /* Turn on ADC interrupt to wake CPU up after conversion is done */
        ADCSRA |= (1 << ADIF) | (1 << ADIE);
        set_sleep_mode(SLEEP_MODE_ADC);
        sleep_mode();
    }
    else {
        /* Perform single conversion with CPU active */
        ADCSRA |= (1 << ADSC);

        /* Wait until the conversion is complete */
        while (ADCSRA & (1 << ADSC)) {}
    }

    /* Get conversion result */
    *dest = (ADC) >> (flags & FLAG_RSHIFT);

    /* Clear the ADIF flag */
    ADCSRA |= (1 << ADIF);

    return 0;
}

#if !AVCC
void periph_adc_ng_init(void)
{
    /* Detect AVCC. Rely on peripheral ADC getting priority over external ones,
     * so that we are ADC 0 */
    const uint8_t idx = ARRAY_SIZE(ref_confs) - 1;
    adc_ng_measure_ref(0, idx, &refs[idx]);
}
#endif
