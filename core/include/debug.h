/**
 * Debug-Header.
 *
 * #define ENABLE_DEBUG, include this and then use DEBUG as printf you can toggle.
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
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

#if ENABLE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#undef ENABLE_DEBUG
#else
#define DEBUG(...)
#endif

