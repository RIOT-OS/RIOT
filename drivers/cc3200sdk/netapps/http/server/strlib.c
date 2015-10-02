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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strlib.h"


/*****************************************************************************
*
*  itoa
*
*  \param  n is the number to be converted to ASCII
*          s is a pointer to an array where the ASCII string will be placed
*          b is the base (10 for decimal)
*
*  \return None
*
*  \brief  Integer to ASCII
*
*****************************************************************************/
char *itoa(int n, char *s, int b) {
	const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	unsigned int i = 0;
	int sign;

	if ((sign = n) < 0)
		n = -n;

	do {
		s[i++] = digits[n % b];
	} while ((n /= b) > 0);

	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	return strrev(s);
}

/*****************************************************************************
*
*  itoa
*
*  \param  str is a pointer to the string to be reversed
*
*  \return None
*
*  \brief  Reverses a string
*
*****************************************************************************/
char *strrev(char *str) {
	char *p1, *p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}

	return str;
}

/* Little endian format */
void WordToBytes(void *pBuff, void const*pvalue, unsigned int NumBytes)
{
	unsigned char *pSource, *pDest;
	int byte = 0;

	pSource = (unsigned char *)pvalue;
	pDest = (unsigned char *)pBuff;

  for(byte = 0; byte < NumBytes; byte++)
  {
		*pDest++ = *pSource++;
	}
}

int String_utoa(int uNum, char *pString)
{
    char digits[] = "0123456789";
    char* ptr;
    int uLength;
    int uTemp = uNum;

    // value 0 is a special format
    if (uNum == 0)
    {
        uLength = 1;
        *pString = '0';
        return uLength;
    }

    // Find out the length of the number, in decimal base
    uLength = 0;
    while (uTemp > 0)
    {
        uTemp /= 10;
        uLength++;
    }

    // Do the actual formatting, right to left
    uTemp = uNum;
    ptr = (char*)pString + uLength;
    while (uTemp > 0)
    {
        --ptr;
        *ptr = digits[uTemp % 10];
        uTemp /= 10;
    }
    return uLength;
}

int iptostring(unsigned char *ip, char *ipstring)
{
  int temp,i, length, uLength;
  char *ptr;
  ip =ip +3;
  ptr = ipstring;
  uLength = 0;
  for (i=0; i<4; i++)
  {
    temp = *ip;
    length = String_utoa((unsigned long) temp, ptr);
    ptr = ptr + length;
    uLength += length;
    *ptr = '.';
    ptr++;
    uLength++;
    ip--;
  }
  return (uLength-1);
}

