/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc    Generic (gnrc) network stack.
 * @ingroup     net
 * @brief       RIOT's modular default IP network stack.
 * @{
 *
 * @file
 * @brief       Pseudo header that includes all gnrc network stack base modules
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef GNRC_NETBASE_H_
#define GNRC_NETBASE_H_

#include "net/netopt.h"
#include "net/gnrc/netdev.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

/* this file does not provide anything on it's own */

#ifdef __cplusplus
}
#endif

#endif /* GNRC_NETBASE_H_ */
/** @} */
