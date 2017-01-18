/**
 * \file
 *
 * \brief Instance description for CCL
 *
 * Copyright (c) 2015 Atmel Corporation. All rights reserved.
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

#ifndef _SAML21_CCL_INSTANCE_
#define _SAML21_CCL_INSTANCE_

/* ========== Register definition for CCL peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_CCL_CTRL               (0x43001C00U) /**< \brief (CCL) Control */
#define REG_CCL_SEQCTRL0           (0x43001C04U) /**< \brief (CCL) SEQ Control x 0 */
#define REG_CCL_SEQCTRL1           (0x43001C05U) /**< \brief (CCL) SEQ Control x 1 */
#define REG_CCL_LUTCTRL0           (0x43001C08U) /**< \brief (CCL) LUT Control x 0 */
#define REG_CCL_LUTCTRL1           (0x43001C0CU) /**< \brief (CCL) LUT Control x 1 */
#define REG_CCL_LUTCTRL2           (0x43001C10U) /**< \brief (CCL) LUT Control x 2 */
#define REG_CCL_LUTCTRL3           (0x43001C14U) /**< \brief (CCL) LUT Control x 3 */
#else
#define REG_CCL_CTRL               (*(RwReg8 *)0x43001C00U) /**< \brief (CCL) Control */
#define REG_CCL_SEQCTRL0           (*(RwReg8 *)0x43001C04U) /**< \brief (CCL) SEQ Control x 0 */
#define REG_CCL_SEQCTRL1           (*(RwReg8 *)0x43001C05U) /**< \brief (CCL) SEQ Control x 1 */
#define REG_CCL_LUTCTRL0           (*(RwReg  *)0x43001C08U) /**< \brief (CCL) LUT Control x 0 */
#define REG_CCL_LUTCTRL1           (*(RwReg  *)0x43001C0CU) /**< \brief (CCL) LUT Control x 1 */
#define REG_CCL_LUTCTRL2           (*(RwReg  *)0x43001C10U) /**< \brief (CCL) LUT Control x 2 */
#define REG_CCL_LUTCTRL3           (*(RwReg  *)0x43001C14U) /**< \brief (CCL) LUT Control x 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for CCL peripheral ========== */
#define CCL_GCLK_ID                 34       // GCLK index for CCL
#define CCL_IO_NUM                  12       // Numer of input pins
#define CCL_LUT_NUM                 4        // Number of LUT in a CCL
#define CCL_SEQ_NUM                 2        // Number of SEQ in a CCL

#endif /* _SAML21_CCL_INSTANCE_ */
