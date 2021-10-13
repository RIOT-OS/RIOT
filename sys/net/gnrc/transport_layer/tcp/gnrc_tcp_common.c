/*
 * Copyright (C) 2020 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       GNRC TCP common function implementation
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */

#include "include/gnrc_tcp_common.h"

static _gnrc_tcp_common_tcb_list_t _list = {NULL, MUTEX_INIT};

_gnrc_tcp_common_tcb_list_t *_gnrc_tcp_common_get_tcb_list(void)
{
    return &_list;
}
