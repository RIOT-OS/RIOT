/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BOARD_INTERNAL_H
#define BOARD_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

extern int _native_null_out_file;
extern int _native_null_in_pipe[2];
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_INTERNAL_H */
