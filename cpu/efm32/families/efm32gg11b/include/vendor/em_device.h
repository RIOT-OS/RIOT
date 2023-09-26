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

#if defined(EFM32GG11B110F2048GM64)
#include "efm32gg11b110f2048gm64.h"

#elif defined(EFM32GG11B110F2048GQ64)
#include "efm32gg11b110f2048gq64.h"

#elif defined(EFM32GG11B110F2048IM64)
#include "efm32gg11b110f2048im64.h"

#elif defined(EFM32GG11B110F2048IQ64)
#include "efm32gg11b110f2048iq64.h"

#elif defined(EFM32GG11B120F2048GM64)
#include "efm32gg11b120f2048gm64.h"

#elif defined(EFM32GG11B120F2048GQ64)
#include "efm32gg11b120f2048gq64.h"

#elif defined(EFM32GG11B120F2048IM64)
#include "efm32gg11b120f2048im64.h"

#elif defined(EFM32GG11B120F2048IQ64)
#include "efm32gg11b120f2048iq64.h"

#elif defined(EFM32GG11B310F2048GL112)
#include "efm32gg11b310f2048gl112.h"

#elif defined(EFM32GG11B310F2048GQ100)
#include "efm32gg11b310f2048gq100.h"

#elif defined(EFM32GG11B320F2048GL112)
#include "efm32gg11b320f2048gl112.h"

#elif defined(EFM32GG11B320F2048GQ100)
#include "efm32gg11b320f2048gq100.h"

#elif defined(EFM32GG11B420F2048GL112)
#include "efm32gg11b420f2048gl112.h"

#elif defined(EFM32GG11B420F2048GL120)
#include "efm32gg11b420f2048gl120.h"

#elif defined(EFM32GG11B420F2048GM64)
#include "efm32gg11b420f2048gm64.h"

#elif defined(EFM32GG11B420F2048GQ100)
#include "efm32gg11b420f2048gq100.h"

#elif defined(EFM32GG11B420F2048GQ64)
#include "efm32gg11b420f2048gq64.h"

#elif defined(EFM32GG11B420F2048IL112)
#include "efm32gg11b420f2048il112.h"

#elif defined(EFM32GG11B420F2048IL120)
#include "efm32gg11b420f2048il120.h"

#elif defined(EFM32GG11B420F2048IM64)
#include "efm32gg11b420f2048im64.h"

#elif defined(EFM32GG11B420F2048IQ100)
#include "efm32gg11b420f2048iq100.h"

#elif defined(EFM32GG11B420F2048IQ64)
#include "efm32gg11b420f2048iq64.h"

#elif defined(EFM32GG11B510F2048GL120)
#include "efm32gg11b510f2048gl120.h"

#elif defined(EFM32GG11B510F2048GM64)
#include "efm32gg11b510f2048gm64.h"

#elif defined(EFM32GG11B510F2048GQ100)
#include "efm32gg11b510f2048gq100.h"

#elif defined(EFM32GG11B510F2048GQ64)
#include "efm32gg11b510f2048gq64.h"

#elif defined(EFM32GG11B510F2048IL120)
#include "efm32gg11b510f2048il120.h"

#elif defined(EFM32GG11B510F2048IM64)
#include "efm32gg11b510f2048im64.h"

#elif defined(EFM32GG11B510F2048IQ100)
#include "efm32gg11b510f2048iq100.h"

#elif defined(EFM32GG11B510F2048IQ64)
#include "efm32gg11b510f2048iq64.h"

#elif defined(EFM32GG11B520F2048GL120)
#include "efm32gg11b520f2048gl120.h"

#elif defined(EFM32GG11B520F2048GM64)
#include "efm32gg11b520f2048gm64.h"

#elif defined(EFM32GG11B520F2048GQ100)
#include "efm32gg11b520f2048gq100.h"

#elif defined(EFM32GG11B520F2048GQ64)
#include "efm32gg11b520f2048gq64.h"

#elif defined(EFM32GG11B520F2048IL120)
#include "efm32gg11b520f2048il120.h"

#elif defined(EFM32GG11B520F2048IM64)
#include "efm32gg11b520f2048im64.h"

#elif defined(EFM32GG11B520F2048IQ100)
#include "efm32gg11b520f2048iq100.h"

#elif defined(EFM32GG11B520F2048IQ64)
#include "efm32gg11b520f2048iq64.h"

#elif defined(EFM32GG11B820F2048GL120)
#include "efm32gg11b820f2048gl120.h"

#elif defined(EFM32GG11B820F2048GL152)
#include "efm32gg11b820f2048gl152.h"

#elif defined(EFM32GG11B820F2048GL192)
#include "efm32gg11b820f2048gl192.h"

#elif defined(EFM32GG11B820F2048GM64)
#include "efm32gg11b820f2048gm64.h"

#elif defined(EFM32GG11B820F2048GQ100)
#include "efm32gg11b820f2048gq100.h"

#elif defined(EFM32GG11B820F2048GQ64)
#include "efm32gg11b820f2048gq64.h"

#elif defined(EFM32GG11B820F2048IL120)
#include "efm32gg11b820f2048il120.h"

#elif defined(EFM32GG11B820F2048IL152)
#include "efm32gg11b820f2048il152.h"

#elif defined(EFM32GG11B820F2048IM64)
#include "efm32gg11b820f2048im64.h"

#elif defined(EFM32GG11B820F2048IQ100)
#include "efm32gg11b820f2048iq100.h"

#elif defined(EFM32GG11B820F2048IQ64)
#include "efm32gg11b820f2048iq64.h"

#elif defined(EFM32GG11B840F1024GL120)
#include "efm32gg11b840f1024gl120.h"

#elif defined(EFM32GG11B840F1024GL152)
#include "efm32gg11b840f1024gl152.h"

#elif defined(EFM32GG11B840F1024GL192)
#include "efm32gg11b840f1024gl192.h"

#elif defined(EFM32GG11B840F1024GM64)
#include "efm32gg11b840f1024gm64.h"

#elif defined(EFM32GG11B840F1024GQ100)
#include "efm32gg11b840f1024gq100.h"

#elif defined(EFM32GG11B840F1024GQ64)
#include "efm32gg11b840f1024gq64.h"

#elif defined(EFM32GG11B840F1024IL120)
#include "efm32gg11b840f1024il120.h"

#elif defined(EFM32GG11B840F1024IL152)
#include "efm32gg11b840f1024il152.h"

#elif defined(EFM32GG11B840F1024IM64)
#include "efm32gg11b840f1024im64.h"

#elif defined(EFM32GG11B840F1024IQ100)
#include "efm32gg11b840f1024iq100.h"

#elif defined(EFM32GG11B840F1024IQ64)
#include "efm32gg11b840f1024iq64.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

