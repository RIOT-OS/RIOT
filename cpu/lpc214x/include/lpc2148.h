/*
 * lpc2148.h
 *
 * Copyright (C) 2009 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * Parts taken from FeuerWhere-Project, lpc2148.h.
 */

#ifndef __LPC2148_H
#define __LPC2148_H

#include "lpc214x.h"

#define F_CCO					240000000
#define CL_CPU_DIV				4									///< CPU clock divider
#define F_CPU					(F_CCO / CL_CPU_DIV)				///< CPU target speed in Hz
#define F_RC_OSCILLATOR			12000000							///< Frequency of internal RC oscillator
#define F_RTC_OSCILLATOR		32767								///< Frequency of RTC oscillator
#define PCLK_DIV                0x2                                 ///< PCLK clock divider, F_CPU/PCLK_DIV=PCLK

#define VIC_SIZE    16

#endif // __LPC2148_H

