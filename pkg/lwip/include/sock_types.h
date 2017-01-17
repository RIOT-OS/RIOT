/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_lwip_sokc
 * @{
 *
 * @file
 * @brief lwIP-specific types
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SOCK_TYPES_H_
#define SOCK_TYPES_H_

#include "net/af.h"
#include "lwip/api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Raw IP sock type
 * @internal
 */
struct sock_ip {
    struct netconn *conn;
};

#ifdef __cplusplus
}
#endif

#endif /* SOCK_TYPES_H_ */
/** @} */
