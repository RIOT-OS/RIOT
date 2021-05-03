/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "sema_inv.h"

bool sema_inv_post(sema_inv_t *s)
{
    if (atomic_fetch_sub_u32(&s->value, 1) == 1) {
        mutex_unlock(&s->lock);
        return true;
    }

    return false;
}

bool sema_inv_post_mask(sema_inv_t *s, uint32_t mask)
{
    if (atomic_fetch_and_u32(&s->value, ~mask) == mask) {
        mutex_unlock(&s->lock);
        return true;
    }

    return false;
}

/** @} */
