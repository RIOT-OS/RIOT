/***************************************************************************//**
 * @file
 * @brief EFR32MG12P_FPUEH register and bit field definitions
 * @version 5.8.3
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
 * @defgroup EFR32MG12P_FPUEH FPUEH
 * @{
 * @brief EFR32MG12P_FPUEH Register Declaration
 ******************************************************************************/
/** FPUEH Register Declaration */
typedef struct {
  __IM uint32_t  IF;  /**< Interrupt Flag Register  */
  __IOM uint32_t IFS; /**< Interrupt Flag Set Register  */
  __IOM uint32_t IFC; /**< Interrupt Flag Clear Register  */
  __IOM uint32_t IEN; /**< Interrupt Enable Register  */
} FPUEH_TypeDef;      /** @} */

/***************************************************************************//**
 * @addtogroup EFR32MG12P_FPUEH
 * @{
 * @defgroup EFR32MG12P_FPUEH_BitFields  FPUEH Bit Fields
 * @{
 ******************************************************************************/

/* Bit fields for FPUEH IF */
#define _FPUEH_IF_RESETVALUE        0x00000000UL                   /**< Default value for FPUEH_IF */
#define _FPUEH_IF_MASK              0x0000003FUL                   /**< Mask for FPUEH_IF */
#define FPUEH_IF_FPIOC              (0x1UL << 0)                   /**< FPU invalid operation */
#define _FPUEH_IF_FPIOC_SHIFT       0                              /**< Shift value for FPUEH_FPIOC */
#define _FPUEH_IF_FPIOC_MASK        0x1UL                          /**< Bit mask for FPUEH_FPIOC */
#define _FPUEH_IF_FPIOC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPIOC_DEFAULT      (_FPUEH_IF_FPIOC_DEFAULT << 0) /**< Shifted mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPDZC              (0x1UL << 1)                   /**< FPU divide-by-zero exception */
#define _FPUEH_IF_FPDZC_SHIFT       1                              /**< Shift value for FPUEH_FPDZC */
#define _FPUEH_IF_FPDZC_MASK        0x2UL                          /**< Bit mask for FPUEH_FPDZC */
#define _FPUEH_IF_FPDZC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPDZC_DEFAULT      (_FPUEH_IF_FPDZC_DEFAULT << 1) /**< Shifted mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPUFC              (0x1UL << 2)                   /**< FPU underflow exception */
#define _FPUEH_IF_FPUFC_SHIFT       2                              /**< Shift value for FPUEH_FPUFC */
#define _FPUEH_IF_FPUFC_MASK        0x4UL                          /**< Bit mask for FPUEH_FPUFC */
#define _FPUEH_IF_FPUFC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPUFC_DEFAULT      (_FPUEH_IF_FPUFC_DEFAULT << 2) /**< Shifted mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPOFC              (0x1UL << 3)                   /**< FPU overflow exception */
#define _FPUEH_IF_FPOFC_SHIFT       3                              /**< Shift value for FPUEH_FPOFC */
#define _FPUEH_IF_FPOFC_MASK        0x8UL                          /**< Bit mask for FPUEH_FPOFC */
#define _FPUEH_IF_FPOFC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPOFC_DEFAULT      (_FPUEH_IF_FPOFC_DEFAULT << 3) /**< Shifted mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPIDC              (0x1UL << 4)                   /**< FPU input denormal exception */
#define _FPUEH_IF_FPIDC_SHIFT       4                              /**< Shift value for FPUEH_FPIDC */
#define _FPUEH_IF_FPIDC_MASK        0x10UL                         /**< Bit mask for FPUEH_FPIDC */
#define _FPUEH_IF_FPIDC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPIDC_DEFAULT      (_FPUEH_IF_FPIDC_DEFAULT << 4) /**< Shifted mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPIXC              (0x1UL << 5)                   /**< FPU inexact exception */
#define _FPUEH_IF_FPIXC_SHIFT       5                              /**< Shift value for FPUEH_FPIXC */
#define _FPUEH_IF_FPIXC_MASK        0x20UL                         /**< Bit mask for FPUEH_FPIXC */
#define _FPUEH_IF_FPIXC_DEFAULT     0x00000000UL                   /**< Mode DEFAULT for FPUEH_IF */
#define FPUEH_IF_FPIXC_DEFAULT      (_FPUEH_IF_FPIXC_DEFAULT << 5) /**< Shifted mode DEFAULT for FPUEH_IF */

/* Bit fields for FPUEH IFS */
#define _FPUEH_IFS_RESETVALUE       0x00000000UL                    /**< Default value for FPUEH_IFS */
#define _FPUEH_IFS_MASK             0x0000003FUL                    /**< Mask for FPUEH_IFS */
#define FPUEH_IFS_FPIOC             (0x1UL << 0)                    /**< Set FPIOC Interrupt Flag */
#define _FPUEH_IFS_FPIOC_SHIFT      0                               /**< Shift value for FPUEH_FPIOC */
#define _FPUEH_IFS_FPIOC_MASK       0x1UL                           /**< Bit mask for FPUEH_FPIOC */
#define _FPUEH_IFS_FPIOC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPIOC_DEFAULT     (_FPUEH_IFS_FPIOC_DEFAULT << 0) /**< Shifted mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPDZC             (0x1UL << 1)                    /**< Set FPDZC Interrupt Flag */
#define _FPUEH_IFS_FPDZC_SHIFT      1                               /**< Shift value for FPUEH_FPDZC */
#define _FPUEH_IFS_FPDZC_MASK       0x2UL                           /**< Bit mask for FPUEH_FPDZC */
#define _FPUEH_IFS_FPDZC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPDZC_DEFAULT     (_FPUEH_IFS_FPDZC_DEFAULT << 1) /**< Shifted mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPUFC             (0x1UL << 2)                    /**< Set FPUFC Interrupt Flag */
#define _FPUEH_IFS_FPUFC_SHIFT      2                               /**< Shift value for FPUEH_FPUFC */
#define _FPUEH_IFS_FPUFC_MASK       0x4UL                           /**< Bit mask for FPUEH_FPUFC */
#define _FPUEH_IFS_FPUFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPUFC_DEFAULT     (_FPUEH_IFS_FPUFC_DEFAULT << 2) /**< Shifted mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPOFC             (0x1UL << 3)                    /**< Set FPOFC Interrupt Flag */
#define _FPUEH_IFS_FPOFC_SHIFT      3                               /**< Shift value for FPUEH_FPOFC */
#define _FPUEH_IFS_FPOFC_MASK       0x8UL                           /**< Bit mask for FPUEH_FPOFC */
#define _FPUEH_IFS_FPOFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPOFC_DEFAULT     (_FPUEH_IFS_FPOFC_DEFAULT << 3) /**< Shifted mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPIDC             (0x1UL << 4)                    /**< Set FPIDC Interrupt Flag */
#define _FPUEH_IFS_FPIDC_SHIFT      4                               /**< Shift value for FPUEH_FPIDC */
#define _FPUEH_IFS_FPIDC_MASK       0x10UL                          /**< Bit mask for FPUEH_FPIDC */
#define _FPUEH_IFS_FPIDC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPIDC_DEFAULT     (_FPUEH_IFS_FPIDC_DEFAULT << 4) /**< Shifted mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPIXC             (0x1UL << 5)                    /**< Set FPIXC Interrupt Flag */
#define _FPUEH_IFS_FPIXC_SHIFT      5                               /**< Shift value for FPUEH_FPIXC */
#define _FPUEH_IFS_FPIXC_MASK       0x20UL                          /**< Bit mask for FPUEH_FPIXC */
#define _FPUEH_IFS_FPIXC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFS */
#define FPUEH_IFS_FPIXC_DEFAULT     (_FPUEH_IFS_FPIXC_DEFAULT << 5) /**< Shifted mode DEFAULT for FPUEH_IFS */

/* Bit fields for FPUEH IFC */
#define _FPUEH_IFC_RESETVALUE       0x00000000UL                    /**< Default value for FPUEH_IFC */
#define _FPUEH_IFC_MASK             0x0000003FUL                    /**< Mask for FPUEH_IFC */
#define FPUEH_IFC_FPIOC             (0x1UL << 0)                    /**< Clear FPIOC Interrupt Flag */
#define _FPUEH_IFC_FPIOC_SHIFT      0                               /**< Shift value for FPUEH_FPIOC */
#define _FPUEH_IFC_FPIOC_MASK       0x1UL                           /**< Bit mask for FPUEH_FPIOC */
#define _FPUEH_IFC_FPIOC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPIOC_DEFAULT     (_FPUEH_IFC_FPIOC_DEFAULT << 0) /**< Shifted mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPDZC             (0x1UL << 1)                    /**< Clear FPDZC Interrupt Flag */
#define _FPUEH_IFC_FPDZC_SHIFT      1                               /**< Shift value for FPUEH_FPDZC */
#define _FPUEH_IFC_FPDZC_MASK       0x2UL                           /**< Bit mask for FPUEH_FPDZC */
#define _FPUEH_IFC_FPDZC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPDZC_DEFAULT     (_FPUEH_IFC_FPDZC_DEFAULT << 1) /**< Shifted mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPUFC             (0x1UL << 2)                    /**< Clear FPUFC Interrupt Flag */
#define _FPUEH_IFC_FPUFC_SHIFT      2                               /**< Shift value for FPUEH_FPUFC */
#define _FPUEH_IFC_FPUFC_MASK       0x4UL                           /**< Bit mask for FPUEH_FPUFC */
#define _FPUEH_IFC_FPUFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPUFC_DEFAULT     (_FPUEH_IFC_FPUFC_DEFAULT << 2) /**< Shifted mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPOFC             (0x1UL << 3)                    /**< Clear FPOFC Interrupt Flag */
#define _FPUEH_IFC_FPOFC_SHIFT      3                               /**< Shift value for FPUEH_FPOFC */
#define _FPUEH_IFC_FPOFC_MASK       0x8UL                           /**< Bit mask for FPUEH_FPOFC */
#define _FPUEH_IFC_FPOFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPOFC_DEFAULT     (_FPUEH_IFC_FPOFC_DEFAULT << 3) /**< Shifted mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPIDC             (0x1UL << 4)                    /**< Clear FPIDC Interrupt Flag */
#define _FPUEH_IFC_FPIDC_SHIFT      4                               /**< Shift value for FPUEH_FPIDC */
#define _FPUEH_IFC_FPIDC_MASK       0x10UL                          /**< Bit mask for FPUEH_FPIDC */
#define _FPUEH_IFC_FPIDC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPIDC_DEFAULT     (_FPUEH_IFC_FPIDC_DEFAULT << 4) /**< Shifted mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPIXC             (0x1UL << 5)                    /**< Clear FPIXC Interrupt Flag */
#define _FPUEH_IFC_FPIXC_SHIFT      5                               /**< Shift value for FPUEH_FPIXC */
#define _FPUEH_IFC_FPIXC_MASK       0x20UL                          /**< Bit mask for FPUEH_FPIXC */
#define _FPUEH_IFC_FPIXC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IFC */
#define FPUEH_IFC_FPIXC_DEFAULT     (_FPUEH_IFC_FPIXC_DEFAULT << 5) /**< Shifted mode DEFAULT for FPUEH_IFC */

/* Bit fields for FPUEH IEN */
#define _FPUEH_IEN_RESETVALUE       0x00000000UL                    /**< Default value for FPUEH_IEN */
#define _FPUEH_IEN_MASK             0x0000003FUL                    /**< Mask for FPUEH_IEN */
#define FPUEH_IEN_FPIOC             (0x1UL << 0)                    /**< FPIOC Interrupt Enable */
#define _FPUEH_IEN_FPIOC_SHIFT      0                               /**< Shift value for FPUEH_FPIOC */
#define _FPUEH_IEN_FPIOC_MASK       0x1UL                           /**< Bit mask for FPUEH_FPIOC */
#define _FPUEH_IEN_FPIOC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPIOC_DEFAULT     (_FPUEH_IEN_FPIOC_DEFAULT << 0) /**< Shifted mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPDZC             (0x1UL << 1)                    /**< FPDZC Interrupt Enable */
#define _FPUEH_IEN_FPDZC_SHIFT      1                               /**< Shift value for FPUEH_FPDZC */
#define _FPUEH_IEN_FPDZC_MASK       0x2UL                           /**< Bit mask for FPUEH_FPDZC */
#define _FPUEH_IEN_FPDZC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPDZC_DEFAULT     (_FPUEH_IEN_FPDZC_DEFAULT << 1) /**< Shifted mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPUFC             (0x1UL << 2)                    /**< FPUFC Interrupt Enable */
#define _FPUEH_IEN_FPUFC_SHIFT      2                               /**< Shift value for FPUEH_FPUFC */
#define _FPUEH_IEN_FPUFC_MASK       0x4UL                           /**< Bit mask for FPUEH_FPUFC */
#define _FPUEH_IEN_FPUFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPUFC_DEFAULT     (_FPUEH_IEN_FPUFC_DEFAULT << 2) /**< Shifted mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPOFC             (0x1UL << 3)                    /**< FPOFC Interrupt Enable */
#define _FPUEH_IEN_FPOFC_SHIFT      3                               /**< Shift value for FPUEH_FPOFC */
#define _FPUEH_IEN_FPOFC_MASK       0x8UL                           /**< Bit mask for FPUEH_FPOFC */
#define _FPUEH_IEN_FPOFC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPOFC_DEFAULT     (_FPUEH_IEN_FPOFC_DEFAULT << 3) /**< Shifted mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPIDC             (0x1UL << 4)                    /**< FPIDC Interrupt Enable */
#define _FPUEH_IEN_FPIDC_SHIFT      4                               /**< Shift value for FPUEH_FPIDC */
#define _FPUEH_IEN_FPIDC_MASK       0x10UL                          /**< Bit mask for FPUEH_FPIDC */
#define _FPUEH_IEN_FPIDC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPIDC_DEFAULT     (_FPUEH_IEN_FPIDC_DEFAULT << 4) /**< Shifted mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPIXC             (0x1UL << 5)                    /**< FPIXC Interrupt Enable */
#define _FPUEH_IEN_FPIXC_SHIFT      5                               /**< Shift value for FPUEH_FPIXC */
#define _FPUEH_IEN_FPIXC_MASK       0x20UL                          /**< Bit mask for FPUEH_FPIXC */
#define _FPUEH_IEN_FPIXC_DEFAULT    0x00000000UL                    /**< Mode DEFAULT for FPUEH_IEN */
#define FPUEH_IEN_FPIXC_DEFAULT     (_FPUEH_IEN_FPIXC_DEFAULT << 5) /**< Shifted mode DEFAULT for FPUEH_IEN */

/** @} */
/** @} End of group EFR32MG12P_FPUEH */
/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

