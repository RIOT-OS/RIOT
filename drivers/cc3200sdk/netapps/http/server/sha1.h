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
 * @defgroup sha1
 *
 * @{
 */

int SHA1(unsigned char *puiInData,unsigned char *puiOutData);
void ConvertToBase64(char *pcOutStr, const char *pccInStr, int iLen);

/// @}
