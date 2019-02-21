/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_cord_epsim CoRE RD Simple Registration Endpoint
 * @ingroup     net_cord
 * @brief       CoRE Resource Directory endpoint using the simple registration
 *              procedure
 * @{
 *
 * @file
 * @brief       Interface for the CoRE RD simple registration endpoint
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NET_CORD_EPSIM_H
#define NET_CORD_EPSIM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Error codes used by the cord_epsim implementation
 */
enum {
    CORD_EPSIM_OK     =  0,     /**< all good */
    CORD_EPSIM_NOADDR = -1,     /**< on address conversion errors */
    CORD_EPSIM_ERROR  = -2,     /**< on other errors */
};

/**
 * @brief   Initiate the node registration by sending an empty CoAP POST message
 *          to the RD server's /.well-known/core resource
 *
 * @return  CORD_EPSIM_OK on success
 * @return  CORD_EPSIM_NOADDR if conversion of RD address fails
 * @return  CORD_EPSIM_ERROR if something goes wrong preparing or sending the
 *          initial request
 */
int cord_epsim_register(void);

#if defined(MODULE_CORD_EPSIM_STANDALONE) || defined(DOXYGEN)
/**
 * @brief   Spawn a new thread that registers the node and updates the
 *          registration with all responding RDs using the simple registration
 *          process
 *
 * @note    Only available with the `cord_epsim_standalone` module compiled in
 *
 * @warning This function must only be called once (typically during system
 *          initialization)
 */
void cord_epsim_run(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_CORD_EPSIM_H */
/** @} */
