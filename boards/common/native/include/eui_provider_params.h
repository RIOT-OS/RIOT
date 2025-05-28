/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_native
 * @{
 *
 * @file
 * @brief       EUI providers found on the board
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#include "native_cli_eui_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    EUI sources on the board
 *          EUI-64 can be provided with the -Z command line argument
 * @{
 */
#define EUI64_PROVIDER_FUNC   native_cli_get_eui64
#ifndef EUI64_PROVIDER_TYPE
#define EUI64_PROVIDER_TYPE   NETDEV_SOCKET_ZEP
#endif
#define EUI64_PROVIDER_INDEX  NETDEV_INDEX_ANY
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
