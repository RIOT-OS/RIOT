/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_msi
 * @{
 *
 * @file
 * @brief       MSI-like mailbox doorbell implementation
 *
 * @author      Ajit Upadhyay <u.ajiit@gmail.com>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>

#include "irq.h"
#include "msi.h"
#include "msi_arch.h"

typedef struct {
    volatile uint32_t valid;
    volatile uint32_t seq;
    volatile uint32_t event;
    volatile uint32_t data;
} msi_slot_t;

typedef struct {
    msi_cb_t cb;
    void *arg;
} msi_handler_t;

static msi_slot_t _slots[MSI_VEC_NUMOF];
static msi_handler_t _handlers[MSI_VEC_NUMOF];
static bool _initialized;

/**
 * @brief   Initialize the MSI-like mailbox and its doorbell backend
 *
 * @details
 * Passes the doorbell identifier to the architecture backend. Every mailbox
 * slot is then reset to the empty state and its sequence number and payload
 * are cleared.
 *
 * Slot initialization and doorbell configuration are performed with
 * interrupts disabled so that an interrupt cannot observe partially
 * initialized mailbox state.
 *
 * This function does not install the architecture-specific interrupt handler.
 * The application must connect the selected doorbell to an ISR that calls
 * @ref msi_isr from it.
 *
 * Calling this function again discards all pending mailbox messages. Existing
 * callback registrations are retained.
 *
 * @param[in] doorbell  Architecture-specific doorbell identifier
 *
 * @retval  0     The mailbox and doorbell backend were initialized
 * @retval  <0    The architecture backend rejected the doorbell
 */
int msi_init(int doorbell)
{
    unsigned state = irq_disable();
    int res = msi_arch_init(doorbell);

    if (res < 0) {
        irq_restore(state);
        return res;
    }

    for (unsigned i = 0; i < MSI_VEC_NUMOF; i++) {
        _slots[i].valid = 0;
        _slots[i].seq = 0;
        _slots[i].event = 0;
        _slots[i].data = 0;
    }

    _initialized = true;
    irq_restore(state);

    return 0;
}

/**
 * @brief   Register a callback for an MSI-like vector
 *
 * @details
 * Associates one mailbox vector with a callback and an opaque argument.
 * When @ref msi_isr consumes a message from this vector, it invokes @p cb
 * with the vector number, event code, payload, and @p arg.
 *
 * Registration is updated inside a critical section so the doorbell ISR
 * cannot observe a callback pointer and argument from different
 * registrations. Registering the same vector again replaces its previous
 * callback and argument. This operation does not alter or consume the
 * vector's mailbox slot.
 *
 * The callback executes in interrupt context and therefore must not block.
 *
 * @param[in] vec  Mailbox vector in the range `0 .. MSI_VEC_NUMOF - 1`
 * @param[in] cb   Non-NULL callback to invoke when the vector is consumed
 * @param[in] arg  Opaque argument passed unchanged to @p cb; may be NULL
 *
 * @retval  0         The callback was registered
 * @retval  -EINVAL   @p vec is outside the configured vector range or
 *                    @p cb is NULL
 */
int msi_register(unsigned vec, msi_cb_t cb, void *arg)
{
    if ((vec >= MSI_VEC_NUMOF) || (cb == NULL)) {
        return -EINVAL;
    }

    unsigned state = irq_disable();

    _handlers[vec].cb = cb;
    _handlers[vec].arg = arg;
    irq_restore(state);

    return 0;
}

/**
 * @brief   Post a message to a vector and ring the doorbell
 *
 * @details
 * Implements the sender side of the MSI-like protocol. The function enters a
 * critical section, verifies that the selected slot is empty, writes the
 * event and payload, increments the slot sequence number, and publishes the
 * message by setting `valid` last. It then asks the architecture backend to
 * trigger the doorbell selected by @ref msi_init.
 *
 * Only one message may be outstanding on each vector. If the slot is still
 * occupied, the function returns @c -EBUSY without modifying the existing
 * message. Different vectors have independent slots but share the same
 * doorbell.
 *
 * On a single-core target, disabling interrupts protects the slot's
 * check-and-publish operation against @ref msi_isr. Restoring interrupts may
 * cause the callback to execute before this function returns.
 *
 * @param[in] vec    Mailbox vector in the range
 *                   `0 .. MSI_VEC_NUMOF - 1`
 * @param[in] event  Application-defined event identifier
 * @param[in] data   Application-defined 32-bit payload
 *
 * @retval  0         The message was published and the doorbell was rung
 * @retval  -EINVAL   @p vec is invalid or @ref msi_init has not completed
 * @retval  -EBUSY    The selected vector already contains a pending message
 */
int msi_post(unsigned vec, uint32_t event, uint32_t data)
{
    if ((vec >= MSI_VEC_NUMOF) || !_initialized) {
        return -EINVAL;
    }

    msi_slot_t *slot = &_slots[vec];
    unsigned state = irq_disable();

    if (slot->valid) {
        irq_restore(state);
        return -EBUSY;
    }

    slot->event = event;
    slot->data = data;
    slot->seq++;
    /* Publish last: the ISR treats valid != 0 as a posted message. */
    slot->valid = 1;
    msi_arch_trigger();
    irq_restore(state);

    return 0;
}

/**
 * @brief   Consume pending mailbox messages and dispatch their callbacks
 *
 * @details
 * Implements the receiver side of the MSI-like protocol. The function scans
 * all vectors in ascending order. For every slot whose `valid` field is set,
 * it copies the event, payload, callback, and callback argument to local
 * variables, then clears `valid` to acknowledge the message and release the
 * slot.
 *
 * The callback is invoked only after its slot has been released. Consequently,
 * a callback may post a new message to the same vector without receiving
 * @c -EBUSY. A posted message with no registered callback is still consumed
 * and acknowledged.
 *
 * This function must be called by the architecture-specific handler connected
 * to the doorbell passed to @ref msi_init. It executes callbacks directly in
 * interrupt context; callbacks must remain short, must not block, and should
 * defer lengthy work to a thread.
 */
void msi_isr(void)
{
    for (unsigned vec = 0; vec < MSI_VEC_NUMOF; vec++) {
        msi_slot_t *slot = &_slots[vec];

        if (!slot->valid) {
            continue;
        }

        uint32_t event = slot->event;
        uint32_t data = slot->data;
        msi_cb_t cb = _handlers[vec].cb;
        void *arg = _handlers[vec].arg;

        slot->valid = 0;

        if (cb) {
            cb(vec, event, data, arg);
        }
    }
}
