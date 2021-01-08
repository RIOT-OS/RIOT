/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M Peripheral Access Layer for Silicon Laboratories
 *        microcontroller devices
 *
 * This is a convenience header file for defining the part number on the
 * build command line, instead of specifying the part specific header file.
 *
 * @verbatim
 * Example: Add "-DEFM32G890F128" to your build options, to define part
 *          Add "#include "em_device.h" to your source files
 * @endverbatim
 *
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


#ifndef EM_DEVICE_H
#define EM_DEVICE_H

#if defined(EFM32HG108F32)
#include "efm32hg108f32.h"

#elif defined(EFM32HG108F64)
#include "efm32hg108f64.h"

#elif defined(EFM32HG110F32)
#include "efm32hg110f32.h"

#elif defined(EFM32HG110F64)
#include "efm32hg110f64.h"

#elif defined(EFM32HG210F32)
#include "efm32hg210f32.h"

#elif defined(EFM32HG210F64)
#include "efm32hg210f64.h"

#elif defined(EFM32HG222F32)
#include "efm32hg222f32.h"

#elif defined(EFM32HG222F64)
#include "efm32hg222f64.h"

#elif defined(EFM32HG308F32)
#include "efm32hg308f32.h"

#elif defined(EFM32HG308F64)
#include "efm32hg308f64.h"

#elif defined(EFM32HG309F32)
#include "efm32hg309f32.h"

#elif defined(EFM32HG309F64)
#include "efm32hg309f64.h"

#elif defined(EFM32HG310F32)
#include "efm32hg310f32.h"

#elif defined(EFM32HG310F64)
#include "efm32hg310f64.h"

#elif defined(EFM32HG321F32)
#include "efm32hg321f32.h"

#elif defined(EFM32HG321F64)
#include "efm32hg321f64.h"

#elif defined(EFM32HG322F32)
#include "efm32hg322f32.h"

#elif defined(EFM32HG322F64)
#include "efm32hg322f64.h"

#elif defined(EFM32HG350F32)
#include "efm32hg350f32.h"

#elif defined(EFM32HG350F64)
#include "efm32hg350f64.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

