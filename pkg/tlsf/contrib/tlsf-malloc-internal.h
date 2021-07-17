/*
 * Copyright (C) 2014-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_tlsf_malloc
 * @{
 * @file
 * @internal
 *
 * @brief   TLSF/malloc internal definitions
 * @author  Juan I Carrano
 *
 */

#ifndef TLSF_MALLOC_INTERNAL_H
#define TLSF_MALLOC_INTERNAL_H

#include "tlsf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROUND_DOWN4(x) (((x)/4)*4) /* Is this necessary??? */

#ifndef TLSF_NATIVE_HEAPSIZE
/**
 * Fixed heap size to use in native.
 *
 * In native there is virtually unlimited memory and no predefined heap area
 * in the linker script, so one needs to define it manually.
 * The default here is 8MiB which should be plenty for RIOT.
 * Note that this has no effect on other platforms.
 */
#define TLSF_NATIVE_HEAPSIZE        (0x800000)
#endif

extern tlsf_t tlsf_malloc_gheap;

void init_tlsf_malloc(void);

#ifdef __cplusplus
}
#endif

#endif /* TLSF_MALLOC_INTERNAL_H */
