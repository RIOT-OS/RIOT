/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_meshcore
 * @{
 *
 * @file
 * @brief       Replacement of the Arduino header for MeshCore
 *
 * The MeshCore sources built by the package only use the C library from the
 * Arduino header.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

/** @} */
