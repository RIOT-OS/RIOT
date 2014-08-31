/*
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup arch
 * @defgroup    native_cpu  Native
 * @brief       Native CPU specific code
 * @details     The native CPU uses system calls to simulate hardware access.
 * @ingroup     cpu
 * @brief       CPU abstraction for the native port
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 */

#ifndef _CPU_H
#define _CPU_H

/* TODO: remove once these have been removed from RIOT: */
void dINT(void);
void eINT(void);

/** @} */
#endif //_CPU_H
