/*
 * board.c - Board initialization for the Zolertia Z1
 * Copyright (C) 2014 INRIA
 *
 * Author : Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_z1
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Zolertia Z1
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

static void z1_ports_init(void)
{
    /* Port 1:
     *  P1.0 is not assigned by default
     *  P1.1 is the bootstrap-loader (BSL) TX pin -> input, special function, default to GND
     *       THIS PIN MUST *NEVER* BE USED IN NORMAL EXECUTION, SINCE IT INTERFERES WITH UART0 !!!
     *  P1.2 receives the FIFOP interrupt from CC2420 -> input, GPIO, default to GND
     *  P1.3 receives the FIFO/GIO0 interrupt from CC2420 -> input, GPIO, default to GND
     *  P1.4 receives the CCA/GIO1 signal from CC2420 -> input, GPIO, default to GND
     *  P1.5 is wired to Vcc -> input, GPIO, default to Vcc
     *  P1.6 receives interrupt INT1 from accelerometer -> input, GPIO, default to GND
     *  P1.7 receives interrupt INT2 from accelerometer -> input, GPIO, default to GND
     */
    P1SEL = 0x02;    /* Port1 Select: 00000010 = 0x02 */
    P1OUT = 0x20;    /* Port1 Output: 00100000 = 0x20 */
    P1DIR = 0x00;    /* Port1 Direction: 00000000 = 0x00 */

    /* Port 2:
     *  P2.0 is not assigned by default
     *  P2.1 is not assigned by default
     *  P2.2 is the bootstrap-loader (BSL) RX pin -> input, special function, default to GND
     *       THIS PIN MUST *NEVER* BE USED IN NORMAL EXECUTION, SINCE IT INTERFERES WITH UART0 !!!
     *  P2.3 is not assigned by default
     *  P2.4 is used as an active-low output to the BSL and USB interface -> output, GPIO, default to Vcc
     *  P2.5 is connected to the active-low "user interrupt" button -> input, GPIO, default to Vcc
     *  P2.6 is not assigned by default
     *  P2.7 receives the ALERT interrupt from TMP102 -> input, GPIO, default to Vcc
     */
    P2SEL = 0x04;    /* Port2 Select: 00000100 = 0x04 */
    P2OUT = 0xB0;    /* Port2 Output: 10110000 = 0xB0 */
    P2DIR = 0xD0;    /* Port2 Direction: 11010000 = 0xD0 */

    /* Port 3:
     *  P3.0 is the radio (CC2420) active-low chip select -> output, GPIO, default to Vcc
     *  P3.1 is SPI's MOSI pin -> output, special function, default to GND
     *  P3.2 is SPI's MISO pin -> input, special function, default to GND
     *  P3.3 is SPI's CLK pin -> output, special function, default to GND
     *  P3.4 is USCI0 UART TX pin -> output, special function, default to Vcc
     *  P3.5 is USCI0 UART RX pin -> input, special function, default to Vcc
     *  P3.6 is USCI1 UART TX pin -> output, special function, default to GND
     *  P3.7 is USCI1 UART RX pin -> input, special function, default to GND
     * NOTES :
     *  - Z1 only uses the USCI0 SPI channel
     *  - UART0 is connected to the micro-USB port (via the CP2102 chip)
     */
    P3SEL = 0xFE;    /* Port3 Select: 11111110 = 0xFE */
    P3OUT = 0x31;    /* Port3 Output: 00110001 = 0x31 */
    P3DIR = 0x5B;    /* Port3 Direction: 01011011 = 0x5B */

    /* Port 4:
     *  P4.0 is not assigned by default (but controls a LED in Z1 "starter pack") -> output, GPIO, default to Vcc
     *  P4.1 receives the SFD interrupt from CC2420, that can be captured by TimerB -> input, special function, default to GND
     *  P4.2 is not assigned by default (but controls a LED in Z1 "starter pack") -> output, GPIO, default to Vcc
     *  P4.3 is not assigned by default (but controls a buzzer in Z1 "starter pack") -> output, GPIO, default to GND
     *  P4.4 is the Flash chip (M25P16) active-low chip select -> output, GPIO, default to Vcc
     *  P4.5 is the radio (CC2420) active-high VREG enable line -> output, GPIO, default to GND
     *  P4.6 is the radio (CC2420) active-low RESET line -> output, GPIO, default to Vcc
     *  P4.7 is not assigned by default (but controls a LED in Z1 "starter pack") -> output, GPIO, default to Vcc
     */
    P4SEL = 0x02;    /* Port4 Select: 00000010 = 0x02 */
    P4OUT = 0xD5;    /* Port4 Output: 11010101 = 0xD5 */
    P4DIR = 0xFD;    /* Port4 Direction: 11111101 = 0xFD */

    /* Port 5:
     *  I2C, and GPIO (LEDs, flash)
     *  P5.0 controls TMP102 power (active high) -> output, GPIO, default to GND
     *  P5.1 is I2C's SDA (data) pin -> input (by default/changeable), special function, default to GND
     *  P5.2 is I2C's SCL (clock) pin -> output, special function, default to GND
     *  P5.3 is not assigned by default
     *  P5.4 controls one of Z1's LEDs (active low) -> output, GPIO, default to Vcc
     *  P5.5 controls one of Z1's LEDs (active low) -> output, GPIO, default to Vcc
     *  P5.6 controls one of Z1's LEDs (active low) -> output, GPIO, default to Vcc
     *  P5.7 is the Flash chip (M25P16) active-low HOLD line -> output, GPIO, default to Vcc
     * NOTES :
     *  - Z1 only uses the USCI1 I2C channel
     *  - P5.3 controls the +5V aux. power regulator in Z1 "starter pack"
     */
    P5SEL = 0x06;    /* Port5 Select: 00000110 = 0x06 */
    P5OUT = 0xF0;    /* Port5 Output: 11110000 = 0xF0 */
    P5DIR = 0xF5;    /* Port5 Direction: 11110101 = 0xF5 */

    /* Port 6:
     *  All of the 8 lines are ADC inputs, none of them is assigned by default
     *   (but P6.4/AD4 is connected to a wheel potentiometer in Z1 "starter pack")
     *   -> put all lines to input, special function, default to GND
     */
    P6SEL = 0xFF;    /* Port6 Select: 11111111 = 0xFF */
    P6OUT = 0x00;    /* Port6 Output: 00000000 = 0x00 */
    P6DIR = 0x00;    /* Port6 Direction: 00000000 = 0xFF */

}

/*---------------------------------------------------------------------------*/
/* taken from Contiki code */
void msp430_init_dco(void)
{
#ifdef  CALIBRATE_MSP430_DCO
#define DELTA    (F_CPU / (F_RC_OSCILLATOR / 8))
    /* This code taken from the FU Berlin sources and reformatted. */
    unsigned int oldcapture = 0;
    unsigned int i;

    /* 10100100 = XT2 is off, ACLK divided by 4, RSELx=4 */
    BCSCTL1 = XT2OFF | DIVA_3 | RSEL2;

    /* Init undivided DCO with internal resistor for MCLK and SMCLK
     * DCO = 32762Hz -> FLL = 2,4576 MHz */
    BCSCTL2 = 0x00;

    BCSCTL1 |= DIVA1 + DIVA0;             /* ACLK = LFXT1CLK/8 */

    for (i = 0xFFFF; i > 0; i--) {        /* Delay for XTAL to settle */
        __nop();
    }

    CCTL2 = CCIS0 + CM0 + CAP;            /* Define CCR2, CAP, ACLK */
    TACTL = TASSEL1 + TACLR + MC1;        /* SMCLK, continuous mode */

    while (1) {
        unsigned int compare;

        while ((CCTL2 & CCIFG) != CCIFG);   /* Wait until capture occurred!*/

        CCTL2 &= ~CCIFG;                    /* Capture occurred, clear flag */
        compare = CCR2;                     /* Get current captured SMCLK */
        compare = compare - oldcapture;     /* SMCLK difference */
        oldcapture = CCR2;                  /* Save current captured SMCLK */

        if (DELTA == compare) {
            break;                            /* if equal, leave "while (1)" */
        }
        else if (DELTA < compare) {        /* DCO is too fast, slow it down */
            DCOCTL--;

            if (DCOCTL == 0xFF) {             /* Did DCO role under? */
                BCSCTL1--;
            }
        }
        else {                            /* -> Select next lower RSEL */
            DCOCTL++;

            if (DCOCTL == 0x00) {             /* Did DCO role over? */
                BCSCTL1++;
            }           /* -> Select next higher RSEL  */
        }
    }

    CCTL2 = 0;                            /* Stop CCR2 function */
    TACTL = 0;                            /* Stop Timer_A */

    BCSCTL1 &= ~(DIVA1 + DIVA0);          /* remove divisor from ACLK again */

    /*
     * On a MSP430F2617 (as on a Z1), for a 8 MHz target frequency,
     * we normally obtain these values:
     *   DCOCTL == 0x9a (i.e.: DCOx == 4 && MODx == 26)
     *                  [the MODx field is the most prone to variation]
     *   BCSCTL1 == 0x0d (i.e.: RSELx == 13)
     */
#else
    /* default values for quick start-up */
    DCOCTL = 0x00;   /* avoid possible temporary overclocking... */
    BCSCTL1 = 0x8d;  /* as seen in Contiki code */
    DCOCTL = 0x88;   /* as seen in Contiki code */
#endif

    /* Other clock configuration */
    BCSCTL1 |= XT2OFF;    /* XT2 not connected on Z1 */
    BCSCTL2 = 0;          /* get MCLK and SMCLK from DCO, without divisor */
    BCSCTL3 = XCAP_1;     /* default value for LFXT1 capacitor and frequency */
}

/* "public" specific initialization function for the Zolertia Z1 hardware */

void board_init(void)
{
    /* init CPU core */
    msp430_cpu_init();

    /* disable watchdog timer */
    WDTCTL     =  WDTPW + WDTHOLD;

    /* init MCU pins as adequate for Z1 hardware */
    z1_ports_init();

    /* initializes DCO */
    msp430_init_dco();

    /* enable interrupts */
    __bis_SR_register(GIE);
}
