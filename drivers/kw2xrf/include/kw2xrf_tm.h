/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Testing interfaces for kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef KW2XRF_TM_H
#define KW2XRF_TM_H

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
#endif /* KW2XRF_TM_H */
