/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Device-specific test header file W5500 ethernet device driver
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */
#ifndef INIT_DEV_H
#define INIT_DEV_H

#include <stdint.h>

#include "kernel_defines.h"
#include "net/netdev.h"

#include "w5500.h"
#include "w5500_params.h"

#ifdef __cplusplus
extern "C" {
#endif

#define W5500_NUM   ARRAY_SIZE(w5500_params)
#define NETDEV_ETH_MINIMAL_NUMOF      W5500_NUM

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* INIT_DEV_H */
/** @} */
