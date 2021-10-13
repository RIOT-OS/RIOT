/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief  Write len bytes of a given buffer into a file
 *
 * @param[out] filename    name of the file to be written
 * @param[in]  buf         a pointer to the buffer which needs to be written
 * @param[in]  len         the number of bytes from buf to be written
 *
 */
int to_file(const char *filename, void *buf, size_t len);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* COMMON_H */
