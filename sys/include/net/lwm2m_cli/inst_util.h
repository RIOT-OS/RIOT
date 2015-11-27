/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    net_lwm2m_cli
 *
 * @{
 * @file
 * @brief       Object instance utility functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandru Razvan Caciulescu <alex.darredevil@gmail.com>
 */

#ifndef LWM2M_CLI_INST_UTIL_H
#define LWM2M_CLI_INST_UTIL_H

#include <stdint.h>

#include "liblwm2m.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Dump the given instance to STDIO
 *
 * @param[in] obj       object that contains the instance
 * @param[in] instance  instance identifier
 */
void lwm2m_cli_inst_dump(lwm2m_object_t *obj, uint16_t instance);

#ifdef __cplusplus
}
#endif

#endif /* LWM2M_CLI_INST_UTIL_H */
/** @} */
