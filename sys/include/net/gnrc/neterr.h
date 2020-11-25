/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_neterr Error reporting
 * @ingroup     net_gnrc
 * @brief       Allows for asynchronous error reporting in the network stack.
 * @{
 *
 * @file
 * @brief   Error reporting definitions.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_GNRC_NETERR_H
#define NET_GNRC_NETERR_H

#include <errno.h>
#include <stdint.h>

#include "msg.h"
#include "net/gnrc/pkt.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   @ref core_msg type for reporting an error.
 */
#define GNRC_NETERR_MSG_TYPE        (0x0206)

/**
 * @brief   Error code to signalise success (no error occurred) to an gnrc_neterr subscriber.
 */
#define GNRC_NETERR_SUCCESS         (0)

/**
 * @brief   Reports an error to all subscribers of errors to @p pkt.
 *
 * @param[in] pkt   Packet snip to report on.
 * @param[in] err   The error code for the packet.
 */
#ifdef MODULE_GNRC_NETERR
static inline void gnrc_neterr_report(gnrc_pktsnip_t *pkt, uint32_t err)
{
    if (pkt->err_sub != KERNEL_PID_UNDEF) {
        msg_t msg;

        msg.type = GNRC_NETERR_MSG_TYPE;
        msg.content.value = err;

        msg_send(&msg, pkt->err_sub);
    }
}
#else
#define gnrc_neterr_report(pkt, err)  (void)pkt; (void)err
#endif

/**
 * @brief   Registers the current thread for errors on a @ref gnrc_pktsnip_t.
 *
 * @param[in] pkt   Packet snip to register for errors.
 *
 * @return  0, on success.
 * @return  EALREADY, if there already someone registered to errors on @p pkt.
 */
#ifdef MODULE_GNRC_NETERR
static inline int gnrc_neterr_reg(gnrc_pktsnip_t *pkt)
{
    if (pkt->err_sub != KERNEL_PID_UNDEF) {
        return EALREADY;
    }
    pkt->err_sub = thread_getpid();
    return 0;
}
#else
#define gnrc_neterr_reg(pkt)  (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETERR_H */
/** @} */
