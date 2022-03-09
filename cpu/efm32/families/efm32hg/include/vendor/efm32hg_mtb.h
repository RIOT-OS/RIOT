/***************************************************************************//**
 * @file
 * @brief EFM32HG_MTB register and bit field definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__ICCARM__)
#pragma system_include       /* Treat file as system include file. */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang system_header  /* Treat file as system include file. */
#endif

/***************************************************************************//**
 * @addtogroup Parts
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @defgroup EFM32HG_MTB
 * @{
 * @brief EFM32HG_MTB Register Declaration
 ******************************************************************************/
typedef struct {
  __IOM uint32_t POSITION; /**< MTB Trace Position Register.  */
  __IOM uint32_t MASTER;   /**< MTB Trace Control Register  */
  __IOM uint32_t FLOW;     /**< MTB Trace Flow Register  */
  __IOM uint32_t BASE;     /**< MTB Trace Base Register  */
} MTB_TypeDef;             /**< MTB Register Declaration *//** @} */

/***************************************************************************//**
 * @defgroup EFM32HG_MTB_BitFields
 * @{
 ******************************************************************************/

/* Bit fields for MTB POSITION */
#define _MTB_POSITION_RESETVALUE         0x00000000UL                         /**< Default value for MTB_POSITION */
#define _MTB_POSITION_MASK               0xFFFFFFFCUL                         /**< Mask for MTB_POSITION */
#define MTB_POSITION_WRAP                (0x1UL << 2)                         /**< Trace wrap bit. */
#define _MTB_POSITION_WRAP_SHIFT         2                                    /**< Shift value for MTB_WRAP */
#define _MTB_POSITION_WRAP_MASK          0x4UL                                /**< Bit mask for MTB_WRAP */
#define _MTB_POSITION_WRAP_DEFAULT       0x00000000UL                         /**< Mode DEFAULT for MTB_POSITION */
#define MTB_POSITION_WRAP_DEFAULT        (_MTB_POSITION_WRAP_DEFAULT << 2)    /**< Shifted mode DEFAULT for MTB_POSITION */
#define _MTB_POSITION_POINTER_SHIFT      3                                    /**< Shift value for MTB_POINTER */
#define _MTB_POSITION_POINTER_MASK       0xFFFFFFF8UL                         /**< Bit mask for MTB_POINTER */
#define _MTB_POSITION_POINTER_DEFAULT    0x00000000UL                         /**< Mode DEFAULT for MTB_POSITION */
#define MTB_POSITION_POINTER_DEFAULT     (_MTB_POSITION_POINTER_DEFAULT << 3) /**< Shifted mode DEFAULT for MTB_POSITION */

/* Bit fields for MTB MASTER */
#define _MTB_MASTER_RESETVALUE           0x00000000UL                        /**< Default value for MTB_MASTER */
#define _MTB_MASTER_MASK                 0x8000027FUL                        /**< Mask for MTB_MASTER */
#define _MTB_MASTER_MASK_SHIFT           0                                   /**< Shift value for MTB_MASK */
#define _MTB_MASTER_MASK_MASK            0x1FUL                              /**< Bit mask for MTB_MASK */
#define _MTB_MASTER_MASK_DEFAULT         0x00000000UL                        /**< Mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_MASK_DEFAULT          (_MTB_MASTER_MASK_DEFAULT << 0)     /**< Shifted mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_TSTARTEN              (0x1UL << 5)                        /**< Trace start input enable. */
#define _MTB_MASTER_TSTARTEN_SHIFT       5                                   /**< Shift value for MTB_TSTARTEN */
#define _MTB_MASTER_TSTARTEN_MASK        0x20UL                              /**< Bit mask for MTB_TSTARTEN */
#define _MTB_MASTER_TSTARTEN_DEFAULT     0x00000000UL                        /**< Mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_TSTARTEN_DEFAULT      (_MTB_MASTER_TSTARTEN_DEFAULT << 5) /**< Shifted mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_TSTOPEN               (0x1UL << 6)                        /**< Trace stop input enable. */
#define _MTB_MASTER_TSTOPEN_SHIFT        6                                   /**< Shift value for MTB_TSTOPEN */
#define _MTB_MASTER_TSTOPEN_MASK         0x40UL                              /**< Bit mask for MTB_TSTOPEN */
#define _MTB_MASTER_TSTOPEN_DEFAULT      0x00000000UL                        /**< Mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_TSTOPEN_DEFAULT       (_MTB_MASTER_TSTOPEN_DEFAULT << 6)  /**< Shifted mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_HALTREQ               (0x1UL << 9)                        /**< Halt request bit. */
#define _MTB_MASTER_HALTREQ_SHIFT        9                                   /**< Shift value for MTB_HALTREQ */
#define _MTB_MASTER_HALTREQ_MASK         0x200UL                             /**< Bit mask for MTB_HALTREQ */
#define _MTB_MASTER_HALTREQ_DEFAULT      0x00000000UL                        /**< Mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_HALTREQ_DEFAULT       (_MTB_MASTER_HALTREQ_DEFAULT << 9)  /**< Shifted mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_EN                    (0x1UL << 31)                       /**< Main trace enable bit. */
#define _MTB_MASTER_EN_SHIFT             31                                  /**< Shift value for MTB_EN */
#define _MTB_MASTER_EN_MASK              0x80000000UL                        /**< Bit mask for MTB_EN */
#define _MTB_MASTER_EN_DEFAULT           0x00000000UL                        /**< Mode DEFAULT for MTB_MASTER */
#define MTB_MASTER_EN_DEFAULT            (_MTB_MASTER_EN_DEFAULT << 31)      /**< Shifted mode DEFAULT for MTB_MASTER */

/* Bit fields for MTB FLOW */
#define _MTB_FLOW_RESETVALUE             0x00000000UL                       /**< Default value for MTB_FLOW */
#define _MTB_FLOW_MASK                   0xFFFFFFFBUL                       /**< Mask for MTB_FLOW */
#define MTB_FLOW_AUTOSTOP                (0x1UL << 0)                       /**< AUTOSTOP enable. */
#define _MTB_FLOW_AUTOSTOP_SHIFT         0                                  /**< Shift value for MTB_AUTOSTOP */
#define _MTB_FLOW_AUTOSTOP_MASK          0x1UL                              /**< Bit mask for MTB_AUTOSTOP */
#define _MTB_FLOW_AUTOSTOP_DEFAULT       0x00000000UL                       /**< Mode DEFAULT for MTB_FLOW */
#define MTB_FLOW_AUTOSTOP_DEFAULT        (_MTB_FLOW_AUTOSTOP_DEFAULT << 0)  /**< Shifted mode DEFAULT for MTB_FLOW */
#define MTB_FLOW_AUTOHALT                (0x1UL << 1)                       /**< AUTOHALT enable. */
#define _MTB_FLOW_AUTOHALT_SHIFT         1                                  /**< Shift value for MTB_AUTOHALT */
#define _MTB_FLOW_AUTOHALT_MASK          0x2UL                              /**< Bit mask for MTB_AUTOHALT */
#define _MTB_FLOW_AUTOHALT_DEFAULT       0x00000000UL                       /**< Mode DEFAULT for MTB_FLOW */
#define MTB_FLOW_AUTOHALT_DEFAULT        (_MTB_FLOW_AUTOHALT_DEFAULT << 1)  /**< Shifted mode DEFAULT for MTB_FLOW */
#define _MTB_FLOW_WATERMARK_SHIFT        3                                  /**< Shift value for MTB_WATERMARK */
#define _MTB_FLOW_WATERMARK_MASK         0xFFFFFFF8UL                       /**< Bit mask for MTB_WATERMARK */
#define _MTB_FLOW_WATERMARK_DEFAULT      0x00000000UL                       /**< Mode DEFAULT for MTB_FLOW */
#define MTB_FLOW_WATERMARK_DEFAULT       (_MTB_FLOW_WATERMARK_DEFAULT << 3) /**< Shifted mode DEFAULT for MTB_FLOW */

/* Bit fields for MTB BASE */
#define _MTB_BASE_RESETVALUE             0x20000000UL                  /**< Default value for MTB_BASE */
#define _MTB_BASE_MASK                   0xFFFFFFFFUL                  /**< Mask for MTB_BASE */
#define _MTB_BASE_BASE_SHIFT             0                             /**< Shift value for MTB_BASE */
#define _MTB_BASE_BASE_MASK              0xFFFFFFFFUL                  /**< Bit mask for MTB_BASE */
#define _MTB_BASE_BASE_DEFAULT           0x20000000UL                  /**< Mode DEFAULT for MTB_BASE */
#define MTB_BASE_BASE_DEFAULT            (_MTB_BASE_BASE_DEFAULT << 0) /**< Shifted mode DEFAULT for MTB_BASE */

/** @} End of group EFM32HG_MTB */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

