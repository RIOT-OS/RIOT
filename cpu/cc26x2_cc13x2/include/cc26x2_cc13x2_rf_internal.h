/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 RF driver
 */

#ifndef CC26X2_CC13X2_RF_INTERNAL_H
#define CC26X2_CC13X2_RF_INTERNAL_H

#include <ti/drivers/rf/RF.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Open RF driver handle
 */
RF_Handle cc26x2_cc13x2_rf_open(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_INTERNAL_H */
