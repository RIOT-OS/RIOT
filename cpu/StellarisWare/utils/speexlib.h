//*****************************************************************************
//
// speexlib.h - Stellaris interface to the speex coder/encoder library.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#ifndef __SPEEXLIB_H__
#define __SPEEXLIB_H__

int SpeexEncodeInit(int iSampleRate, int iComplexity, int iQuality);
int SpeexEncode(short *pusInBuffer, unsigned long ulInSize,
                unsigned char *pucOutBuffer, unsigned long ulOutSize);
int SpeexEncodeQualitySet(int iQuality);
int SpeexEncodeFrameSizeGet(void);
int SpeexDecodeFrameSizeGet(void);
int SpeexDecodeInit(void);
int SpeexDecode(unsigned char *pucInBuffer, unsigned long ulInSize,
                unsigned char *pucOutBuffer, unsigned long ulOutSize);

#endif
