/***************************************************************************//**
 * @file em_rmu.h
 * @brief Reset Management Unit (RMU) peripheral API
 * @version 4.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
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


#ifndef __SILICON_LABS_EM_RMU_H_
#define __SILICON_LABS_EM_RMU_H_

#include "em_device.h"
#if defined(RMU_COUNT) && (RMU_COUNT > 0)

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RMU
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** RMU controlled peripheral reset control and reset source control */
typedef enum
{
#if defined( RMU_CTRL_BURSTEN )
  /** Reset control over Backup Power Domain */
  rmuResetBU = _RMU_CTRL_BURSTEN_SHIFT,
#endif
  /** Allow Cortex-M3 lock up signal */
  rmuResetLockUp = _RMU_CTRL_LOCKUPRDIS_SHIFT
} RMU_Reset_TypeDef;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/** RMU_LockupResetDisable kept for backwards compatibility */
#define RMU_LockupResetDisable(A) RMU_ResetControl(rmuResetLockUp, A)

void RMU_ResetControl(RMU_Reset_TypeDef reset, bool enable);
void RMU_ResetCauseClear(void);
uint32_t RMU_ResetCauseGet(void);

/** @} (end addtogroup RMU) */
/** @} (end addtogroup EM_Library) */

#ifdef __cplusplus
}
#endif

#endif /* defined(RMU_COUNT) && (RMU_COUNT > 0) */
#endif /* __SILICON_LABS_EM_RMU_H_ */
