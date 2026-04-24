/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-duemilanove
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Duemilanove board
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
