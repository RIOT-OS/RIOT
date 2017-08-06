/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k22f NXP Kinetis K22F
 * @ingroup         cpu
 * @brief           CPU specific implementations for the NXP Kinetis K22F
 *                  Cortex-M4 MCU
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef CPU_MODEL_MK22FN512VLH12
#include "vendor/MK22F51212.h"
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
