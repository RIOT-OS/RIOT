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

#if defined(EFM32GG12B110F1024GM64)
#include "efm32gg12b110f1024gm64.h"

#elif defined(EFM32GG12B110F1024GQ64)
#include "efm32gg12b110f1024gq64.h"

#elif defined(EFM32GG12B110F1024IM64)
#include "efm32gg12b110f1024im64.h"

#elif defined(EFM32GG12B110F1024IQ64)
#include "efm32gg12b110f1024iq64.h"

#elif defined(EFM32GG12B130F512GM64)
#include "efm32gg12b130f512gm64.h"

#elif defined(EFM32GG12B130F512GQ64)
#include "efm32gg12b130f512gq64.h"

#elif defined(EFM32GG12B130F512IM64)
#include "efm32gg12b130f512im64.h"

#elif defined(EFM32GG12B130F512IQ64)
#include "efm32gg12b130f512iq64.h"

#elif defined(EFM32GG12B310F1024GL112)
#include "efm32gg12b310f1024gl112.h"

#elif defined(EFM32GG12B310F1024GQ100)
#include "efm32gg12b310f1024gq100.h"

#elif defined(EFM32GG12B330F512GL112)
#include "efm32gg12b330f512gl112.h"

#elif defined(EFM32GG12B330F512GQ100)
#include "efm32gg12b330f512gq100.h"

#elif defined(EFM32GG12B390F1024GL112)
#include "efm32gg12b390f1024gl112.h"

#elif defined(EFM32GG12B390F512GL112)
#include "efm32gg12b390f512gl112.h"

#elif defined(EFM32GG12B410F1024GL112)
#include "efm32gg12b410f1024gl112.h"

#elif defined(EFM32GG12B410F1024GL120)
#include "efm32gg12b410f1024gl120.h"

#elif defined(EFM32GG12B410F1024GM64)
#include "efm32gg12b410f1024gm64.h"

#elif defined(EFM32GG12B410F1024GQ100)
#include "efm32gg12b410f1024gq100.h"

#elif defined(EFM32GG12B410F1024GQ64)
#include "efm32gg12b410f1024gq64.h"

#elif defined(EFM32GG12B410F1024IL112)
#include "efm32gg12b410f1024il112.h"

#elif defined(EFM32GG12B410F1024IL120)
#include "efm32gg12b410f1024il120.h"

#elif defined(EFM32GG12B410F1024IM64)
#include "efm32gg12b410f1024im64.h"

#elif defined(EFM32GG12B410F1024IQ100)
#include "efm32gg12b410f1024iq100.h"

#elif defined(EFM32GG12B410F1024IQ64)
#include "efm32gg12b410f1024iq64.h"

#elif defined(EFM32GG12B430F512GL112)
#include "efm32gg12b430f512gl112.h"

#elif defined(EFM32GG12B430F512GL120)
#include "efm32gg12b430f512gl120.h"

#elif defined(EFM32GG12B430F512GM64)
#include "efm32gg12b430f512gm64.h"

#elif defined(EFM32GG12B430F512GQ100)
#include "efm32gg12b430f512gq100.h"

#elif defined(EFM32GG12B430F512GQ64)
#include "efm32gg12b430f512gq64.h"

#elif defined(EFM32GG12B430F512IL112)
#include "efm32gg12b430f512il112.h"

#elif defined(EFM32GG12B430F512IL120)
#include "efm32gg12b430f512il120.h"

#elif defined(EFM32GG12B430F512IM64)
#include "efm32gg12b430f512im64.h"

#elif defined(EFM32GG12B430F512IQ100)
#include "efm32gg12b430f512iq100.h"

#elif defined(EFM32GG12B430F512IQ64)
#include "efm32gg12b430f512iq64.h"

#elif defined(EFM32GG12B510F1024GL112)
#include "efm32gg12b510f1024gl112.h"

#elif defined(EFM32GG12B510F1024GL120)
#include "efm32gg12b510f1024gl120.h"

#elif defined(EFM32GG12B510F1024GM64)
#include "efm32gg12b510f1024gm64.h"

#elif defined(EFM32GG12B510F1024GQ100)
#include "efm32gg12b510f1024gq100.h"

#elif defined(EFM32GG12B510F1024GQ64)
#include "efm32gg12b510f1024gq64.h"

#elif defined(EFM32GG12B510F1024IL112)
#include "efm32gg12b510f1024il112.h"

#elif defined(EFM32GG12B510F1024IL120)
#include "efm32gg12b510f1024il120.h"

#elif defined(EFM32GG12B510F1024IM64)
#include "efm32gg12b510f1024im64.h"

#elif defined(EFM32GG12B510F1024IQ100)
#include "efm32gg12b510f1024iq100.h"

#elif defined(EFM32GG12B510F1024IQ64)
#include "efm32gg12b510f1024iq64.h"

#elif defined(EFM32GG12B530F512GL112)
#include "efm32gg12b530f512gl112.h"

#elif defined(EFM32GG12B530F512GL120)
#include "efm32gg12b530f512gl120.h"

#elif defined(EFM32GG12B530F512GM64)
#include "efm32gg12b530f512gm64.h"

#elif defined(EFM32GG12B530F512GQ100)
#include "efm32gg12b530f512gq100.h"

#elif defined(EFM32GG12B530F512GQ64)
#include "efm32gg12b530f512gq64.h"

#elif defined(EFM32GG12B530F512IL112)
#include "efm32gg12b530f512il112.h"

#elif defined(EFM32GG12B530F512IL120)
#include "efm32gg12b530f512il120.h"

#elif defined(EFM32GG12B530F512IM64)
#include "efm32gg12b530f512im64.h"

#elif defined(EFM32GG12B530F512IQ100)
#include "efm32gg12b530f512iq100.h"

#elif defined(EFM32GG12B530F512IQ64)
#include "efm32gg12b530f512iq64.h"

#elif defined(EFM32GG12B810F1024GL112)
#include "efm32gg12b810f1024gl112.h"

#elif defined(EFM32GG12B810F1024GL120)
#include "efm32gg12b810f1024gl120.h"

#elif defined(EFM32GG12B810F1024GM64)
#include "efm32gg12b810f1024gm64.h"

#elif defined(EFM32GG12B810F1024GQ100)
#include "efm32gg12b810f1024gq100.h"

#elif defined(EFM32GG12B810F1024GQ64)
#include "efm32gg12b810f1024gq64.h"

#elif defined(EFM32GG12B810F1024IL112)
#include "efm32gg12b810f1024il112.h"

#elif defined(EFM32GG12B810F1024IL120)
#include "efm32gg12b810f1024il120.h"

#elif defined(EFM32GG12B810F1024IM64)
#include "efm32gg12b810f1024im64.h"

#elif defined(EFM32GG12B810F1024IQ100)
#include "efm32gg12b810f1024iq100.h"

#elif defined(EFM32GG12B810F1024IQ64)
#include "efm32gg12b810f1024iq64.h"

#elif defined(EFM32GG12B830F512GL112)
#include "efm32gg12b830f512gl112.h"

#elif defined(EFM32GG12B830F512GL120)
#include "efm32gg12b830f512gl120.h"

#elif defined(EFM32GG12B830F512GM64)
#include "efm32gg12b830f512gm64.h"

#elif defined(EFM32GG12B830F512GQ100)
#include "efm32gg12b830f512gq100.h"

#elif defined(EFM32GG12B830F512GQ64)
#include "efm32gg12b830f512gq64.h"

#elif defined(EFM32GG12B830F512IL112)
#include "efm32gg12b830f512il112.h"

#elif defined(EFM32GG12B830F512IL120)
#include "efm32gg12b830f512il120.h"

#elif defined(EFM32GG12B830F512IM64)
#include "efm32gg12b830f512im64.h"

#elif defined(EFM32GG12B830F512IQ100)
#include "efm32gg12b830f512iq100.h"

#elif defined(EFM32GG12B830F512IQ64)
#include "efm32gg12b830f512iq64.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

