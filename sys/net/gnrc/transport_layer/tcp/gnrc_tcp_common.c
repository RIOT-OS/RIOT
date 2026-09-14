/*
 * SPDX-FileCopyrightText: 2020 Simon Brummer
 * SPDX-License-Identifier: LGPL-2.1-only
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
