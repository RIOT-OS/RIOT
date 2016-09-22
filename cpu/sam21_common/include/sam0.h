/*
 * Copyright (C) 2016 Kees Bakker
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam21_common
 * @{
 *
 * @file
 * @brief           Wrapper include file for samd21.h, samr21.h, etc
 *
 * @author          Kees Bakker <kees@sodaq.com>
 */

#ifndef SAM0_H
#define SAM0_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SAMR21G18A__) || defined(__ATSAMR21G18A__)
#include "cmsis/samr21/include/samr21g18a.h"
#elif defined(__SAMR21E18A__) || defined(__ATSAMR21E18A__)
#include "cmsis/samr21/include/samr21e18a.h"
#else
  #error "Unsupported SAM0 variant."
#endif

#ifdef __cplusplus
}
#endif

#endif /* SAM0_H */
/** @} */
