/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include <stdint.h>

#include "architecture.h"

#include "net/unicoap.h"

/**
 * @defgroup net_unicoap_private Private API
 * @ingroup  net_unicoap_internal
 * @brief    Discover implementation details of `unicoap`
 * @{
 *
 * @warning Do not call any of these APIs and do not interact with any of unicoap's private types,
 * unless you know what you are doing. You risk corrupting `unicoap`'s internal state.
 */

/**
 * @file
 * @brief  Private API
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* MARK: - Debugging */
/**
 * @name Debugging
 * @{
 */
/** @brief Debug log prefix */
#define UNICOAP_DEBUG_PREFIX "coap"

#ifndef DOXYGEN
#  define _UNICOAP_NEED_HAVE "(need %" PRIuSIZE ", have %" PRIuSIZE ")"
#endif

#ifndef DOXYGEN
/**
 * @brief Debug with category prefix
 *
 * Prints `unicoap.category: __VA_ARGS__`
 *
 * @param category Category string
 */
#  define _UNICOAP_PREFIX_DEBUG(category, ...) DEBUG(UNICOAP_DEBUG_PREFIX category ": " __VA_ARGS__)
#  define UNICOAP_DEBUG(...)                   _UNICOAP_PREFIX_DEBUG("", __VA_ARGS__)
#  define OPTIONS_DEBUG(...)                   _UNICOAP_PREFIX_DEBUG(".options", __VA_ARGS__)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
