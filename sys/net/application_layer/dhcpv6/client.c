/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <stdbool.h>

#include "event.h"
#include "event/timeout.h"
#include "log.h"
#include "kernel_defines.h"
#include "net/dhcpv6/client.h"
#include "net/dhcpv6.h"
#include "net/sock/udp.h"
#include "random.h"
#include "timex.h"
#if IS_USED(MODULE_ZTIMER)
#include "ztimer.h"
#else
#include "xtimer.h"
#include "xtimer/implementation.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#include "_dhcpv6.h"

/**
 * @brief   Representation of a generic lease
 */
typedef struct {
    union {
        uint32_t id;
        struct {
            uint16_t netif;
            uint16_t type;
        } info;
    } ia_id;
} lease_t;

/**
 * @brief   Representation of a DHCPv6 prefix deligation lease
 * @extends lease_t
 */
typedef struct {
    lease_t parent;
    ipv6_addr_t pfx;
    uint8_t pfx_len;
    uint8_t leased;
} pfx_lease_t;

/**
 * @brief   Representation of a DHCPv6 address lease
 * @extends lease_t
 */
typedef struct {
    lease_t parent;
    ipv6_addr_t addr;
    uint8_t leased;
} addr_lease_t;

/**
 * @brief   Client representation of a DHCPv6 server
 */
typedef struct {
    dhcpv6_duid_t duid;
    uint32_t t1;
    uint8_t pref;
    uint8_t duid_len;
} server_t;

static uint8_t send_buf[DHCPV6_CLIENT_SEND_BUFLEN];
static uint8_t recv_buf[DHCPV6_CLIENT_BUFLEN];
static uint8_t best_adv[DHCPV6_CLIENT_BUFLEN];
static uint8_t duid[DHCPV6_CLIENT_DUID_LEN];
static addr_lease_t addr_leases[CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX];
static pfx_lease_t pfx_leases[CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX];
static server_t server;
static event_timeout_t solicit_renew_timeout, rebind_timeout;
static event_queue_t *event_queue;
static sock_udp_t sock;
static sock_udp_ep_t local = { .family = AF_INET6, .port = DHCPV6_CLIENT_PORT };
static sock_udp_ep_t remote = { .family = AF_INET6, .port = DHCPV6_SERVER_PORT,
                                .addr = {
                                   .ipv6 = DHCPV6_ALL_RELAY_AGENTS_AND_SERVERS
                                } };
static uint32_t sol_max_rt = DHCPV6_SOL_MAX_RT;
static uint32_t t2, rebind_time;
static uint32_t transaction_start;
static uint32_t transaction_id;
static uint8_t duid_len = sizeof(dhcpv6_duid_l2_t);

static const char mud_url[] = CONFIG_DHCPV6_CLIENT_MUD_URL;

static void _post_solicit_servers(void);
static void _solicit_servers(event_t *event);
static void _request(event_t *event);
static void _renew(event_t *event);
static void _rebind(event_t *event);

static void _set_event_timeout_ms(event_timeout_t *timeout, event_t *event,
                                  uint32_t delay_ms);
static void _set_event_timeout_sec(event_timeout_t *timeout, event_t *event,
                                   uint32_t delay_sec);
static void _clear_event_timeout(event_timeout_t *timeout);

static event_t solicit_servers = { .handler = _solicit_servers };
static event_t request = { .handler = _request };
static event_t renew = { .handler = _renew };
static event_t rebind = { .handler = _rebind };

#ifdef MODULE_AUTO_INIT_DHCPV6_CLIENT
static char _thread_stack[DHCPV6_CLIENT_STACK_SIZE];
static void *_thread(void *args);
static kernel_pid_t _thread_pid;

void dhcpv6_client_auto_init(void)
{
    if (_thread_pid <= 0) {
        _thread_pid = thread_create(_thread_stack, DHCPV6_CLIENT_STACK_SIZE,
                                    DHCPV6_CLIENT_PRIORITY,
                                    THREAD_CREATE_STACKTEST,
                                    _thread, NULL, "dhcpv6-client");
    }
}

static void *_thread(void *args)
{
    (void)args;
    event_queue_t event_queue;
    event_queue_init(&event_queue);
    dhcpv6_client_init(&event_queue, SOCK_ADDR_ANY_NETIF);
    dhcpv6_client_start();
    event_loop(&event_queue);   /* never returns */
    return NULL;
}
#endif /* MODULE_AUTO_INIT_DHCPV6_CLIENT */

void _print_ia_na_debug_info(uint16_t netif, int result_code)
{
    if (result_code == 0) {
        return;
    } else {
        DEBUG("DHCPv6 client: No free address lease available to configure "
              "IA_NA for network interface %i\n", netif);
    }
}

void _initialize_ia_na(uint16_t netif)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
        return;
    }

    int res;

    /* If no specific interface ID is given, check all
       interfaces if DHCP IA_NA is enabled. Otherwise
       use the specific interface ID. */
    if (netif == SOCK_ADDR_ANY_NETIF) {
        netif_t* netif = NULL;
        while ((netif = netif_iter(netif))) {
            int16_t netif_id = netif_get_id(netif);
            if (netif_id < 0) {
                continue;
            }
            if (dhcpv6_client_check_ia_na(netif_id)) {
                res = dhcpv6_client_req_ia_na(netif_id);
                _print_ia_na_debug_info(netif_id, res);
            }
        }
    } else if (dhcpv6_client_check_ia_na(netif)) {
        res = dhcpv6_client_req_ia_na(netif);
        _print_ia_na_debug_info(netif, res);
    }
}

void dhcpv6_client_init(event_queue_t *eq, uint16_t netif)
{
    assert(eq->waiter != NULL);
    if (IS_USED(MODULE_DHCPV6_CLIENT_MUD_URL)) {
        assert(strlen(mud_url) <= MAX_MUD_URL_LENGTH);
        assert(strncmp(mud_url, "https://", 8) == 0);
    }

    _initialize_ia_na(netif);

    event_queue = eq;
    local.netif = netif;
    remote.netif = netif;
}

void dhcpv6_client_start(void)
{
    duid_len = dhcpv6_client_get_duid_l2(local.netif,
                                         (dhcpv6_duid_l2_t *)&duid);
    assert(event_queue != NULL);
    if (duid_len > 0) {
        uint32_t delay = random_uint32_range(0, DHCPV6_SOL_MAX_DELAY * MS_PER_SEC);

        sock_udp_create(&sock, &local, NULL, 0);
        _set_event_timeout_ms(&solicit_renew_timeout, &solicit_servers, delay);
    }
}

void dhcpv6_client_req_ia_pd(unsigned netif, unsigned pfx_len)
{
    pfx_lease_t *lease = NULL;

    assert(IS_USED(MODULE_DHCPV6_CLIENT_IA_PD));
    assert(pfx_len <= 128);
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_PD)) {
        LOG_WARNING("DHCPv6 client: Unable to request IA_PD as module "
                    "`dhcpv6_client_ia_pd` is not used\n");
        return;
    }
    for (unsigned i = 0; i < CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
        if (pfx_leases[i].parent.ia_id.id == 0) {
            lease = &pfx_leases[i];
            lease->parent.ia_id.info.netif = netif;
            lease->parent.ia_id.info.type = DHCPV6_OPT_IA_PD;
            lease->pfx_len = pfx_len;
            break;
        }
    }
}

int dhcpv6_client_req_ia_na(unsigned netif)
{
    assert(IS_USED(MODULE_DHCPV6_CLIENT_IA_NA));
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
        LOG_WARNING("DHCPv6 client: Unable to request IA_NA as module "
                    "`dhcpv6_client_ia_na` is not used\n");
        return -ENOTSUP;
    }

    addr_lease_t *lease = NULL;

    for (unsigned i = 0; i < CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX; i++) {
        if (addr_leases[i].parent.ia_id.id == 0) {
            lease = &addr_leases[i];
            lease->parent.ia_id.info.netif = netif;
            lease->parent.ia_id.info.type = DHCPV6_OPT_IA_NA;
            return 0;
        }
    }

    return -ENOMEM;
}

static void _post_solicit_servers(void)
{
    event_post(event_queue, &solicit_servers);
}

static void _generate_tid(void)
{
    transaction_id = random_uint32() & 0xffffff;
}

static void _set_tid(uint8_t *tgt)
{
    tgt[0] = (transaction_id & 0xff0000) >> 16;
    tgt[1] = (transaction_id & 0xff00) >> 8;
    tgt[2] = transaction_id & 0xff;
}

static inline bool _is_tid(dhcpv6_msg_t *msg)
{
    uint32_t tid = (((uint32_t)msg->tid[0]) << 16) |
                   (((uint32_t)msg->tid[1]) << 8) |
                   (msg->tid[2]);

    return (transaction_id == (tid));
}

static inline uint32_t _now_cs(void)
{
#if IS_USED(MODULE_ZTIMER)
    return (uint32_t)(ztimer_now(ZTIMER_MSEC) / MS_PER_CS);
#else
    return (uint32_t)(xtimer_now_usec64() / US_PER_CS);
#endif
}

static inline uint32_t _now_sec(void)
{
#if IS_USED(MODULE_ZTIMER)
    return (uint32_t)ztimer_now(ZTIMER_SEC);
#else
    return (uint32_t)(xtimer_now_usec64() / US_PER_SEC);
#endif
}

static inline uint16_t _compose_cid_opt(dhcpv6_opt_duid_t *cid)
{
    uint16_t len = duid_len;

    cid->type = byteorder_htons(DHCPV6_OPT_CID);
    cid->len = byteorder_htons(len);
    memcpy(cid->duid, duid, duid_len);
    return len + sizeof(dhcpv6_opt_t);
}

static inline uint16_t _compose_sid_opt(dhcpv6_opt_duid_t *sid)
{
    uint16_t len = server.duid_len;

    sid->type = byteorder_htons(DHCPV6_OPT_SID);
    sid->len = byteorder_htons(len);
    memcpy(sid->duid, server.duid.u8, server.duid_len);
    return len + sizeof(dhcpv6_opt_t);
}

static inline uint16_t _get_elapsed_time(void)
{
    uint32_t now = _now_cs();
    uint32_t elapsed_time = transaction_start - now;

    if (elapsed_time > UINT16_MAX) {
        /* now overflowed since transaction_start */
        elapsed_time = (UINT32_MAX - transaction_start) + now + 1;
    }
    return elapsed_time;
}

static inline size_t _compose_elapsed_time_opt(dhcpv6_opt_elapsed_time_t *time)
{
    uint16_t len = 2U;

    time->type = byteorder_htons(DHCPV6_OPT_ELAPSED_TIME);
    time->len = byteorder_htons(len);
    time->elapsed_time = byteorder_htons(_get_elapsed_time());
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _compose_mud_url_opt(dhcpv6_opt_mud_url_t *mud_url_opt,
                                          size_t len_max)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_MUD_URL)) {
        return 0;
    }
    uint16_t len = strlen(mud_url);

    if (len > len_max) {
        assert(len <= len_max);
        return 0;
    }

    mud_url_opt->type = byteorder_htons(DHCPV6_OPT_MUD_URL);
    mud_url_opt->len = byteorder_htons(len);
    memcpy(mud_url_opt->mud_string, mud_url, len);
    return len + sizeof(dhcpv6_opt_mud_url_t);
}

static inline size_t _compose_oro_opt(dhcpv6_opt_oro_t *oro, uint16_t *opts,
                                        unsigned opts_num)
{
    uint16_t len = 2U * opts_num;

    oro->type = byteorder_htons(DHCPV6_OPT_ORO);
    oro->len = byteorder_htons(len);
    for (unsigned i = 0; i < opts_num; i++) {
        oro->opt_codes[i] = byteorder_htons(opts[i]);
    }
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _compose_ia_pd_opt(dhcpv6_opt_ia_pd_t *ia_pd,
                                        uint32_t ia_id, uint16_t opts_len)
{
    uint16_t len = 12U + opts_len;

    ia_pd->type = byteorder_htons(DHCPV6_OPT_IA_PD);
    ia_pd->len = byteorder_htons(len);
    ia_pd->ia_id = byteorder_htonl(ia_id);
    ia_pd->t1.u32 = 0;
    ia_pd->t2.u32 = 0;
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _compose_ia_na_opt(dhcpv6_opt_ia_na_t *ia_na,
                                        uint32_t ia_id, uint16_t opts_len)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
        return 0;
    }

    uint16_t len = 12U + opts_len;

    ia_na->type = byteorder_htons(DHCPV6_OPT_IA_NA);
    ia_na->len = byteorder_htons(len);
    ia_na->ia_id = byteorder_htonl(ia_id);
    ia_na->t1.u32 = 0;
    ia_na->t2.u32 = 0;
    return len + sizeof(dhcpv6_opt_t);
}

static inline size_t _add_ia_na(uint8_t *buf, size_t len_max)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
        return 0;
    }

    size_t msg_len = 0;

    for (unsigned i = 0; i < CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX; i++) {
        uint32_t ia_id = addr_leases[i].parent.ia_id.id;
        if (ia_id != 0) {
            dhcpv6_opt_ia_na_t *ia_na = (dhcpv6_opt_ia_na_t *)(&buf[msg_len]);

            msg_len += _compose_ia_na_opt(ia_na, ia_id, 0U);
        }
    }

    if (msg_len > len_max) {
        assert(0);
        return 0;
    }

    return msg_len;
}

static inline size_t _add_ia_pd_from_config(uint8_t *buf, size_t len_max)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_PD)) {
        return 0;
    }

    size_t msg_len = 0;

    for (unsigned i = 0; i < CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX; i++) {
        uint32_t ia_id = pfx_leases[i].parent.ia_id.id;
        if (ia_id != 0) {
            dhcpv6_opt_ia_pd_t *ia_pd = (dhcpv6_opt_ia_pd_t *)(&buf[msg_len]);

            msg_len += _compose_ia_pd_opt(ia_pd, ia_id, 0U);
        }
    }

    if (msg_len > len_max) {
        assert(0);
        return 0;
    }

    return msg_len;
}

static inline int32_t get_rand_ms_factor(void)
{
    int32_t res = ((int32_t)random_uint32_range(0, 200));
    res -= 100;
    return res;
}

static inline uint32_t _irt_ms(uint16_t irt, bool greater_irt)
{
    uint32_t irt_ms = (irt * MS_PER_SEC);
    int32_t factor = get_rand_ms_factor();

    if (greater_irt && (factor < 0)) {
        factor = -factor;
    }
    irt_ms += (factor * irt_ms) / MS_PER_SEC;
    return irt_ms;
}

static inline uint32_t _sub_rt_ms(uint32_t rt_prev_ms, uint16_t mrt)
{
    uint32_t sub_rt_ms = (2 * rt_prev_ms) +
                         ((get_rand_ms_factor() * rt_prev_ms) / MS_PER_SEC);

    if (sub_rt_ms > (mrt * MS_PER_SEC)) {
        uint32_t mrt_ms = mrt * MS_PER_SEC;

        sub_rt_ms = mrt_ms + ((get_rand_ms_factor() * mrt_ms) / MS_PER_SEC);
    }
    return sub_rt_ms;
}

static inline size_t _opt_len(dhcpv6_opt_t *opt)
{
    return sizeof(dhcpv6_opt_t) + byteorder_ntohs(opt->len);
}

static inline dhcpv6_opt_t *_opt_next(dhcpv6_opt_t *opt)
{
    return (dhcpv6_opt_t *)(((uint8_t *)opt) + _opt_len(opt));
}

static bool _check_status_opt(dhcpv6_opt_status_t *status)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if ((status != NULL) && (status->code.u16 != DHCPV6_STATUS_SUCCESS)) {
            size_t msg_len = byteorder_ntohs(status->len);
            char msg[msg_len - 1];

            strncpy(msg, status->msg, msg_len - 2);
            DEBUG("DHCPv6 client: server returned error (%u) \"%s\"\n",
                byteorder_ntohs(status->code), msg);
        }
    }

    /* DHCPV6_STATUS_SUCCESS is 0, so we don't need to fix byte order */
    return (status == NULL) || (status->code.u16 == DHCPV6_STATUS_SUCCESS);
}

static bool _check_cid_opt(dhcpv6_opt_duid_t *cid)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if ((byteorder_ntohs(cid->len) != duid_len) ||
            (memcmp(cid->duid, duid, duid_len) != 0)) {
            DEBUG("DHCPv6 client: message is not for me\n");
        }
    }

    return ((byteorder_ntohs(cid->len) == duid_len) &&
            (memcmp(cid->duid, duid, duid_len) == 0));
}

static bool _check_sid_opt(dhcpv6_opt_duid_t *sid)
{
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if ((byteorder_ntohs(sid->len) != server.duid_len) ||
            (memcmp(sid->duid, server.duid.u8, server.duid_len) != 0)) {
            DEBUG("DHCPv6 client: message is not from my server\n");
        }
    }

    return ((byteorder_ntohs(sid->len) == server.duid_len) &&
            (memcmp(sid->duid, server.duid.u8, server.duid_len) == 0));
}

/* discard stale messages in the receive buffer */
static void _flush_stale_replies(sock_udp_t *sock)
{
    int res;
    while ((res = sock_udp_recv(sock, recv_buf, sizeof(recv_buf), 0, NULL)) >= 0) {
        DEBUG("DHCPv6 client: discarding %d stale bytes\n", res);
    }
}

static int _preparse_advertise(uint8_t *adv, size_t len, uint8_t **buf)
{
    dhcpv6_opt_duid_t *cid = NULL, *sid = NULL;
    dhcpv6_opt_pref_t *pref = NULL;
    dhcpv6_opt_status_t *status = NULL;
    dhcpv6_opt_ia_pd_t *ia_pd = NULL;
    dhcpv6_opt_ia_na_t *ia_na = NULL;
    size_t orig_len = len;
    uint8_t pref_val = 0;

    DEBUG("DHCPv6 client: received ADVERTISE\n");
    if ((len < sizeof(dhcpv6_msg_t)) || !_is_tid((dhcpv6_msg_t *)adv)) {
        DEBUG("DHCPv6 client: packet too small or transaction ID wrong\n");
        return -1;
    }
    len -= sizeof(dhcpv6_msg_t);
    for (dhcpv6_opt_t *opt = (dhcpv6_opt_t *)(&adv[sizeof(dhcpv6_msg_t)]);
         len > 0; len -= _opt_len(opt), opt = _opt_next(opt)) {
        if (len > orig_len) {
            DEBUG("DHCPv6 client: ADVERTISE options overflow packet boundaries\n");
            return -1;
        }
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_CID:
                cid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_SID:
                sid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_STATUS:
                status = (dhcpv6_opt_status_t *)opt;
                break;
            case DHCPV6_OPT_IA_PD:
                if (IS_USED(MODULE_DHCPV6_CLIENT_IA_PD)) {
                    ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                }
                break;
            case DHCPV6_OPT_IA_NA:
                if (IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
                    ia_na = (dhcpv6_opt_ia_na_t *)opt;
                }
                break;
            case DHCPV6_OPT_PREF:
                pref = (dhcpv6_opt_pref_t *)opt;
                break;
            default:
                break;
        }
    }
    if ((cid == NULL) || (sid == NULL) ||
        (IS_USED(MODULE_DHCPV6_CLIENT_IA_PD) && (ia_pd == NULL)) ||
        (IS_USED(MODULE_DHCPV6_CLIENT_IA_NA) && (ia_na == NULL))) {
        DEBUG("DHCPv6 client: ADVERTISE does not contain either server ID, "
              "client ID, IA_PD or IA_NA option\n");
        return -1;
    }
    if (!_check_status_opt(status) || !_check_cid_opt(cid)) {
        return -1;
    }
    if (pref != NULL) {
        pref_val = pref->value;
    }
    if ((server.duid_len == 0) || (pref_val > server.pref)) {
        memcpy(best_adv, recv_buf, orig_len);
        if (buf != NULL) {
            *buf = best_adv;
        }
        server.duid_len = byteorder_ntohs(sid->len);
        memcpy(server.duid.u8, sid->duid, server.duid_len);
        server.pref = pref_val;
    }
    return pref_val;
}

static void _schedule_t2(void)
{
    if (t2 < UINT32_MAX) {
        rebind_time = _now_sec() + t2;
        _clear_event_timeout(&rebind_timeout);
        DEBUG("DHCPv6 client: scheduling REBIND in %lu sec\n",
              (unsigned long)t2);
        _set_event_timeout_sec(&rebind_timeout, &rebind, t2);
    }
}

static void _schedule_t1_t2(void)
{
    if (server.t1 < UINT32_MAX) {
        _clear_event_timeout(&solicit_renew_timeout);
        DEBUG("DHCPv6 client: scheduling RENEW in %lu sec\n",
              (unsigned long)server.t1);
        _set_event_timeout_sec(&solicit_renew_timeout, &renew, server.t1);
    }
    _schedule_t2();
}

static void _update_t1_t2(unsigned lease_t1, unsigned lease_t2)
{
    if ((lease_t1 != 0) && (lease_t2 != 0) &&
        ((server.t1 == 0) || (server.t1 >= lease_t1)) &&
        ((t2 == 0) || (t2 >= lease_t2))) {
        server.t1 = lease_t1;
        t2 = lease_t2;
        _schedule_t1_t2();
    }
}

static void _update_t2(unsigned lease_t1, unsigned lease_t2)
{
    if ((lease_t1 != 0) && (lease_t2 != 0) &&
        (server.t1 > lease_t1) && (t2 > lease_t2)) {
        server.t1 = lease_t1;
        t2 = lease_t2;
        _schedule_t2();
    }
}

static void _update_addr_lease(const dhcpv6_opt_iaaddr_t *iaaddr, addr_lease_t *lease) {
    if (iaaddr != NULL) {
        uint32_t valid = byteorder_ntohl(iaaddr->valid);
        uint32_t pref = byteorder_ntohl(iaaddr->pref);

        lease->leased = 1U;
        memcpy(&lease->addr, &iaaddr->addr, sizeof(ipv6_addr_t));
        dhcpv6_client_conf_addr(
                lease->parent.ia_id.info.netif, &lease->addr,
                valid, pref
            );
    }
}

static void _update_prefix_lease(const dhcpv6_opt_iapfx_t *iapfx, pfx_lease_t *lease)
{
    if (iapfx != NULL) {
        uint32_t valid = byteorder_ntohl(iapfx->valid);
        uint32_t pref = byteorder_ntohl(iapfx->pref);

        lease->pfx_len = iapfx->pfx_len;
        lease->leased = 1U;
        ipv6_addr_init_prefix(&lease->pfx,
                              &iapfx->pfx,
                              iapfx->pfx_len);
        if (iapfx->pfx_len > 0) {
            dhcpv6_client_conf_prefix(lease->parent.ia_id.info.netif,
                                      &lease->pfx, lease->pfx_len,
                                      valid, pref);
        }
    }
}

static void _parse_advertise(uint8_t *adv, size_t len)
{
    dhcpv6_opt_smr_t *smr = NULL;

    /* might not have been executed when not received in first retransmission
     * window => redo even if already done */
    if (_preparse_advertise(adv, len, NULL) < 0) {
        uint32_t delay = _irt_ms(DHCPV6_SOL_TIMEOUT, true);
        /* SOLICIT new server */
        _set_event_timeout_ms(&solicit_renew_timeout, &solicit_servers, delay);
        return;
    }
    DEBUG("DHCPv6 client: scheduling REQUEST\n");
    event_post(event_queue, &request);
    for (dhcpv6_opt_t *opt = (dhcpv6_opt_t *)(&adv[sizeof(dhcpv6_msg_t)]);
         len > 0; len -= _opt_len(opt), opt = _opt_next(opt)) {
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_IA_PD:
                for (unsigned i = 0;
                     IS_USED(MODULE_DHCPV6_CLIENT_IA_PD) &&
                     (i < CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX);
                     i++) {
                    dhcpv6_opt_ia_pd_t *ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                    unsigned pd_t1, pd_t2;
                    uint32_t ia_id = byteorder_ntohl(ia_pd->ia_id);
                    size_t ia_pd_len = byteorder_ntohs(ia_pd->len) -
                                       (sizeof(dhcpv6_opt_ia_pd_t) - sizeof(dhcpv6_opt_t));
                    size_t ia_pd_orig_len = ia_pd_len;

                    if (pfx_leases[i].parent.ia_id.id != ia_id) {
                        continue;
                    }
                    /* check for status */
                    for (dhcpv6_opt_t *ia_pd_opt = (dhcpv6_opt_t *)(ia_pd + 1);
                         ia_pd_len > 0;
                         ia_pd_len -= _opt_len(ia_pd_opt),
                         ia_pd_opt = _opt_next(ia_pd_opt)) {
                        if (ia_pd_len > ia_pd_orig_len) {
                            DEBUG("DHCPv6 client: IA_PD options overflow option "
                                  "boundaries\n");
                            return;
                        }
                        switch (byteorder_ntohs(ia_pd_opt->type)) {
                            case DHCPV6_OPT_STATUS: {
                                if (!_check_status_opt((dhcpv6_opt_status_t *)ia_pd_opt)) {
                                    continue;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    pd_t1 = byteorder_ntohl(ia_pd->t1);
                    pd_t2 = byteorder_ntohl(ia_pd->t2);
                    _update_t2(pd_t1, pd_t2);
                }
                break;
            case DHCPV6_OPT_IA_NA:
                for (unsigned i = 0;
                    IS_USED(MODULE_DHCPV6_CLIENT_IA_NA) &&
                    i < CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX;
                    i++) {
                    dhcpv6_opt_ia_na_t *ia_na = (dhcpv6_opt_ia_na_t *)opt;
                    unsigned na_t1, na_t2;
                    uint32_t ia_id = byteorder_ntohl(ia_na->ia_id);
                    size_t ia_na_len = byteorder_ntohs(ia_na->len) -
                                       (sizeof(dhcpv6_opt_ia_na_t) - sizeof(dhcpv6_opt_t));
                    size_t ia_na_orig_len = ia_na_len;

                    if (addr_leases[i].parent.ia_id.id != ia_id) {
                        continue;
                    }

                    /* check for status */
                    for (dhcpv6_opt_t *ia_na_opt = (dhcpv6_opt_t *)(ia_na + 1);
                         ia_na_len > 0;
                         ia_na_len -= _opt_len(ia_na_opt),
                         ia_na_opt = _opt_next(ia_na_opt)) {
                        if (ia_na_len > ia_na_orig_len) {
                            DEBUG("DHCPv6 client: IA_NA options overflow option "
                                  "boundaries\n");
                            return;
                        }
                        switch (byteorder_ntohs(ia_na_opt->type)) {
                            case DHCPV6_OPT_STATUS: {
                                if (!_check_status_opt((dhcpv6_opt_status_t *)ia_na_opt)) {
                                    continue;
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    }
                    na_t1 = byteorder_ntohl(ia_na->t1);
                    na_t2 = byteorder_ntohl(ia_na->t2);
                    _update_t2(na_t1, na_t2);
                }
                break;
            case DHCPV6_OPT_SMR:
                smr = (dhcpv6_opt_smr_t *)opt;
                break;
            default:
                break;
        }
    }
    if (smr != NULL) {
        sol_max_rt = byteorder_ntohl(smr->value);
    }
    return;
}

static bool _parse_ia_pd_option(dhcpv6_opt_ia_pd_t *ia_pd)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_PD)) {
        return true;
    }

    for (unsigned i = 0; (i < CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX); i++) {
        dhcpv6_opt_iapfx_t *iapfx = NULL;
        pfx_lease_t *lease = &pfx_leases[i];
        unsigned pd_t1, pd_t2;
        uint32_t ia_id = byteorder_ntohl(ia_pd->ia_id);
        size_t ia_pd_len = byteorder_ntohs(ia_pd->len) -
                           (sizeof(dhcpv6_opt_ia_pd_t) - sizeof(dhcpv6_opt_t));
        size_t ia_pd_orig_len = ia_pd_len;

        if (lease->parent.ia_id.id != ia_id) {
            continue;
        }
        /* check for status */
        for (dhcpv6_opt_t *ia_pd_opt = (dhcpv6_opt_t *)(ia_pd + 1);
             ia_pd_len > 0;
             ia_pd_len -= _opt_len(ia_pd_opt),
             ia_pd_opt = _opt_next(ia_pd_opt)) {
            if (ia_pd_len > ia_pd_orig_len) {
                DEBUG("DHCPv6 client: IA_PD options overflow option "
                      "boundaries\n");
                return false;
            }
            switch (byteorder_ntohs(ia_pd_opt->type)) {
                case DHCPV6_OPT_STATUS: {
                    if (!_check_status_opt((dhcpv6_opt_status_t *)ia_pd_opt)) {
                        continue;
                    }
                    break;
                }
                case DHCPV6_OPT_IAPFX: {
                    dhcpv6_opt_iapfx_t *this_iapfx = (dhcpv6_opt_iapfx_t *)ia_pd_opt;
                    if ((!lease->leased) ||
                        (iapfx == NULL) ||
                        ((this_iapfx->pfx_len == lease->pfx_len) &&
                        ipv6_addr_match_prefix(&this_iapfx->pfx,
                                               &lease->pfx) >= lease->pfx_len)) {
                        /* only take first prefix for now */
                        iapfx = this_iapfx;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        pd_t1 = byteorder_ntohl(ia_pd->t1);
        pd_t2 = byteorder_ntohl(ia_pd->t2);
        _update_t1_t2(pd_t1, pd_t2);
        _update_prefix_lease(iapfx, lease);
    }

    return true;
}

static bool _parse_ia_na_option(dhcpv6_opt_ia_na_t *ia_na)
{
    if (!IS_USED(MODULE_DHCPV6_CLIENT_IA_NA)) {
        return true;
    }

    for (unsigned i = 0; (i < CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX); i++) {
        dhcpv6_opt_iaaddr_t *iaaddr = NULL;
        addr_lease_t *lease = &addr_leases[i];
        unsigned na_t1, na_t2;
        uint32_t ia_id = byteorder_ntohl(ia_na->ia_id);
        size_t ia_na_len = byteorder_ntohs(ia_na->len) -
                            (sizeof(dhcpv6_opt_ia_na_t) - sizeof(dhcpv6_opt_t));
        size_t ia_na_orig_len = ia_na_len;

        if (lease->parent.ia_id.id != ia_id) {
            continue;
        }
        /* check for status */
        for (dhcpv6_opt_t *ia_na_opt = (dhcpv6_opt_t *)(ia_na + 1);
                ia_na_len > 0;
                ia_na_len -= _opt_len(ia_na_opt),
                ia_na_opt = _opt_next(ia_na_opt)) {
            if (ia_na_len > ia_na_orig_len) {
                DEBUG("DHCPv6 client: IA_NA options overflow option "
                        "boundaries\n");
                return false;
            }
            switch (byteorder_ntohs(ia_na_opt->type)) {
                case DHCPV6_OPT_STATUS: {
                    if (!_check_status_opt((dhcpv6_opt_status_t *)ia_na_opt)) {
                        continue;
                    }
                    break;
                }
                case DHCPV6_OPT_IAADDR: {
                    dhcpv6_opt_iaaddr_t *this_iaaddr = (dhcpv6_opt_iaaddr_t *)ia_na_opt;
                    if ((!lease->leased) ||
                        (iaaddr == NULL)) {
                        /* only take first address for now */
                        iaaddr = this_iaaddr;
                    }
                    break;
                }
                default:
                    break;
            }
        }
        na_t1 = byteorder_ntohl(ia_na->t1);
        na_t2 = byteorder_ntohl(ia_na->t2);
        _update_t1_t2(na_t1, na_t2);
        _update_addr_lease(iaaddr, lease);
    }

    return true;
}

static bool _parse_reply(uint8_t *rep, size_t len)
{
    dhcpv6_opt_duid_t *cid = NULL, *sid = NULL;
    dhcpv6_opt_ia_pd_t *ia_pd = NULL;
    dhcpv6_opt_ia_na_t *ia_na = NULL;
    dhcpv6_opt_status_t *status = NULL;
    dhcpv6_opt_smr_t *smr = NULL;
    size_t orig_len = len;

    DEBUG("DHCPv6 client: received REPLY\n");
    if ((len < sizeof(dhcpv6_msg_t)) || !_is_tid((dhcpv6_msg_t *)rep)) {
        DEBUG("DHCPv6 client: packet too small or transaction ID wrong\n");
        return false;
    }
    for (dhcpv6_opt_t *opt = (dhcpv6_opt_t *)(&rep[sizeof(dhcpv6_msg_t)]);
         len > 0; len -= _opt_len(opt), opt = _opt_next(opt)) {
        if (len > orig_len) {
            DEBUG("DHCPv6 client: ADVERTISE options overflow packet boundaries\n");
            return false;
        }
        switch (byteorder_ntohs(opt->type)) {
            case DHCPV6_OPT_CID:
                cid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_SID:
                sid = (dhcpv6_opt_duid_t *)opt;
                break;
            case DHCPV6_OPT_STATUS:
                status = (dhcpv6_opt_status_t *)opt;
                break;
            case DHCPV6_OPT_IA_PD:
                if (IS_USED(MODULE_DHCPV6_CLIENT_IA_PD)) {
                    ia_pd = (dhcpv6_opt_ia_pd_t *)opt;
                }
                break;
            case DHCPV6_OPT_IA_NA:
                ia_na = (dhcpv6_opt_ia_na_t *)opt;
                break;
            case DHCPV6_OPT_SMR:
                smr = (dhcpv6_opt_smr_t *)opt;
                break;
            default:
                break;
        }
    }
    if ((cid == NULL) || (sid == NULL) ||
        (IS_USED(MODULE_DHCPV6_CLIENT_IA_PD) && (ia_pd == NULL)) ||
        (IS_USED(MODULE_DHCPV6_CLIENT_IA_NA) && (ia_na == NULL))) {
        DEBUG("DHCPv6 client: ADVERTISE does not contain either server ID, "
              "client ID, IA_PD or IA_NA option\n");
        return false;
    }
    if (!_check_cid_opt(cid) || !_check_sid_opt(sid)) {
        return false;
    }
    if (smr != NULL) {
        sol_max_rt = byteorder_ntohl(smr->value);
    }
    if (!_check_status_opt(status)) {
        return false;
    }
    len = orig_len - sizeof(dhcpv6_msg_t);
    for (dhcpv6_opt_t *opt = (dhcpv6_opt_t *)(&rep[sizeof(dhcpv6_msg_t)]);
         len > 0; len -= _opt_len(opt), opt = _opt_next(opt)) {
        switch (byteorder_ntohs(opt->type)) {
#if IS_USED(MODULE_DHCPV6_CLIENT_DNS)
            case DHCPV6_OPT_DNS_RNS:
                dhcpv6_client_dns_rns_conf((dhcpv6_opt_dns_rns_t *)opt,
                                           remote.netif);
                break;
#endif  /* IS_USED(MODULE_DHCPV6_CLIENT_DNS) */
            case DHCPV6_OPT_IA_PD:
                if (_parse_ia_pd_option((dhcpv6_opt_ia_pd_t *)opt)) {
                     /* No error occurred */
                     break;
                 } else {
                     /* Something went wrong */
                     return false;
                 }
            case DHCPV6_OPT_IA_NA:
                 if (_parse_ia_na_option((dhcpv6_opt_ia_na_t *)opt)) {
                     /* No error occurred */
                     break;
                 } else {
                     /* Something went wrong */
                     return false;
                 }
            default:
                break;
        }
    }
    return true;
}

static size_t _compose_message(dhcpv6_msg_t *msg, uint8_t type)
{
    msg->type = type;
    _generate_tid();
    _set_tid(msg->tid);
    uint16_t oro_opts[] = { DHCPV6_OPT_SMR };
    size_t msg_len = sizeof(dhcpv6_msg_t);

    transaction_start = _now_cs();

    msg_len += _compose_cid_opt((dhcpv6_opt_duid_t *)&send_buf[msg_len]);

    if (type != DHCPV6_REBIND && type != DHCPV6_SOLICIT) {
        /* See RFC 8415, Appendix B */
        msg_len += _compose_sid_opt((dhcpv6_opt_duid_t *)&send_buf[msg_len]);
    }

    msg_len += _compose_mud_url_opt((dhcpv6_opt_mud_url_t *)&send_buf[msg_len],
                                    sizeof(send_buf) - msg_len);

    msg_len += _compose_oro_opt((dhcpv6_opt_oro_t *)&send_buf[msg_len], oro_opts,
                                ARRAY_SIZE(oro_opts));

    msg_len += _add_ia_na(&send_buf[msg_len], sizeof(send_buf) - msg_len);
    msg_len += _add_ia_pd_from_config(&send_buf[msg_len], sizeof(send_buf) - msg_len);

    return msg_len;
}

static void _solicit_servers(event_t *event)
{
    dhcpv6_msg_t *msg = (dhcpv6_msg_t *)&send_buf[0];
    dhcpv6_opt_elapsed_time_t *time;
    uint8_t *buf = NULL;
    uint32_t retrans_timeout = _irt_ms(DHCPV6_SOL_TIMEOUT, true);
    size_t msg_len;
    int res, best_res = 0;
    bool first_rt = true;

    (void)event;

    msg_len = _compose_message(msg, DHCPV6_SOLICIT);
    time = (dhcpv6_opt_elapsed_time_t *)&send_buf[msg_len];
    msg_len += _compose_elapsed_time_opt(time);

    DEBUG("DHCPv6 client: send SOLICIT\n");
    _flush_stale_replies(&sock);
    res = sock_udp_send(&sock, send_buf, msg_len, &remote);
    assert(res > 0);    /* something went terribly wrong */
    while (((res = sock_udp_recv(&sock, recv_buf, sizeof(recv_buf),
                                 retrans_timeout * US_PER_MS, NULL)) <= 0) ||
           (first_rt && (res > 0)) ||
           ((res > 0) && (recv_buf[0] != DHCPV6_ADVERTISE))) {
        if (first_rt && (res > 0) && (recv_buf[0] == DHCPV6_ADVERTISE)) {
            int parse_res;

            DEBUG("DHCPv6 client: initial transmission, collect best advertise\n");
            retrans_timeout -= (_get_elapsed_time() * MS_PER_CS);
            parse_res = _preparse_advertise(recv_buf, res, &buf);
            if (buf != NULL) {
                best_res = res;
            }
            if ((parse_res == UINT8_MAX) ||
                (retrans_timeout > (DHCPV6_SOL_MAX_RT * MS_PER_SEC))) {
                /* retrans_timeout underflowed => don't retry to receive */
                break;
            }
        }
        else if (buf == NULL) {
            DEBUG("DHCPv6 client: resend SOLICIT\n");
            first_rt = false;
            retrans_timeout = _sub_rt_ms(retrans_timeout, DHCPV6_SOL_MAX_RT);
            _compose_elapsed_time_opt(time);
            res = sock_udp_send(&sock, send_buf, msg_len, &remote);
            assert(res > 0);    /* something went terribly wrong */
        }
        else {
            break;
        }
    }
    if (buf == NULL) {
        buf = recv_buf;
        best_res = res;
    }
    if (best_res > 0) {
        _parse_advertise(buf, best_res);
    }
}

static uint32_t _calculate_mrd_from_leases(void)
{
    uint32_t mrd = 0;
    /* calculate MRD from prefix leases */
    for (unsigned i = 0;
            IS_USED(MODULE_DHCPV6_CLIENT_IA_PD) &&
            (i < CONFIG_DHCPV6_CLIENT_PFX_LEASE_MAX);
            i++) {
        const pfx_lease_t *lease = &pfx_leases[i];
        uint32_t valid_until = dhcpv6_client_prefix_valid_until(
                lease->parent.ia_id.info.netif,
                &lease->pfx, lease->pfx_len
            );
        if (valid_until > mrd) {
            mrd = valid_until;
        }
    }
    /* calculate MRD from addr_leases */
    for (unsigned i = 0;
            IS_USED(MODULE_DHCPV6_CLIENT_IA_NA) &&
            (i < CONFIG_DHCPV6_CLIENT_ADDR_LEASE_MAX);
            i++) {
        const addr_lease_t *lease = &addr_leases[i];
        uint32_t valid_until = dhcpv6_client_addr_valid_until(
                lease->parent.ia_id.info.netif, &lease->addr
            );
        if (valid_until > mrd) {
            mrd = valid_until;
        }
    }

    return mrd;
}

static void _request_renew_rebind(uint8_t type)
{
    dhcpv6_msg_t *msg = (dhcpv6_msg_t *)&send_buf[0];
    dhcpv6_opt_elapsed_time_t *time;
    uint32_t retrans_timeout;
    size_t msg_len;
    int res;
    uint8_t retrans = 0;
    uint16_t irt;
    uint16_t mrt;
    uint16_t mrc = 0;
    uint32_t mrd = 0;

    switch (type) {
        case DHCPV6_REQUEST:
            irt = DHCPV6_REQ_TIMEOUT;
            mrt = DHCPV6_REQ_MAX_RT;
            mrc = DHCPV6_REQ_MAX_RC;
            break;
        case DHCPV6_RENEW:
            irt = DHCPV6_REN_TIMEOUT;
            mrt = DHCPV6_REN_MAX_RT;
            mrd = rebind_time - _now_sec();
            break;
        case DHCPV6_REBIND: {
            irt = DHCPV6_REB_TIMEOUT;
            mrt = DHCPV6_REB_MAX_RT;
            mrd = _calculate_mrd_from_leases();
            if (mrd == 0) {
                /* all leases already expired, don't try to rebind and
                 * solicit immediately */
                _post_solicit_servers();
                return;
            }
            break;
        }
        default:
            return;
    }
    retrans_timeout = _irt_ms(irt, false);
    msg_len = _compose_message(msg, type);

    time = (dhcpv6_opt_elapsed_time_t *)&send_buf[msg_len];
    msg_len += _compose_elapsed_time_opt(time);

    _flush_stale_replies(&sock);
    while (sock_udp_send(&sock, send_buf, msg_len, &remote) <= 0) {}
    while (((res = sock_udp_recv(&sock, recv_buf, sizeof(recv_buf),
                                 retrans_timeout * US_PER_MS, NULL)) <= 0) ||
           ((res > 0) && (recv_buf[0] != DHCPV6_REPLY))) {
        if ((mrd > 0) && (_get_elapsed_time() > (mrd * CS_PER_SEC))) {
            break;
        }
        retrans_timeout = _sub_rt_ms(retrans_timeout, mrt);
        if ((mrc > 0) && (++retrans) >= mrc) {
            break;
        }
        _compose_elapsed_time_opt(time);
        DEBUG("DHCPv6 client: resend %s\n",
              (type == DHCPV6_REQUEST) ? "REQUEST" :
              (type == DHCPV6_RENEW) ? "RENEW": "REBIND");
        sock_udp_send(&sock, send_buf, msg_len, &remote);
    }
    if ((res > 0) && (recv_buf[0] == DHCPV6_REPLY)) {
        if (!_parse_reply(recv_buf, res)) {
            /* try again */
            event_post(event_queue, &request);
        }
    }
    else if (type == DHCPV6_REBIND) {
        _post_solicit_servers();
    }
}

static void _request(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send REQUEST\n");
    _request_renew_rebind(DHCPV6_REQUEST);
}

static void _renew(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send RENEW\n");
    _request_renew_rebind(DHCPV6_RENEW);
}

static void _rebind(event_t *event)
{
    (void)event;
    DEBUG("DHCPv6 client: send REBIND\n");
    _request_renew_rebind(DHCPV6_REBIND);
}

static void _set_event_timeout_ms(event_timeout_t *timeout, event_t *event,
                                  uint32_t delay_ms)
{
#if IS_USED(MODULE_EVENT_TIMEOUT_ZTIMER)
    event_timeout_ztimer_init(timeout, ZTIMER_MSEC, event_queue, event);
    event_timeout_set(timeout, delay_ms);
#else
    event_timeout_init(timeout, event_queue, event);
    event_timeout_set(timeout, delay_ms * US_PER_MS);
#endif
}

static void _set_event_timeout_sec(event_timeout_t *timeout, event_t *event,
                                   uint32_t delay_sec)
{
#if IS_USED(MODULE_EVENT_TIMEOUT_ZTIMER)
    event_timeout_ztimer_init(timeout, ZTIMER_SEC, event_queue, event);
    event_timeout_set(timeout, delay_sec);
#else
    event_timeout_init(timeout, event_queue, event);
    /* use xtimer_set64 instead of event_timeout_set to prevent overflows */
    xtimer_set64(&timeout->timer, ((uint64_t)delay_sec) * US_PER_SEC);
#endif
}

static void _clear_event_timeout(event_timeout_t *timeout)
{
    event_timeout_clear(timeout);
}

/** @} */
