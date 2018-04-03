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

#if defined(EFR32MG1P131F256GM48)
#include "efr32mg1p131f256gm48.h"

#elif defined(EFR32MG1P132F256GJ43)
#include "efr32mg1p132f256gj43.h"

#elif defined(EFR32MG1P132F256GM32)
#include "efr32mg1p132f256gm32.h"

#elif defined(EFR32MG1P132F256GM48)
#include "efr32mg1p132f256gm48.h"

#elif defined(EFR32MG1P132F256IM32)
#include "efr32mg1p132f256im32.h"

#elif defined(EFR32MG1P132F256IM48)
#include "efr32mg1p132f256im48.h"

#elif defined(EFR32MG1P133F256GM48)
#include "efr32mg1p133f256gm48.h"

#elif defined(EFR32MG1P231F256GM48)
#include "efr32mg1p231f256gm48.h"

#elif defined(EFR32MG1P232F256GJ43)
#include "efr32mg1p232f256gj43.h"

#elif defined(EFR32MG1P232F256GM32)
#include "efr32mg1p232f256gm32.h"

#elif defined(EFR32MG1P232F256GM48)
#include "efr32mg1p232f256gm48.h"

#elif defined(EFR32MG1P232F256IM32)
#include "efr32mg1p232f256im32.h"

#elif defined(EFR32MG1P232F256IM48)
#include "efr32mg1p232f256im48.h"

#elif defined(EFR32MG1P233F256GM48)
#include "efr32mg1p233f256gm48.h"

#elif defined(EFR32MG1P233F256IM48)
#include "efr32mg1p233f256im48.h"

#elif defined(EFR32MG1P632F256GM32)
#include "efr32mg1p632f256gm32.h"

#elif defined(EFR32MG1P632F256IM32)
#include "efr32mg1p632f256im32.h"

#elif defined(EFR32MG1P732F256GM32)
#include "efr32mg1p732f256gm32.h"

#elif defined(EFR32MG1P732F256IM32)
#include "efr32mg1p732f256im32.h"

#else
#error "em_device.h: PART NUMBER undefined"
#endif
#endif /* EM_DEVICE_H */
#ifdef __cplusplus
}
#endif

