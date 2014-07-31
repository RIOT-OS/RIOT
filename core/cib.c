/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup     core_util
 * @{
 *
 * @file        cib.c
 * @brief       Circular integer buffer implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cib.h"

void cib_init(cib_t *cib, unsigned int size)
{
    cib->read_count = 0;
    cib->write_count = 0;
    cib->complement = 0 - size;
}

int cib_avail(cib_t *cib)
{
    return (int)(cib->write_count - cib->read_count);
}

int cib_get(cib_t *cib)
{
    int avail = cib_avail(cib);

    if (avail > 0) {
        return (int)(cib->read_count++ & ~cib->complement);
    }

    return -1;
}

int cib_put(cib_t *cib)
{
    int avail = cib_avail(cib);

    if ((int)(avail + cib->complement) < 0) {
        return (int)(cib->write_count++ & ~(cib->complement));
    }

    return -1;
}

/*
int main() {
    cib_t cib;

    cib_init(&cib, 0);

    int res = cib_get(&cib);

    printf("%i\n", res);
}
*/
