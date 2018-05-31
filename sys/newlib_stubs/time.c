/*
 * Copyright (C) 2018 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_newlib_stubs
 * @{
 *
 * @file
 * @brief       Provide stub for _gettimeofday_r (wall time).
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>
#include <sys/time.h>

/**
 * Get wall time in milliseconds since the epoch (not implemented).
 *
 * This is a stub that always fails for systems without RTC.
 *
 * @param[out]  r    Reentrant structure.
 * @param[out]  tp   Not modified.
 * @param  tpz   Not used.
 *
 * @return  -1, this function always fails. errno is set to ENOSYS.
 */
int _gettimeofday_r(struct _reent *r, struct timeval *restrict tp,
                    void *restrict tzp)
{
    (void) tp;
    (void) tzp;
    r->_errno = ENOSYS;
    return -1;
}
