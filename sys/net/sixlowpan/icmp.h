/*
 * 6lowpan neighbor discovery
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlownd.h
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

extern unsigned int nd_nbr_cache_rem_pid;


void recv_echo_req(void);
void recv_echo_repl(void);
void recv_rtr_sol(void);
void recv_rtr_adv(void);
void recv_nbr_adv(void);
void recv_nbr_sol(void);

void nbr_cache_auto_rem(void);
int8_t plist_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
                 uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1);

ndp_a6br_cache_t *abr_add_context(uint16_t version, ipv6_addr_t *abr_addr,
                                  uint8_t cid);
void abr_remove_context(uint8_t cid);

uint16_t icmpv6_csum(uint8_t proto);
#endif /* _SIXLOWPAN_ICMP_H*/
