/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_gsm
 * @brief       A generic implementation of the GSM driver
 *
 * @{
 *
 * @file
 * @brief   GSM-independent driver
 *
 * @author  Max van Kessel
 */
#ifndef GSM_GENERIC_H
#define GSM_GENERIC_H

#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsm gsm_generic_t;

typedef struct gsm_driver gsm_generic_driver_t;

extern const gsm_generic_driver_t gsm_generic_driver;

#ifdef __cplusplus
}
#endif

#endif /* GSM_GENERIC_H */
/** @} */
