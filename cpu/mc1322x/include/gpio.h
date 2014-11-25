/*
 * gpio.h - GPIO driver for redbee
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GPIO_H
#define GPIO_H

// TODO: why do we need to include this for macro expansion?
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Structure-based GPIO access
   Example usage:

   GPIO->FUNC_SEL0 |= 0x00008000; // set a whole register

   GPIO->FUNC_SEL_08 = 2;        // set just one pin

   #define MY_PIN GPIO_08
   GPIO->FUNC_SEL.MY_PIN = 2;    // same, to allow #define for pin names
   GPIO->DATA.MY_PIN = 1;

   gpio_set(GPIO_08);            // efficiently set or clear a single output bit
   gpio_reset(GPIO_08);
*/

// GPIO to Function Alias macros:

#define ADC0 GPIO_30
#define ADC1 GPIO_31
#define ADC2 GPIO_32
#define ADC3 GPIO_33
#define ADC4 GPIO_34
#define ADC5 GPIO_35
#define ADC6 GPIO_36
#define ADC7 GPIO_37
#define TDO GPIO_49
#define TDI GPIO_48
#define TCK GPIO_47
#define TMS GPIO_46
#define U2RTS GPIO_21
#define U2CTS GPIO_20
#define U2RX GPIO_19
#define U2TX GPIO_18
#define U1RTS GPIO_17
#define U1CTS GPIO_16
#define U1RX GPIO_15
#define U1TX GPIO_14
#define SDA GPIO_13
#define SCL GPIO_12
#define TMR3 GPIO_11
#define TMR2 GPIO_10
#define TMR1 GPIO_09
#define TMR0 GPIO_08
#define SCK GPIO_07
#define MOSI GPIO_06
#define MISO GPIO_05
#define SS GPIO_04
#define BTCK GPIO_03
#define FSYN GPIO_02
#define SSIRX GPIO_01
#define SSITX GPIO_00
#define KBI7 GPIO_29
#define KBI6 GPIO_28
#define KBI5 GPIO_27
#define KBI4 GPIO_26
#define KBI3 GPIO_25
#define KBI2 GPIO_24
#define KBI1 GPIO_23
#define KBI0 GPIO_22
#define TXON GPIO_44
#define RXON GPIO_45
#define ANT1 GPIO_42
#define ANT2 GPIO_43
#define VREF2H GPIO_38
#define VREF2L GPIO_39
#define VREF1H GPIO_40
#define VREF1L GPIO_41
#define MDO0 GPIO_51
#define MDO1 GPIO_52
#define MDO2 GPIO_53
#define MDO3 GPIO_54
#define MDO4 GPIO_55
#define MDO5 GPIO_56
#define MDO6 GPIO_57
#define MDO7 GPIO_58
#define MSEO0 GPIO_59
#define MSEO1 GPIO_60
#define RDY GPIO_61
#define EVTO GPIO_62
#define MCKO GPIO_50
#define EVTI GPIO_63


#define _V(x,n,i) uint32_t x##_##i : n;
#define _REP(x,n) \
        _V(x,n,00) _V(x,n,01) _V(x,n,02) _V(x,n,03) _V(x,n,04) _V(x,n,05) _V(x,n,06) _V(x,n,07) \
        _V(x,n,08) _V(x,n,09) _V(x,n,10) _V(x,n,11) _V(x,n,12) _V(x,n,13) _V(x,n,14) _V(x,n,15) \
        _V(x,n,16) _V(x,n,17) _V(x,n,18) _V(x,n,19) _V(x,n,20) _V(x,n,21) _V(x,n,22) _V(x,n,23) \
        _V(x,n,24) _V(x,n,25) _V(x,n,26) _V(x,n,27) _V(x,n,28) _V(x,n,29) _V(x,n,30) _V(x,n,31) \
        _V(x,n,32) _V(x,n,33) _V(x,n,34) _V(x,n,35) _V(x,n,36) _V(x,n,37) _V(x,n,38) _V(x,n,39) \
        _V(x,n,40) _V(x,n,41) _V(x,n,42) _V(x,n,43) _V(x,n,44) _V(x,n,45) _V(x,n,46) _V(x,n,47) \
        _V(x,n,48) _V(x,n,49) _V(x,n,50) _V(x,n,51) _V(x,n,52) _V(x,n,53) _V(x,n,54) _V(x,n,55) \
        _V(x,n,56) _V(x,n,57) _V(x,n,58) _V(x,n,59) _V(x,n,60) _V(x,n,61) _V(x,n,62) _V(x,n,63)

struct GPIO_struct {
#define _IO(x) \
        union { struct { uint32_t x##0; uint32_t x##1; }; \
                struct { _REP(x, 1) };  \
                struct GPIO_##x { _REP(GPIO, 1) } x; };
#define _IO_2bit(x)     \
        union { struct { uint32_t x##0; uint32_t x##1; uint32_t x##2; uint32_t x##3; }; \
                struct { _REP(x, 2) };  \
                struct GPIO_##x { _REP(GPIO, 2) } x; };

        _IO(PAD_DIR);
        _IO(DATA);
        _IO(PAD_PU_EN);
        _IO_2bit(FUNC_SEL);
        _IO(DATA_SEL);
        _IO(PAD_PU_SEL);
        _IO(PAD_HYST_EN);
        _IO(PAD_KEEP);
        _IO(DATA_SET);
        _IO(DATA_RESET);
        _IO(PAD_DIR_SET);
        _IO(PAD_DIR_RESET);
};
#undef _IO
#undef _IO_2bit

/* Build an enum lookup to map GPIO_08 -> 8 */
#undef _V
#define _V(x,n,i) __NUM_GPIO_GPIO_##i,
enum { _REP(0,0) };

/* Macros to set or reset a data pin in the fastest possible way */
#define gpio_set(gpio_xx) __gpio_set(gpio_xx)
#define __gpio_set(gpio_xx)                                             \
        ((__NUM_GPIO_##gpio_xx < 32)                                    \
         ? (GPIO->DATA_SET0 = (1 << (__NUM_GPIO_##gpio_xx - 0)))        \
         : (GPIO->DATA_SET1 = (1 << (__NUM_GPIO_##gpio_xx - 32))))
#define gpio_reset(gpio_xx) __gpio_reset(gpio_xx)
#define __gpio_reset(gpio_xx)                                           \
        ((__NUM_GPIO_##gpio_xx < 32)                                    \
         ? (GPIO->DATA_RESET0 = (1 << (__NUM_GPIO_##gpio_xx - 0)))      \
         : (GPIO->DATA_RESET1 = (1 << (__NUM_GPIO_##gpio_xx - 32))))

#undef _REP
#undef _V

static volatile struct GPIO_struct * const GPIO = (void *) (0x80000000);

#ifdef __cplusplus
}
#endif

#endif
