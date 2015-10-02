//*****************************************************************************
// Copyright (C) 2014 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************

#ifndef _HTTP_DEBUG_H_
#define _HTTP_DEBUG_H_

#include "HttpConfig.h"

/**
 * @defgroup HttpDebug Debugging routines
 * This module implements debug routines which enable debug traces to be sent to a logger
 * Note this module is only compiled if HTTP_CORE_ENABLE_DEBUG is defined in HttpConfig.h
 *
 * @{
 */

/**
 * Send formatted debug trace to logger
 * @param pFormat The printf-style format to send
 * @param ... The rest of the parameters
 */
#ifndef NOTERM
#define HttpDebug           Report
#else
#define HttpDebug(x, ...)
#endif
/**
 * Send string debug trace to logger
 * @param pString The string to send
 */
void HttpDebugStr(const char* pString);

/**
 * Assert that a certain condition is true
 * @param condition A value that must be nonzero
 */
void HttpAssert(int condition);

#endif // _HTTP_STRING_H_

/// @}
