/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

/*
  platform.h

  This file contains the platform specific typedefs
*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "simplelink.h"

typedef unsigned char   u8;  /* Range: 0 to 255; refers network buf */
typedef signed   char   i8;  /* Range: -127 to 127 and signed maths */
typedef char            c8;  /* ASCII string and value of 0 to 127  */

typedef unsigned short u16;
typedef signed short   i16;
typedef unsigned long  u32;
typedef signed long    i32;

extern void platform_timer_init();
extern u32 platform_get_time_in_secs();
#endif /* __PLATFORM_H__ */
