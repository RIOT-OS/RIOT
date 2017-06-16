/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <sys/types.h>

#ifdef __cplusplus
 extern "C" {
#endif

off_t fsize(const char *filename);
int to_file(const char *filename, void *buf, size_t len);
int from_file(const char *filename, void *buf, size_t len);
int do_sha256(const char *filename, void *tgt, size_t offset);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* COMMON_H */
