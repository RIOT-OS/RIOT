/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL fallback functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>

#include "saul.h"

int saul_write_notsup(const void *dev, const phydat_t *dat)
{
    (void)dev;
    (void)dat;
    return -ENOTSUP;
}

/* No need to consume ROM for a second implementation, just create an
 * alias symbol to saul_write_notsup */
#if defined(__APPLE__)
#  pragma weak saul_write_notsup = saul_read_notsup
#else
__attribute__((alias("saul_write_notsup")))
#endif
int saul_read_notsup(const void *dev, phydat_t *dat);
