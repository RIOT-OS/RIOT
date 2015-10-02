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

#include "string.h"
#include <stdlib.h>
#include "HttpString.h"
//#include "HttpCore.h"

/**
 * @addtogroup HttpString
 * @{
 */

char digits[] = "0123456789";
char hexDigits[] = "0123456789abcdef";

int HttpString_strcmp(struct HttpBlob first, struct HttpBlob second)
{
    int min,res;
    if (first.uLength > second.uLength)
        min = second.uLength;
    else
        min = first.uLength;

    // Compare a common length which might be equal
    res = memcmp(first.pData, second.pData, min);
    if (res != 0)
        return res;

    // Common length is equal, so the longer blob is "larger"
    if (first.uLength > second.uLength)
        return 1;
    else if (first.uLength < second.uLength)
        return -1;
    else
        return 0;
}

static void ToLower(char * str, UINT16 len)
{
    int i;
    for (i=0 ; i<=len; i++)
    {
        if (str[i]>='A' && str[i]<='Z')
            str[i] = str[i] + 32;
    }
}

UINT8* HttpString_nextToken(char* pToken, UINT16 uTokenLength, struct HttpBlob blob)
{
    UINT8* pch = blob.pData;
    struct HttpBlob partialBlob;
    struct HttpBlob token;
    token.pData = (UINT8*)pToken;
    token.uLength = uTokenLength;

   	ToLower((char *)blob.pData, blob.uLength);

    while (pch < blob.pData + blob.uLength)
    {
        // Calculate how many blob bytes we should search
        int nMaxCount = blob.uLength - (pch - blob.pData) - uTokenLength + 1;
        if (nMaxCount < 1)
            return NULL;

        // Search for the first character of the token
        pch = (UINT8*)memchr(pch, pToken[0], nMaxCount);
        if (pch==NULL)
            return NULL;

        // Found first character, now compare the rest
        partialBlob.pData = pch;
        partialBlob.uLength = uTokenLength;
        if (HttpString_strcmp(token, partialBlob)==0)
            return pch;

        // Skip this byte, and look for the token in the rest of the blob
        ++pch;
    }
    return NULL;
}

UINT8* HttpString_nextDelimiter(char* pToken, UINT16 uTokenLength, struct HttpBlob blob)
{
    UINT8* pch = blob.pData;
    struct HttpBlob partialBlob;
    struct HttpBlob token;
    token.pData = (UINT8*)pToken;
    token.uLength = uTokenLength;

   	//ToLower((char *)blob.pData, blob.uLength);

    while (pch < blob.pData + blob.uLength)
    {
        // Calculate how many blob bytes we should search
        int nMaxCount = blob.uLength - (pch - blob.pData) - uTokenLength + 1;
        if (nMaxCount < 1)
            return NULL;

        // Search for the first character of the token
        pch = (UINT8*)memchr(pch, pToken[0], nMaxCount);
        if (pch==NULL)
            return NULL;

        // Found first character, now compare the rest
        partialBlob.pData = pch;
        partialBlob.uLength = uTokenLength;
        if (HttpString_strcmp(token, partialBlob)==0)
            return pch;

        // Skip this byte, and look for the token in the rest of the blob
        ++pch;
    }
    return NULL;
}

UINT32 HttpString_atou(struct HttpBlob string)
{
    return atoi((const char*)string.pData);
}

void HttpString_utoa(UINT32 uNum, struct HttpBlob* pString)
{
    char* ptr;
    UINT32 uTemp = uNum;

    // value 0 is a special format
    if (uNum == 0)
    {
        pString->uLength = 1;
        *pString->pData = '0';
        return;
    }

    // Find out the length of the number, in decimal base
    pString->uLength = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        pString->uLength++;
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = (char*)pString->pData + pString->uLength;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = digits[uTemp % 10];
        uTemp /= 10;
    }
}

void HttpString_htoa(UINT32 uNum, struct HttpBlob* pString, UINT8 bPadZero)
{
    UINT8* ptr;
    UINT32 uTemp = uNum;

    if (!bPadZero)
    {
        // value 0 is a special format
        if (uNum == 0)
        {
            pString->uLength = 1;
            *pString->pData = '0';
            return;
        }

        // Find out the length of the number, in hexadecimal base
        pString->uLength = 0;
        while (uTemp > 0)
        {
            uTemp /= 16;
            pString->uLength++;
        }
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = pString->pData + pString->uLength;
    while (ptr > pString->pData)
    {
        --ptr;
        *ptr = (UINT8)hexDigits[uTemp % 16];
        uTemp /= 16;
    }
}

/// @}
