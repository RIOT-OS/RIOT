/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
*******************************************************************************/

/**
 * @file
 * @ingroup		lpc2387_adc
 * @brief		LPC2387 ADC
 *
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3250 $
 *
 * @note   		$Id: lpc2387-adc.c 3250 2011-03-11 09:45:44Z schmittb $
 */

// cpu
#include "lpc2387.h"
#include "lpc23xx.h"
#include "lpc2387-adc.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#if ENABLE_DEBUG
#include "hwtimer.h"
#endif

/*---------------------------------------------------------------------------*/
void
adc_init(void)
{
    // enable clock /Power for ADC
    PCONP |= BIT12;

    // peripheral Clock Selection for ADC
    PCLKSEL0 |= 0x03000000; 	// pclock = cclock/8

    // set A/D control register AD0CR
    AD0CR = (0x01 << 0) | 	/* SEL=1, select channel 0~7 on ADC0 */
            (0xff << 8) |  	/* CLKDIV = 1, ADC_CLK = PCLK/10 = 0.45 MHz */
            (0 << 16) | 		/* BURST = 0, no BURST, software controlled */
            (0 << 17) |  		/* CLKS = 0, 11 clocks/10 bits */
            (1 << 21) |  		/* PDN = 1, normal operation */
            (0 << 22) |  		/* TEST1:0 = 00 */
            (0 << 24) |  		/* START = 0 A/D conversion stops */
            (0 << 27);		/* EDGE = 0 (CAP/MAT signal falling,trigger A/D conversion) */
}
/*---------------------------------------------------------------------------*/
void
adc_init_1(void)
{
    // enable clock /Power for ADC
    PCONP |= BIT12;

    //PIN0.24 function select AD0.1
    PINSEL1 |= BIT16;

    // peripheral Clock Selection for ADC
    PCLKSEL0 |= 0x03000000; 	// pclock = cclock/8

    // set A/D control register AD0CR
    AD0CR = (0x01 << 0) | 	/* SEL=1, select channel 0~7 on ADC0 */
            (0x00 << 8) |  	/* CLKDIV = 1, ADC_CLK = PCLK/1 = 4.5 MHz */
            (0 << 16) | 		/* BURST = 0, no BURST, software controlled */
            (0 << 17) |  		/* CLKS = 0, 11 clocks/10 bits */
            (1 << 21) |  		/* PDN = 1, normal operation */
            (0 << 22) |  		/* TEST1:0 = 00 */
            (0 << 24) |  		/* START = 0 A/D conversion stops */
            (0 << 27);		/* EDGE = 0 (CAP/MAT signal falling,trigger A/D conversion) */
}
/*---------------------------------------------------------------------------*/
void adc_init_2(void)
{
    // enable clock /Power for ADC
    PCONP |= BIT12;

    // PIN0.23 function select to AD0.0
    PINSEL1 |= BIT14;

    // peripheral Clock Selection for ADC
    PCLKSEL0 |= 0x03000000; 	// pclock = cclock/8

    // set A/D control register AD0CR
    AD0CR = (0x01 << 0) | 	/* SEL=1, select channel 0 on ADC0 */
            (0x00 << 8) |  	/* CLKDIV = 1, ADC_CLK = PCLK/1 = 4.5 MHz */
            (0 << 16) | 		/* BURST = 0, no BURST */
            (0 << 17) |  		/* CLKS = 0, 11 clocks/10 bits */
            (1 << 21) |  		/* PDN = 1, normal operation */
            (0 << 22) |  		/* TEST1:0 = 00 */
            (0 << 24) |  		/* START = 0 A/D conversion stops */
            (0 << 27);		/* EDGE = 0 (CAP/MAT signal falling,trigger A/D conversion) */
}
/*---------------------------------------------------------------------------*/
uint16_t adc_read(uint8_t channel)
{
#if ENABLE_DEBUG
    uint32_t t1, t2;
#endif
    uint32_t regVal, adc_data;

    /* channel number is 0 through 7 */
    if (channel >= ADC_NUM) {
        channel = 0;		/* reset channel number to 0 */
    }

    /* switch channel, start A/D convert */
    AD0CR &= 0xFFFFFF00;
#if ENABLE_DEBUG
    t1 = hwtimer_now();
#endif
    AD0CR |= (1 << 24) | (1 << channel);

#if ENABLE_DEBUG
    t2 = hwtimer_now();
#endif

    while (1) {
        /* read result of A/D conversion */
        regVal = *(volatile unsigned long *)(AD0_BASE_ADDR + ADC_OFFSET + ADC_INDEX * channel);

        /* wait until end of A/D convert */
        if (regVal & ADC_DONE) {
            break;
        }
    }

    AD0CR &= 0xF8FFFFFF;			/* stop ADC now */

    if (regVal & ADC_OVERRUN) {	/* save data when it's not overrun, otherwise, return zero */
        return 0;
    }

    adc_data = (regVal >> 6) & 0x3FF;
    DEBUG("%s, %d: %lu\n", __FILE__, __LINE__, t1);
    DEBUG("%s, %d: %lu\n", __FILE__, __LINE__, t2);
    return (uint16_t) adc_data;				/* return A/D conversion value */
}
