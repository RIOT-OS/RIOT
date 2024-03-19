/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup     sys_bhp_msg Message based implementation of Bottom Half Processor
 * @ingroup      sys_bhp
 *
 * @note         Offloading ISRs with messages is in general a terrible idea,
 *               because messages can be lost. Use it with care or prefer
 *               alternatives such as @ref sys_bhp_event.
 *
 * @brief        Bottom Half Processor module for generic IRQ offloading using
 *               messages.
 * @{
 *
 * @author       José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef BHP_MSG_H
#define BHP_MSG_H

#include "msg.h"
#include "thread.h"
#include "bhp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The message type to trigger Bottom Half Processing
 */
#define BHP_MSG_BH_REQUEST 0x1539

/**
 * @brief Message based Bottom Half Processor descriptor
 */
typedef struct {
    bhp_t bhp;          /**< Bottom Half Processor descriptor */
    kernel_pid_t pid;   /**< PID of the thread that process the Bottom Half Processor */
    msg_t msg;          /**< Message containing the Bottom Half Processing request */
} bhp_msg_t;

/**
 * @brief Init a Bottom Half Processor to be used with messages
 * @post the target PID is set to @ref KERNEL_PID_UNDEF
 *
 * @param[in] bhp_msg   The message based Bottom Half Processor descriptor
 * @param[in] cb        Bottom Half Processor callback
 * @param[in] ctx       Context of @p cb
 */
void bhp_msg_init(bhp_msg_t *bhp_msg, bhp_cb_t cb, void *ctx);

/**
 * @brief Message based Bottom Half Processor ISR callback
 *        To be called from ISR in order to trigger the Bottom Half Processor.
 *        This sends a message with type @ref BHP_MSG_BH_REQUEST to the
 *        target thread, which should process the event accordingly by calling
 *        @ref bhp_msg_handler.
 *
 * @note This function asserts that the target PID is valid. Therefore, make sure
 * to call @ref bhp_msg_claim_thread beforehand.
 *
 * @param[in] bhp_msg_ctx     Pointer to the message based Bottom Half Processor
 * (@ref bhp_msg_t)
 *                            Bottom Half Processor.
 */
void bhp_msg_isr_cb(void *bhp_msg_ctx);

/**
 * @brief Claim a thread with a message queue to be used as Bottom Half Processor.
 *
 * @param[in] bhp_msg    The message based Bottom Half Processor descriptor
 * @param[in] pid        PID of the target thread
 */
static inline void bhp_msg_claim_thread(bhp_msg_t *bhp_msg, kernel_pid_t pid)
{
    bhp_msg->pid = pid;
}

/**
 * @brief   Handle a Bottom Half Processor message with type @ref
 *          BHP_MSG_BH_REQUEST.
 *          This function will call @ref bhp_irq_handler internally.
 *
 * @note This function asserts that the message type is @ref BHP_MSG_BH_REQUEST.
 *
 * @param[in] msg   Pointer to the Bottom Half Processor message.
 */
static inline void bhp_msg_handler(msg_t *msg)
{
    assert(msg->type == BHP_MSG_BH_REQUEST);
    bhp_irq_handler(msg->content.ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* BHP_MSG_H */
/** @} */
