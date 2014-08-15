/***************************************************************************//**
 * @file em_version.h
 * @brief Assign correct part number for include file
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


#ifndef __EM_VERSION_H
#define __EM_VERSION_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EM_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Version
 * @{
 ******************************************************************************/

/** Version number of emlib peripheral API */
#define _EMLIB_VERSION 3.20.6
/** Major version of emlib */
#define _EMLIB_VERSION_MAJOR 3
/** Minor version of emlib */
#define _EMLIB_VERSION_MINOR 20
/** Patch revision of emlib */
#define _EMLIB_VERSION_PATCH 6

/** @} (end addtogroup Version) */
/** @} (end addtogroup EM_Library) */

#ifdef __cplusplus
}
#endif

#endif /* __EM_VERSION_H */
