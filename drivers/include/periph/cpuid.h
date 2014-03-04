/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file        periph/cpuid.h
 * @brief       Provides access the CPU's serial number
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __PERIPH_CPUID_H_
#define __PERIPH_CPUID_H_

#include "cpu-conf.h"

#if CPUID_ID_LEN
/**
 * @brief   Gets the serial number of the CPU.
 *
 * @param[out] id   The serial number of the CPU of length CPU_ID_LEN (must be
 *                  defined in the CPU's cpu-conf.h)
 */
void cpuid_get(void *id);
#endif /* CPUID_ID_LEN */

#endif /* __PERIPH_CPUID_H_ */
/**
 * @}
 */
