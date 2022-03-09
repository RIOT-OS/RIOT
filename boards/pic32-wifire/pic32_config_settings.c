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
#include "vendor/p32mz2048efg100.h"

/*
 * Note banked access only applies to MZ part MX only has 1 set of registers
 * similar to the MZ's lower alias.Thus when working with MX parts comment
 * out the *_B* entries, note the address in the comments are different for MX
 * too so a different linker script is required between MX and MZ to place
 * these registers at the correct addresses. MM parts have completely different
 * config registers, so this file is not applicable.
 *
 * Note when programming via Microchip IPE (tested using a Pickit-3) entries
 * need to exist in the programming file for both the lower alias and the
 * config1 configuration spaces (starting at 0x1FC0FFC0 and 0x1FC4FFC0)
 * hence the duplicate entries in different sections allowing the linker to
 * place them at different addresses.
 */

/*
 *  DEVCFG3_LA  @ 0x1FC0FFC0 (lower alias)
 *  ADEVFGC3_LA @ 0x1FC0FF40 (alternate devcfg3 in lower alias)
 *  DEVCFG3_B1  @ 0x1FC4FFC0 (config space 1)
 *  ADEVCFG3_B1 @ 0x1FC4FF40 (alternate devcfg3 in config space 1)
 *  DEVCFG3_B2  @ 0x1FC6FFC0 (config space 1)
 *  ADEVCFG3_B2 @ 0x1FC6FF40 (alternate devcfg3 in config space 2)
 *
 *
 *  USERID
 *  FMIIEN      OFF Ethernet RMII/MII Enable RMII Enabled
 *  FETHIO      ON  Ethernet I/O Pin Select Default Ethernet I/O
 *  PGL1WAY     OFF Permission Group Lock One Way Configuration Allow multiple reconfigurations
 *  PMDL1WAY    OFF Peripheral Module Disable Configuration Allow multiple reconfigurations
 *  IOL1WAY     OFF Peripheral Pin Select Configuration Allow multiple reconfigurations
 *  FUSBIDIO    OFF USB USBID Selection Controlled by Port Function
 */
volatile uint32_t DEVCFG3_LA  __attribute__((used, section(".devcfg3_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG3_USERID_MASK     |  0xFFFF   << _DEVCFG3_USERID_POSITION)
    & (~_DEVCFG3_FMIIEN_MASK     |  0        << _DEVCFG3_FMIIEN_POSITION)
    & (~_DEVCFG3_FETHIO_MASK     |  1        << _DEVCFG3_FETHIO_POSITION)
    & (~_DEVCFG3_PGL1WAY_MASK    |  0        << _DEVCFG3_PGL1WAY_POSITION)
    & (~_DEVCFG3_PMDL1WAY_MASK   |  0        << _DEVCFG3_PMDL1WAY_POSITION)
    & (~_DEVCFG3_IOL1WAY_MASK    |  0        << _DEVCFG3_IOL1WAY_POSITION)
    & (~_DEVCFG3_FUSBIDIO_MASK   |  0        << _DEVCFG3_FUSBIDIO_POSITION);

volatile uint32_t ADEVCFG3_LA __attribute__((used, section(".adevcfg3_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG3_USERID_MASK     |  0xFFFF   << _DEVCFG3_USERID_POSITION)
    & (~_DEVCFG3_FMIIEN_MASK     |  0        << _DEVCFG3_FMIIEN_POSITION)
    & (~_DEVCFG3_FETHIO_MASK     |  1        << _DEVCFG3_FETHIO_POSITION)
    & (~_DEVCFG3_PGL1WAY_MASK    |  0        << _DEVCFG3_PGL1WAY_POSITION)
    & (~_DEVCFG3_PMDL1WAY_MASK   |  0        << _DEVCFG3_PMDL1WAY_POSITION)
    & (~_DEVCFG3_IOL1WAY_MASK    |  0        << _DEVCFG3_IOL1WAY_POSITION)
    & (~_DEVCFG3_FUSBIDIO_MASK   |  0        << _DEVCFG3_FUSBIDIO_POSITION);

volatile uint32_t DEVCFG3_B1  __attribute__((used, section(".devcfg3_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG3_USERID_MASK     |  0xFFFF   << _DEVCFG3_USERID_POSITION)
    & (~_DEVCFG3_FMIIEN_MASK     |  0        << _DEVCFG3_FMIIEN_POSITION)
    & (~_DEVCFG3_FETHIO_MASK     |  1        << _DEVCFG3_FETHIO_POSITION)
    & (~_DEVCFG3_PGL1WAY_MASK    |  0        << _DEVCFG3_PGL1WAY_POSITION)
    & (~_DEVCFG3_PMDL1WAY_MASK   |  0        << _DEVCFG3_PMDL1WAY_POSITION)
    & (~_DEVCFG3_IOL1WAY_MASK    |  0        << _DEVCFG3_IOL1WAY_POSITION)
    & (~_DEVCFG3_FUSBIDIO_MASK   |  0        << _DEVCFG3_FUSBIDIO_POSITION);

volatile uint32_t ADEVCFG3_B1 __attribute__((used, section(".adevcfg3_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG3_USERID_MASK     |  0xFFFF   << _DEVCFG3_USERID_POSITION)
    & (~_DEVCFG3_FMIIEN_MASK     |  0        << _DEVCFG3_FMIIEN_POSITION)
    & (~_DEVCFG3_FETHIO_MASK     |  1        << _DEVCFG3_FETHIO_POSITION)
    & (~_DEVCFG3_PGL1WAY_MASK    |  0        << _DEVCFG3_PGL1WAY_POSITION)
    & (~_DEVCFG3_PMDL1WAY_MASK   |  0        << _DEVCFG3_PMDL1WAY_POSITION)
    & (~_DEVCFG3_IOL1WAY_MASK    |  0        << _DEVCFG3_IOL1WAY_POSITION)
    & (~_DEVCFG3_FUSBIDIO_MASK   |  0        << _DEVCFG3_FUSBIDIO_POSITION);
/*
 * Not needed by default:
 * volatile uint32_t DEVCFG3_B2 __attribute__((used,section(".devcfg3_b2")))
 * = DEVCFG3_LA;
 * volatile uint32_t ADEVCFG3_B2 __attribute__((used,section(".adevcfg3_la")))
 * = DEVCFG3_LA;
 *
 */

/*
 * DEVCFG2_LA  @ 0x1FC0FFC4 (lower alias)
 * ADEVFGC2_LA @ 0x1FC0FF44 (alternate devcfg2 in lower alias)
 * DEVCFG2_B1  @ 0x1FC4FFC4 (config space 1)
 * ADEVCFG2_B1 @ 0x1FC4FF44 (alternate devcfg2 in config space 1)
 * DEVCFG2_B2  @ 0x1FC6FFC4 (config space 1)
 * ADEVCFG2_B2 @ 0x1FC6FF44 (alternate devcfg2 in config space 2)
 *
 *  24MHz OSC / 3 * 50 / 2 = 200MHz
 *
 *  FPLLIDIV    DIV_3           System PLL Input Divider 3x Divider
 *  FPLLRNG     RANGE_5_10_MHZ  System PLL Input Range   5-10 MHz Input
 *  FPLLICLK    PLL_POSC        System PLL Input Clock Selection POSC is input to the System PLL
 *  FPLLMULT    MUL_50          System PLL Multiplier    PLL Multiply by 50
 *  FPLLODIV    DIV_2           System PLL Output Clock Divider    2x Divider
 *  UPLLFSEL    FREQ_24MHZ      USB PLL Input Frequency Selection  USB PLL input is 24 MHz
 */

volatile uint32_t DEVCFG2_LA  __attribute__ ((used, section(".devcfg2_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG2_FPLLIDIV_MASK     |  2        << _DEVCFG2_FPLLIDIV_POSITION)
    & (~_DEVCFG2_FPLLRNG_MASK      |  0x1      << _DEVCFG2_FPLLRNG_POSITION)
    & (~_DEVCFG2_FPLLICLK_MASK     |  0x0      << _DEVCFG2_FPLLICLK_POSITION)
    & (~_DEVCFG2_FPLLMULT_MASK     |  49       << _DEVCFG2_FPLLMULT_POSITION)
    & (~_DEVCFG2_FPLLODIV_MASK     |  1        << _DEVCFG2_FPLLODIV_POSITION)
    & (~_DEVCFG2_UPLLFSEL_MASK     |  0x1      << _DEVCFG2_UPLLFSEL_POSITION);

volatile uint32_t ADEVCFG2_LA __attribute__ ((used, section(".adevcfg2_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG2_FPLLIDIV_MASK     |  2        << _DEVCFG2_FPLLIDIV_POSITION)
    & (~_DEVCFG2_FPLLRNG_MASK      |  0x1      << _DEVCFG2_FPLLRNG_POSITION)
    & (~_DEVCFG2_FPLLICLK_MASK     |  0x0      << _DEVCFG2_FPLLICLK_POSITION)
    & (~_DEVCFG2_FPLLMULT_MASK     |  49       << _DEVCFG2_FPLLMULT_POSITION)
    & (~_DEVCFG2_FPLLODIV_MASK     |  1        << _DEVCFG2_FPLLODIV_POSITION)
    & (~_DEVCFG2_UPLLFSEL_MASK     |  0x1      << _DEVCFG2_UPLLFSEL_POSITION);

volatile uint32_t DEVCFG2_B1  __attribute__ ((used, section(".devcfg2_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG2_FPLLIDIV_MASK     |  2        << _DEVCFG2_FPLLIDIV_POSITION)
    & (~_DEVCFG2_FPLLRNG_MASK      |  0x1      << _DEVCFG2_FPLLRNG_POSITION)
    & (~_DEVCFG2_FPLLICLK_MASK     |  0x0      << _DEVCFG2_FPLLICLK_POSITION)
    & (~_DEVCFG2_FPLLMULT_MASK     |  49       << _DEVCFG2_FPLLMULT_POSITION)
    & (~_DEVCFG2_FPLLODIV_MASK     |  1        << _DEVCFG2_FPLLODIV_POSITION)
    & (~_DEVCFG2_UPLLFSEL_MASK     |  0x1      << _DEVCFG2_UPLLFSEL_POSITION);

volatile uint32_t ADEVCFG2_B1 __attribute__ ((used, section(".adevcfg2_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG2_FPLLIDIV_MASK     |  2        << _DEVCFG2_FPLLIDIV_POSITION)
    & (~_DEVCFG2_FPLLRNG_MASK      |  0x1      << _DEVCFG2_FPLLRNG_POSITION)
    & (~_DEVCFG2_FPLLICLK_MASK     |  0x0      << _DEVCFG2_FPLLICLK_POSITION)
    & (~_DEVCFG2_FPLLMULT_MASK     |  49       << _DEVCFG2_FPLLMULT_POSITION)
    & (~_DEVCFG2_FPLLODIV_MASK     |  1        << _DEVCFG2_FPLLODIV_POSITION)
    & (~_DEVCFG2_UPLLFSEL_MASK     |  0x1      << _DEVCFG2_UPLLFSEL_POSITION);
/* Not needed by default: */
/* uint32_t DEVCFG2_B2  __attribute__ ((section(".devcfg2_b2")))  = DEVCFG2_LA; */
/* uint32_t ADEVCFG2_B2 __attribute__ ((section(".adevcfg2_b2"))) = DEVCFG2_LA; */

/*
 * DEVCFG1_LA  @ 0x1FC0FFC8 (lower alias)
 * ADEVFGC1_LA @ 0x1FC0FF48 (alternate devcfg1 in lower alias)
 * DEVCFG1_B1  @ 0x1FC4FFC8 (config space 1)
 * ADEVCFG1_B1 @ 0x1FC4FF48 (alternate devcfg1 in config space 1)
 * DEVCFG1_B2  @ 0x1FC6FFC8 (config space 1)
 * ADEVCFG1_B2 @ 0x1FC6FF48 (alternate devcfg1 in config space 2)
 *
 *  FNOSC       SPLL        Oscillator Selection Bits   System PLL
 *  DMTINTV     WIN_127_128 DMT Count Window Interval   Window/Interval value is 127/128 counter value
 *  FSOSCEN     OFF         Secondary Oscillator Enable Disable SOSC
 *  IESO        ON          Internal/External Switch Over   Enabled
 *  POSCMOD     EC          Primary Oscillator Configuration    External clock mode
 *  OSCIOFNC    OFF         CLKO Output Signal Active on the OSCO Pin   Disabled (1)
 *  FCKSM       CSDCMD      Clock Switching and Monitor Selection   Clock Switch Disabled, FSCM Disabled
 *  WDTPS       PS1048576   Watchdog Timer Postscaler   1:1048576
 *  WDTSPGM     STOP        Watchdog Timer Stop During Flash Programming    WDT stops during Flash programming
 *  WINDIS      NORMAL      Watchdog Timer Window Mode  Watchdog Timer is in non-Window mode
 *  FWDTEN      OFF         Watchdog Timer Enable   WDT Disabled
 *  FWDTWINSZ   WINSZ_25    Watchdog Timer Window Size  Window size is 25%
 *  DMTCNT      DMT8        Deadman Timer Count Selection   2^8 (256)
 *  FDMTEN      OFF         Deadman Timer Enable    Deadman Timer is disabled
 */

volatile uint32_t DEVCFG1_LA  __attribute__ ((used, section(".devcfg1_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG1_FNOSC_MASK       |  0x1      << _DEVCFG1_FNOSC_POSITION)
    & (~_DEVCFG1_DMTINTV_MASK     |  0x7      << _DEVCFG1_DMTINTV_POSITION)
    & (~_DEVCFG1_FSOSCEN_MASK     |  0        << _DEVCFG1_FSOSCEN_POSITION)
    & (~_DEVCFG1_IESO_MASK        |  1        << _DEVCFG1_IESO_POSITION)
    & (~_DEVCFG1_POSCMOD_MASK     |  0x0      << _DEVCFG1_POSCMOD_POSITION)
    & (~_DEVCFG1_OSCIOFNC_MASK    |  1        << _DEVCFG1_OSCIOFNC_POSITION)
    & (~_DEVCFG1_FCKSM_MASK       |  0x0      << _DEVCFG1_FCKSM_POSITION)
    & (~_DEVCFG1_WDTPS_MASK       |  0x14     << _DEVCFG1_WDTPS_POSITION)
    & (~_DEVCFG1_WDTSPGM_MASK     |  1        << _DEVCFG1_WDTSPGM_POSITION)
    & (~_DEVCFG1_WINDIS_MASK      |  1        << _DEVCFG1_WINDIS_POSITION)
    & (~_DEVCFG1_FWDTEN_MASK      |  0        << _DEVCFG1_FWDTEN_POSITION)
    & (~_DEVCFG1_FWDTWINSZ_MASK   |  0x3      << _DEVCFG1_FWDTWINSZ_POSITION)
    & (~_DEVCFG1_DMTCNT_MASK      |  0x0      << _DEVCFG1_DMTCNT_POSITION)
    & (~_DEVCFG1_FDMTEN_MASK      |  0        << _DEVCFG1_FDMTEN_POSITION);

volatile uint32_t ADEVCFG1_LA __attribute__ ((used, section(".adevcfg1_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG1_FNOSC_MASK       |  0x1      << _DEVCFG1_FNOSC_POSITION)
    & (~_DEVCFG1_DMTINTV_MASK     |  0x7      << _DEVCFG1_DMTINTV_POSITION)
    & (~_DEVCFG1_FSOSCEN_MASK     |  0        << _DEVCFG1_FSOSCEN_POSITION)
    & (~_DEVCFG1_IESO_MASK        |  1        << _DEVCFG1_IESO_POSITION)
    & (~_DEVCFG1_POSCMOD_MASK     |  0x0      << _DEVCFG1_POSCMOD_POSITION)
    & (~_DEVCFG1_OSCIOFNC_MASK    |  1        << _DEVCFG1_OSCIOFNC_POSITION)
    & (~_DEVCFG1_FCKSM_MASK       |  0x0      << _DEVCFG1_FCKSM_POSITION)
    & (~_DEVCFG1_WDTPS_MASK       |  0x14     << _DEVCFG1_WDTPS_POSITION)
    & (~_DEVCFG1_WDTSPGM_MASK     |  1        << _DEVCFG1_WDTSPGM_POSITION)
    & (~_DEVCFG1_WINDIS_MASK      |  1        << _DEVCFG1_WINDIS_POSITION)
    & (~_DEVCFG1_FWDTEN_MASK      |  0        << _DEVCFG1_FWDTEN_POSITION)
    & (~_DEVCFG1_FWDTWINSZ_MASK   |  0x3      << _DEVCFG1_FWDTWINSZ_POSITION)
    & (~_DEVCFG1_DMTCNT_MASK      |  0x0      << _DEVCFG1_DMTCNT_POSITION)
    & (~_DEVCFG1_FDMTEN_MASK      |  0        << _DEVCFG1_FDMTEN_POSITION);

volatile uint32_t DEVCFG1_B1  __attribute__ ((used, section(".devcfg1_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG1_FNOSC_MASK       |  0x1      << _DEVCFG1_FNOSC_POSITION)
    & (~_DEVCFG1_DMTINTV_MASK     |  0x7      << _DEVCFG1_DMTINTV_POSITION)
    & (~_DEVCFG1_FSOSCEN_MASK     |  0        << _DEVCFG1_FSOSCEN_POSITION)
    & (~_DEVCFG1_IESO_MASK        |  1        << _DEVCFG1_IESO_POSITION)
    & (~_DEVCFG1_POSCMOD_MASK     |  0x0      << _DEVCFG1_POSCMOD_POSITION)
    & (~_DEVCFG1_OSCIOFNC_MASK    |  1        << _DEVCFG1_OSCIOFNC_POSITION)
    & (~_DEVCFG1_FCKSM_MASK       |  0x0      << _DEVCFG1_FCKSM_POSITION)
    & (~_DEVCFG1_WDTPS_MASK       |  0x14     << _DEVCFG1_WDTPS_POSITION)
    & (~_DEVCFG1_WDTSPGM_MASK     |  1        << _DEVCFG1_WDTSPGM_POSITION)
    & (~_DEVCFG1_WINDIS_MASK      |  1        << _DEVCFG1_WINDIS_POSITION)
    & (~_DEVCFG1_FWDTEN_MASK      |  0        << _DEVCFG1_FWDTEN_POSITION)
    & (~_DEVCFG1_FWDTWINSZ_MASK   |  0x3      << _DEVCFG1_FWDTWINSZ_POSITION)
    & (~_DEVCFG1_DMTCNT_MASK      |  0x0      << _DEVCFG1_DMTCNT_POSITION)
    & (~_DEVCFG1_FDMTEN_MASK      |  0        << _DEVCFG1_FDMTEN_POSITION);

volatile uint32_t ADEVCFG1_B1 __attribute__ ((used, section(".adevcfg1_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG1_FNOSC_MASK       |  0x1      << _DEVCFG1_FNOSC_POSITION)
    & (~_DEVCFG1_DMTINTV_MASK     |  0x7      << _DEVCFG1_DMTINTV_POSITION)
    & (~_DEVCFG1_FSOSCEN_MASK     |  0        << _DEVCFG1_FSOSCEN_POSITION)
    & (~_DEVCFG1_IESO_MASK        |  1        << _DEVCFG1_IESO_POSITION)
    & (~_DEVCFG1_POSCMOD_MASK     |  0x0      << _DEVCFG1_POSCMOD_POSITION)
    & (~_DEVCFG1_OSCIOFNC_MASK    |  1        << _DEVCFG1_OSCIOFNC_POSITION)
    & (~_DEVCFG1_FCKSM_MASK       |  0x0      << _DEVCFG1_FCKSM_POSITION)
    & (~_DEVCFG1_WDTPS_MASK       |  0x14     << _DEVCFG1_WDTPS_POSITION)
    & (~_DEVCFG1_WDTSPGM_MASK     |  1        << _DEVCFG1_WDTSPGM_POSITION)
    & (~_DEVCFG1_WINDIS_MASK      |  1        << _DEVCFG1_WINDIS_POSITION)
    & (~_DEVCFG1_FWDTEN_MASK      |  0        << _DEVCFG1_FWDTEN_POSITION)
    & (~_DEVCFG1_FWDTWINSZ_MASK   |  0x3      << _DEVCFG1_FWDTWINSZ_POSITION)
    & (~_DEVCFG1_DMTCNT_MASK      |  0x0      << _DEVCFG1_DMTCNT_POSITION)
    & (~_DEVCFG1_FDMTEN_MASK      |  0        << _DEVCFG1_FDMTEN_POSITION);

/* Not needed by default: */
/* uint32_t DEVCFG1_B2 __attribute__ ((section(".devcfg1_b2"))) = DEVCFG1_LA; */
/* uint32_t ADEVCFG1_B2 __attribute__ ((section(".adevcfg1_b2"))) = DEVCFG1_LA */

/*
 * DEVCFG0_LA  @ 0x1FC0FFCC (lower alias)
 * ADEVFGC0_LA @ 0x1FC0FF4C (alternate devcfg0 in lower alias)
 * DEVCFG0_B1  @ 0x1FC4FFCC (config space 1)
 * ADEVCFG0_B1 @ 0x1FC4FF4C (alternate devcfg0 in config space 1)
 * DEVCFG0_B2  @ 0x1FC6FFCC (config space 1)
 * ADEVCFG0_B2 @ 0x1FC6FF4C (alternate devcfg0 in config space 2)
 *
 * DEBUG      OFF          Background Debugger Enable  Debugger is disabled
 * JTAGEN     ON           JTAG Enable JTAG Port Enabled
 * ICESEL     ICS_PGx2     ICE/ICD Comm Channel Select Communicate on PGEC2/PGED2
 * TRCEN      ON           Trace Enable    Trace features in the CPU are disabled
 * BOOTISA    MIPS32       Boot ISA Selection  Boot code and Exception code is MIPS32
 * FECCCON    OFF_UNLOCKED Dynamic Flash ECC Configuration ECC and Dynamic ECC are disabled (ECCCON bits are writable)
 * FSLEEP     OFF          Flash Sleep Mode    Flash is powered down when the device is in Sleep mode
 * DBGPER     PG_ALL       Debug Mode CPU Access Permission    Allow CPU access to all permission regions
 * SMCLR      MCLR_NORM    Soft Master Clear Enable bit    MCLR pin generates a normal system Reset
 * SOSCGAIN   GAIN_2X      Secondary Oscillator Gain Control bits  2x gain setting
 * SOSCBOOST  ON           Secondary Oscillator Boost Kick Start Enable bit    Boost the kick start of the oscillator
 * POSCGAIN   GAIN_2X      Primary Oscillator Gain Control bits    2x gain setting
 * POSCBOOST  ON           Primary Oscillator Boost Kick Start Enable bit  Boost the kick start of the oscillator
 * EJTAGBEN   NORMAL       EJTAG Boot  Normal EJTAG functionality
 */

volatile uint32_t DEVCFG0_LA   __attribute__ ((used, section(".devcfg0_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG0_DEBUG_MASK       |  0x3      << _DEVCFG0_DEBUG_POSITION)
    & (~_DEVCFG0_JTAGEN_MASK      |  0x1      << _DEVCFG0_JTAGEN_POSITION)
    & (~_DEVCFG0_ICESEL_MASK      |  0x2      << _DEVCFG0_ICESEL_POSITION)
    & (~_DEVCFG0_TRCEN_MASK       |  0x1      << _DEVCFG0_TRCEN_POSITION)
    & (~_DEVCFG0_BOOTISA_MASK     |  0x1      << _DEVCFG0_BOOTISA_POSITION)
    & (~_DEVCFG0_FECCCON_MASK     |  0x3      << _DEVCFG0_FECCCON_POSITION)
    & (~_DEVCFG0_FSLEEP_MASK      |  0x1      << _DEVCFG0_FSLEEP_POSITION)
    & (~_DEVCFG0_DBGPER_MASK      |  0x7      << _DEVCFG0_DBGPER_POSITION)
    & (~_DEVCFG0_SMCLR_MASK       |  0x1      << _DEVCFG0_SMCLR_POSITION)
    & (~_DEVCFG0_SOSCGAIN_MASK    |  0x2      << _DEVCFG0_SOSCGAIN_POSITION)
    & (~_DEVCFG0_SOSCBOOST_MASK   |  0x1      << _DEVCFG0_SOSCBOOST_POSITION)
    & (~_DEVCFG0_POSCGAIN_MASK    |  0x2      << _DEVCFG0_POSCGAIN_POSITION)
    & (~_DEVCFG0_POSCBOOST_MASK   |  0x1      << _DEVCFG0_POSCBOOST_POSITION)
    & (~_DEVCFG0_EJTAGBEN_MASK    |  0x1      << _DEVCFG0_EJTAGBEN_POSITION);

volatile uint32_t ADEVCFG0_LA  __attribute__ ((used, section(".adevcfg0_la"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG0_DEBUG_MASK       |  0x3      << _DEVCFG0_DEBUG_POSITION)
    & (~_DEVCFG0_JTAGEN_MASK      |  0x1      << _DEVCFG0_JTAGEN_POSITION)
    & (~_DEVCFG0_ICESEL_MASK      |  0x2      << _DEVCFG0_ICESEL_POSITION)
    & (~_DEVCFG0_TRCEN_MASK       |  0x1      << _DEVCFG0_TRCEN_POSITION)
    & (~_DEVCFG0_BOOTISA_MASK     |  0x1      << _DEVCFG0_BOOTISA_POSITION)
    & (~_DEVCFG0_FECCCON_MASK     |  0x3      << _DEVCFG0_FECCCON_POSITION)
    & (~_DEVCFG0_FSLEEP_MASK      |  0x1      << _DEVCFG0_FSLEEP_POSITION)
    & (~_DEVCFG0_DBGPER_MASK      |  0x7      << _DEVCFG0_DBGPER_POSITION)
    & (~_DEVCFG0_SMCLR_MASK       |  0x1      << _DEVCFG0_SMCLR_POSITION)
    & (~_DEVCFG0_SOSCGAIN_MASK    |  0x2      << _DEVCFG0_SOSCGAIN_POSITION)
    & (~_DEVCFG0_SOSCBOOST_MASK   |  0x1      << _DEVCFG0_SOSCBOOST_POSITION)
    & (~_DEVCFG0_POSCGAIN_MASK    |  0x2      << _DEVCFG0_POSCGAIN_POSITION)
    & (~_DEVCFG0_POSCBOOST_MASK   |  0x1      << _DEVCFG0_POSCBOOST_POSITION)
    & (~_DEVCFG0_EJTAGBEN_MASK    |  0x1      << _DEVCFG0_EJTAGBEN_POSITION);

volatile uint32_t DEVCFG0_B1   __attribute__ ((used, section(".devcfg0_b1"))) =
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG0_DEBUG_MASK       |  0x3      << _DEVCFG0_DEBUG_POSITION)
    & (~_DEVCFG0_JTAGEN_MASK      |  0x1      << _DEVCFG0_JTAGEN_POSITION)
    & (~_DEVCFG0_ICESEL_MASK      |  0x2      << _DEVCFG0_ICESEL_POSITION)
    & (~_DEVCFG0_TRCEN_MASK       |  0x1      << _DEVCFG0_TRCEN_POSITION)
    & (~_DEVCFG0_BOOTISA_MASK     |  0x1      << _DEVCFG0_BOOTISA_POSITION)
    & (~_DEVCFG0_FECCCON_MASK     |  0x3      << _DEVCFG0_FECCCON_POSITION)
    & (~_DEVCFG0_FSLEEP_MASK      |  0x1      << _DEVCFG0_FSLEEP_POSITION)
    & (~_DEVCFG0_DBGPER_MASK      |  0x7      << _DEVCFG0_DBGPER_POSITION)
    & (~_DEVCFG0_SMCLR_MASK       |  0x1      << _DEVCFG0_SMCLR_POSITION)
    & (~_DEVCFG0_SOSCGAIN_MASK    |  0x2      << _DEVCFG0_SOSCGAIN_POSITION)
    & (~_DEVCFG0_SOSCBOOST_MASK   |  0x1      << _DEVCFG0_SOSCBOOST_POSITION)
    & (~_DEVCFG0_POSCGAIN_MASK    |  0x2      << _DEVCFG0_POSCGAIN_POSITION)
    & (~_DEVCFG0_POSCBOOST_MASK   |  0x1      << _DEVCFG0_POSCBOOST_POSITION)
    & (~_DEVCFG0_EJTAGBEN_MASK    |  0x1      << _DEVCFG0_EJTAGBEN_POSITION);

volatile uint32_t ADEVCFG0_B1  __attribute__ ((used, section(".adevcfg0_b1")))=
    0xffffffff /* unused bits must be 1 */
    & (~_DEVCFG0_DEBUG_MASK       |  0x3      << _DEVCFG0_DEBUG_POSITION)
    & (~_DEVCFG0_JTAGEN_MASK      |  0x1      << _DEVCFG0_JTAGEN_POSITION)
    & (~_DEVCFG0_ICESEL_MASK      |  0x2      << _DEVCFG0_ICESEL_POSITION)
    & (~_DEVCFG0_TRCEN_MASK       |  0x1      << _DEVCFG0_TRCEN_POSITION)
    & (~_DEVCFG0_BOOTISA_MASK     |  0x1      << _DEVCFG0_BOOTISA_POSITION)
    & (~_DEVCFG0_FECCCON_MASK     |  0x3      << _DEVCFG0_FECCCON_POSITION)
    & (~_DEVCFG0_FSLEEP_MASK      |  0x1      << _DEVCFG0_FSLEEP_POSITION)
    & (~_DEVCFG0_DBGPER_MASK      |  0x7      << _DEVCFG0_DBGPER_POSITION)
    & (~_DEVCFG0_SMCLR_MASK       |  0x1      << _DEVCFG0_SMCLR_POSITION)
    & (~_DEVCFG0_SOSCGAIN_MASK    |  0x2      << _DEVCFG0_SOSCGAIN_POSITION)
    & (~_DEVCFG0_SOSCBOOST_MASK   |  0x1      << _DEVCFG0_SOSCBOOST_POSITION)
    & (~_DEVCFG0_POSCGAIN_MASK    |  0x2      << _DEVCFG0_POSCGAIN_POSITION)
    & (~_DEVCFG0_POSCBOOST_MASK   |  0x1      << _DEVCFG0_POSCBOOST_POSITION)
    & (~_DEVCFG0_EJTAGBEN_MASK    |  0x1      << _DEVCFG0_EJTAGBEN_POSITION);

/*
 * uint32_t DEVCFG0_B2  __attribute__ ((section(".devcfg0_b2")))
 * = 0xFFFFF7D7;
 * uint32_t ADEVCFG0_B2  __attribute__ ((section(".adevcfg0_b2")))
 * = 0xFFFFF7D7;
 *
 */

/*
 * DEVCP0_LA  @ 0x1FC0FFDC (lower alias)
 * ADEVCP0_LA @ 0x1FC0FF5C (alternate devcp0 in lower alias)
 * DEVCP0_B1  @ 0x1FC4FFDC (config space 1)
 * ADEVCP0_B1 @ 0x1FC4FF5C (alternate devcp0 in config space 1)
 * DEVCP0_B2  @ 0x1FC6FFDC (config space 1)
 * ADEVCP0_B2 @ 0x1FC6FF5C (alternate devcp0 in config space 2
 *
 * CP   OFF Code Protect    Protection Disabled, unused bits must be 1.
 */

volatile uint32_t DEVCP0_LA  __attribute__ ((used, section(".devcp0_la")))  =
    0xFFFFFFFF | _DEVCP0_CP_MASK;
volatile uint32_t ADEVCP0_LA __attribute__ ((used, section(".adevcp0_la"))) =
    0xFFFFFFFF | _DEVCP0_CP_MASK;
volatile uint32_t DEVCP0_B1  __attribute__ ((used, section(".devcp0_b1")))  =
    0xFFFFFFFF | _DEVCP0_CP_MASK;
volatile uint32_t ADEVCP0_B1 __attribute__ ((used, section(".adevcp0_b1"))) =
    0xFFFFFFFF | _DEVCP0_CP_MASK;
/* not needed by default */
/* uint32_t DEVCP0_B2   __attribute__ ((section(".devcp0_b1"))) = 0xFFFFFFFF; */
/* uint32_t ADEVCP0_B2  __attribute__ ((section(".adevcp0_b1"))) = 0xFFFFFFFF; */

/*
 * SEQ_B1[0..3] @ 1FC0FFF0
 * SEQ_B1[0..3] @ 1FC4FFF0
 *
 * TSEQ     Boot Flash True Sequence Number
 * CSEQ     Boot Flash Complement Sequence Number
 */

volatile uint32_t SEQ_LA[4] __attribute__ ((used, section(".seq_la"))) =
    { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
volatile uint32_t SEQ_B1[4] __attribute__ ((used, section(".seq_b1"))) =
    { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
/*
 * Not needed by default:
 * uint32_t SEQ_B2[4] __attribute__ ((section(".seq_b2"))) =
 *   {0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF};
 */

/*
 * STUPIDLY Microchip has hard coded the MSB bit of devsign to 0, So even if
 * you erase the whole device, everything returns 0xFFFFFFF except this 1
 * register (and its alternate) which return 0x7FFFFFF!!
 *
 * We set it in the output image so verification doesn't fail
 *
 * DEVSIGN0 @ 0xBFC0FFEC
 * ADEVSIGN0 @ 0xBFC0FF6C
 *
 */

volatile uint32_t DEVSIGN_LA  __attribute__ ((used, section(".devsign_la")))  = 0x7FFFFFFF;
volatile uint32_t ADEVSIGN_LA __attribute__ ((used, section(".adevsign_la"))) = 0x7FFFFFFF;
volatile uint32_t DEVSIGN_B1  __attribute__ ((used, section(".devsign_b1")))  = 0x7FFFFFFF;
volatile uint32_t ADEVSIGN_B1 __attribute__ ((used, section(".adevsign_b1"))) = 0x7FFFFFFF;
volatile uint32_t DEVSIGN_B2  __attribute__ ((used, section(".devsign_b2")))  = 0x7FFFFFFF;
volatile uint32_t ADEVSIGN_B2 __attribute__ ((used, section(".adevsign_b2"))) = 0x7FFFFFFF;

/*
 * Without a reference to this function from elsewhere LD throws the whole
 * compile unit away even though the data is 'volatile' and 'used' !!!
 */
void dummy(void)
{
    (void)1;
}
