/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    ColorOutputter.h
 */
#ifndef __COLOROUTPUTTER_H__
#define __COLOROUTPUTTER_H__

#include "Outputter.h"

#ifdef __cplusplus
extern "C" {
#endif

OutputterRef ColorOutputter_outputter(void);

void ColorOutputter_printStatistics(OutputterRef self, TestResultRef result);

#ifdef __cplusplus
}
#endif

#endif/*__COLOROUTPUTTER_H__*/
/** @} */
