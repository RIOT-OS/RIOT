/**
 * \file
 *
 * \brief Instance description for AES
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef _SAML21_AES_INSTANCE_
#define _SAML21_AES_INSTANCE_

/* ========== Register definition for AES peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_AES_CTRLA              (0x42003400U) /**< \brief (AES) Control A */
#define REG_AES_CTRLB              (0x42003404U) /**< \brief (AES) Control B */
#define REG_AES_INTENCLR           (0x42003405U) /**< \brief (AES) Interrupt Enable Clear */
#define REG_AES_INTENSET           (0x42003406U) /**< \brief (AES) Interrupt Enable Set */
#define REG_AES_INTFLAG            (0x42003407U) /**< \brief (AES) Interrupt Flag Status */
#define REG_AES_DATABUFPTR         (0x42003408U) /**< \brief (AES) Data buffer pointer */
#define REG_AES_DBGCTRL            (0x42003409U) /**< \brief (AES) Debug control */
#define REG_AES_KEYWORD0           (0x4200340CU) /**< \brief (AES) Keyword 0 */
#define REG_AES_KEYWORD1           (0x42003410U) /**< \brief (AES) Keyword 1 */
#define REG_AES_KEYWORD2           (0x42003414U) /**< \brief (AES) Keyword 2 */
#define REG_AES_KEYWORD3           (0x42003418U) /**< \brief (AES) Keyword 3 */
#define REG_AES_KEYWORD4           (0x4200341CU) /**< \brief (AES) Keyword 4 */
#define REG_AES_KEYWORD5           (0x42003420U) /**< \brief (AES) Keyword 5 */
#define REG_AES_KEYWORD6           (0x42003424U) /**< \brief (AES) Keyword 6 */
#define REG_AES_KEYWORD7           (0x42003428U) /**< \brief (AES) Keyword 7 */
#define REG_AES_INDATA             (0x42003438U) /**< \brief (AES) Indata */
#define REG_AES_INTVECTV0          (0x4200343CU) /**< \brief (AES) Initialisation Vector 0 */
#define REG_AES_INTVECTV1          (0x42003440U) /**< \brief (AES) Initialisation Vector 1 */
#define REG_AES_INTVECTV2          (0x42003444U) /**< \brief (AES) Initialisation Vector 2 */
#define REG_AES_INTVECTV3          (0x42003448U) /**< \brief (AES) Initialisation Vector 3 */
#define REG_AES_HASHKEY0           (0x4200345CU) /**< \brief (AES) Hash key 0 */
#define REG_AES_HASHKEY1           (0x42003460U) /**< \brief (AES) Hash key 1 */
#define REG_AES_HASHKEY2           (0x42003464U) /**< \brief (AES) Hash key 2 */
#define REG_AES_HASHKEY3           (0x42003468U) /**< \brief (AES) Hash key 3 */
#define REG_AES_GHASH0             (0x4200346CU) /**< \brief (AES) Galois Hash 0 */
#define REG_AES_GHASH1             (0x42003470U) /**< \brief (AES) Galois Hash 1 */
#define REG_AES_GHASH2             (0x42003474U) /**< \brief (AES) Galois Hash 2 */
#define REG_AES_GHASH3             (0x42003478U) /**< \brief (AES) Galois Hash 3 */
#define REG_AES_CIPLEN             (0x42003480U) /**< \brief (AES) Cipher Length */
#define REG_AES_RANDSEED           (0x42003484U) /**< \brief (AES) Random Seed */
#else
#define REG_AES_CTRLA              (*(RwReg  *)0x42003400U) /**< \brief (AES) Control A */
#define REG_AES_CTRLB              (*(RwReg8 *)0x42003404U) /**< \brief (AES) Control B */
#define REG_AES_INTENCLR           (*(RwReg8 *)0x42003405U) /**< \brief (AES) Interrupt Enable Clear */
#define REG_AES_INTENSET           (*(RwReg8 *)0x42003406U) /**< \brief (AES) Interrupt Enable Set */
#define REG_AES_INTFLAG            (*(RwReg8 *)0x42003407U) /**< \brief (AES) Interrupt Flag Status */
#define REG_AES_DATABUFPTR         (*(RwReg8 *)0x42003408U) /**< \brief (AES) Data buffer pointer */
#define REG_AES_DBGCTRL            (*(WoReg8 *)0x42003409U) /**< \brief (AES) Debug control */
#define REG_AES_KEYWORD0           (*(WoReg  *)0x4200340CU) /**< \brief (AES) Keyword 0 */
#define REG_AES_KEYWORD1           (*(WoReg  *)0x42003410U) /**< \brief (AES) Keyword 1 */
#define REG_AES_KEYWORD2           (*(WoReg  *)0x42003414U) /**< \brief (AES) Keyword 2 */
#define REG_AES_KEYWORD3           (*(WoReg  *)0x42003418U) /**< \brief (AES) Keyword 3 */
#define REG_AES_KEYWORD4           (*(WoReg  *)0x4200341CU) /**< \brief (AES) Keyword 4 */
#define REG_AES_KEYWORD5           (*(WoReg  *)0x42003420U) /**< \brief (AES) Keyword 5 */
#define REG_AES_KEYWORD6           (*(WoReg  *)0x42003424U) /**< \brief (AES) Keyword 6 */
#define REG_AES_KEYWORD7           (*(WoReg  *)0x42003428U) /**< \brief (AES) Keyword 7 */
#define REG_AES_INDATA             (*(RwReg  *)0x42003438U) /**< \brief (AES) Indata */
#define REG_AES_INTVECTV0          (*(WoReg  *)0x4200343CU) /**< \brief (AES) Initialisation Vector 0 */
#define REG_AES_INTVECTV1          (*(WoReg  *)0x42003440U) /**< \brief (AES) Initialisation Vector 1 */
#define REG_AES_INTVECTV2          (*(WoReg  *)0x42003444U) /**< \brief (AES) Initialisation Vector 2 */
#define REG_AES_INTVECTV3          (*(WoReg  *)0x42003448U) /**< \brief (AES) Initialisation Vector 3 */
#define REG_AES_HASHKEY0           (*(RwReg  *)0x4200345CU) /**< \brief (AES) Hash key 0 */
#define REG_AES_HASHKEY1           (*(RwReg  *)0x42003460U) /**< \brief (AES) Hash key 1 */
#define REG_AES_HASHKEY2           (*(RwReg  *)0x42003464U) /**< \brief (AES) Hash key 2 */
#define REG_AES_HASHKEY3           (*(RwReg  *)0x42003468U) /**< \brief (AES) Hash key 3 */
#define REG_AES_GHASH0             (*(RwReg  *)0x4200346CU) /**< \brief (AES) Galois Hash 0 */
#define REG_AES_GHASH1             (*(RwReg  *)0x42003470U) /**< \brief (AES) Galois Hash 1 */
#define REG_AES_GHASH2             (*(RwReg  *)0x42003474U) /**< \brief (AES) Galois Hash 2 */
#define REG_AES_GHASH3             (*(RwReg  *)0x42003478U) /**< \brief (AES) Galois Hash 3 */
#define REG_AES_CIPLEN             (*(RwReg  *)0x42003480U) /**< \brief (AES) Cipher Length */
#define REG_AES_RANDSEED           (*(RwReg  *)0x42003484U) /**< \brief (AES) Random Seed */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for AES peripheral ========== */
#define AES_DMAC_ID_RD              45       // DMA DATA Read trigger
#define AES_DMAC_ID_WR              44       // DMA DATA Write trigger

#endif /* _SAML21_AES_INSTANCE_ */
