/***************************************************************************//**
 * @file
 * @brief EFM32PG12B_VDAC_OPA register and bit field definitions
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
 * @brief VDAC_OPA VDAC OPA Register
 * @ingroup EFM32PG12B_VDAC
 ******************************************************************************/
typedef struct {
  __IM uint32_t  APORTREQ;      /**< Operational Amplifier APORT Request Status Register  */
  __IM uint32_t  APORTCONFLICT; /**< Operational Amplifier APORT Conflict Status Register  */
  __IOM uint32_t CTRL;          /**< Operational Amplifier Control Register  */
  __IOM uint32_t TIMER;         /**< Operational Amplifier Timer Control Register  */
  __IOM uint32_t MUX;           /**< Operational Amplifier Mux Configuration Register  */
  __IOM uint32_t OUT;           /**< Operational Amplifier Output Configuration Register  */
  __IOM uint32_t CAL;           /**< Operational Amplifier Calibration Register  */
  uint32_t       RESERVED0[1U]; /**< Reserved future */
} VDAC_OPA_TypeDef;

/** @} End of group Parts */
#ifdef __cplusplus
}
#endif

