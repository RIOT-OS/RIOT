/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup net_unicoap
 * @brief   Core functionality
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include <errno.h>

#include "container.h"
#include "event.h"
#include "mutex.h"
#include "random.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG CONFIG_UNICOAP_DEBUG_LOGGING
#include "debug.h"
#include "private.h"

typedef struct {
#if UNICOAP_HAVE_CLIENT_STATE || DOXYGEN
    /** @ref unicoap_memo_t */
    unicoap_client_memo_t client_memos[CONFIG_UNICOAP_CLIENT_MEMOS_CAPACITY];
#endif

    /* TODO: Advanced client features: Exchange-layer state objects */
} _client_state_t;

/**
 * @brief State of the unicoap stack.
 *
 * @pre access from the thread with @ref _unicoap_pid, or while it is @ref KERNEL_PID_UNDEF
 */
static inline _client_state_t *_state(void)
{
    static _client_state_t _state = {};

    assert(thread_getpid() == _unicoap_pid || _unicoap_pid == KERNEL_PID_UNDEF);
    return &_state;
}

/** @brief Gets client memo array from @p state object */
static inline unicoap_client_memo_t *_state_get_client_memos(void) {
#if UNICOAP_HAVE_CLIENT_STATE
    return _state()->client_memos;
#else
    (void)state;
    return NULL;
#endif
}

/** @brief Gets length of client memo array from @p state object */
static inline size_t _state_get_client_memos_capacity(void) {
    return CONFIG_UNICOAP_CLIENT_MEMOS_CAPACITY;
}

/* MARK: - State */

static unicoap_client_memo_t* _alloc_client(void) {
    /* Find empty slot in list of transactions */
    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    for (size_t i = 0; i < _state_get_client_memos_capacity(); i += 1) {
        if (client_memos[i].super.endpoint.proto == UNICOAP_PROTO_UNSPECIFIED) {
            return &client_memos[i];
        }
    }
    _STATE_DEBUG("[client] no space to alloc\n");
    return NULL;
}

bool unicoap_memo_is_client(unicoap_memo_t* memo)
{
    (void)memo;
    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    if (!client_memos) {
        return false;
    }
    return (uintptr_t)memo >= (uintptr_t)&client_memos[0] &&
           (uintptr_t)memo <= (uintptr_t)&client_memos[_state_get_client_memos_capacity() - 1];
}

static inline size_t _client_index(unicoap_client_memo_t* memo)
{
    /* debugging-only */
    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    assert(client_memos);
    return ((uintptr_t)memo - (uintptr_t)client_memos) / sizeof(*client_memos);
}

static void _deinit_client(unicoap_client_memo_t* memo)
{
    if (!UNICOAP_HAVE_CLIENT_STATE) {
        return;
    }
    _STATE_DEBUG("[client #%" PRIuSIZE "] release\n", _client_index(memo));
    memo->callback._any = NULL;
    memo->callback_arg = NULL;
    memo->flags = 0;
#if IS_USED(MODULE_UNICOAP_CLIENT_CANCELLATION)
    memo->reference_id = 0;
#endif
}

unicoap_client_memo_t* unicoap_client_memo_create(const unicoap_endpoint_t* endpoint)
{
    if (!UNICOAP_HAVE_CLIENT_STATE) {
        return NULL;
    }
    assert(endpoint);
    assert(endpoint->proto != UNICOAP_PROTO_UNSPECIFIED);
    unicoap_client_memo_t* memo = _alloc_client();
    if (memo) {
        _STATE_DEBUG("[client #%" PRIuSIZE "] alloc\n", _client_index(memo));
        memo->super.endpoint = *endpoint;
    }
    return memo;
}

static void _free(unicoap_memo_t* memo)
{
    unicoap_event_cancel(&memo->exchange.timeout);
    memset(memo, 0, sizeof(*memo));
}

void unicoap_client_memo_free(unicoap_client_memo_t* memo, int error)
{
    if (!UNICOAP_HAVE_CLIENT_STATE) {
        return;
    }
    assert(memo);
    unicoap_proto_t proto = memo->super.endpoint.proto;
    memo->super.endpoint.proto = UNICOAP_PROTO_UNSPECIFIED;
    _deinit_client(memo);
    /* On the exchange layer, we're hiding the actual reason from the driver. It should
     * not matter to the messaging layer whether we release state because of an error or because
     * the state object is released as usual. Should the messaging layer rely on this
     * information, the exchange-messaging abstraction has a design flaw. */
    if (unicoap_memo_messaging_state(&memo->super)) {
        unicoap_messaging_notify(
            unicoap_memo_messaging_state(&memo->super),
            error ? unicoap_layer_notification_async_failure_from_errno(error) : UNICOAP_LAYER_NOTIFICATION_STATE_RELEASE,
            NULL, proto);
    }
    _free(&memo->super);
}

void unicoap_exchange_notify_all_client(const unicoap_endpoint_t* endpoint, unicoap_layer_notification_t type, void* arg)
{
    unicoap_memo_t* memo = NULL;
    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    for (size_t i = 0; i < _state_get_client_memos_capacity(); i += 1) {
        memo = &client_memos[i].super;
        if (unicoap_endpoint_is_equal(endpoint, &memo->endpoint)) {
            unicoap_exchange_notify(memo, type, arg);
        }
    }
}

unicoap_client_memo_t* unicoap_client_memo_find_token(const unicoap_endpoint_t* endpoint,
                                                      const uint8_t* token, size_t token_length)
{
    assert(token);

    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    for (size_t i = 0; i < _state_get_client_memos_capacity(); i += 1) {
        unicoap_client_memo_t* memo = &client_memos[i];

        if ((!endpoint || unicoap_endpoint_is_equal(&memo->super.endpoint, endpoint)) &&
            token_length == sizeof(memo->token) &&
            memcmp(memo->token, token, token_length) == 0) {
            return memo;
        }
    }
    return NULL;
}

#define UNICOAP_REFNO_MASK_MIN_INDEX (0x7000)
#define UNICOAP_REFNO_MASK_ID        (0xfff)

unicoap_client_memo_t* unicoap_client_memo_find_refno(int refno)
{
    (void)refno;
    assert(refno > 0);
#if IS_USED(MODULE_UNICOAP_CLIENT_CANCELLATION)
    /* First bit is sign bit, then 3 bits for min index and then 12 bits of randomness.
     * For 16 or fewer memos, we thus don't have to search at all. */
    size_t index_min = (refno & UNICOAP_REFNO_MASK_MIN_INDEX) >> 12;
    uint16_t reference_id = refno & UNICOAP_REFNO_MASK_ID;
    _STATE_DEBUG("find refno=%i (min_client_ix=#%" PRIuSIZE ", refid=%u)\n", refno, index_min, reference_id);
    unicoap_client_memo_t *client_memos = _state_get_client_memos();
    for (size_t i = index_min; i < _state_get_client_memos_capacity(); i += 1) {
        unicoap_client_memo_t* memo = &client_memos[i];
        if (memo->reference_id == reference_id) {
            return memo;
        }
    }
#endif
    return NULL;
}

int unicoap_client_memo_assign_refno(unicoap_client_memo_t* memo)
{
    (void)memo;
    assert(memo);
#if IS_USED(MODULE_UNICOAP_CLIENT_CANCELLATION)
    /* Memos require a unique reference ID and array index for fast lookup. Because functions return
     * the refno as an int where negative values represent errors, we assume a minimum 16-bit int
     * and use the 15 positive bits.
     * 15 available bits  =  12 bits (reference ID) + 3 bits (minimum index)
     * Since 3 bits represent a maximum value of 7, we store the minimum index instead of the full
     * size_t. This provides O(1) lookup for up to 7 memos and narrows the search space for larger
     * arrays. */
    memo->reference_id = random_uint32_range(1, 0xfff);                     /* 12 bits for reference ID */
    int refno = memo->reference_id | (MIN(_client_index(memo), 0x7) << 12); /* 3 bits min index */
    _STATE_DEBUG("assign refno=%i (min_client_ix=#%" PRIuSIZE ", refid=%u)\n",
                 refno, MIN(_client_index(memo), 0x7), memo->reference_id);
    return refno;
#else
    return 0;
#endif
}

int unicoap_init_client(event_queue_t *queue)
{
    (void)queue;
    return 0;
}
