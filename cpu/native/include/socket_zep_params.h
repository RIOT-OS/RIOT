/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_socket_zep
 * @{
 *
 * @file
 * @brief   Configuration parameters for the @ref drivers_socket_zep driver
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SOCKET_ZEP_PARAMS_H
#define SOCKET_ZEP_PARAMS_H

#include "socket_zep.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Number of allocated parameters at @ref socket_zep_params
 *
 * @note    This was desided to only be confiruable on compile-time to be
 *          more similar to actual boards
 */
#ifndef SOCKET_ZEP_MAX
#define SOCKET_ZEP_MAX              (1)
#endif

/**
 * @name   Default parameters for native argument parsing
 * @{
 */
#define SOCKET_ZEP_PORT_DEFAULT         "17754" /**< default port */
#define SOCKET_ZEP_LOCAL_ADDR_DEFAULT   "::"    /**< default local address */
/**
 * @}
 */

/**
 * @brief   socket_zep configurations
 */
extern socket_zep_params_t socket_zep_params[SOCKET_ZEP_MAX];

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_ZEP_PARAMS_H */
/** @} */
