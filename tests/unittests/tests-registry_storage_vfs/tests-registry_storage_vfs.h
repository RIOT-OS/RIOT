/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    unittests
 * @brief       Unittests for the ``registry_storage_vfs`` module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#pragma once

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The entry point of this test suite.
 */
void tests_registry_storage_vfs(void);

#ifdef __cplusplus
}
#endif
/** @} */
