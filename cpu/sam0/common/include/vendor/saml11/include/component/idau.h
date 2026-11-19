/**
 * \file
 *
 * \brief Component description for IDAU
 *
 * Copyright (c) 2018 Microchip Technology Inc.
 *
 * \license_start
 *
 * \page License
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * \license_stop
 *
 */

/* file generated from device description version 2018-08-31T13:51:56Z */
#ifndef _SAML11_IDAU_COMPONENT_H_
#define _SAML11_IDAU_COMPONENT_H_
#define _SAML11_IDAU_COMPONENT_         /**< \deprecated  Backward compatibility for ASF */

/** \addtogroup SAML_SAML11 Implementation Defined Attribution Unit
 *  @{
 */
/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR IDAU */
/* ========================================================================== */

#define IDAU_U2803                      /**< (IDAU) Module ID */
#define REV_IDAU 0x100                  /**< (IDAU) Module revision */

/* -------- IDAU_SECCTRL : (IDAU Offset: 0x01) (R/W 8) SECCTRL -------- */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { 
  struct {
    uint8_t  :2;                        /**< bit:   0..1  Reserved */
    uint8_t  RXN:1;                     /**< bit:      2  CPU RAM is eXecute Never                 */
    uint8_t  :5;                        /**< bit:   3..7  Reserved */
  } bit;                                /**< Structure used for bit  access */
  uint8_t  reg;                         /**< Type used for register access */
} IDAU_SECCTRL_Type;
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#define IDAU_SECCTRL_OFFSET                 (0x01)                                        /**<  (IDAU_SECCTRL) SECCTRL  Offset */
#define IDAU_SECCTRL_RESETVALUE             _U_(0x03)                                     /**<  (IDAU_SECCTRL) SECCTRL  Reset Value */

#define IDAU_SECCTRL_RXN_Pos                2                                              /**< (IDAU_SECCTRL) CPU RAM is eXecute Never Position */
#define IDAU_SECCTRL_RXN_Msk                (_U_(0x1) << IDAU_SECCTRL_RXN_Pos)             /**< (IDAU_SECCTRL) CPU RAM is eXecute Never Mask */
#define IDAU_SECCTRL_RXN                    IDAU_SECCTRL_RXN_Msk                           /**< \deprecated Old style mask definition for 1 bit bitfield. Use IDAU_SECCTRL_RXN_Msk instead */
#define IDAU_SECCTRL_MASK                   _U_(0x04)                                      /**< \deprecated (IDAU_SECCTRL) Register MASK  (Use IDAU_SECCTRL_Msk instead)  */
#define IDAU_SECCTRL_Msk                    _U_(0x04)                                      /**< (IDAU_SECCTRL) Register Mask  */


/* -------- IDAU_SCFGB : (IDAU Offset: 0x04) (R/W 32) SCFGB -------- */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { 
  struct {
    uint32_t BS:8;                      /**< bit:   0..7  Boot Secure                              */
    uint32_t BNSC:6;                    /**< bit:  8..13  Boot Secure, Non-secure Callable         */
    uint32_t :2;                        /**< bit: 14..15  Reserved */
    uint32_t BOOTPROT:8;                /**< bit: 16..23  Boot Protection                          */
    uint32_t :8;                        /**< bit: 24..31  Reserved */
  } bit;                                /**< Structure used for bit  access */
  uint32_t reg;                         /**< Type used for register access */
} IDAU_SCFGB_Type;
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#define IDAU_SCFGB_OFFSET                   (0x04)                                        /**<  (IDAU_SCFGB) SCFGB  Offset */
#define IDAU_SCFGB_RESETVALUE               _U_(0x00)                                     /**<  (IDAU_SCFGB) SCFGB  Reset Value */

#define IDAU_SCFGB_BS_Pos                   0                                              /**< (IDAU_SCFGB) Boot Secure Position */
#define IDAU_SCFGB_BS_Msk                   (_U_(0xFF) << IDAU_SCFGB_BS_Pos)               /**< (IDAU_SCFGB) Boot Secure Mask */
#define IDAU_SCFGB_BS(value)                (IDAU_SCFGB_BS_Msk & ((value) << IDAU_SCFGB_BS_Pos))
#define IDAU_SCFGB_BNSC_Pos                 8                                              /**< (IDAU_SCFGB) Boot Secure, Non-secure Callable Position */
#define IDAU_SCFGB_BNSC_Msk                 (_U_(0x3F) << IDAU_SCFGB_BNSC_Pos)             /**< (IDAU_SCFGB) Boot Secure, Non-secure Callable Mask */
#define IDAU_SCFGB_BNSC(value)              (IDAU_SCFGB_BNSC_Msk & ((value) << IDAU_SCFGB_BNSC_Pos))
#define IDAU_SCFGB_BOOTPROT_Pos             16                                             /**< (IDAU_SCFGB) Boot Protection Position */
#define IDAU_SCFGB_BOOTPROT_Msk             (_U_(0xFF) << IDAU_SCFGB_BOOTPROT_Pos)         /**< (IDAU_SCFGB) Boot Protection Mask */
#define IDAU_SCFGB_BOOTPROT(value)          (IDAU_SCFGB_BOOTPROT_Msk & ((value) << IDAU_SCFGB_BOOTPROT_Pos))
#define IDAU_SCFGB_MASK                     _U_(0xFF3FFF)                                  /**< \deprecated (IDAU_SCFGB) Register MASK  (Use IDAU_SCFGB_Msk instead)  */
#define IDAU_SCFGB_Msk                      _U_(0xFF3FFF)                                  /**< (IDAU_SCFGB) Register Mask  */


/* -------- IDAU_SCFGA : (IDAU Offset: 0x08) (R/W 32) SCFGA -------- */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { 
  struct {
    uint32_t AS:8;                      /**< bit:   0..7  Application Secure                       */
    uint32_t ANSC:6;                    /**< bit:  8..13  Application Secure, Non-secure Callable  */
    uint32_t :2;                        /**< bit: 14..15  Reserved */
    uint32_t DS:4;                      /**< bit: 16..19  DATAFLASH Data Secure                    */
    uint32_t :12;                       /**< bit: 20..31  Reserved */
  } bit;                                /**< Structure used for bit  access */
  uint32_t reg;                         /**< Type used for register access */
} IDAU_SCFGA_Type;
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#define IDAU_SCFGA_OFFSET                   (0x08)                                        /**<  (IDAU_SCFGA) SCFGA  Offset */
#define IDAU_SCFGA_RESETVALUE               _U_(0x00)                                     /**<  (IDAU_SCFGA) SCFGA  Reset Value */

#define IDAU_SCFGA_AS_Pos                   0                                              /**< (IDAU_SCFGA) Application Secure Position */
#define IDAU_SCFGA_AS_Msk                   (_U_(0xFF) << IDAU_SCFGA_AS_Pos)               /**< (IDAU_SCFGA) Application Secure Mask */
#define IDAU_SCFGA_AS(value)                (IDAU_SCFGA_AS_Msk & ((value) << IDAU_SCFGA_AS_Pos))
#define IDAU_SCFGA_ANSC_Pos                 8                                              /**< (IDAU_SCFGA) Application Secure, Non-secure Callable Position */
#define IDAU_SCFGA_ANSC_Msk                 (_U_(0x3F) << IDAU_SCFGA_ANSC_Pos)             /**< (IDAU_SCFGA) Application Secure, Non-secure Callable Mask */
#define IDAU_SCFGA_ANSC(value)              (IDAU_SCFGA_ANSC_Msk & ((value) << IDAU_SCFGA_ANSC_Pos))
#define IDAU_SCFGA_DS_Pos                   16                                             /**< (IDAU_SCFGA) DATAFLASH Data Secure Position */
#define IDAU_SCFGA_DS_Msk                   (_U_(0xF) << IDAU_SCFGA_DS_Pos)                /**< (IDAU_SCFGA) DATAFLASH Data Secure Mask */
#define IDAU_SCFGA_DS(value)                (IDAU_SCFGA_DS_Msk & ((value) << IDAU_SCFGA_DS_Pos))
#define IDAU_SCFGA_MASK                     _U_(0xF3FFF)                                   /**< \deprecated (IDAU_SCFGA) Register MASK  (Use IDAU_SCFGA_Msk instead)  */
#define IDAU_SCFGA_Msk                      _U_(0xF3FFF)                                   /**< (IDAU_SCFGA) Register Mask  */


/* -------- IDAU_SCFGR : (IDAU Offset: 0x0c) (R/W 8) SCFGR -------- */
#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { 
  struct {
    uint8_t  RS:7;                      /**< bit:   0..6  RAM Secure                               */
    uint8_t  :1;                        /**< bit:      7  Reserved */
  } bit;                                /**< Structure used for bit  access */
  uint8_t  reg;                         /**< Type used for register access */
} IDAU_SCFGR_Type;
#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */

#define IDAU_SCFGR_OFFSET                   (0x0C)                                        /**<  (IDAU_SCFGR) SCFGR  Offset */
#define IDAU_SCFGR_RESETVALUE               _U_(0x00)                                     /**<  (IDAU_SCFGR) SCFGR  Reset Value */

#define IDAU_SCFGR_RS_Pos                   0                                              /**< (IDAU_SCFGR) RAM Secure Position */
#define IDAU_SCFGR_RS_Msk                   (_U_(0x7F) << IDAU_SCFGR_RS_Pos)               /**< (IDAU_SCFGR) RAM Secure Mask */
#define IDAU_SCFGR_RS(value)                (IDAU_SCFGR_RS_Msk & ((value) << IDAU_SCFGR_RS_Pos))
#define IDAU_SCFGR_MASK                     _U_(0x7F)                                      /**< \deprecated (IDAU_SCFGR) Register MASK  (Use IDAU_SCFGR_Msk instead)  */
#define IDAU_SCFGR_Msk                      _U_(0x7F)                                      /**< (IDAU_SCFGR) Register Mask  */


#if !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief IDAU hardware registers */
typedef struct {  /* Implementation Defined Attribution Unit */
  __I  uint8_t                        Reserved1[1];
  __IO IDAU_SECCTRL_Type              SECCTRL;        /**< Offset: 0x01 (R/W   8) SECCTRL */
  __I  uint8_t                        Reserved2[2];
  __IO IDAU_SCFGB_Type                SCFGB;          /**< Offset: 0x04 (R/W  32) SCFGB */
  __IO IDAU_SCFGA_Type                SCFGA;          /**< Offset: 0x08 (R/W  32) SCFGA */
  __IO IDAU_SCFGR_Type                SCFGR;          /**< Offset: 0x0C (R/W   8) SCFGR */
} Idau;


#endif /* !(defined(__ASSEMBLER__) || defined(__IAR_SYSTEMS_ASM__)) */
/** @}  end of Implementation Defined Attribution Unit */

#endif /* _SAML11_IDAU_COMPONENT_H_ */
