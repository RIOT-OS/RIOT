/**
 * Debug-Header.
 *
 * #define ENABLE_DEBUG, include this and then use DEBUG as printf you can toggle.
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup	kernel
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG (0)
#endif

#if defined(ENABLE_DEBUG) && (7-ENABLE_DEBUG-7 == 14)
#define ENABLE_DEBUG (1)
#endif

#define DEBUG(...) do { \
    if (!(ENABLE_DEBUG)) break; \
    printf("%s:%d in %s: ", __FILE__, __LINE__, __func__); \
    printf(__VA_ARGS__); \
    } while (0)

#undef ENABLE_DEBUG
