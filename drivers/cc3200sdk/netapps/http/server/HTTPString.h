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

#ifndef _HTTP_STRING_H_
#define _HTTP_STRING_H_

/**
 * @defgroup HttpString String routines helper module
 * This module implements some string and buffer-related helper routines
 *
 * @{
 */

#include "datatypes.h"

struct HttpBlob
{
    UINT16 uLength;
    UINT8* pData;
};

/**
 * Utility macro which sets an existing blob
 */
#define HttpBlobSetConst(blob, constString) \
    {   \
        (blob).pData = (UINT8*)constString; \
        (blob).uLength = sizeof(constString) - 1; \
    }

/**
 * Perform string comparison between two strings.
 * @param first Pointer to data about the first blob or string
 * @param second Pointer to data about the second blob or string
 * @return zero if equal, positive if first is later in order, negative if second is later in order
 */
int HttpString_strcmp(struct HttpBlob first, struct HttpBlob second);

/**
 * return pointer to the next token
 * @param token Pointer to data of the first token
 * @param blob Pointer to data of the search string/blob
 * @return pointer if found, otherwize NULL
 */
UINT8* HttpString_nextToken(char* pToken, UINT16 uTokenLength, struct HttpBlob blob);

UINT8* HttpString_nextDelimiter(char* pToken, UINT16 uTokenLength, struct HttpBlob blob);

/**
 * Parse a string representation of an unsigned decimal number
 * Stops at any non-digit character.
 * @param string The string to parse
 * @return The parsed value
 */
UINT32 HttpString_atou(struct HttpBlob string);

/**
 * Format an unsigned decimal number as a string
 * @param uNum The number to format
 * @param[in,out] pString A string buffer which returns the formatted string. On entry, uLength is the maximum length of the buffer, upon return uLength is the actual length of the formatted string
 */
void HttpString_utoa(UINT32 uNum, struct HttpBlob* pString);

/**
 * Format an unsigned decimal number as an hexdecimal string
 * @param uNum The number to format
 * @param bPadZero nonzero to pad with zeros up to the string length, or zero to use minimal length required to represent the number
 * @param[in,out] pString A string buffer which returns the formatted string. On entry, uLength is the maximum length of the buffer, upon return uLength is the actual length of the formatted string
 */
void HttpString_htoa(UINT32 uNum, struct HttpBlob* pString, UINT8 bPadZero);

/// @}

#endif // _HTTP_STRING_H_
