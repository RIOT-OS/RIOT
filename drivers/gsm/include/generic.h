/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef GENERIC_H
#define GENERIC_H

#include "gsm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gsm generic_t;

typedef struct gsm_driver generic_driver_t;

extern const generic_driver_t generic_driver;

#ifdef __cplusplus
}
#endif

#endif /* GENERIC_H */
