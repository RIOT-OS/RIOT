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


#ifndef EM_DEVICE_H
#define EM_DEVICE_H

#if defined(EFM32GG230F1024)
#include "efm32gg230f1024.h"

#elif defined(EFM32GG230F512)
#include "efm32gg230f512.h"

#elif defined(EFM32GG232F1024)
#include "efm32gg232f1024.h"

#elif defined(EFM32GG232F512)
#include "efm32gg232f512.h"

#elif defined(EFM32GG280F1024)
#include "efm32gg280f1024.h"

#elif defined(EFM32GG280F512)
#include "efm32gg280f512.h"

#elif defined(EFM32GG290F1024)
#include "efm32gg290f1024.h"

#elif defined(EFM32GG290F512)
#include "efm32gg290f512.h"

#elif defined(EFM32GG295F1024)
#include "efm32gg295f1024.h"

#elif defined(EFM32GG295F512)
#include "efm32gg295f512.h"

#elif defined(EFM32GG330F1024)
#include "efm32gg330f1024.h"

#elif defined(EFM32GG330F512)
#include "efm32gg330f512.h"

#elif defined(EFM32GG332F1024)
#include "efm32gg332f1024.h"

#elif defined(EFM32GG332F512)
#include "efm32gg332f512.h"

#elif defined(EFM32GG380F1024)
#include "efm32gg380f1024.h"

#elif defined(EFM32GG380F512)
#include "efm32gg380f512.h"

#elif defined(EFM32GG390F1024)
#include "efm32gg390f1024.h"

#elif defined(EFM32GG390F512)
#include "efm32gg390f512.h"

#elif defined(EFM32GG395F1024)
#include "efm32gg395f1024.h"

#elif defined(EFM32GG395F512)
#include "efm32gg395f512.h"

#elif defined(EFM32GG840F1024)
#include "efm32gg840f1024.h"

#elif defined(EFM32GG840F512)
#include "efm32gg840f512.h"

#elif defined(EFM32GG842F1024)
#include "efm32gg842f1024.h"

#elif defined(EFM32GG842F512)
#include "efm32gg842f512.h"

#elif defined(EFM32GG880F1024)
#include "efm32gg880f1024.h"

#elif defined(EFM32GG880F512)
#include "efm32gg880f512.h"

#elif defined(EFM32GG890F1024)
#include "efm32gg890f1024.h"

#elif defined(EFM32GG890F512)
#include "efm32gg890f512.h"

#elif defined(EFM32GG895F1024)
#include "efm32gg895f1024.h"

#elif defined(EFM32GG895F512)
#include "efm32gg895f512.h"

#elif defined(EFM32GG900F1024)
#include "efm32gg900f1024.h"

#elif defined(EFM32GG900F512)
#include "efm32gg900f512.h"

#elif defined(EFM32GG940F1024)
#include "efm32gg940f1024.h"

#elif defined(EFM32GG940F512)
#include "efm32gg940f512.h"

#elif defined(EFM32GG942F1024)
#include "efm32gg942f1024.h"

#elif defined(EFM32GG942F512)
#include "efm32gg942f512.h"

#elif defined(EFM32GG980F1024)
#include "efm32gg980f1024.h"

#elif defined(EFM32GG980F512)
#include "efm32gg980f512.h"

#elif defined(EFM32GG990F1024)
#include "efm32gg990f1024.h"

#elif defined(EFM32GG990F512)
#include "efm32gg990f512.h"

#elif defined(EFM32GG995F1024)
#include "efm32gg995f1024.h"

#elif defined(EFM32GG995F512)
#include "efm32gg995f512.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

