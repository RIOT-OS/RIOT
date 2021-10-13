/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdint.h>
#include "vendor/p32mx470f512h.h"
/*
 *  DEVCFG3  @ 0x1FC02FF0
 *
 *
 *  USERID
 *  FSRSSEL     7   Assign IPL 7 to a shadow register set.
 *  PMDLIWAY    1
 *  IOL1WAY     1
 *  FUSBIDIO    OFF USB USBID Selection Controlled by Port Function
 *  FVBUSONIO   ON  VBUSON pin is controlled by the USB module function

 */
volatile uint32_t _DEVCFG3 __attribute__((used, section(".devcfg3"))) =
    0x0 /* unused bits must be 0 */
    | (_DEVCFG3_USERID_MASK     &  0xFFFF   << _DEVCFG3_USERID_POSITION)
    | (_DEVCFG3_FSRSSEL_MASK    &  7        << _DEVCFG3_FSRSSEL_POSITION)
    | (_DEVCFG3_PMDL1WAY_MASK   &  1        << _DEVCFG3_PMDL1WAY_POSITION)
    | (_DEVCFG3_IOL1WAY_MASK    &  1        << _DEVCFG3_IOL1WAY_POSITION)
    | (_DEVCFG3_FUSBIDIO_MASK   &  0        << _DEVCFG3_FUSBIDIO_POSITION)
    | (_DEVCFG3_FVBUSONIO_MASK  &  1        << _DEVCFG3_FVBUSONIO_POSITION);

/* Note this sets the PLL to 96MHz (8/2 * 24) which is only supported by 3xx
 * and 4xx parts and assumes an 8MHz XTAL.
 *
 * 1xx/2xx/53x/57x only support 50MHz (use 8/2 x 24 / 2 = 48Mhz)
 * 5xx/6xx/7xx only support 80Mhz (use 8/2 * 20 = 80MHz).
 *
 *
 * DEVCFG2  @ 0x1FC02FF4 (
 *
 *  FPLLIDIV    DIV_2     System PLL Input Divider  2x Divider
 *  FPLLMUL     24x       System PLL Multiplier PLL Multiply by 24, 8/2 x 24 = 96MHz
 *  UPLLIDIV    DIV_12x   USB PLL divider
 *  UPLLEN      OFF       USB PLL disabled
 *  FPLLODIV    DIV_1     System PLL Output Clock Divider   1x Divider
 */

volatile uint32_t _DEVCFG2 __attribute__ ((used, section(".devcfg2"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG2_FPLLIDIV_MASK   |  1        << _DEVCFG2_FPLLIDIV_POSITION)
    & (~_DEVCFG2_FPLLMUL_MASK    |  7        << _DEVCFG2_FPLLMUL_POSITION)
    & (~_DEVCFG2_UPLLIDIV_MASK   |  7        << _DEVCFG2_UPLLIDIV_POSITION)
    & (~_DEVCFG2_UPLLEN_MASK     |  0        << _DEVCFG2_UPLLEN_POSITION)
    & (~_DEVCFG2_FPLLODIV_MASK   |  0        << _DEVCFG2_FPLLODIV_POSITION);

/*
 * DEVCFG1  @ 0x1FC02FF8
 *
 * FNOSC     PRIPLL   Oscillator Selection Bits Primary Osc w/PLL (XT+,HS+,EC+PLL)
 * FSOSCEN   ON       Secondary Oscillator Enable Enabled
 * IESO      ON       Internal/External Switch Over Enabled
 * OSCIOFNC  OFF CLKO Output Signal Active on the OSCO Pin Disabled
 * FPBDIV    DIV_1    Peripheral Clock Divisor  Pb_Clk is Sys_Clk/1
 * FCKSM     CSDCMD   Clock Switching and Monitor Selection Clock Switch Disable, FSCM Disabled
 * WDTPS     PS2      Watchdog Timer Postscaler 1:2
 * WINDIS    OFF      Watchdog Timer Window Enable Watchdog Timer is in Non-Window Mode
 * FWDTEN    OFF      Watchdog Timer Enable WDT Disabled (SWDTEN Bit Controls)
 * FWDTWINSZ 25%
 */

volatile uint32_t _DEVCFG1 __attribute__ ((used, section(".devcfg1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG1_FNOSC_MASK     |  3        << _DEVCFG1_FNOSC_POSITION)
    & (~_DEVCFG1_FSOSCEN_MASK   |  1        << _DEVCFG1_FSOSCEN_POSITION)
    & (~_DEVCFG1_IESO_MASK      |  1        << _DEVCFG1_IESO_POSITION)
    & (~_DEVCFG1_POSCMOD_MASK   |  1        << _DEVCFG1_POSCMOD_POSITION)
    & (~_DEVCFG1_OSCIOFNC_MASK  |  1        << _DEVCFG1_OSCIOFNC_POSITION)
    & (~_DEVCFG1_FPBDIV_MASK    |  0        << _DEVCFG1_FPBDIV_POSITION)
    & (~_DEVCFG1_FCKSM_MASK     |  3        << _DEVCFG1_FCKSM_POSITION)
    & (~_DEVCFG1_WDTPS_MASK     |  1        << _DEVCFG1_WDTPS_POSITION)
    & (~_DEVCFG1_WINDIS_MASK    |  0        << _DEVCFG1_WINDIS_POSITION)
    & (~_DEVCFG1_FWDTEN_MASK    |  0        << _DEVCFG1_FWDTEN_POSITION)
    & (~_DEVCFG1_FWDTWINSZ_MASK |  3        << _DEVCFG1_FWDTWINSZ_POSITION);

/*
 * DEVCFG0  @ 0x1FC02FFC
 *
 * DEBUG    OFF      Background Debugger Enable Debugger is disabled
 * JTAGEN   ON       JTAG Enable    JTAG Port Enabled
 * ICESEL   ICS_PGx1 CE/ICD Comm Channel Select Communicate on PGEC1/PGED1
 * PWP      OFF      Program Flash Write Protect Disable
 * BWP      OFF      Boot Flash Write Protect bit Protection Disabled
 * CP       OFF      Code Protect Protection Disabled
 */

volatile uint32_t _DEVCFG0 __attribute__ ((used, section(".devcfg0"))) =
    0x7fffffff /* unused bits must be 1 except MSB which is 0 for some odd reason */
    & (~_DEVCFG0_DEBUG_MASK     |  3        << _DEVCFG0_DEBUG_POSITION)
    & (~_DEVCFG0_JTAGEN_MASK    |  1        << _DEVCFG0_JTAGEN_POSITION)
    & (~_DEVCFG0_ICESEL_MASK    |  3        << _DEVCFG0_ICESEL_POSITION)
    & (~_DEVCFG0_PWP_MASK       |  0xff     << _DEVCFG0_PWP_POSITION)
    & (~_DEVCFG0_BWP_MASK       |  1        << _DEVCFG0_BWP_POSITION)
    & (~_DEVCFG0_CP_MASK        |  1        << _DEVCFG0_CP_POSITION);

/*
 * Without a reference to this function from elsewhere LD throws the whole
 * compile unit away even though the data is 'volatile' and 'used' !!!
 */
void dummy(void)
{
    (void)1;
}
