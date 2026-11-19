/**
 * \file
 *
 * \brief Instance description for PORT
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAMR30_PORT_INSTANCE_
#define _SAMR30_PORT_INSTANCE_

/* ========== Register definition for PORT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PORT_DIR0              (0x40002800U) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (0x40002804U) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (0x40002808U) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (0x4000280CU) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (0x40002810U) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (0x40002814U) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (0x40002818U) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (0x4000281CU) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (0x40002820U) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (0x40002824U) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (0x40002828U) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (0x4000282CU) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (0x40002830U) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (0x40002840U) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (0x40002880U) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (0x40002884U) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (0x40002888U) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (0x4000288CU) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (0x40002890U) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (0x40002894U) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (0x40002898U) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (0x4000289CU) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (0x400028A0U) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (0x400028A4U) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (0x400028A8U) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (0x400028ACU) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (0x400028B0U) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (0x400028C0U) /**< \brief (PORT) Pin Configuration 1 */
#define REG_PORT_DIR2              (0x40002900U) /**< \brief (PORT) Data Direction 2 */
#define REG_PORT_DIRCLR2           (0x40002904U) /**< \brief (PORT) Data Direction Clear 2 */
#define REG_PORT_DIRSET2           (0x40002908U) /**< \brief (PORT) Data Direction Set 2 */
#define REG_PORT_DIRTGL2           (0x4000290CU) /**< \brief (PORT) Data Direction Toggle 2 */
#define REG_PORT_OUT2              (0x40002910U) /**< \brief (PORT) Data Output Value 2 */
#define REG_PORT_OUTCLR2           (0x40002914U) /**< \brief (PORT) Data Output Value Clear 2 */
#define REG_PORT_OUTSET2           (0x40002918U) /**< \brief (PORT) Data Output Value Set 2 */
#define REG_PORT_OUTTGL2           (0x4000291CU) /**< \brief (PORT) Data Output Value Toggle 2 */
#define REG_PORT_IN2               (0x40002920U) /**< \brief (PORT) Data Input Value 2 */
#define REG_PORT_CTRL2             (0x40002924U) /**< \brief (PORT) Control 2 */
#define REG_PORT_WRCONFIG2         (0x40002928U) /**< \brief (PORT) Write Configuration 2 */
#define REG_PORT_EVCTRL2           (0x4000292CU) /**< \brief (PORT) Event Input Control 2 */
#define REG_PORT_PMUX2             (0x40002930U) /**< \brief (PORT) Peripheral Multiplexing 2 */
#define REG_PORT_PINCFG2           (0x40002940U) /**< \brief (PORT) Pin Configuration 2 */
#else
#define REG_PORT_DIR0              (*(RwReg  *)0x40002800U) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (*(RwReg  *)0x40002804U) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (*(RwReg  *)0x40002808U) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (*(RwReg  *)0x4000280CU) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (*(RwReg  *)0x40002810U) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (*(RwReg  *)0x40002814U) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (*(RwReg  *)0x40002818U) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (*(RwReg  *)0x4000281CU) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (*(RoReg  *)0x40002820U) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (*(RwReg  *)0x40002824U) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (*(WoReg  *)0x40002828U) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_EVCTRL0           (*(RwReg  *)0x4000282CU) /**< \brief (PORT) Event Input Control 0 */
#define REG_PORT_PMUX0             (*(RwReg  *)0x40002830U) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (*(RwReg  *)0x40002840U) /**< \brief (PORT) Pin Configuration 0 */
#define REG_PORT_DIR1              (*(RwReg  *)0x40002880U) /**< \brief (PORT) Data Direction 1 */
#define REG_PORT_DIRCLR1           (*(RwReg  *)0x40002884U) /**< \brief (PORT) Data Direction Clear 1 */
#define REG_PORT_DIRSET1           (*(RwReg  *)0x40002888U) /**< \brief (PORT) Data Direction Set 1 */
#define REG_PORT_DIRTGL1           (*(RwReg  *)0x4000288CU) /**< \brief (PORT) Data Direction Toggle 1 */
#define REG_PORT_OUT1              (*(RwReg  *)0x40002890U) /**< \brief (PORT) Data Output Value 1 */
#define REG_PORT_OUTCLR1           (*(RwReg  *)0x40002894U) /**< \brief (PORT) Data Output Value Clear 1 */
#define REG_PORT_OUTSET1           (*(RwReg  *)0x40002898U) /**< \brief (PORT) Data Output Value Set 1 */
#define REG_PORT_OUTTGL1           (*(RwReg  *)0x4000289CU) /**< \brief (PORT) Data Output Value Toggle 1 */
#define REG_PORT_IN1               (*(RoReg  *)0x400028A0U) /**< \brief (PORT) Data Input Value 1 */
#define REG_PORT_CTRL1             (*(RwReg  *)0x400028A4U) /**< \brief (PORT) Control 1 */
#define REG_PORT_WRCONFIG1         (*(WoReg  *)0x400028A8U) /**< \brief (PORT) Write Configuration 1 */
#define REG_PORT_EVCTRL1           (*(RwReg  *)0x400028ACU) /**< \brief (PORT) Event Input Control 1 */
#define REG_PORT_PMUX1             (*(RwReg  *)0x400028B0U) /**< \brief (PORT) Peripheral Multiplexing 1 */
#define REG_PORT_PINCFG1           (*(RwReg  *)0x400028C0U) /**< \brief (PORT) Pin Configuration 1 */
#define REG_PORT_DIR2              (*(RwReg  *)0x40002900U) /**< \brief (PORT) Data Direction 2 */
#define REG_PORT_DIRCLR2           (*(RwReg  *)0x40002904U) /**< \brief (PORT) Data Direction Clear 2 */
#define REG_PORT_DIRSET2           (*(RwReg  *)0x40002908U) /**< \brief (PORT) Data Direction Set 2 */
#define REG_PORT_DIRTGL2           (*(RwReg  *)0x4000290CU) /**< \brief (PORT) Data Direction Toggle 2 */
#define REG_PORT_OUT2              (*(RwReg  *)0x40002910U) /**< \brief (PORT) Data Output Value 2 */
#define REG_PORT_OUTCLR2           (*(RwReg  *)0x40002914U) /**< \brief (PORT) Data Output Value Clear 2 */
#define REG_PORT_OUTSET2           (*(RwReg  *)0x40002918U) /**< \brief (PORT) Data Output Value Set 2 */
#define REG_PORT_OUTTGL2           (*(RwReg  *)0x4000291CU) /**< \brief (PORT) Data Output Value Toggle 2 */
#define REG_PORT_IN2               (*(RoReg  *)0x40002920U) /**< \brief (PORT) Data Input Value 2 */
#define REG_PORT_CTRL2             (*(RwReg  *)0x40002924U) /**< \brief (PORT) Control 2 */
#define REG_PORT_WRCONFIG2         (*(WoReg  *)0x40002928U) /**< \brief (PORT) Write Configuration 2 */
#define REG_PORT_EVCTRL2           (*(RwReg  *)0x4000292CU) /**< \brief (PORT) Event Input Control 2 */
#define REG_PORT_PMUX2             (*(RwReg  *)0x40002930U) /**< \brief (PORT) Peripheral Multiplexing 2 */
#define REG_PORT_PINCFG2           (*(RwReg  *)0x40002940U) /**< \brief (PORT) Pin Configuration 2 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PORT peripheral ========== */
#define PORT_BITS                   84      
#define PORT_DIR_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DIR_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_DRVSTR                 1        // DRVSTR supported?
#define PORT_DRVSTR_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_DRVSTR_IMPLEMENTED     { 0xD8FFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_EVENT_IMPLEMENTED      { 0xCBFFFFFF, 0xC0C3FFFF, 0x00000000 }
#define PORT_EV_NUM                 4       
#define PORT_INEN_DEFAULT_VAL       { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_INEN_IMPLEMENTED       { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_ODRAIN                 0        // ODRAIN supported?
#define PORT_ODRAIN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_ODRAIN_IMPLEMENTED     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_DEFAULT_VAL        { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_OUT_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PIN_IMPLEMENTED        { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PMUXBIT0_DEFAULT_VAL   { 0x00000000, 0x00000000, 0x000D0000 }
#define PORT_PMUXBIT0_IMPLEMENTED   { 0xDBFFFFFF, 0xC0C3FFFF, 0x00000000 }
#define PORT_PMUXBIT1_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT1_IMPLEMENTED   { 0xDBFFFFF3, 0xC0C3FF0F, 0x00000000 }
#define PORT_PMUXBIT2_DEFAULT_VAL   { 0x40000000, 0x00000000, 0x000D0000 }
#define PORT_PMUXBIT2_IMPLEMENTED   { 0xDBFFFFF3, 0xC0C3FF0F, 0x00000000 }
#define PORT_PMUXBIT3_DEFAULT_VAL   { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PMUXBIT3_IMPLEMENTED   { 0xC3CF0FF0, 0x00C3CFC7, 0x00000000 }
#define PORT_PMUXEN_DEFAULT_VAL     { 0x40000000, 0x00000000, 0x00000000 }
#define PORT_PMUXEN_IMPLEMENTED     { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_PULLEN_DEFAULT_VAL     { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_PULLEN_IMPLEMENTED     { 0xDBFFFFFF, 0xC0C3FFFF, 0x000D0000 }
#define PORT_SLEWLIM                0        // SLEWLIM supported?
#define PORT_SLEWLIM_DEFAULT_VAL    { 0x00000000, 0x00000000, 0x00000000 }
#define PORT_SLEWLIM_IMPLEMENTED    { 0x00000000, 0x00000000, 0x00000000 }

#endif /* _SAMR30_PORT_INSTANCE_ */
