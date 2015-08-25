/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   slip parameters example, used by auto_init_gnrc_netif
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef SLIP_PARAMS_H
#define SLIP_PARAMS_H

#include "net/gnrc/slip.h"

#ifdef __cplusplus
extern "C" {
#endif

static gnrc_slip_params_t xbee_params[] = {
    {
        .uart = SLIP_UART,
        .baudrate = SLIP_BAUDRATE,
    },
};

#ifdef __cplusplus
}
#endif
#endif /* SLIP_PARAMS_H */
/** @} */
