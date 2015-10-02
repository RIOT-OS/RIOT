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

/**
 * @addtogroup HttpDebug
 *
 * @{
 */


#include "HttpDebug.h"


#ifdef HTTP_CORE_ENABLE_DEBUG

void HttpDebugStr(const char* pString)
{
#ifndef NOTERM
    Report("%s\n\r", pString);
#endif
}

void HttpAssert(int condition)
{
    if (condition)
        return;
#ifndef NOTERM
    Report("ASSERTION!\n\r");
#endif

    while (1)
    {
    }
}

#else

void HttpDebugStr(const char* pString)
{

}
void HttpAssert(int condition)
{

}
#endif

/// @}
