/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 *
 * @{
 *
 * @file    Test backend in RAM
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef CONFIGURATION_BACKEND_RAM_H
#define CONFIGURATION_BACKEND_RAM_H

#include "configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_CONFIGURATION_BACKEND_RAM_BUF_SIZE
/**
 *  @brief  Static buffer size for the RAM backend to store the encoded configuration
 */
#define CONFIG_CONFIGURATION_BACKEND_RAM_BUF_SIZE (1024)
#endif

extern const conf_backend_ops_t conf_backend_ram_ops;

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_BACKEND_RAM_H */
/** @} */
