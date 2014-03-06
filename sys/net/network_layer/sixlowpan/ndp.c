#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "kernel.h"
#include "msg.h"
#include "sixlowpan/ndp.h"
#include "sixlowpan/types.h"
#include "vtimer.h"

#include "ndp.h"
#include "ip.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define NDP_TIMER_STACK_SIZE        (KERNEL_CONF_STACKSIZE_DEFAULT)
#define NDP_TIMER_QUEUE_SIZE        (64)
#define NDP_TIMER_CALLBACK_BUFFER   (30)

/**
 * @brief   The time for the delay timeout in micro seconds
 *
 * @see <a href="http://tools.ietf.org/html/rfc4861#section-10">
 *          RFC 4861, section 10
 *      </a>.
 */
#define NDP_DELAY_FIRST_PROBE_TIME  (5000000)
#define RETURN_ERROR(nr)            errno = nr; return nr

#define NDP_CALLBACK_DELAY_TIMEOUT_ARGS_NUM             (1)
#define NDP_CALLBACK_RETRANSMISSION_TIMEOUT_ARGS_NUM    (6)
#define NDP_CALLBACK_REACHABILITY_TIMEOUT_ARGS_NUM      (1)

static msg_t ndp_timer_queue[NDP_TIMER_QUEUE_SIZE];
static char ndp_timer_queue[NDP_TIMER_STACK_SIZE];
static int ndp_timer_thread_pid = -1;

typedef struct {
    uint8_t active;
    vtimer_t timer;
    void (*callback)(void **);
    void **args;
} ndp_timer_callback_t;

static ndp_timer_callback_t ndp_timer_callback_buffer[NDP_TIMER_CALLBACK_BUFFER];

int ndp_timer_set_callback(vtimer_t *t, int usec, ndp_timer_callback_t *callback);
int ndp_state_change_on_retransmission_timeout(ndp_neighbor_cache_t *nce,
                                               int last_retries_count,
                                               ipv6_addr_t *src,
                                               ipv6_addr_t *dest,
                                               ipv6_addr_t *target,
                                               uint8_t aro);
int ndp_state_change_on_reachabilaty_timeout(ndp_neighbor_cache_t *nce);
int ndp_state_change_on_delay_timeout(ndp_neighbor_cache_t *nce)

int ndp_state_change_handle_na(ndp_neighbor_cache_t *nce, int if_id,
                               icmpv6_hdr_t *ndp_message,
                               icmpv6_ndp_opt_stllao_t *sllao);
int ndp_state_change_handle_ndp_message(ndp_neighbor_cache_t *nce, int if_id,
                                        ipv6_addr_t *ipaddr,
                                        icmpv6_hdr_t *ndp_message,
                                        icmpv6_ndp_opt_stllao_t stllao);

void ndp_timer_thread(void)
{
    msg_t msg;
    msg_init_queue(ndp_timer_queue, NDP_TIMER_QUEUE_SIZE);
    while(1) {
        msg_receive(&msg);
        if (msg.type == MSG_TIMER) {
            ndp_timer_callback_t *t = (ndp_timer_callback_t *)msg.content.ptr;
            t->callback(t->args);
            t->active = 0;
        }
    }
}

int ndp_timer_set_callback(int usec, void (*callback)(void **), void **args)
{
    timex_t time = {0, usec};
    ndp_timer_callback_t *c;

    for (int i = 0; i < NDP_TIMER_CALLBACK_BUFFER; i++) {
        if (!ndp_timer_callback_buffer[i].active) {
            c = &ndp_timer_callback_buffer[i];
            break;
        }
    }

    if (i >= NDP_TIMER_CALLBACK_BUFFER) {
        errno = ENOBUFS;
        return -ENOBUFS;
    }

    c->active = 1;
    c->callback = callback;
    c->args = args;

    return vtimer_set_msg(&c.timer, time, ndp_timer_thread_pid, (int)c);
}

void **ndp_callback_delay_timeout_set_args(ndp_neighbor_cache_t *nce)
{
    void **args = malloc(sizeof(void *) * NDP_CALLBACK_DELAY_TIMEOUT_ARGS_NUM);

    args[0] = nce;

    return args;
}

void **ndp_callback_reachability_timeout_set_args(ndp_neighbor_cache_t *nce)
{
    void **args = malloc(sizeof(void *) * NDP_CALLBACK_REACHABILITY_TIMEOUT_ARGS_NUM);

    args[0] = nce;

    return args;
}

void **ndp_callback_retransmission_timeout_set_args(ndp_neighbor_cache_t *nce,
                                                    int last_retries_count,
                                                    ipv6_addr_t *src,
                                                    ipv6_addr_t *dest,
                                                    ipv6_addr_t *target,
                                                    uint8_t aro)
{
    void **args = malloc(sizeof(void *) * NDP_CALLBACK_RETRANSMISSION_TIMEOUT_ARGS_NUM);

    args[0] = nce;

    args[1] = malloc(sizeof(int));
    *(args[1]) = ++last_retries_count;

    args[2] = malloc(sizeof(ipv6_addr_t));
    memcpy(args[2], src, sizeof(ipv6_addr_t));

    args[3] = malloc(sizeof(ipv6_addr_t));
    memcpy(args[3], dest, sizeof(ipv6_addr_t));

    args[4] = malloc(sizeof(ipv6_addr_t));
    memcpy(args[4], target, sizeof(ipv6_addr_t));

    args[5] = malloc(sizeof(uint8_t));
    *(args[5]) = aro;

    return args;
}

void ndp_callback_delay_timeout_free_args(void **args)
{
    free(args);
}

void ndp_callback_retransmission_timeout_free_args(void **args)
{
    for (int i = 1; i < NDP_CALLBACK_RETRANSMISSION_TIMEOUT_ARGS_NUM; i++) {
        free(args[i]);
    }

    free(args);
}

void ndp_callback_reachability_timeout_free_args(void **args)
{
    free(args);
}

void ndp_callback_delay_timeout(void **args)
{
    ndp_neighbor_cache_t *nce = args[0];

    if (ndp_state_change_on_delay_timeout(nce))
    {
        DEBUG("Error occured [errno: %d]\n", errno);
    }
}

void ndp_callback_retransmission_timeout(void **args) {
    ndp_neighbor_cache_t *nce = args[0];
    int *last_retries_count = args[1];
    ipv6_addr_t *src = args[2];
    ipv6_addr_t *dest = args[3];
    ipv6_addr_t *target = args[4];
    uint8_t *aro = args[5];

    if (ndp_state_change_on_retransmission_timeout(nce, *last_retries_count,
                                                   src, dest, target, *aro))
    {
        DEBUG("Error occured [errno: %d]\n", errno);
    }
}

void ndp_callback_reachability_timeout(void **args)
{
    ndp_neighbor_cache_t *nce = args[0];

    if (ndp_state_change_on_reachabilaty_timeout(nce))
    {
        DEBUG("Error occured [errno: %d]\n", errno);
    }
}

static inline int ndp_nce_lladdr_equals_stllao(ndp_neighbor_cache_t *nce,
                                               icmpv6_ndp_opt_stllao_t *stllao)
{
    uint8_t length = ndp_get_link_layer_address_len_from_llao(stllao);

    return nce->lladdr_len == length && memcmp(nce->lladdr, stllao + 1, length);
}

static inline uint8_t ndp_nce_store_from_stllao(ndp_neighbor_cache_t *nce,
                                                icmpv6_ndp_opt_stllao_t *stllao)
{
    uint8_t length = ndp_get_link_layer_address_len_from_llao(stllao);

    if (length > 0) {
        memcpy(nce->lladdr, stllao + 1, length);
    }
    else {
        errno = EINVAL;
    }

    return length;
}

int ndp_state_change_on_ipv6_packet_send(ndp_neighbor_cache_t *nce, int if_id,
                                         ipv6_addr_t *src, ipv6_addr_t *dest,
                                         uint8_t aro)
{
    if (nce == NULL) {
        ipv6_addr_t sol_nodes;
        int retrans_timer;

        if (!ipv6_net_if_get_ext(if_id)) {
            RETURN_ERROR(EINVAL);
        }

        retrans_timer = ipv6_net_if_get_ext(if_id)->adv_retrans_timer;

        if (ndp_neighbor_cache_add(if_id, dest, NULL, 0, 0,
                                   NDP_NCE_STATUS_INCOMPLETE,
                                   NDP_NCE_TYPE_TENTATIVE,
                                   NDP_NS_RETRANSMISSION_TIMEOUT * 3) != 
                NDP_OPT_ARO_STATE_SUCCESS) {
            RETURN_ERROR(ENOBUFS);
        }
        ipv6_addr_set_solicited_node_addr(&sol_nodes, dest);
        icmpv6_send_neighbor_sol(src, sol_nodes, dest, OPT_SLLAO, aro);
        void **args = ndp_callback_retransmission_timeout_set_args(nce, 0, src,
                sol_nodes, dest, aro);

        if (ndp_timer_set_callback(retrans_timer * 1000,
                                   ndp_callback_retransmission_timeout,
                                   args)) {
            DEBUG("Error on retransmission timer start [errno %d]\n",
                  errno);
            ndp_callback_retransmission_timeout_free_args(args);
            return errno;
        }

        return 0;
    } 
    else if (nce->state = NDP_NCE_STATUS_STALE) {
        void **args = ndp_callback_delay_timeout_set_args(nce);

        if (ndp_timer_set_callback(NDP_DELAY_FIRST_PROBE_TIME,
                                   ndp_callback_delay_timeout,
                                   args)) {
            DEBUG("Error on delay timer start [errno %d]\n",
                  errno);
            ndp_callback_delay_timeout_set_args(args);
            return errno;
        }
        return 0;
    }

    RETURN_ERROR(EINVAL);
}

int ndp_state_change_handle_na(ndp_neighbor_cache_t *nce, int if_id,
                               icmpv6_neighbor_adv_hdr_t *na,
                               icmpv6_ndp_opt_stllao_t *sllao)
{
    uint8_t lladdr_len = 0;

    if (nce == NULL || !ipv6_net_if_get_ext(if_id)) {
        RETURN_ERROR(EINVAL);
    }

    if (sllao && !(lladdr_len = ndp_get_link_layer_address_len_from_llao(sllao))) {
        return errno;
    }

    if (nce->state == NDP_NCE_STATUS_INCOMPLETE) {
        if (sllao) {
            if (!(na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED)) {
                nce->state = NDP_NCE_STATUS_STALE;
            }
            else {
                void **args = ndp_callback_reachability_timeout_set_args(nce);
                int reachable_time = ipv6_net_if_get_ext(if_id)->adv_reachable_time;
                // TODO: send queued packets

                nce->lladdr_len = (uint8_t)lladdr_len;
                nce->if_id = if_id
                nce->state = NDP_NCE_STATUS_REACHABLE;
                nce->isrouter = (na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_ROUTER);

                if (ndp_timer_set_callback(reachable_time * 1000,
                            ndp_callback_reachability_timeout, args)) {
                    DEBUG("Error on reachability timer start [errno %d]\n",
                          errno);
                    ndp_callback_retransmission_timeout_free_args(args);
                    return errno;
                }
            }
        }
    }
    else {
        if ((na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED) &&
            !(na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE)) {

            /* if stored link layer address is the same as in sllao */
            if (sllao && ndp_nce_lladdr_equals_stllao(nce, sllao)) {
                nce->state == NDP_NCE_STATUS_REACHABLE;
            }
            else {
                if (nce->state == NDP_NCE_STATUS_REACHABLE) {
                    nce->state = NDP_NCE_STATUS_STALE;
                }
            }
        }
        else if ((na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED) &&
                 (na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE)) {
            /* if different link-layer address in sllao */
            if (!sllao || !ndp_nce_lladdr_equals_stllao(nce, sllao)) {
                ndp_nce_store_from_stllao(nce, sllao)
                nce->state = NDP_NCE_STATUS_REACHABLE;
            }
        }
        else if (!(na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_SOLICITED) &&
                 (na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_OVERRIDE)) {
            if (!sllao || !ndp_nce_lladdr_equals_stllao(nce, sllao)) {
                ndp_nce_store_from_stllao(nce, sllao)
                nce->state = NDP_NCE_STATUS_STALE;
            }
        }
    }

    nce->isrouter = (na->rso & ICMPV6_NEIGHBOR_ADV_FLAG_ROUTER) != 0;

    return 0;
}


int ndp_state_change_handle_ndp_message(ndp_neighbor_cache_t *nce,
                                        int if_id,
                                        ipv6_addr_t *ipaddr,
                                        icmpv6_hdr_t *ndp_message,
                                        icmpv6_ndp_opt_stllao_t *stllao)
{
    uint8_t lladdr_len = 0;

    if (!(lladdr_len = ndp_get_link_layer_address_len_from_llao(sllao))) {
        return errno;
    }

    if (nce == NULL) {
        if (stllao) {
            int lladdr_length;
            uint8_t lladdr[8];

            if (ndp_message->type == ICMPV6_TYPE_ROUTER_ADV) {
                RETURN_ERROR(EINVAL);
            }

            if (ndp_neighbor_cache_add(if_id, ipaddr, lladdr, (uint8_t)lladdr_len,
                                       0, NDP_NCE_STATUS_STALE,
                                       NDP_NCE_TYPE_TENTATIVE, 0xffff)) {
                return 0;
            }
            else {
                RETURN_ERROR(ENOBUFS);
            }
        }
        else {
            return 0;
        }
    }

    if (nce->state == NDP_NCE_STATUS_INCOMPLETE) {
        if (stllao == NULL && ndp_message->type != ICMPV6_TYPE_NEIGHBOR_SOL &&
                ndp_message->type != ICMPV6_TYPE_ROUTER_SOL &&
                ndp_message->type != ICMPV6_TYPE_REDIRECT) {
            RETURN_ERROR(EINVAL);
        }
        else if (stllao) {
            ndp_nce_store_from_stllao(nce, stllao);
            // TODO send queued packets
        }
    }
    else if (stllao) {
        if (!ndp_nce_lladdr_equals_stllao(nce, stllao)) {
            ndp_nce_store_from_stllao(nce, stllao);
        }
        nce->state = NDP_NCE_STATUS_STALE;
    }

    return 0;
}

int ndp_state_change_on_ndp_message_receive(ndp_neighbor_cache_t *nce, int if_id,
                                            ipv6_addr_t *ipaddr,
                                            icmpv6_hdr_t *ndp_message)
{
    if (ndp_message->type == ICMPV6_TYPE_NEIGHBOR_ADV) {
        return ndp_state_change_handle_na(nce, if_id, ndp_message, sllao);
    } 
    else {
        return ndp_state_change_handle_ndp_message(nce, if_id, ipaddr,
                                                   ndp_message, sllao);
    }

    return 0; 
}

int ndp_state_change_on_upper_layer_reachability_conf(ndp_neighbor_cache_t *nce)
{
    if (nce == NULL || nce->state == NDP_NCE_STATUS_INCOMPLETE) {
        RETURN_ERROR(EINVAL);
    }

    nce->state = NDP_NCE_STATUS_REACHABLE;
    return 0;
}

int ndp_state_change_on_delay_timeout(ndp_neighbor_cache_t *nce)
{
    if (nce == NULL || nce->state != NDP_NCE_STATUS_DELAY) {
        RETURN_ERROR(EINVAL);
    }

    nce->state = NDP_NCE_STATUS_PROBE;
    return 0;
}

int ndp_state_change_on_retransmission_timeout(ndp_neighbor_cache_t *nce,
                                               int last_retries_count,
                                               ipv6_addr_t *src,
                                               ipv6_addr_t *dest,
                                               ipv6_addr_t *target,
                                               uint8_t aro)
{
    if (nce == NULL ||
        (nce->state != NDP_NCE_STATUS_INCOMPLETE && nce->state != NDP_NCE_STATUS_PROBE) ||
        !ipv6_net_if_get_ext(nce->if_id)) {
        RETURN_ERROR(EINVAL);
    }

    if (*last_retries_count < NDP_NS_RETRANSMISSION_RETRIES) {
        void **args = ndp_callback_retransmission_timeout_set_args(nce,
                last_retries_count, src, dest, target, aro);
        int retrans_timer = ipv6_net_if_get_ext(nce->if_id);

        icmpv6_send_neighbor_sol(src, dest, target, OPT_SLLAO, aro);

        if (ndp_timer_set_callback(retrans_timer * 1000,
                                   ndp_callback_retransmission_timeout,
                                   args)) {
            DEBUG("Error on retransmission timer start [errno %d]\n",
                  errno);
            ndp_callback_retransmission_timeout_free_args(args);
            return errno;
        }
    }
    else {
        if (nce->state == NDP_NCE_STATUS_INCOMPLETE) {
            // TODO: send ICMPv6 error with code 3
        }

        nbr_cache_rem(dest);
    }

    return 0;
}

int ndp_state_change_on_reachabilaty_timeout(ndp_neighbor_cache_t *nce)
{
    if (nce == NULL || nce->state != NDP_NCE_STATUS_REACHABLE) {
        RETURN_ERROR(EINVAL);
    }

    nce->state = NDP_NCE_STATUS_STALE;
    return 0;
}
