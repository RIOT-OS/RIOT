/***************************************************************************//**
 * @file em_dbg.h
 * @brief Debug (DBG) API
 * @version 3.20.6
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


#ifndef __EM_DBG_H
#define __EM_DBG_H

#include <stdbool.h>
#include "em_device.h"

#if defined ( CoreDebug_DHCSR_C_DEBUGEN_Msk )

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup DBG
 * @{
 ******************************************************************************/

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Check if a debugger is connected (and debug session activated)
 *
 * @details
 *   Used to make run-time decisions depending on whether a debug session
 *   has been active since last reset, ie using a debug probe or similar. In
 *   some cases special handling is required in that scenario.
 *
 * @return
 *   true if a debug session is active since last reset, otherwise false.
 ******************************************************************************/
__STATIC_INLINE bool DBG_Connected(void)
{
  if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)
  {
    return true;
  }

  return false;
}


void DBG_SWOEnable(unsigned int location);

/** @} (end addtogroup DBG) */
/** @} (end addtogroup EM_Library) */

#ifdef __cplusplus
}
#endif

#endif /* defined ( CoreDebug_DHCSR_C_DEBUGEN_Msk ) */

#endif /* __EM_DBG_H */
