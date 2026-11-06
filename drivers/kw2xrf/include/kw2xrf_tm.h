/*
 * SPDX-FileCopyrightText: 2016 Phytec Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Testing interfaces for kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_getset.h"
#include "net/netopt.h"

/**
 * @brief   Valid test modes to be used with @ref kw2xrf_set_test_mode.
 */
enum mkw2xrf_testmode {
    KW2XRF_TM_CTX_PREAMBLE = NETOPT_RF_TESTMODE_CTX_PRBS9 + 1,
    KW2XRF_TM_CTX_2MHZ,
    KW2XRF_TM_CTX_200KHZ,
    KW2XRF_TM_CTX_1MBPS_PRBS9,
    KW2XRF_TM_CTX_EXT,
    KW2XRF_TM_CTX_NM0,
    KW2XRF_TM_CTX_NM1,
};

/**
 * @brief   Set the test mode for the kw2xrf device.
 *
 * @param[in] dev       Device descriptor
 * @param[in] mode      Test mode (must be one of @ref mkw2xrf_testmode)
 *
 * @retval 1 on success
 * @retval != 1 otherwise
 */
int kw2xrf_set_test_mode(kw2xrf_t *dev, uint8_t mode);

#ifdef __cplusplus
}
#endif

/** @} */
