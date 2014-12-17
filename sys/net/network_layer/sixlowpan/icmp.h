/*
 * 6lowpan neighbor discovery
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_sixlowpan
 * @{
 * @file    sixlowpan/icmp.h
 * @brief   6lowpan neighbor discovery constants, data structs, and prototypes
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef _SIXLOWPAN_ICMP_H
#define _SIXLOWPAN_ICMP_H

#include <stdint.h>

#include "vtimer.h"
#include "sixlowpan/icmp.h"
#include "sixlowpan/ndp.h"
#include "sixlowpan/types.h"

#include "lowpan.h"
#include "ip.h"

#ifdef __cplusplus
extern "C" {
#endif

enum option_types_t {
    OPT_SLLAO = 1,
    OPT_TLLAO,
    OPT_PI,
    OPT_MTU,
    OPT_ARO,
    OPT_6CO,
    OPT_ABRO,
    OPT_DAR,
    OPT_DAC,
};

extern kernel_pid_t nd_nbr_cache_rem_pid;


void recv_echo_req(void);
void recv_echo_repl(void);
void recv_rtr_sol(void);
void recv_rtr_adv(void);
void recv_nbr_adv(void);
void recv_nbr_sol(void);

void nbr_cache_auto_rem(void);
ndp_a6br_cache_t *abr_add_context(uint16_t version, ipv6_addr_t *abr_addr,
                                  uint8_t cid);
void abr_remove_context(uint8_t cid);

#ifdef __cplusplus
}
#endif

#endif /* _SIXLOWPAN_ICMP_H*/
