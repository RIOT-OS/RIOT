/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       sysinit abstraction layer for RIOT adaption
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef SYSINIT_SYSINIT_H
#define SYSINIT_SYSINIT_H

#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DPL assert macro
 */
#define SYSINIT_PANIC_ASSERT(rc)        assert(rc);

#ifdef __cplusplus
}
#endif

#endif /* SYSINIT_SYSINIT_H */
