/**
 * atomic function declarations
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
 */

#ifndef _ATOMIC_H
#define _ATOMIC_H

/**
 * @brief sets "val" to "set", returns old "val", atomically
 */

extern unsigned int atomic_set_return(unsigned int *val, unsigned int set);

/**
 * @}
 */
#endif /* _ATOMIC_H */
