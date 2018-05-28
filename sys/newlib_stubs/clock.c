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
 * @brief       Provide stub for _times_r (process' CPU time).
 *
 * By providing _times_r this module indirectly provides a stub for clock().
 * If a real implementation is needed, see the newlib_syscalls_clock module.
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>
#include <sys/times.h>

/**
 * Current process times (not implemented).
 *
 * @param[out]  ptms    Not modified.
 *
 * @return  -1, this function always fails. errno is set to ENOSYS.
 */
clock_t _times_r(struct _reent *ptr, struct tms *ptms)
{
    (void)ptms;
    ptr->_errno = ENOSYS;

    return (-1);
}
