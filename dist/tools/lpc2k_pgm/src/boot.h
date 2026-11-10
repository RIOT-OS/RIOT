/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef BOOT_H
#define BOOT_H

/**
 * @brief structure to hold boot partition
 */

typedef struct {
    int size;           /**< size of partition */
    const int *prog;    /**< partition data */
} boot_t;

#endif /* BOOT_H */
