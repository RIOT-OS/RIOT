/**
 * Debug-Header. 
 *
 * #define ENABLE_DEBUG, include this and then use DEBUG as printf you can toggle.
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup	kernel
 * @{
 * @file
 * @author Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdio.h>

#ifdef ENABLE_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif
