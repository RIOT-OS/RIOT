/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Get mock manifest
 *
 * @param[out] buf      Manifest buffer
 * @param[in] buf_len   Maximum length of @p buf
 *
 * @return Actual length of the manifest
 */
ssize_t testsuite_suit_manifest_get(uint8_t *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
