/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_cryptoauthlib
 * @{
 *
 * @file
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cryptoauthlib.h"
#include "atca.h"
#include "atca_params.h"

/**
 * @brief   Helper function to use the library's unittests
 *          This function is defined in the cryptoauth library via patch.
 *          It is used to pass commands to run built-in unit tests of the library.
 */
int atca_run_cmd(char *command, size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
