/*
 * Copyright (C) 2010 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __HASH_STRING_H
#define __HASH_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned long hash_string(unsigned char *str);
int cmp_string(char *a, char *b);

#ifdef __cplusplus
}
#endif

#endif /* __HASH_STRING_H */
