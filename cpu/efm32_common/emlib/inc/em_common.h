/***************************************************************************//**
 * @file em_common.h
 * @brief General purpose utilities.
 * @version 4.3.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
#ifndef EM_COMMON_H
#define EM_COMMON_H

#include "em_device.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @defgroup emlib EMLIB
 * @brief Low-level peripheral library
 * @details
 *  EMLIB is a low-level peripheral support library that provides a unified API for
 *  all EFM32, EZR32 and EFR32 MCUs and SoCs from Silicon Laboratories.
 *
 *  EMLIB modules are provided for all peripherals and core features. The library
 *  implements no interrupt handlers. Static data, critical sections and module
 *  interdependencies are kept at a minimum.
 *
 * @note
 *  EMLIB functions assert on error if DEBUG_EFM is defined. See @ref em_assert.h
 *  for more information on error handling and assertions.
 * @n @n
 *  EMLIB does not implement support for radio features. Please refer to stack
 *  documentation for more information on RF support.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup COMMON
 * @brief General purpose utilities.
 * @details
 *  General purpose utilities for polyfilling compiler support etc.
 * @{
 ******************************************************************************/

#if !defined(__GNUC__)
/* Not GCC compilers */

/** Macro for getting minimum value. */
#define SL_MIN(a, b) ((a) < (b) ? (a) : (b))

/** Macro for getting maximum value. */
#define SL_MAX(a, b) ((a) > (b) ? (a) : (b))

/** Macros for handling packed structs. */
#define STRINGIZE(X) #X
#define SL_PACK_START(X) _Pragma(STRINGIZE(pack(X)))
#define SL_PACK_END()    _Pragma("pack()")
#define SL_ATTRIBUTE_PACKED

#if defined(__CC_ARM)
/** MDK-ARM compiler: Macros for handling aligned structs. */
#define SL_ALIGN(X) __align(X)
#endif

#if defined(__ICCARM__)
/** IAR Embedded Workbench: Macros for handling aligned structs. */
#define SL_ALIGN(X) _Pragma(STRINGIZE(data_alignment=X))
#endif

#define SL_ATTRIBUTE_ALIGN(X)

#else // !defined(__GNUC__)
/* GCC compilers */

/** Macro for getting minimum value. No sideeffects, a and b are evaluated once only. */
#define SL_MIN(a, b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); _a < _b ? _a : _b;})

/** Macro for getting maximum value. No sideeffects, a and b are evaluated once only. */
#define SL_MAX(a, b) ({__typeof__(a) _a = (a); __typeof__(b) _b = (b); _a > _b ? _a : _b;})

/** GCC style macro for handling packed structs. */
#define SL_ATTRIBUTE_PACKED __attribute__ ((packed))

/** Macro for handling packed structs.
 *  @n Use this macro before the struct definition.
 *  @n X denotes the maximum alignment of struct members. X is not supported with
 *  GCC. GCC always use 1 byte maximum alignment.
 */
#define SL_PACK_START(x)

/** Macro for handling packed structs.
 *  @n Use this macro after the struct definition.
 *  @n With GCC, add SL_ATTRIBUTE_PACKED after the closing } of the struct
 *  definition.
 */
#define SL_PACK_END()

/** GCC style macro for aligning a variable. */
#define SL_ATTRIBUTE_ALIGN(X) __attribute__ ((aligned(X)))

/** Macro for aligning a variable.
 *  @n Use this macro before the variable definition.
 *  @n X denotes the storage alignment value in bytes.
 *  @n To be gcc compatibele use SL_ATTRIBUTE_ALIGN(X) before the ; on normal
 *  variables. Use SL_ATTRIBUTE_ALIGN(X) before the opening { on struct variables.
 */
#define SL_ALIGN(X)

#endif // !defined(__GNUC__)

/***************************************************************************//**
 * @brief
 *   Count trailing number of zero's. Use CLZ instruction if available.
 *
 * @param[in] value
 *   Data value to check for number of trailing zero bits.
 *
 * @return
 *   Number of trailing zero's in value.
 ******************************************************************************/
__STATIC_INLINE uint32_t SL_CTZ(uint32_t value)
{
#if (__CORTEX_M >= 3)
  return __CLZ(__RBIT(value));

#else
  uint32_t zeros;
  for(zeros=0; (zeros<32) && ((value&0x1) == 0); zeros++, value>>=1);
  return zeros;
#endif
}

/** @deprecated New code should use @ref SL_MIN(). */
#define EFM32_MIN(a, b)     SL_MIN(a, b)
/** @deprecated New code should use @ref SL_MAX(). */
#define EFM32_MAX(a, b)     SL_MAX(a, b)
/** @deprecated New code should use @ref SL_PACK_START(). */
#define EFM32_PACK_START(X) SL_PACK_START(X)
/** @deprecated New code should use @ref SL_PACK_END(). */
#define EFM32_PACK_END()    SL_PACK_END()
/** @deprecated New code should use @ref SL_ALIGN(). */
#define EFM32_ALIGN(X)      SL_ALIGN(X)

/***************************************************************************//**
 * @brief
 *   Count trailing number of zero's. Use CLZ instruction if available.
 *
 * @deprecated
 *   Deprecated function. New code should use @ref SL_CTZ().

 * @param[in] value
 *   Data value to check for number of trailing zero bits.
 *
 * @return
 *   Number of trailing zero's in value.
 ******************************************************************************/
__STATIC_INLINE uint32_t EFM32_CTZ(uint32_t value)
{
#if (__CORTEX_M >= 3)
  return __CLZ(__RBIT(value));

#else
  uint32_t zeros;
  for(zeros=0; (zeros<32) && ((value&0x1) == 0); zeros++, value>>=1);
  return zeros;
#endif
}

/** @} (end addtogroup COMMON) */
/** @} (end addtogroup emlib) */

#ifdef __cplusplus
}
#endif

#endif /* EM_COMMON_H */
