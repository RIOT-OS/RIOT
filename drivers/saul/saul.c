/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
__attribute__((alias("saul_write_notsup")))
int saul_read_notsup(const void *dev, phydat_t *dat);
