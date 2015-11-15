/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    lwip_opts   lwIP options
 * @ingroup     lwip
 * @brief       Options for the lwIP stack
 * @{
 *
 * @file
 * @brief   Option definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef LWIP_LWIPOPTS_H_
#define LWIP_LWIPOPTS_H_

#include "thread.h"
#include "net/gnrc/netif/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   lwIP configuration macros.
 * @see     lwIP documentation
 * @{
 */
#define LWIP_SOCKET             (0)
#define MEMP_MEM_MALLOC         (1)
#define NETIF_MAX_HWADDR_LEN    (GNRC_NETIF_HDR_L2ADDR_MAX_LEN)

#define TCPIP_THREAD_STACKSIZE  (THREAD_STACKSIZE_DEFAULT)

#define MEM_ALIGNMENT           (4)
#ifndef MEM_SIZE
/* packet buffer size of GNRC + stack for TCP/IP */
#define MEM_SIZE                (TCPIP_THREAD_STACKSIZE + 6144)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_LWIPOPTS_H_ */
/** @} */
