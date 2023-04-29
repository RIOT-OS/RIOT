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

const conf_backend_t *configuration_backend_ram_get(void);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_BACKEND_RAM_H */
/** @} */
