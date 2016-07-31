/**
 * \file
 *
 * \brief Component description for WDT
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAMR21_WDT_COMPONENT_
#define _SAMR21_WDT_COMPONENT_

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR WDT */
/* ========================================================================== */
/** \addtogroup SAMR21_WDT Watchdog Timer */
/*@{*/

#define WDT_U2203
#define REV_WDT                     0x200

/* -------- WDT_CTRL : (WDT Offset: 0x0) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :1;               /*!< bit:      0  Reserved                           */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  WEN:1;            /*!< bit:      2  Watchdog Timer Window Mode Enable  */
    uint8_t  :4;               /*!< bit:  3.. 6  Reserved                           */
    uint8_t  ALWAYSON:1;       /*!< bit:      7  Always-On                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CTRL_OFFSET             0x0          /**< \brief (WDT_CTRL offset) Control */
#define WDT_CTRL_RESETVALUE         0x00         /**< \brief (WDT_CTRL reset_value) Control */

#define WDT_CTRL_ENABLE_Pos         1            /**< \brief (WDT_CTRL) Enable */
#define WDT_CTRL_ENABLE             (0x1u << WDT_CTRL_ENABLE_Pos)
#define WDT_CTRL_WEN_Pos            2            /**< \brief (WDT_CTRL) Watchdog Timer Window Mode Enable */
#define WDT_CTRL_WEN                (0x1u << WDT_CTRL_WEN_Pos)
#define WDT_CTRL_ALWAYSON_Pos       7            /**< \brief (WDT_CTRL) Always-On */
#define WDT_CTRL_ALWAYSON           (0x1u << WDT_CTRL_ALWAYSON_Pos)
#define WDT_CTRL_MASK               0x86u        /**< \brief (WDT_CTRL) MASK Register */

/* -------- WDT_CONFIG : (WDT Offset: 0x1) (R/W  8) Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PER:4;            /*!< bit:  0.. 3  Time-Out Period                    */
    uint8_t  WINDOW:4;         /*!< bit:  4.. 7  Window Mode Time-Out Period        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CONFIG_OFFSET           0x1          /**< \brief (WDT_CONFIG offset) Configuration */
#define WDT_CONFIG_RESETVALUE       0xBB         /**< \brief (WDT_CONFIG reset_value) Configuration */

#define WDT_CONFIG_PER_Pos          0            /**< \brief (WDT_CONFIG) Time-Out Period */
#define WDT_CONFIG_PER_Msk          (0xFu << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER(value)       ((WDT_CONFIG_PER_Msk & ((value) << WDT_CONFIG_PER_Pos)))
#define   WDT_CONFIG_PER_0_Val            0x0u   /**< \brief (WDT_CONFIG) 8 clock cycles */
#define   WDT_CONFIG_PER_1_Val            0x1u   /**< \brief (WDT_CONFIG) 16 clock cycles */
#define   WDT_CONFIG_PER_2_Val            0x2u   /**< \brief (WDT_CONFIG) 32 clock cycles */
#define   WDT_CONFIG_PER_3_Val            0x3u   /**< \brief (WDT_CONFIG) 64 clock cycles */
#define   WDT_CONFIG_PER_4_Val            0x4u   /**< \brief (WDT_CONFIG) 128 clock cycles */
#define   WDT_CONFIG_PER_5_Val            0x5u   /**< \brief (WDT_CONFIG) 256 clock cycles */
#define   WDT_CONFIG_PER_6_Val            0x6u   /**< \brief (WDT_CONFIG) 512 clock cycles */
#define   WDT_CONFIG_PER_7_Val            0x7u   /**< \brief (WDT_CONFIG) 1024 clock cycles */
#define   WDT_CONFIG_PER_8_Val            0x8u   /**< \brief (WDT_CONFIG) 2048 clock cycles */
#define   WDT_CONFIG_PER_9_Val            0x9u   /**< \brief (WDT_CONFIG) 4096 clock cycles */
#define   WDT_CONFIG_PER_10_Val           0xAu   /**< \brief (WDT_CONFIG) 8192 clock cycles */
#define   WDT_CONFIG_PER_11_Val           0xBu   /**< \brief (WDT_CONFIG) 16384 clock cycles */
#define WDT_CONFIG_PER_0            (WDT_CONFIG_PER_0_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_1            (WDT_CONFIG_PER_1_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_2            (WDT_CONFIG_PER_2_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_3            (WDT_CONFIG_PER_3_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_4            (WDT_CONFIG_PER_4_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_5            (WDT_CONFIG_PER_5_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_6            (WDT_CONFIG_PER_6_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_7            (WDT_CONFIG_PER_7_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_8            (WDT_CONFIG_PER_8_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_9            (WDT_CONFIG_PER_9_Val          << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_10           (WDT_CONFIG_PER_10_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_PER_11           (WDT_CONFIG_PER_11_Val         << WDT_CONFIG_PER_Pos)
#define WDT_CONFIG_WINDOW_Pos       4            /**< \brief (WDT_CONFIG) Window Mode Time-Out Period */
#define WDT_CONFIG_WINDOW_Msk       (0xFu << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW(value)    ((WDT_CONFIG_WINDOW_Msk & ((value) << WDT_CONFIG_WINDOW_Pos)))
#define   WDT_CONFIG_WINDOW_0_Val         0x0u   /**< \brief (WDT_CONFIG) 8 clock cycles */
#define   WDT_CONFIG_WINDOW_1_Val         0x1u   /**< \brief (WDT_CONFIG) 16 clock cycles */
#define   WDT_CONFIG_WINDOW_2_Val         0x2u   /**< \brief (WDT_CONFIG) 32 clock cycles */
#define   WDT_CONFIG_WINDOW_3_Val         0x3u   /**< \brief (WDT_CONFIG) 64 clock cycles */
#define   WDT_CONFIG_WINDOW_4_Val         0x4u   /**< \brief (WDT_CONFIG) 128 clock cycles */
#define   WDT_CONFIG_WINDOW_5_Val         0x5u   /**< \brief (WDT_CONFIG) 256 clock cycles */
#define   WDT_CONFIG_WINDOW_6_Val         0x6u   /**< \brief (WDT_CONFIG) 512 clock cycles */
#define   WDT_CONFIG_WINDOW_7_Val         0x7u   /**< \brief (WDT_CONFIG) 1024 clock cycles */
#define   WDT_CONFIG_WINDOW_8_Val         0x8u   /**< \brief (WDT_CONFIG) 2048 clock cycles */
#define   WDT_CONFIG_WINDOW_9_Val         0x9u   /**< \brief (WDT_CONFIG) 4096 clock cycles */
#define   WDT_CONFIG_WINDOW_10_Val        0xAu   /**< \brief (WDT_CONFIG) 8192 clock cycles */
#define   WDT_CONFIG_WINDOW_11_Val        0xBu   /**< \brief (WDT_CONFIG) 16384 clock cycles */
#define WDT_CONFIG_WINDOW_0         (WDT_CONFIG_WINDOW_0_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_1         (WDT_CONFIG_WINDOW_1_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_2         (WDT_CONFIG_WINDOW_2_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_3         (WDT_CONFIG_WINDOW_3_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_4         (WDT_CONFIG_WINDOW_4_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_5         (WDT_CONFIG_WINDOW_5_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_6         (WDT_CONFIG_WINDOW_6_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_7         (WDT_CONFIG_WINDOW_7_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_8         (WDT_CONFIG_WINDOW_8_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_9         (WDT_CONFIG_WINDOW_9_Val       << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_10        (WDT_CONFIG_WINDOW_10_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_WINDOW_11        (WDT_CONFIG_WINDOW_11_Val      << WDT_CONFIG_WINDOW_Pos)
#define WDT_CONFIG_MASK             0xFFu        /**< \brief (WDT_CONFIG) MASK Register */

/* -------- WDT_EWCTRL : (WDT Offset: 0x2) (R/W  8) Early Warning Interrupt Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EWOFFSET:4;       /*!< bit:  0.. 3  Early Warning Interrupt Time Offset */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_EWCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_EWCTRL_OFFSET           0x2          /**< \brief (WDT_EWCTRL offset) Early Warning Interrupt Control */
#define WDT_EWCTRL_RESETVALUE       0x0B         /**< \brief (WDT_EWCTRL reset_value) Early Warning Interrupt Control */

#define WDT_EWCTRL_EWOFFSET_Pos     0            /**< \brief (WDT_EWCTRL) Early Warning Interrupt Time Offset */
#define WDT_EWCTRL_EWOFFSET_Msk     (0xFu << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET(value)  ((WDT_EWCTRL_EWOFFSET_Msk & ((value) << WDT_EWCTRL_EWOFFSET_Pos)))
#define   WDT_EWCTRL_EWOFFSET_0_Val       0x0u   /**< \brief (WDT_EWCTRL) 8 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_1_Val       0x1u   /**< \brief (WDT_EWCTRL) 16 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_2_Val       0x2u   /**< \brief (WDT_EWCTRL) 32 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_3_Val       0x3u   /**< \brief (WDT_EWCTRL) 64 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_4_Val       0x4u   /**< \brief (WDT_EWCTRL) 128 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_5_Val       0x5u   /**< \brief (WDT_EWCTRL) 256 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_6_Val       0x6u   /**< \brief (WDT_EWCTRL) 512 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_7_Val       0x7u   /**< \brief (WDT_EWCTRL) 1024 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_8_Val       0x8u   /**< \brief (WDT_EWCTRL) 2048 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_9_Val       0x9u   /**< \brief (WDT_EWCTRL) 4096 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_10_Val      0xAu   /**< \brief (WDT_EWCTRL) 8192 clock cycles */
#define   WDT_EWCTRL_EWOFFSET_11_Val      0xBu   /**< \brief (WDT_EWCTRL) 16384 clock cycles */
#define WDT_EWCTRL_EWOFFSET_0       (WDT_EWCTRL_EWOFFSET_0_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_1       (WDT_EWCTRL_EWOFFSET_1_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_2       (WDT_EWCTRL_EWOFFSET_2_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_3       (WDT_EWCTRL_EWOFFSET_3_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_4       (WDT_EWCTRL_EWOFFSET_4_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_5       (WDT_EWCTRL_EWOFFSET_5_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_6       (WDT_EWCTRL_EWOFFSET_6_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_7       (WDT_EWCTRL_EWOFFSET_7_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_8       (WDT_EWCTRL_EWOFFSET_8_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_9       (WDT_EWCTRL_EWOFFSET_9_Val     << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_10      (WDT_EWCTRL_EWOFFSET_10_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_EWOFFSET_11      (WDT_EWCTRL_EWOFFSET_11_Val    << WDT_EWCTRL_EWOFFSET_Pos)
#define WDT_EWCTRL_MASK             0x0Fu        /**< \brief (WDT_EWCTRL) MASK Register */

/* -------- WDT_INTENCLR : (WDT Offset: 0x4) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EW:1;             /*!< bit:      0  Early Warning Interrupt Enable     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTENCLR_OFFSET         0x4          /**< \brief (WDT_INTENCLR offset) Interrupt Enable Clear */
#define WDT_INTENCLR_RESETVALUE     0x00         /**< \brief (WDT_INTENCLR reset_value) Interrupt Enable Clear */

#define WDT_INTENCLR_EW_Pos         0            /**< \brief (WDT_INTENCLR) Early Warning Interrupt Enable */
#define WDT_INTENCLR_EW             (0x1u << WDT_INTENCLR_EW_Pos)
#define WDT_INTENCLR_MASK           0x01u        /**< \brief (WDT_INTENCLR) MASK Register */

/* -------- WDT_INTENSET : (WDT Offset: 0x5) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EW:1;             /*!< bit:      0  Early Warning Interrupt Enable     */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTENSET_OFFSET         0x5          /**< \brief (WDT_INTENSET offset) Interrupt Enable Set */
#define WDT_INTENSET_RESETVALUE     0x00         /**< \brief (WDT_INTENSET reset_value) Interrupt Enable Set */

#define WDT_INTENSET_EW_Pos         0            /**< \brief (WDT_INTENSET) Early Warning Interrupt Enable */
#define WDT_INTENSET_EW             (0x1u << WDT_INTENSET_EW_Pos)
#define WDT_INTENSET_MASK           0x01u        /**< \brief (WDT_INTENSET) MASK Register */

/* -------- WDT_INTFLAG : (WDT Offset: 0x6) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EW:1;             /*!< bit:      0  Early Warning                      */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_INTFLAG_OFFSET          0x6          /**< \brief (WDT_INTFLAG offset) Interrupt Flag Status and Clear */
#define WDT_INTFLAG_RESETVALUE      0x00         /**< \brief (WDT_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define WDT_INTFLAG_EW_Pos          0            /**< \brief (WDT_INTFLAG) Early Warning */
#define WDT_INTFLAG_EW              (0x1u << WDT_INTFLAG_EW_Pos)
#define WDT_INTFLAG_MASK            0x01u        /**< \brief (WDT_INTFLAG) MASK Register */

/* -------- WDT_STATUS : (WDT Offset: 0x7) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_STATUS_OFFSET           0x7          /**< \brief (WDT_STATUS offset) Status */
#define WDT_STATUS_RESETVALUE       0x00         /**< \brief (WDT_STATUS reset_value) Status */

#define WDT_STATUS_SYNCBUSY_Pos     7            /**< \brief (WDT_STATUS) Synchronization Busy */
#define WDT_STATUS_SYNCBUSY         (0x1u << WDT_STATUS_SYNCBUSY_Pos)
#define WDT_STATUS_MASK             0x80u        /**< \brief (WDT_STATUS) MASK Register */

/* -------- WDT_CLEAR : (WDT Offset: 0x8) ( /W  8) Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CLEAR:8;          /*!< bit:  0.. 7  Watchdog Clear                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} WDT_CLEAR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define WDT_CLEAR_OFFSET            0x8          /**< \brief (WDT_CLEAR offset) Clear */
#define WDT_CLEAR_RESETVALUE        0x00         /**< \brief (WDT_CLEAR reset_value) Clear */

#define WDT_CLEAR_CLEAR_Pos         0            /**< \brief (WDT_CLEAR) Watchdog Clear */
#define WDT_CLEAR_CLEAR_Msk         (0xFFu << WDT_CLEAR_CLEAR_Pos)
#define WDT_CLEAR_CLEAR(value)      ((WDT_CLEAR_CLEAR_Msk & ((value) << WDT_CLEAR_CLEAR_Pos)))
#define   WDT_CLEAR_CLEAR_KEY_Val         0xA5u   /**< \brief (WDT_CLEAR) Clear Key */
#define WDT_CLEAR_CLEAR_KEY         (WDT_CLEAR_CLEAR_KEY_Val       << WDT_CLEAR_CLEAR_Pos)
#define WDT_CLEAR_MASK              0xFFu        /**< \brief (WDT_CLEAR) MASK Register */

/** \brief WDT hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO WDT_CTRL_Type             CTRL;        /**< \brief Offset: 0x0 (R/W  8) Control */
  __IO WDT_CONFIG_Type           CONFIG;      /**< \brief Offset: 0x1 (R/W  8) Configuration */
  __IO WDT_EWCTRL_Type           EWCTRL;      /**< \brief Offset: 0x2 (R/W  8) Early Warning Interrupt Control */
       RoReg8                    Reserved1[0x1];
  __IO WDT_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x4 (R/W  8) Interrupt Enable Clear */
  __IO WDT_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x5 (R/W  8) Interrupt Enable Set */
  __IO WDT_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x6 (R/W  8) Interrupt Flag Status and Clear */
  __I  WDT_STATUS_Type           STATUS;      /**< \brief Offset: 0x7 (R/   8) Status */
  __O  WDT_CLEAR_Type            CLEAR;       /**< \brief Offset: 0x8 ( /W  8) Clear */
} Wdt;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _SAMR21_WDT_COMPONENT_ */
