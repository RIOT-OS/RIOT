/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
/*
 *  DEVCFG3  @ 0x1FC02FF0
 *
 *
 *	USERID
 *	FSRSSEL		7	Assign IPL 7 to a shadow register set.
 *	FMIIEN		OFF	Ethernet RMII/MII Enable	RMII Enabled
 *	FETHIO		ON	Ethernet I/O Pin Select	Default Ethernet I/O
 *	FUSBIDIO	OFF	USB USBID Selection	Controlled by Port Function
 *	FVBUSONIO 	ON	VBUSON pin is controlled by the USB module function

 */
volatile uint32_t DEVCFG3 __attribute__((used, section(".devcfg3"))) = 0x86FFFFFF;

/* Note this sets the PLL to 120MHz which is only supported by 3xx and 4xx parts
 * and assumes an 8MHz XTAL.
 *
 * 1xx/2xx/53x/57x only support 50MHz (use 8 x 24 / 4 = 48Mhz)
 * 5xx/6xx/7xx only support 80Mhz (use 8/2 * 20 = 80MHz).
 *
 *
 * DEVCFG2  @ 0x1FC02FF4 (
 *
 *	FPLLIDIV	DIV_1		System PLL Input Divider	1x Divider
 *	FPLLMUL		15x		System PLL Multiplier	PLL Multiply by 15, 8 x 15 = 120MHz
 *	UPLLIDIV	DIV_256		USB PLL divider
 *	UPLLEN		OFF		USB PLL disabled
 *	FPLLODIV	DIV_1		System PLL Output Clock Divider	1x Divider
 */

volatile uint32_t DEVCFG2 __attribute__ ((used, section(".devcfg2"))) = 0xFFF8F888;


/*
 * DEVCFG1  @ 0x1FC02FF8
 *
 * FNOSC	PRIPLL	Oscillator Selection Bits	Primary Osc w/PLL (XT+,HS+,EC+PLL)
 * FSOSCEN	ON	Secondary Oscillator Enable	Enabled
 * IESO	ON	Internal/External Switch Over	Enabled
 * OSCIOFNC	OFF	CLKO Output Signal Active on the OSCO Pin	Disabled
 * FPBDIV	DIV_1	Peripheral Clock Divisor	Pb_Clk is Sys_Clk/1
 * FCKSM	CSDCMD	Clock Switching and Monitor Selection	Clock Switch Disable, FSCM Disabled
 * WDTPS	PS2	Watchdog Timer Postscaler	1:2
 * WINDIS	OFF	Watchdog Timer Window Enable	Watchdog Timer is in Non-Window Mode
 * FWDTEN	OFF	Watchdog Timer Enable	WDT Disabled (SWDTEN Bit Controls)
 */

volatile uint32_t DEVCFG1 __attribute__ ((used, section(".devcfg1"))) = 0XFF61CDFB;


/*
 * DEVCFG0  @ 0x1FC02FFC
 *
 * DEBUG	OFF		Background Debugger Enable	Debugger is disabled
 * JTAGEN	ON		JTAG Enable	JTAG Port Enabled
 * ICESEL	ICS_PGx1	ICE/ICD Comm Channel Select	Communicate on PGEC1/PGED1
 * PWP	OFF	Program Flash Write Protect	Disable
 * BWP	OFF	Boot Flash Write Protect bit	Protection Disabled
 * CP	OFF	Code Protect	Protection Disabled
 */

volatile uint32_t DEVCFG0 __attribute__ ((used, section(".devcfg0"))) = 0x7FFFFFFF;

/*
 * Without a reference to this function from elsewhere LD throws the whole compile unit
 * away even though the data is 'volatile' and 'used' !!!
 */
void dummy(void)
{
	(void)1;
}

