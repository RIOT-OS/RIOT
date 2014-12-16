/*
 * Copyright (C) 2013 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup sys_hash_string
 * @ingroup sys
 *
 * @brief Hash string implementation
 *
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <string.h>
#include "hash_string.h"

unsigned long hash_string(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;    /* hash * 33 + c */
    }

    return hash;
}

int cmp_string(char *a, char *b)
{
    return (strcmp(a, b) == 0);
}
