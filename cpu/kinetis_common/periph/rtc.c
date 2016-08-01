/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common_rtt
 *
 * @{
 *
 * @file
 * @brief       RTC wrapper for the RTT hardware
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "rtc.h"
#include "binrtc.h"
#include "clist.h"

rtc_t cpu_rtc = {
    .rtc_op = &binrtc_ops,
    .list_entry = CLIST_INIT,
    .name = "cpu",
};
