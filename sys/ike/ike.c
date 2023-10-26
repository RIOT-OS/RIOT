/*
 * Copyright (C) 2022 Juliusz Neuman
 *
 *
 * @ingroup sys_ps
 * @{
 * @file
 * @brief   IKE
 * @author      Juliusz Neuman <neuman.juliusz@gmail.com>
 * @}
 */

#include "ike/ike.h"
#include "ike/ike_structures.h"
#include "ike/chunk.h"
#include "ike/ike_payloads.h"
#include "ike/ike_kernel_iface.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <net/sock/udp.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/dh.h>

#include "net/gnrc/ipv6.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/netreg.h"
#include "net/utils.h"
#include "hashes/sha1.h"
#include "xtimer.h"
#include "od.h"

#include "random.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define DEBUG_CHUNK(...) if (ENABLE_DEBUG) printf_chunk(__VA_ARGS__);


#define IKE_NONCE_I_LEN 32
#define MSG_BUF_LEN 1280

#define ID_I "sun.strongswan.org"
#define PSK "This is a strong password"

typedef struct {
    ike_state_t state;
    uint64_t ike_spi_i;
    uint64_t ike_spi_r;
    uint32_t child_spi_i;
    uint32_t child_spi_r;
    chunk_t ike_nonce_i;
    chunk_t ike_nonce_r;
    ike_transform_encr_t ike_encr;
    size_t ike_key_size;
    ike_transform_prf_t ike_prf;
    ike_transform_integ_t ike_integ;
    ike_transform_dh_t ike_dh;
    ike_transform_esn_t ike_esn;
    ike_transform_encr_t child_encr;
    size_t child_key_size;
    ike_transform_integ_t child_integ;
    ike_transform_esn_t child_esn;
    DhKey wc_priv_key;
    WC_RNG wc_rng;
    chunk_t pubkey_i;
    chunk_t privkey_i;
    chunk_t pubkey_r;
    chunk_t shared_secret;
    chunk_t skeyseed;
    chunk_t sk_d;
    chunk_t sk_ai;
    chunk_t sk_ar;
    chunk_t sk_ei;
    chunk_t sk_er;
    chunk_t sk_pi;
    chunk_t sk_pr;
    chunk_t id_i;
    chunk_t psk;
    sha1_context real_message_1_digest_ctx;
    sha1_context real_message_2_digest_ctx;
    struct {
        ipv6_addr_t start;
        ipv6_addr_t end;
    } local_ts;
    struct {
        ipv6_addr_t start;
        ipv6_addr_t end;
    } remote_ts;
    struct {
        ike_id_type_t type;
        chunk_t id;
    } remote_id;
    ipv6_addr_t local_ip;
    ipv6_addr_t remote_ip;
    uint32_t sp_in_idx;
    uint32_t sp_out_idx;
} _ike_ctx_t;

static _ike_ctx_t ike_ctx;
static uint32_t key_gen_time;
static uint32_t dh_time;

static int _send_data(char *addr_str, char *data, size_t datalen);
static int _receive_data(char *addr_str, char *data, size_t *datalen, uint32_t timeout);
static int _init_context(void);
static int _reset_context(void);
static int _build_init_i(char *msg, size_t *msg_len);
static int _build_auth_i(char *msg, size_t *msg_len);
static int _parse_init_r(char *msg, size_t msg_len);
static int _parse_auth_r(char *msg, size_t msg_len);
static int _parse_auth_r_decrypted(char *msg, size_t msg_len, ike_payload_type_t first_type);
static int _build_delete_i(char *msg, size_t *msg_len);
static int _generate_key(void);
static int _get_secrets(void);
static void _get_auth_secret(chunk_t *inner_key);
static void _prf(chunk_t key, chunk_t seed, chunk_t *out);
static void _prf_plus(chunk_t key, chunk_t seed, size_t len, chunk_t *out);
static int _generate_child_key(void);

static inline uint32_t untoh32(void *network)
{
    char *unaligned = (char *)network;
    uint32_t tmp;

    memcpy(&tmp, unaligned, sizeof(tmp));
    return ntohl(tmp);
}

int ike_init(char *addr_str)
{
    size_t len;
    char data_out[MSG_BUF_LEN];
    char data_in[MSG_BUF_LEN];
    uint32_t timeout = 5;
    uint32_t start = xtimer_now_usec();

    if (ike_ctx.state != IKE_STATE_OFF) {
        if (_reset_context() < 0) {
            DEBUG("Resetting IKE context failed\n");
            goto error;
        }
    }
    uint32_t _reset_context_ts = xtimer_now_usec();

    if (_init_context() < 0) {
        DEBUG("Initiating IKE context failed\n");
        goto error;
    }
    uint32_t _init_context_ts = xtimer_now_usec();

    if (_build_init_i(data_out, &len) < 0) {
        DEBUG("Building IKE INIT message failed\n");
        goto error;
    }
    uint32_t _build_init_i_ts = xtimer_now_usec();

    if (_send_data(addr_str, data_out, len) < 0) {
        DEBUG("Sending IKE INIT message failed\n");
        goto error;
    }
    uint32_t _send_data1_ts = xtimer_now_usec();

    if (_receive_data(addr_str, data_in, &len, timeout) < 0) {
        DEBUG("Receiving IKE INIT message failed\n");
        // TODO: retry
        goto error;
    }
    uint32_t _receive_data1_ts = xtimer_now_usec();

    if (_parse_init_r(data_in, len) < 0) {
        DEBUG("Parsing IKE INIT message failed\n");
        goto error;
    }
    uint32_t _parse_init_r_ts = xtimer_now_usec();

    if (_build_auth_i(data_out, &len) < 0) {
        DEBUG("Building IKE AUTH message failed\n");
        goto error;
    }
    uint32_t _build_auth_i_ts = xtimer_now_usec();

    if (_send_data(addr_str, data_out, len) < 0) {
        DEBUG("Sending IKE AUTH message failed\n");
        goto error;
    }
    uint32_t _send_data2_ts = xtimer_now_usec();

    if (_receive_data(addr_str, data_in, &len, timeout) < 0) {
        DEBUG("Receiving IKE AUTH message failed\n");
        // TODO: retry
        goto error;
    }
    uint32_t _receive_data2_ts = xtimer_now_usec();

    if (_parse_auth_r(data_in, len) < 0) {
        DEBUG("Parsing IKE AUTH message failed\n");
        goto error;
    }
    uint32_t _parse_auth_r_ts = xtimer_now_usec();

    if (_generate_child_key() < 0) {
        DEBUG("Generating Child Keying material failed\n");
        goto error;
    }
    ike_ctx.state = IKE_STATE_ESTABLISHED;
    uint32_t total = xtimer_now_usec() - start;

    printf("Stats\n");
    printf("%7" PRIu32 " us total established in \n", total);
    printf("%7" PRIu32 " us reset context\n", _reset_context_ts - start);
    printf("%7" PRIu32 " us init context\n", _init_context_ts - _reset_context_ts);
    printf("%7" PRIu32 " us build INIT\n", _build_init_i_ts - _init_context_ts);
    printf("%7" PRIu32 " us send INIT\n", _send_data1_ts - _build_init_i_ts);
    printf("%7" PRIu32 " us receive INIT\n", _receive_data1_ts - _send_data1_ts);
    printf("%7" PRIu32 " us parse INIT\n", _parse_init_r_ts - _receive_data1_ts);
    printf("%7" PRIu32 " us build AUTH\n", _build_auth_i_ts - _parse_init_r_ts);
    printf("%7" PRIu32 " us send AUTH\n", _send_data2_ts - _build_auth_i_ts);
    printf("%7" PRIu32 " us receive AUTH\n", _receive_data2_ts - _send_data2_ts);
    printf("%7" PRIu32 " us parse AUTH\n", _parse_auth_r_ts - _receive_data2_ts);
    printf("%7" PRIu32 " us generate_key\n", key_gen_time);
    printf("%7" PRIu32 " us get_secret\n", dh_time);

    return 0;
error:
    _reset_context();
    return -1;
}

int ike_terminate(void)
{
    size_t len;
    char data_out[MSG_BUF_LEN];
    char data_in[MSG_BUF_LEN];
    char addr_str[IPV6_ADDR_MAX_STR_LEN];

    ipv6_addr_to_str(addr_str, &ike_ctx.remote_ip, IPV6_ADDR_MAX_STR_LEN);
    uint32_t timeout = 5;


    if (ike_ctx.state != IKE_STATE_ESTABLISHED) {
        DEBUG("IKE not established\n");
        goto error;
    }
    if (_build_delete_i(data_out, &len) < 0) {
        DEBUG("Building IKE DELETE message failed\n");
        goto error;
    }
    if (_send_data(addr_str, data_out, len) < 0) {
        DEBUG("Sending IKE DELETE message failed\n");
        goto error;
    }
    if (_receive_data(addr_str, data_in, &len, timeout) < 0) {
        DEBUG("Receiving IKE DELETE message failed\n");
        // TODO: retry
        goto error;
    }
    _reset_context();
    return 0;
error:
    _reset_context();
    return -1;
}

static int _send_data(char *addr_str, char *data, size_t datalen)
{
    char *iface;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t sckv;
    sock_udp_t *sck = &sckv;
    ipv6_addr_t *tmp_addr;

    /* Parsing <address> */
    iface = ipv6_addr_split_iface(addr_str);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
    }
    else {
        gnrc_netif_t *netif = gnrc_netif_get_by_pid(atoi(iface));
        if (netif == NULL) {
            DEBUG("ERROR: interface not valid\n");
            return -1;
        }
        remote.netif = (uint16_t)netif->pid;
    }
    if (ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, addr_str) == NULL) {
        DEBUG("ERROR: unable to parse destination address\n");
        return -1;
    }
    tmp_addr = (ipv6_addr_t *)remote.addr.ipv6;
    ike_ctx.remote_ip = *tmp_addr;
    remote.port = 500;
    local.port = 500;
    if (sock_udp_create(sck, &local, &remote, 0)) {
        DEBUG("ERROR: Unable to create UDP sock\n");
        return -1;
    }
    if (sock_udp_send(sck, data, datalen, &remote) < 0) {
        DEBUG("ERROR: Data not sent\n");
        sock_udp_close(sck);
        return -1;
    }
    sock_udp_close(sck);
    return 0;
}

static int _receive_data(char *addr_str, char *data, size_t *datalen, uint32_t timeout)
{
    char *iface;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_t sckv;
    sock_udp_t *sck = &sckv;
    ssize_t recv_len;
    sock_udp_aux_rx_t aux = { .flags = SOCK_AUX_GET_LOCAL };

    /* Parsing <address> */
    iface = ipv6_addr_split_iface(addr_str);
    if (!iface) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
    }
    else {
        gnrc_netif_t *netif = gnrc_netif_get_by_pid(atoi(iface));
        if (netif == NULL) {
            DEBUG("ERROR: interface not valid\n");
            return -1;
        }
        remote.netif = (uint16_t)netif->pid;
    }
    if (ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, addr_str) == NULL) {
        DEBUG("ERROR: unable to parse destination address\n");
        return -1;
    }
    local.port = 500;
    remote.port = 500;
    if (sock_udp_create(sck, &local, &remote, 0)) {
        DEBUG("ERROR: Unable to create UDP sock\n");
        return -1;
    }
    recv_len = sock_udp_recv_aux(sck, data, MSG_BUF_LEN, timeout * 1000000, NULL, &aux);
    if (recv_len < 0) {
        DEBUG("ERROR: Data not received\n");
        sock_udp_close(sck);
        return -1;
    }
    ipv6_addr_t *tmp_addr = (ipv6_addr_t *)aux.local.addr.ipv6;

    ike_ctx.local_ip = *tmp_addr;
    *datalen = recv_len;
    sock_udp_close(sck);
    return 0;
}

static int _reset_context(void)
{
    DEBUG("Resetting IKE context\n");
    clear_esp(ike_ctx.child_spi_i, ike_ctx.child_spi_r, ike_ctx.sp_in_idx, ike_ctx.sp_out_idx);
    ike_ctx.state = IKE_STATE_OFF;
    ike_ctx.ike_spi_i = 0;
    ike_ctx.ike_spi_r = 0;
    ike_ctx.child_spi_i = 0;
    ike_ctx.child_spi_r = 0;
    free_chunk(&ike_ctx.ike_nonce_i);
    free_chunk(&ike_ctx.privkey_i);
    free_chunk(&ike_ctx.pubkey_i);
    free_chunk(&ike_ctx.shared_secret);
    free_chunk(&ike_ctx.skeyseed);
    free_chunk(&ike_ctx.sk_d);
    free_chunk(&ike_ctx.sk_ai);
    free_chunk(&ike_ctx.sk_ar);
    free_chunk(&ike_ctx.sk_ei);
    free_chunk(&ike_ctx.sk_er);
    free_chunk(&ike_ctx.sk_pi);
    free_chunk(&ike_ctx.sk_pr);
    free_chunk(&ike_ctx.id_i);
    free_chunk(&ike_ctx.psk);
    free_chunk(&ike_ctx.remote_id.id);
    wc_FreeDhKey(&ike_ctx.wc_priv_key);
    wc_FreeRng(&ike_ctx.wc_rng);

    return 0;
}

static int _init_context(void)
{
    DEBUG("Initiating IKE context\n");

    /* Generate random values */
    uint64_t ike_spi_i;
    uint32_t child_spi_i;

    random_bytes((uint8_t *)&ike_spi_i, sizeof(uint64_t));
    DEBUG("New IKE initiator SPI: 0x%" "llX" "\n", ike_spi_i);
    random_bytes((uint8_t *)&child_spi_i, sizeof(uint32_t));
    DEBUG("New Child initiator SPI: 0x%" PRIX32 "\n", child_spi_i);

    chunk_t ike_nonce_i = malloc_chunk(IKE_NONCE_I_LEN);

    random_bytes((uint8_t *)ike_nonce_i.ptr, ike_nonce_i.len);
    DEBUG("New IKE initiatior Nonce:\n");
    DEBUG_CHUNK(ike_nonce_i, 8);

    chunk_t id_i = malloc_chunk(strlen(ID_I));

    memcpy(id_i.ptr, ID_I, id_i.len);

    chunk_t psk = malloc_chunk(strlen(PSK));

    memcpy(psk.ptr, PSK, psk.len);

    _ike_ctx_t new_ike_ctx = {
        .ike_spi_i = ike_spi_i,
        .child_spi_i = child_spi_i,
        .state = IKE_STATE_NEGOTIATION,
        .ike_nonce_i = ike_nonce_i,
        .ike_encr = IKE_TRANSFORM_ENCR_AES_CBC,
        .ike_key_size = 128,
        .ike_prf = IKE_TRANSFORM_PRF_HMAC_SHA1,
        .ike_integ = IKE_TRANSFORM_AUTH_HMAC_SHA1_96,
        .child_encr = IKE_TRANSFORM_ENCR_AES_CBC,
        .child_key_size = 128,
        .child_integ = IKE_TRANSFORM_AUTH_HMAC_SHA1_96,
        .ike_dh = IKE_TRANSFORM_MODP2048,
        .ike_esn = IKE_TRANSFORM_ESN_OFF,
        .id_i = id_i,
        .psk = psk,
        .sp_in_idx = -1,
        .sp_out_idx = -1,
    };

    ike_ctx = new_ike_ctx;
    uint32_t _generate_key_start = xtimer_now_usec();

    _generate_key(); // TODO: check fail
    key_gen_time = xtimer_now_usec() - _generate_key_start;
    return 0;
}

static int _build_init_i(char *msg, size_t *msg_len)
{
    size_t cur_len = 0;
    size_t new_len;
    int error;

    /* Construct IKE header */
    ike_header_t hdr = {
        .ike_sa_spi_i = htonll(ike_ctx.ike_spi_i),
        .ike_sa_spi_r = 0,
        .next_payload = IKE_PT_SECURITY_ASSOCIATION,
        .mjver_mnver = IKE_V2_MJVER | IKE_V2_MNVER,
        .exchange_type = IKE_ET_IKE_SA_INIT,
        .flags = IKE_INITIATOR_FLAG,
        .message_id = htonl(0),
        .length = 0,
    };

    cur_len += sizeof(hdr);

    /* Construct SA payload */
    error = build_sa_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len, IKE_PT_NOTIFY,
                             IKE_PROTO_IKE, ike_ctx.ike_encr,
                             ike_ctx.ike_prf, ike_ctx.ike_integ, ike_ctx.ike_dh, ike_ctx.ike_esn,
                             ike_ctx.ike_key_size, empty_chunk);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Initial Contact Notify payload */
    error = build_notify_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len, IKE_PT_NONCE, 0,
                                 IKE_NOTIFY_TYPE_INITIAL_CONTACT, empty_chunk, empty_chunk);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Nonce payload */
    error = build_nonce_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len, IKE_PT_KEY_EXCHANGE,
                                ike_ctx.ike_nonce_i);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Key Exchange payload */
    error = build_key_exchange_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                                       IKE_PT_NO_NEXT_PAYLOAD, ike_ctx.ike_dh, ike_ctx.pubkey_i);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Prepend header */
    hdr.length = htonl(cur_len);
    memcpy(msg, &hdr, sizeof(hdr));
    *msg_len = cur_len;
    chunk_t auth_secret;

    _get_auth_secret(&auth_secret);
    sha1_init_hmac(&ike_ctx.real_message_1_digest_ctx, auth_secret.ptr,
                   auth_secret.len);
    free_chunk(&auth_secret);
    sha1_update(&ike_ctx.real_message_1_digest_ctx, msg, cur_len);

    return 0;
}

static int _parse_init_r(char *msg, size_t msg_len)
{
    size_t remaining_len = msg_len;
    char *p = msg;
    ike_header_t *ike_hdr;
    size_t cur_len;
    ike_payload_type_t next_type;

    if (msg_len < sizeof(ike_header_t)) {
        DEBUG("Message too short\n");
        return -EMSGSIZE;
    }
    ike_hdr = (ike_header_t *)p;
    if (ntohl(ike_hdr->length) != msg_len) {
        DEBUG("Message length mismatch\n");
        return -EMSGSIZE;
    }
    if (ike_ctx.ike_spi_i != ntohll(ike_hdr->ike_sa_spi_i)) {
        DEBUG("SPI mismatch\n");
        return -EPROTO;
    }
    ike_ctx.ike_spi_r = ntohll(ike_hdr->ike_sa_spi_r);
    // TODO: more checks
    next_type = ike_hdr->next_payload;
    remaining_len -= sizeof(ike_header_t);
    p += sizeof(ike_header_t);
    while (remaining_len > 0) {
        switch (next_type) {
        case IKE_PT_NONCE:
            if (process_nonce_payload(p, remaining_len, &cur_len, &next_type,
                                      &ike_ctx.ike_nonce_r) < 0) {
                DEBUG("Nonce payload parsing failed\n");
                return -1;
            }
            DEBUG("Parsed nonce payload of size %u\n", cur_len);
            DEBUG_CHUNK(ike_ctx.ike_nonce_r, 4);
            break;
        case IKE_PT_SECURITY_ASSOCIATION:
            if (process_sa_payload(p, remaining_len, &cur_len, &next_type, NULL, NULL, NULL,
                                   NULL, NULL, NULL, NULL, &empty_chunk) < 0) {
                DEBUG("Nonce payload parsing failed\n");
                return -1;
            }
            DEBUG("Parsed nonce payload of size %u\n", cur_len);
            DEBUG_CHUNK(ike_ctx.ike_nonce_r, 4);
            break;
        case IKE_PT_KEY_EXCHANGE:
        {}
            ike_transform_dh_t dh_r;
            if (process_key_exchange_payload(p, remaining_len, &cur_len, &next_type, &dh_r,
                                             &ike_ctx.pubkey_r)) {
                DEBUG("Nonce payload parsing failed\n");
                return -1;
            }
            DEBUG("Parsed key exchange payload of size %u\n", cur_len);
            DEBUG_CHUNK(ike_ctx.pubkey_r, 8);
            break;
        default:
            if (process_unknown_payload(p, remaining_len, &cur_len, &next_type) < 0) {
                DEBUG("Unknown payload parsing failed\n");
                return -1;
            }
            DEBUG("Parsed unknown payload of size %u\n", cur_len);
        }
        remaining_len -= cur_len;
        p += cur_len;
    }
    uint32_t _get_secrets_start = xtimer_now_usec();

    if (_get_secrets() < 0) {
        DEBUG("Getting secrets failed\n");
    }
    dh_time = xtimer_now_usec() - _get_secrets_start;
    chunk_t auth_secret;

    _get_auth_secret(&auth_secret);
    sha1_init_hmac(&ike_ctx.real_message_2_digest_ctx, auth_secret.ptr,
                   auth_secret.len);
    free_chunk(&auth_secret);
    sha1_update(&ike_ctx.real_message_2_digest_ctx, msg, msg_len);
    return 0;
}

static int _build_auth_i(char *msg, size_t *msg_len)
{
    size_t cur_len = 0;
    size_t new_len;
    int error;

    /* Construct IKE header */
    ike_header_t hdr = {
        .ike_sa_spi_i = htonll(ike_ctx.ike_spi_i),
        .ike_sa_spi_r = htonll(ike_ctx.ike_spi_r),
        .next_payload = IKE_PT_ENCRYPTED_AND_AUTHENTICATED,
        .mjver_mnver = IKE_V2_MJVER | IKE_V2_MNVER,
        .exchange_type = IKE_ET_IKE_AUTH,
        .flags = IKE_INITIATOR_FLAG,
        .message_id = htonl(1),
    };

    memcpy(msg, &hdr, sizeof(hdr));
    cur_len += sizeof(hdr);

    /* Construct Identification payload */
    error = build_identification_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                                         IKE_PT_AUTHENTICATION, IKE_ID_TYPE_FQDN, ike_ctx.id_i);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Authentication payload */
    chunk_t authed_data = malloc_chunk(HASH_SIZE_SHA1);

    sha1_update(&ike_ctx.real_message_1_digest_ctx, ike_ctx.ike_nonce_r.ptr,
                ike_ctx.ike_nonce_r.len);
    chunk_t idx =
    { .len = new_len - sizeof(ike_generic_payload_header_t),
      .ptr = msg + cur_len - new_len + sizeof(ike_generic_payload_header_t) };
    chunk_t maced_id;

    _prf(ike_ctx.sk_pi, idx, &maced_id);
    sha1_update(&ike_ctx.real_message_1_digest_ctx, maced_id.ptr, maced_id.len);
    free_chunk(&maced_id);
    sha1_final_hmac(&ike_ctx.real_message_1_digest_ctx, authed_data.ptr);
    if (error < 0) {
        return error;
    }
    error = build_auth_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                               IKE_PT_NOTIFY, IKE_AUTH_METHOD_PSK, authed_data);
    free_chunk(&authed_data);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Use Transpor Mode Notify payload */
    error = build_notify_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                                 IKE_PT_SECURITY_ASSOCIATION, 0,
                                 IKE_NOTIFY_TYPE_USE_TRANSPORT_MODE, empty_chunk, empty_chunk);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct SA payload */
    uint32_t n_spi = htonl(ike_ctx.child_spi_i);

    error = build_sa_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                             IKE_PT_TRAFFIC_SELECTOR_I, IKE_PROTO_ESP, ike_ctx.child_encr, 0,
                             ike_ctx.child_integ, 0, IKE_TRANSFORM_ESN_OFF, ike_ctx.child_key_size,
                             (chunk_t){ .ptr = (char *)&n_spi, .len = sizeof(n_spi) });
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct TSi payload */
    error = build_ts_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                             IKE_PT_TRAFFIC_SELECTOR_R);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct TSr payload */
    error =
        build_ts_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len, IKE_PT_NO_NEXT_PAYLOAD);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Encrypted payload */
    chunk_t enc_in = {
        .ptr = msg + sizeof(hdr),
        .len = cur_len - sizeof(hdr),
    };

    error = build_encrypted_payload(msg + sizeof(hdr), MSG_BUF_LEN - sizeof(hdr), &new_len,
                                    IKE_PT_IDENTIFICATION_I,
                                    enc_in, ike_ctx.sk_ei, ike_ctx.sk_ai);
    if (error < 0) {
        return error;
    }

    *msg_len = sizeof(hdr) + new_len;
    return 0;
}

static int _parse_auth_r(char *msg, size_t msg_len)
{
    size_t remaining_len = msg_len;
    char *p = msg;
    ike_header_t *ike_hdr;
    size_t cur_len;
    ike_payload_type_t next_type;
    chunk_t decrypted_msg;
    int error;

    if (msg_len < sizeof(ike_header_t)) {
        DEBUG("Message too short\n");
        return -EMSGSIZE;
    }
    ike_hdr = (ike_header_t *)p;
    if (ntohl(ike_hdr->length) != msg_len) {
        DEBUG("Message length mismatch\n");
        return -EMSGSIZE;
    }
    if (ike_ctx.ike_spi_r != ntohll(ike_hdr->ike_sa_spi_r) ||
        ike_ctx.ike_spi_i != ntohll(ike_hdr->ike_sa_spi_i)) {
        DEBUG("SPI mismatch\n");
        return -EPROTO;
    }
    // TODO: more checks
    next_type = ike_hdr->next_payload;
    remaining_len -= sizeof(ike_header_t);
    p += sizeof(ike_header_t);
    error = process_encrypted_payload(p, remaining_len, &cur_len, &next_type, &decrypted_msg,
                                      ike_ctx.sk_er, ike_ctx.sk_ar);
    if (error) {
        DEBUG("Payload decryption failed\n");
        return -EBADMSG;
    }
    DEBUG("Decrypted data:\n");
    DEBUG_CHUNK(decrypted_msg, 8);
    if (_parse_auth_r_decrypted(decrypted_msg.ptr, decrypted_msg.len, next_type)) {
        DEBUG("Parsing decrypted content failed\n");
        return -EBADMSG;
    }
    free_chunk(&decrypted_msg);
    return 0;
}

static int _parse_auth_r_decrypted(char *msg, size_t msg_len, ike_payload_type_t first_type)
{
    ike_payload_type_t next_type = first_type;
    size_t remaining_len = msg_len;
    char *p = msg;
    size_t cur_len;
    chunk_t idx = empty_chunk;
    uint8_t auth_data_recv_buff[HASH_SIZE_SHA1];
    uint8_t idx_buff[MAX_IDX_SIZE];
    chunk_t auth_data_recv = chunk_from_buff(auth_data_recv_buff);

    while (remaining_len > 0) {
        switch (next_type) {
        case IKE_PT_SECURITY_ASSOCIATION:
        {}
            uint32_t spi;
            chunk_t n_spi = { .ptr = (char *)&spi, .len = sizeof(spi) };
            if (process_sa_payload(p, remaining_len, &cur_len, &next_type, NULL, NULL, NULL,
                                   NULL, NULL, NULL, NULL, &n_spi) < 0) {
                DEBUG("SA payload parsing failed\n");
                return -1;
            }
            ike_ctx.child_spi_r = ntohl(spi);
            break;
        case IKE_PT_IDENTIFICATION_R:
            if (process_identification_payload(p, remaining_len, &cur_len, &next_type,
                                               &ike_ctx.remote_id.type,
                                               &ike_ctx.remote_id.id) < 0) {
                DEBUG("ID payload parsing failed\n");
                return -1;
            }
            idx.len = cur_len - sizeof(ike_generic_payload_header_t);
            if (idx.len > countof(idx_buff)) {
                DEBUG("IDX too long\n");
                return -1;
            }
            idx.ptr = (char *)idx_buff;
            memcpy(idx.ptr, p + sizeof(ike_generic_payload_header_t), idx.len);
            DEBUG("Received ID (%d):\n", ike_ctx.remote_id.type);
            DEBUG_CHUNK(ike_ctx.remote_id.id, 8);
            DEBUG("%.*s\n", ike_ctx.remote_id.id.len, ike_ctx.remote_id.id.ptr);
            break;
        case IKE_PT_AUTHENTICATION:
        {}
            ike_auth_method_t auth_method;
            if (process_auth_payload(p, remaining_len, &cur_len, &next_type, &auth_method,
                                     &auth_data_recv) < 0) {
                DEBUG("AUTH payload parsing failed\n");
                return -1;
            }
            if (auth_method != IKE_AUTH_METHOD_PSK) {
                DEBUG("Unsupported AUTH method\n");
                return -1;
            }
            break;
        case IKE_PT_TRAFFIC_SELECTOR_I:
            if (process_ts_payload(p, remaining_len, &cur_len, &next_type, &ike_ctx.local_ts.start,
                                   &ike_ctx.local_ts.end) < 0) {
                DEBUG("TSi payload parsing failed\n");
                return -1;
            }
            DEBUG("Received TSi\n");
            DEBUG("From:\n");
            //od_hex_dump(&ike_ctx.local_ts.start, 16, 16);
            DEBUG("To:\n");
            //od_hex_dump(&ike_ctx.local_ts.end, 16, 16);
            break;
        case IKE_PT_TRAFFIC_SELECTOR_R:
            if (process_ts_payload(p, remaining_len, &cur_len, &next_type, &ike_ctx.remote_ts.start,
                                   &ike_ctx.remote_ts.end) < 0) {
                DEBUG("TSr payload parsing failed\n");
                return -1;
            }
            DEBUG("Received TSi\n");
            DEBUG("From:\n");
            //od_hex_dump(&ike_ctx.remote_ts.start, 16, 16);
            DEBUG("To:\n");
            //od_hex_dump(&ike_ctx.remote_ts.end, 16, 16);
            break;
        default:
            if (process_unknown_payload(p, remaining_len, &cur_len, &next_type) < 0) {
                DEBUG("Unknown payload parsing failed\n");
                return -1;
            }
            DEBUG("Parsed unknown payload of size %u\n", cur_len);
        }
        remaining_len -= cur_len;
        p += cur_len;
    }
    // verify
    /* Construct Authentication */
    uint8_t authed_data_buff[HASH_SIZE_SHA1];
    chunk_t authed_data = { .len = sizeof(authed_data_buff),
                            .ptr = (char *)authed_data_buff };
    chunk_t maced_id;

    _prf(ike_ctx.sk_pr, idx, &maced_id);

    sha1_update(&ike_ctx.real_message_2_digest_ctx, ike_ctx.ike_nonce_i.ptr,
                ike_ctx.ike_nonce_i.len);
    sha1_update(&ike_ctx.real_message_2_digest_ctx, maced_id.ptr, maced_id.len);
    free_chunk(&maced_id);
    sha1_final_hmac(&ike_ctx.real_message_2_digest_ctx, authed_data.ptr);

    if (authed_data.len != auth_data_recv.len ||
        memcmp(authed_data.ptr, auth_data_recv.ptr, authed_data.len)) {
        DEBUG("Authentication failed\n");
        return -1;
    }
    return 0;
}

static int _build_delete_i(char *msg, size_t *msg_len)
{
    size_t cur_len = 0;
    size_t new_len;
    int error;

    /* Construct IKE header */
    ike_header_t hdr = {
        .ike_sa_spi_i = htonll(ike_ctx.ike_spi_i),
        .ike_sa_spi_r = htonll(ike_ctx.ike_spi_r),
        .next_payload = IKE_PT_ENCRYPTED_AND_AUTHENTICATED,
        .mjver_mnver = IKE_V2_MJVER | IKE_V2_MNVER,
        .exchange_type = IKE_ET_INFORMATIONAL,
        .flags = IKE_INITIATOR_FLAG,
        .message_id = htonl(2),
    };

    memcpy(msg, &hdr, sizeof(hdr));
    cur_len += sizeof(hdr);

    /* Construct Delete payload */
    error = build_delete_payload(msg + cur_len, MSG_BUF_LEN - cur_len, &new_len,
                                 IKE_PT_NO_NEXT_PAYLOAD);
    if (error < 0) {
        return error;
    }
    cur_len += new_len;

    /* Construct Encrypted payload */
    chunk_t enc_in = {
        .ptr = msg + sizeof(hdr),
        .len = cur_len - sizeof(hdr),
    };

    error = build_encrypted_payload(msg + sizeof(hdr), MSG_BUF_LEN - sizeof(hdr), &new_len,
                                    IKE_PT_DELETE,
                                    enc_in, ike_ctx.sk_ei, ike_ctx.sk_ai);
    if (error < 0) {
        return error;
    }

    *msg_len = sizeof(hdr) + new_len;
    return 0;
}

static int _generate_child_key(void)
{
    uint8_t enc_i_buff[16];
    uint8_t int_i_buff[20];
    uint8_t enc_r_buff[16];
    uint8_t int_r_buff[20];
    uint8_t nonces_buff[MAX_NONCE_SIZE * 2];
    chunk_t enc_i = chunk_from_buff(enc_i_buff);
    chunk_t int_i = chunk_from_buff(int_i_buff);
    chunk_t enc_r = chunk_from_buff(enc_r_buff);
    chunk_t int_r = chunk_from_buff(int_r_buff);
    chunk_t parts[] = { enc_i, int_i, enc_r, int_r };
    chunk_t nonces = { .len = ike_ctx.ike_nonce_i.len + ike_ctx.ike_nonce_r.len };
    chunk_t out;

    if (nonces.len > countof(nonces_buff)) {
        DEBUG("Nonces too big\n");
        return -1;
    }

    nonces.ptr = (char *)nonces_buff;
    memcpy(nonces.ptr, ike_ctx.ike_nonce_i.ptr, ike_ctx.ike_nonce_i.len);
    memcpy(nonces.ptr + ike_ctx.ike_nonce_i.len, ike_ctx.ike_nonce_r.ptr, ike_ctx.ike_nonce_r.len);
    _prf_plus(ike_ctx.sk_d, nonces, 72, &out);
    DEBUG("Generated keying material\n");
    DEBUG_CHUNK(out, 8);
    char *p = out.ptr;

    for (size_t i = 0; i < countof(parts); ++i) {
        chunk_t part = parts[i];
        memcpy(part.ptr, p, part.len);
        p += part.len;
    }
    DEBUG("Encryption Initiator\n");
    DEBUG_CHUNK(enc_i, 8);
    DEBUG("Encryption Responder\n");
    DEBUG_CHUNK(enc_r, 8);
    DEBUG("Integrity Initiator\n");
    DEBUG_CHUNK(int_i, 8);
    DEBUG("Integrity Responder\n");
    DEBUG_CHUNK(int_r, 8);
    free_chunk(&out);
    if (install_esp(int_i, int_r, enc_i, enc_r, ike_ctx.local_ip, ike_ctx.remote_ip,
                    ike_ctx.child_spi_i, ike_ctx.child_spi_r, (int *)&ike_ctx.sp_in_idx,
                    (int *)&ike_ctx.sp_out_idx)) {
        DEBUG("Failed to install SA in kernel database\n");
        return -1;
    }
    return 0;
}
static int _generate_key(void)
{
    const unsigned char p[] =
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2,
      0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08,
      0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79,
      0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D,
      0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76,
      0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6,
      0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11,
      0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8,
      0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8,
      0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56,
      0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E,
      0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46,
      0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2,
      0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6,
      0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18,
      0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF };
    const unsigned char g[] = { 0x02 };

    ike_ctx.pubkey_i = malloc_chunk(256);
    ike_ctx.privkey_i = malloc_chunk(29);

    if (wc_InitRng(&ike_ctx.wc_rng) != 0) {
        return -1;
    }
    if (wc_InitDhKey(&ike_ctx.wc_priv_key) != 0) {
        wc_FreeRng(&ike_ctx.wc_rng);
        return -1;
    }
    if (wc_DhSetKey(&ike_ctx.wc_priv_key, p, sizeof(p), g, sizeof(g)) != 0) {
        wc_FreeRng(&ike_ctx.wc_rng);
        wc_FreeDhKey(&ike_ctx.wc_priv_key);
        return -1;
    }
    if (wc_DhGenerateKeyPair(&ike_ctx.wc_priv_key, &ike_ctx.wc_rng,
                             (unsigned char *)ike_ctx.privkey_i.ptr,
                             &ike_ctx.privkey_i.len,
                             (unsigned char *)ike_ctx.pubkey_i.ptr, &ike_ctx.pubkey_i.len) != 0) {
        wc_FreeRng(&ike_ctx.wc_rng);
        wc_FreeDhKey(&ike_ctx.wc_priv_key);
        return -1;
    }
    DEBUG("Pub:\n");
    DEBUG_CHUNK(ike_ctx.pubkey_i, 8);
    DEBUG("Priv:\n");
    DEBUG_CHUNK(ike_ctx.privkey_i, 8);
    return 0;
}

static int _get_secrets(void)
{
    ike_ctx.shared_secret = malloc_chunk(ike_ctx.pubkey_i.len);
    word32 len;

    if (wc_DhAgree(&ike_ctx.wc_priv_key, (unsigned char *)ike_ctx.shared_secret.ptr, &len,
                   (unsigned char *)ike_ctx.privkey_i.ptr, ike_ctx.privkey_i.len,
                   (unsigned char *)ike_ctx.pubkey_r.ptr, ike_ctx.pubkey_r.len) != 0) {
        DEBUG("Getting shared secret failed\n");
        return -1;
    }
    ike_ctx.shared_secret.len = len;
    DEBUG("Shared secret:\n");
    DEBUG_CHUNK(ike_ctx.shared_secret, 8);
    chunk_t nonce_concat = malloc_chunk(ike_ctx.ike_nonce_i.len + ike_ctx.ike_nonce_r.len);

    memcpy(nonce_concat.ptr, ike_ctx.ike_nonce_i.ptr, ike_ctx.ike_nonce_i.len);
    memcpy(nonce_concat.ptr + ike_ctx.ike_nonce_i.len, ike_ctx.ike_nonce_r.ptr,
           ike_ctx.ike_nonce_r.len);
    _prf(nonce_concat, ike_ctx.shared_secret, &ike_ctx.skeyseed);
    free_chunk(&nonce_concat);
    DEBUG("SKEYSEED:\n");
    DEBUG_CHUNK(ike_ctx.skeyseed, 8);
    chunk_t crypto_concat = empty_chunk;
    chunk_t ni_nr_spi_spr =
        malloc_chunk(
            ike_ctx.ike_nonce_i.len + ike_ctx.ike_nonce_r.len + sizeof(ike_ctx.ike_spi_i) +
            sizeof(ike_ctx.ike_spi_r));
    uint64_t n_ike_spi_i = htonll(ike_ctx.ike_spi_i);
    uint64_t n_ike_spi_r = htonll(ike_ctx.ike_spi_r);

    memcpy(ni_nr_spi_spr.ptr, ike_ctx.ike_nonce_i.ptr, ike_ctx.ike_nonce_i.len);
    memcpy(ni_nr_spi_spr.ptr + ike_ctx.ike_nonce_i.len, ike_ctx.ike_nonce_r.ptr,
           ike_ctx.ike_nonce_r.len);
    memcpy(ni_nr_spi_spr.ptr + ike_ctx.ike_nonce_i.len + ike_ctx.ike_nonce_r.len, &n_ike_spi_i,
           sizeof(ike_ctx.ike_spi_i));
    memcpy(ni_nr_spi_spr.ptr + ike_ctx.ike_nonce_i.len + ike_ctx.ike_nonce_r.len +
           sizeof(ike_ctx.ike_spi_i), &n_ike_spi_r, sizeof(ike_ctx.ike_spi_r));

    _prf_plus(ike_ctx.skeyseed, ni_nr_spi_spr,
              5 * KEY_SIZE_SHA1 + 2 * (ike_ctx.ike_key_size / 8),
              &crypto_concat);
    ike_ctx.sk_d = malloc_chunk(KEY_SIZE_SHA1);
    ike_ctx.sk_ai = malloc_chunk(KEY_SIZE_SHA1);
    ike_ctx.sk_ar = malloc_chunk(KEY_SIZE_SHA1);
    ike_ctx.sk_ei = malloc_chunk(ike_ctx.ike_key_size / 8);
    ike_ctx.sk_er = malloc_chunk(ike_ctx.ike_key_size / 8);
    ike_ctx.sk_pi = malloc_chunk(KEY_SIZE_SHA1);
    ike_ctx.sk_pr = malloc_chunk(KEY_SIZE_SHA1);
    chunk_t *parts[] = {
        &ike_ctx.sk_d,
        &ike_ctx.sk_ai,
        &ike_ctx.sk_ar,
        &ike_ctx.sk_ei,
        &ike_ctx.sk_er,
        &ike_ctx.sk_pi,
        &ike_ctx.sk_pr,
    };
    const char *parts_names[] = {
        "SK_d",
        "SK_ai",
        "SK_ar",
        "SK_ei",
        "SK_er",
        "SK_pi",
        "SK_pr",
    };
    char *p = crypto_concat.ptr;

    for (unsigned int i = 0; i < sizeof(parts) / sizeof(*parts); ++i) {
        chunk_t *part = parts[i];
        memcpy(part->ptr, p, part->len);
        p += part->len;
        DEBUG("%s", parts_names[i]);
        DEBUG_CHUNK(*part, 8);
    }

    free_chunk(&ni_nr_spi_spr);
    free_chunk(&crypto_concat);

    return 0;
}

static void _get_auth_secret(chunk_t *inner_key)
{
    char pad_text[] = "Key Pad for IKEv2";
    chunk_t textc = { .ptr = pad_text, .len = strlen(pad_text) };

    _prf(ike_ctx.psk, textc, inner_key);
}

static void _prf(chunk_t key, chunk_t seed, chunk_t *out)
{
    sha1_context hash;

    sha1_init_hmac(&hash, (unsigned char *)key.ptr, key.len);
    sha1_update(&hash, (unsigned char *)seed.ptr, seed.len);
    *out = malloc_chunk(HASH_SIZE_SHA1);
    sha1_final_hmac(&hash, (unsigned char *)out->ptr);
}

static void _prf_plus(chunk_t key, chunk_t seed, size_t len, chunk_t *out)
{
    uint8_t cnt = 0x01;
    chunk_t ret = empty_chunk;
    chunk_t first_seed = malloc_chunk(seed.len + 1);
    chunk_t next_seed = malloc_chunk(HASH_SIZE_SHA1 + seed.len + 1);

    memcpy(first_seed.ptr, seed.ptr, seed.len);
    memcpy(first_seed.ptr + seed.len, &cnt, 1);
    memcpy(next_seed.ptr + HASH_SIZE_SHA1, seed.ptr, seed.len);
    chunk_t t = empty_chunk;

    _prf(key, first_seed, &t);
    ret = malloc_chunk(t.len);
    memcpy(ret.ptr, t.ptr, t.len);
    while (ret.len < len) {
        cnt++;
        memcpy(next_seed.ptr, t.ptr, t.len);
        memcpy(next_seed.ptr + HASH_SIZE_SHA1 + seed.len, &cnt, 1);
        free_chunk(&t);
        _prf(key, next_seed, &t);
        ret.ptr = realloc(ret.ptr, ret.len + HASH_SIZE_SHA1);
        memcpy(ret.ptr + ret.len, t.ptr, t.len);
        ret.len += HASH_SIZE_SHA1;
    }
    *out = ret;
    free_chunk(&first_seed);
    free_chunk(&next_seed);
    free_chunk(&t);
}
