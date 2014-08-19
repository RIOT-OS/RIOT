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
 * @endverbatim
 * @version 3.20.6
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Laboratories, Inc. http://www.silabs.com</b>
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

#ifndef __EM_DEVICE_H
#define __EM_DEVICE_H


#if defined(EFM32TG108F16)
#include "efm32tg108f16.h"

#elif defined(EFM32TG108F32)
#include "efm32tg108f32.h"

#elif defined(EFM32TG108F4)
#include "efm32tg108f4.h"

#elif defined(EFM32TG108F8)
#include "efm32tg108f8.h"

#elif defined(EFM32TG110F16)
#include "efm32tg110f16.h"

#elif defined(EFM32TG110F32)
#include "efm32tg110f32.h"

#elif defined(EFM32TG110F4)
#include "efm32tg110f4.h"

#elif defined(EFM32TG110F8)
#include "efm32tg110f8.h"

#elif defined(EFM32TG210F16)
#include "efm32tg210f16.h"

#elif defined(EFM32TG210F32)
#include "efm32tg210f32.h"

#elif defined(EFM32TG210F8)
#include "efm32tg210f8.h"

#elif defined(EFM32TG222F16)
#include "efm32tg222f16.h"

#elif defined(EFM32TG222F32)
#include "efm32tg222f32.h"

#elif defined(EFM32TG222F8)
#include "efm32tg222f8.h"

#elif defined(EFM32TG225F16)
#include "efm32tg225f16.h"

#elif defined(EFM32TG225F32)
#include "efm32tg225f32.h"

#elif defined(EFM32TG225F8)
#include "efm32tg225f8.h"

#elif defined(EFM32TG230F16)
#include "efm32tg230f16.h"

#elif defined(EFM32TG230F32)
#include "efm32tg230f32.h"

#elif defined(EFM32TG230F8)
#include "efm32tg230f8.h"

#elif defined(EFM32TG232F16)
#include "efm32tg232f16.h"

#elif defined(EFM32TG232F32)
#include "efm32tg232f32.h"

#elif defined(EFM32TG232F8)
#include "efm32tg232f8.h"

#elif defined(EFM32TG822F16)
#include "efm32tg822f16.h"

#elif defined(EFM32TG822F32)
#include "efm32tg822f32.h"

#elif defined(EFM32TG822F8)
#include "efm32tg822f8.h"

#elif defined(EFM32TG825F16)
#include "efm32tg825f16.h"

#elif defined(EFM32TG825F32)
#include "efm32tg825f32.h"

#elif defined(EFM32TG825F8)
#include "efm32tg825f8.h"

#elif defined(EFM32TG840F16)
#include "efm32tg840f16.h"

#elif defined(EFM32TG840F32)
#include "efm32tg840f32.h"

#elif defined(EFM32TG840F8)
#include "efm32tg840f8.h"

#elif defined(EFM32TG842F16)
#include "efm32tg842f16.h"

#elif defined(EFM32TG842F32)
#include "efm32tg842f32.h"

#elif defined(EFM32TG842F8)
#include "efm32tg842f8.h"


#else
#error "em_device.h: PART NUMBER undefined"
#endif

#endif
