/*
 * extlibs_common.h - Header file defining  common MACROS for OTA and FLC lib
 *
 * Copyright (C) 2014 Texas Instruments Incorporated
 *
 * All rights reserved. Property of Texas Instruments Incorporated.
 * Restricted rights to use, duplicate or disclose this code are
 * granted through contract.
 * The program may not be used without the written permission of
 * Texas Instruments Incorporated or against the terms and conditions
 * stipulated in the agreement under which this program has been supplied,
 * and under no circumstances can it be used with non-TI connectivity device.
 *
*/
#ifndef __EXTLIBS_COMMON_H__
#define __EXTLIBS_COMMON_H__

#ifdef    __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* open Report for debug */
//#define DEBUG_PRINT

#ifdef _WIN32

#ifdef DEBUG_PRINT
#define Report printf
#else
#define Report(...)
#endif

#else

#ifdef DEBUG_PRINT
extern void Report(_u8 *format, ...);
#else
#define Report(...)
#endif


#endif

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /*  __EXTLIBS_COMMON_H__ */

