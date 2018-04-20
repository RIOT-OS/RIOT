/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_gsm_gprs GPRS
 * @ingroup     drivers_gsm
 * @brief       A generic implementation of the GSM GPRS API
 *
 * @{
 *
 * @file
 * @brief   GSM-independent GPRS message driver
 *
 * @author  Max van Kessel <maxvankessel@betronic.nl>
 */
#ifndef GPRS_H
#define GPRS_H

#include <stdint.h>

#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GSM_CONTEXT_IP = 0,
    GSM_CONTEXT_PPP,
    GSM_CONTEXT_IPV6,
    GSM_CONTEXT_IP4V6,
    GSM_CONTEXT_COUNT,
} gsm_context_t;

/**
 * @brief   Attach to GPRS service
 *
 * @param[in]   dev     device to operate on
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_grps_attach(gsm_t *dev);

/**
 * @brief   Detach from GPRS service
 *
 * @param[in]   dev     device to operate on
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_grps_detach(gsm_t *dev);

/**
 * @brief Setup gprs pdp context
 *
 * @param[in]   dev     device to operate on
 * @param[in]   ctx     context to setup
 * @param[in]   type    pdp type
 * @param[in]   apn     access point name
 * @param[in]   user    user identifier (optional)
 * @param[in]   pass    password        (optional)
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_gprs_setup_pdp_context(gsm_t *dev, uint8_t ctx, gsm_context_t type,
        const char * apn, const char * user, const char * pass);


/**
 * @brief   Activate PDP context
 *
 * @param[in]   dev     device to operate on
 * @param[in]   ctx     context to activate
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_grps_activate_context(gsm_t *dev, uint8_t ctx);

/**
 * @brief   Deactivate PDP context
 *
 * @param[in]   dev     device to operate on
 * @param[in]   ctx     context to deactivate
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int gsm_grps_deactivate_context(gsm_t *dev, uint8_t ctx);

/**
 * @brief   Gets device network registration
 *
 * @param[in] dev   Device to operate on
 *
 * @return  registration value
 * @return  < 0 for failure
 */
int gsm_gprs_get_registration(gsm_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* GPRS_H */
