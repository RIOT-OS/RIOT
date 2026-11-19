/**
 * \file
 *
 * \brief Instance description for SERCOM5
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \asf_license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the Licence at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \asf_license_stop
 *
 */

#ifndef _SAML21_SERCOM5_INSTANCE_
#define _SAML21_SERCOM5_INSTANCE_

/* ========== Register definition for SERCOM5 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_SERCOM5_I2CM_CTRLA     (0x43000400) /**< \brief (SERCOM5) I2CM Control A */
#define REG_SERCOM5_I2CM_CTRLB     (0x43000404) /**< \brief (SERCOM5) I2CM Control B */
#define REG_SERCOM5_I2CM_BAUD      (0x4300040C) /**< \brief (SERCOM5) I2CM Baud Rate */
#define REG_SERCOM5_I2CM_INTENCLR  (0x43000414) /**< \brief (SERCOM5) I2CM Interrupt Enable Clear */
#define REG_SERCOM5_I2CM_INTENSET  (0x43000416) /**< \brief (SERCOM5) I2CM Interrupt Enable Set */
#define REG_SERCOM5_I2CM_INTFLAG   (0x43000418) /**< \brief (SERCOM5) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CM_STATUS    (0x4300041A) /**< \brief (SERCOM5) I2CM Status */
#define REG_SERCOM5_I2CM_SYNCBUSY  (0x4300041C) /**< \brief (SERCOM5) I2CM Synchronization Busy */
#define REG_SERCOM5_I2CM_ADDR      (0x43000424) /**< \brief (SERCOM5) I2CM Address */
#define REG_SERCOM5_I2CM_DATA      (0x43000428) /**< \brief (SERCOM5) I2CM Data */
#define REG_SERCOM5_I2CM_DBGCTRL   (0x43000430) /**< \brief (SERCOM5) I2CM Debug Control */
#define REG_SERCOM5_I2CS_CTRLA     (0x43000400) /**< \brief (SERCOM5) I2CS Control A */
#define REG_SERCOM5_I2CS_CTRLB     (0x43000404) /**< \brief (SERCOM5) I2CS Control B */
#define REG_SERCOM5_I2CS_INTENCLR  (0x43000414) /**< \brief (SERCOM5) I2CS Interrupt Enable Clear */
#define REG_SERCOM5_I2CS_INTENSET  (0x43000416) /**< \brief (SERCOM5) I2CS Interrupt Enable Set */
#define REG_SERCOM5_I2CS_INTFLAG   (0x43000418) /**< \brief (SERCOM5) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CS_STATUS    (0x4300041A) /**< \brief (SERCOM5) I2CS Status */
#define REG_SERCOM5_I2CS_SYNCBUSY  (0x4300041C) /**< \brief (SERCOM5) I2CS Synchronization Busy */
#define REG_SERCOM5_I2CS_ADDR      (0x43000424) /**< \brief (SERCOM5) I2CS Address */
#define REG_SERCOM5_I2CS_DATA      (0x43000428) /**< \brief (SERCOM5) I2CS Data */
#define REG_SERCOM5_SPI_CTRLA      (0x43000400) /**< \brief (SERCOM5) SPI Control A */
#define REG_SERCOM5_SPI_CTRLB      (0x43000404) /**< \brief (SERCOM5) SPI Control B */
#define REG_SERCOM5_SPI_BAUD       (0x4300040C) /**< \brief (SERCOM5) SPI Baud Rate */
#define REG_SERCOM5_SPI_INTENCLR   (0x43000414) /**< \brief (SERCOM5) SPI Interrupt Enable Clear */
#define REG_SERCOM5_SPI_INTENSET   (0x43000416) /**< \brief (SERCOM5) SPI Interrupt Enable Set */
#define REG_SERCOM5_SPI_INTFLAG    (0x43000418) /**< \brief (SERCOM5) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM5_SPI_STATUS     (0x4300041A) /**< \brief (SERCOM5) SPI Status */
#define REG_SERCOM5_SPI_SYNCBUSY   (0x4300041C) /**< \brief (SERCOM5) SPI Synchronization Busy */
#define REG_SERCOM5_SPI_ADDR       (0x43000424) /**< \brief (SERCOM5) SPI Address */
#define REG_SERCOM5_SPI_DATA       (0x43000428) /**< \brief (SERCOM5) SPI Data */
#define REG_SERCOM5_SPI_DBGCTRL    (0x43000430) /**< \brief (SERCOM5) SPI Debug Control */
#define REG_SERCOM5_USART_CTRLA    (0x43000400) /**< \brief (SERCOM5) USART Control A */
#define REG_SERCOM5_USART_CTRLB    (0x43000404) /**< \brief (SERCOM5) USART Control B */
#define REG_SERCOM5_USART_BAUD     (0x4300040C) /**< \brief (SERCOM5) USART Baud Rate */
#define REG_SERCOM5_USART_RXPL     (0x4300040E) /**< \brief (SERCOM5) USART Receive Pulse Length */
#define REG_SERCOM5_USART_INTENCLR (0x43000414) /**< \brief (SERCOM5) USART Interrupt Enable Clear */
#define REG_SERCOM5_USART_INTENSET (0x43000416) /**< \brief (SERCOM5) USART Interrupt Enable Set */
#define REG_SERCOM5_USART_INTFLAG  (0x43000418) /**< \brief (SERCOM5) USART Interrupt Flag Status and Clear */
#define REG_SERCOM5_USART_STATUS   (0x4300041A) /**< \brief (SERCOM5) USART Status */
#define REG_SERCOM5_USART_SYNCBUSY (0x4300041C) /**< \brief (SERCOM5) USART Synchronization Busy */
#define REG_SERCOM5_USART_DATA     (0x43000428) /**< \brief (SERCOM5) USART Data */
#define REG_SERCOM5_USART_DBGCTRL  (0x43000430) /**< \brief (SERCOM5) USART Debug Control */
#else
#define REG_SERCOM5_I2CM_CTRLA     (*(RwReg  *)0x43000400UL) /**< \brief (SERCOM5) I2CM Control A */
#define REG_SERCOM5_I2CM_CTRLB     (*(RwReg  *)0x43000404UL) /**< \brief (SERCOM5) I2CM Control B */
#define REG_SERCOM5_I2CM_BAUD      (*(RwReg  *)0x4300040CUL) /**< \brief (SERCOM5) I2CM Baud Rate */
#define REG_SERCOM5_I2CM_INTENCLR  (*(RwReg8 *)0x43000414UL) /**< \brief (SERCOM5) I2CM Interrupt Enable Clear */
#define REG_SERCOM5_I2CM_INTENSET  (*(RwReg8 *)0x43000416UL) /**< \brief (SERCOM5) I2CM Interrupt Enable Set */
#define REG_SERCOM5_I2CM_INTFLAG   (*(RwReg8 *)0x43000418UL) /**< \brief (SERCOM5) I2CM Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CM_STATUS    (*(RwReg16*)0x4300041AUL) /**< \brief (SERCOM5) I2CM Status */
#define REG_SERCOM5_I2CM_SYNCBUSY  (*(RoReg  *)0x4300041CUL) /**< \brief (SERCOM5) I2CM Synchronization Busy */
#define REG_SERCOM5_I2CM_ADDR      (*(RwReg  *)0x43000424UL) /**< \brief (SERCOM5) I2CM Address */
#define REG_SERCOM5_I2CM_DATA      (*(RwReg8 *)0x43000428UL) /**< \brief (SERCOM5) I2CM Data */
#define REG_SERCOM5_I2CM_DBGCTRL   (*(RwReg8 *)0x43000430UL) /**< \brief (SERCOM5) I2CM Debug Control */
#define REG_SERCOM5_I2CS_CTRLA     (*(RwReg  *)0x43000400UL) /**< \brief (SERCOM5) I2CS Control A */
#define REG_SERCOM5_I2CS_CTRLB     (*(RwReg  *)0x43000404UL) /**< \brief (SERCOM5) I2CS Control B */
#define REG_SERCOM5_I2CS_INTENCLR  (*(RwReg8 *)0x43000414UL) /**< \brief (SERCOM5) I2CS Interrupt Enable Clear */
#define REG_SERCOM5_I2CS_INTENSET  (*(RwReg8 *)0x43000416UL) /**< \brief (SERCOM5) I2CS Interrupt Enable Set */
#define REG_SERCOM5_I2CS_INTFLAG   (*(RwReg8 *)0x43000418UL) /**< \brief (SERCOM5) I2CS Interrupt Flag Status and Clear */
#define REG_SERCOM5_I2CS_STATUS    (*(RwReg16*)0x4300041AUL) /**< \brief (SERCOM5) I2CS Status */
#define REG_SERCOM5_I2CS_SYNCBUSY  (*(RoReg  *)0x4300041CUL) /**< \brief (SERCOM5) I2CS Synchronization Busy */
#define REG_SERCOM5_I2CS_ADDR      (*(RwReg  *)0x43000424UL) /**< \brief (SERCOM5) I2CS Address */
#define REG_SERCOM5_I2CS_DATA      (*(RwReg8 *)0x43000428UL) /**< \brief (SERCOM5) I2CS Data */
#define REG_SERCOM5_SPI_CTRLA      (*(RwReg  *)0x43000400UL) /**< \brief (SERCOM5) SPI Control A */
#define REG_SERCOM5_SPI_CTRLB      (*(RwReg  *)0x43000404UL) /**< \brief (SERCOM5) SPI Control B */
#define REG_SERCOM5_SPI_BAUD       (*(RwReg8 *)0x4300040CUL) /**< \brief (SERCOM5) SPI Baud Rate */
#define REG_SERCOM5_SPI_INTENCLR   (*(RwReg8 *)0x43000414UL) /**< \brief (SERCOM5) SPI Interrupt Enable Clear */
#define REG_SERCOM5_SPI_INTENSET   (*(RwReg8 *)0x43000416UL) /**< \brief (SERCOM5) SPI Interrupt Enable Set */
#define REG_SERCOM5_SPI_INTFLAG    (*(RwReg8 *)0x43000418UL) /**< \brief (SERCOM5) SPI Interrupt Flag Status and Clear */
#define REG_SERCOM5_SPI_STATUS     (*(RwReg16*)0x4300041AUL) /**< \brief (SERCOM5) SPI Status */
#define REG_SERCOM5_SPI_SYNCBUSY   (*(RoReg  *)0x4300041CUL) /**< \brief (SERCOM5) SPI Synchronization Busy */
#define REG_SERCOM5_SPI_ADDR       (*(RwReg  *)0x43000424UL) /**< \brief (SERCOM5) SPI Address */
#define REG_SERCOM5_SPI_DATA       (*(RwReg  *)0x43000428UL) /**< \brief (SERCOM5) SPI Data */
#define REG_SERCOM5_SPI_DBGCTRL    (*(RwReg8 *)0x43000430UL) /**< \brief (SERCOM5) SPI Debug Control */
#define REG_SERCOM5_USART_CTRLA    (*(RwReg  *)0x43000400UL) /**< \brief (SERCOM5) USART Control A */
#define REG_SERCOM5_USART_CTRLB    (*(RwReg  *)0x43000404UL) /**< \brief (SERCOM5) USART Control B */
#define REG_SERCOM5_USART_BAUD     (*(RwReg16*)0x4300040CUL) /**< \brief (SERCOM5) USART Baud Rate */
#define REG_SERCOM5_USART_RXPL     (*(RwReg8 *)0x4300040EUL) /**< \brief (SERCOM5) USART Receive Pulse Length */
#define REG_SERCOM5_USART_INTENCLR (*(RwReg8 *)0x43000414UL) /**< \brief (SERCOM5) USART Interrupt Enable Clear */
#define REG_SERCOM5_USART_INTENSET (*(RwReg8 *)0x43000416UL) /**< \brief (SERCOM5) USART Interrupt Enable Set */
#define REG_SERCOM5_USART_INTFLAG  (*(RwReg8 *)0x43000418UL) /**< \brief (SERCOM5) USART Interrupt Flag Status and Clear */
#define REG_SERCOM5_USART_STATUS   (*(RwReg16*)0x4300041AUL) /**< \brief (SERCOM5) USART Status */
#define REG_SERCOM5_USART_SYNCBUSY (*(RoReg  *)0x4300041CUL) /**< \brief (SERCOM5) USART Synchronization Busy */
#define REG_SERCOM5_USART_DATA     (*(RwReg16*)0x43000428UL) /**< \brief (SERCOM5) USART Data */
#define REG_SERCOM5_USART_DBGCTRL  (*(RwReg8 *)0x43000430UL) /**< \brief (SERCOM5) USART Debug Control */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for SERCOM5 peripheral ========== */
#define SERCOM5_DMAC_ID_RX                   // Index of DMA RX trigger
#define SERCOM5_DMAC_ID_TX                   // Index of DMA TX trigger
#define SERCOM5_GCLK_ID_CORE        24      
#define SERCOM5_GCLK_ID_SLOW        23      
#define SERCOM5_INT_MSB             3       
#define SERCOM5_PMSB                3       

#endif /* _SAML21_SERCOM5_INSTANCE_ */
