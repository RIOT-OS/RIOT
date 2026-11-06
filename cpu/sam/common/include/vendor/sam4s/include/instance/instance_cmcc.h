/**
 * \file
 *
 * Copyright (c) 2012-2015 Atmel Corporation. All rights reserved.
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

#ifndef _SAM4S_CMCC_INSTANCE_
#define _SAM4S_CMCC_INSTANCE_

/* ========== Register definition for CMCC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
  #define REG_CMCC_TYPE                    (0x4007C000U) /**< \brief (CMCC) Cache Type Register */
  #define REG_CMCC_CFG                     (0x4007C004U) /**< \brief (CMCC) Cache Configuration Register */
  #define REG_CMCC_CTRL                    (0x4007C008U) /**< \brief (CMCC) Cache Control Register */
  #define REG_CMCC_SR                      (0x4007C00CU) /**< \brief (CMCC) Cache Status Register */
  #define REG_CMCC_MAINT0                  (0x4007C020U) /**< \brief (CMCC) Cache Maintenance Register 0 */
  #define REG_CMCC_MAINT1                  (0x4007C024U) /**< \brief (CMCC) Cache Maintenance Register 1 */
  #define REG_CMCC_MCFG                    (0x4007C028U) /**< \brief (CMCC) Cache Monitor Configuration Register */
  #define REG_CMCC_MEN                     (0x4007C02CU) /**< \brief (CMCC) Cache Monitor Enable Register */
  #define REG_CMCC_MCTRL                   (0x4007C030U) /**< \brief (CMCC) Cache Monitor Control Register */
  #define REG_CMCC_MSR                     (0x4007C034U) /**< \brief (CMCC) Cache Monitor Status Register */
#else
  #define REG_CMCC_TYPE   (*(__I  uint32_t*)0x4007C000U) /**< \brief (CMCC) Cache Type Register */
  #define REG_CMCC_CFG    (*(__IO uint32_t*)0x4007C004U) /**< \brief (CMCC) Cache Configuration Register */
  #define REG_CMCC_CTRL   (*(__O  uint32_t*)0x4007C008U) /**< \brief (CMCC) Cache Control Register */
  #define REG_CMCC_SR     (*(__I  uint32_t*)0x4007C00CU) /**< \brief (CMCC) Cache Status Register */
  #define REG_CMCC_MAINT0 (*(__O  uint32_t*)0x4007C020U) /**< \brief (CMCC) Cache Maintenance Register 0 */
  #define REG_CMCC_MAINT1 (*(__O  uint32_t*)0x4007C024U) /**< \brief (CMCC) Cache Maintenance Register 1 */
  #define REG_CMCC_MCFG   (*(__IO uint32_t*)0x4007C028U) /**< \brief (CMCC) Cache Monitor Configuration Register */
  #define REG_CMCC_MEN    (*(__IO uint32_t*)0x4007C02CU) /**< \brief (CMCC) Cache Monitor Enable Register */
  #define REG_CMCC_MCTRL  (*(__O  uint32_t*)0x4007C030U) /**< \brief (CMCC) Cache Monitor Control Register */
  #define REG_CMCC_MSR    (*(__I  uint32_t*)0x4007C034U) /**< \brief (CMCC) Cache Monitor Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM4S_CMCC_INSTANCE_ */
