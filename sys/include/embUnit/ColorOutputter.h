/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @{
 *
 * @file
 *
 * @author Janos Kutscherauer <noshky@gmail.com>
 */

#include "Outputter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief embUnit coloured output formatter
 *
 * @return pointer to the OutputterImplement struct for this outputter
 */
OutputterRef ColorOutputter_outputter(void);

/**
 * @brief coloured output formatter for test statistics
 *
 * @param[in] self      pointer to the used OutputterImplement
 * @param[in] result    the test results to print
 */
void ColorOutputter_printStatistics(OutputterRef self, TestResultRef result);

#ifdef __cplusplus
}
#endif

/** @} */
