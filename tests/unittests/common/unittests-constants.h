/*
 * Copyright (C) 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __UNITTESTS_CONSTANTS_H__
#define __UNITTESTS_CONSTANTS_H_
#include "embUnit/embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some randomly generated but still deterministic values for testing */
#ifndef TEST_STRING4
#define TEST_STRING4 "J&(d"
#endif
#ifndef TEST_STRING8
#define TEST_STRING8 "o\\rrB/q"
#endif
#ifndef TEST_STRING12
#define TEST_STRING12 "50U'HLKC3_ft"
#endif
#ifndef TEST_STRING16
#define TEST_STRING16 "nvxuO*6o3C=a6g7]"
#endif
#ifndef TEST_STRING64
#define TEST_STRING64 "3key9'=4\\wvAM&;.E&U/rOG:>\\C!r{K k.E[%;(}_I(D%XQk8+T>_M^G'Mm49,n:))]}"
#endif
#ifndef TEST_INT8
#define TEST_INT8 (-69)
#endif
#ifndef TEST_INT16
#define TEST_INT16 (-12229)
#endif
#ifndef TEST_INT32
#define TEST_INT32 (-1894529023L)
#endif
#ifndef TEST_INT64
#define TEST_INT64 (-4562349816927799244LL)
#endif
#ifndef TEST_UINT8
#define TEST_UINT8 (95U)
#endif
#ifndef TEST_UINT16
#define TEST_UINT16 (10098U)
#endif
#ifndef TEST_UINT32
#define TEST_UINT32 (2831907245LU)
#endif
#ifndef TEST_UINT64
#define TEST_UINT64 (13500266397057512199LLU)
#endif

#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ) || defined(CPU_ATMEGA2560)
#define TEST_INT TEST_INT16
#else
#define TEST_INT TEST_INT32
#endif

#ifdef __cplusplus
}
#endif

#endif /* __UNITTESTS_CONSTANTS_H_ */
/** @} */
