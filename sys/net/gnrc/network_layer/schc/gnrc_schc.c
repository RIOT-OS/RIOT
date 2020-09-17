/*
 * Copyright (C) 2020 Bart Moons <bamoons.moons@ugent.be>
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "net/gnrc/schc.h"

kernel_pid_t gnrc_schc_init(void)
{
#if IS_USED(MODULE_LIBSCHC_GNRC)
    return libschc_gnrc_init();
#else
    LOG_ERROR("gnrc_schc: no SCHC implementation found\n");
    return -ENOTSUP;
#endif
}
