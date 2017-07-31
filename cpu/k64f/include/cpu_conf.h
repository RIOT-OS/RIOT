/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k64f NXP Kinetis K64F
 * @ingroup         cpu
 * @brief           CPU specific implementations for the NXP Kinetis K64F
 *                  Cortex-M4 MCU
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Johann Fischer <j.fischer@phytec.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef CPU_MODEL_MK64FN1M0VLL12
#include "vendor/MK64F12.h"
#else
#error "undefined CPU_MODEL"
#endif

#include "cpu_conf_kinetis.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
