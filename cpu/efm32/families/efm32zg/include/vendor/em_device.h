/**************************************************************************//**
 * @file em_device.h
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files

 *
 * @endverbatim
 * @version 5.4.0
 ******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


#ifndef EM_DEVICE_H
#define EM_DEVICE_H

#if defined(EFM32ZG108F16)
#include "efm32zg108f16.h"

#elif defined(EFM32ZG108F32)
#include "efm32zg108f32.h"

#elif defined(EFM32ZG108F4)
#include "efm32zg108f4.h"

#elif defined(EFM32ZG108F8)
#include "efm32zg108f8.h"

#elif defined(EFM32ZG110F16)
#include "efm32zg110f16.h"

#elif defined(EFM32ZG110F32)
#include "efm32zg110f32.h"

#elif defined(EFM32ZG110F4)
#include "efm32zg110f4.h"

#elif defined(EFM32ZG110F8)
#include "efm32zg110f8.h"

#elif defined(EFM32ZG210F16)
#include "efm32zg210f16.h"

#elif defined(EFM32ZG210F32)
#include "efm32zg210f32.h"

#elif defined(EFM32ZG210F4)
#include "efm32zg210f4.h"

#elif defined(EFM32ZG210F8)
#include "efm32zg210f8.h"

#elif defined(EFM32ZG222F16)
#include "efm32zg222f16.h"

#elif defined(EFM32ZG222F32)
#include "efm32zg222f32.h"

#elif defined(EFM32ZG222F4)
#include "efm32zg222f4.h"

#elif defined(EFM32ZG222F8)
#include "efm32zg222f8.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

