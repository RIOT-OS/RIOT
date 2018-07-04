/*
 * Copyright (C) 2016-17 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_nanocoap
 * @{
 *
 * @file
 * @brief       Nanocoap sock helpers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "net/nanocoap.h"
#include "net/sock/udp.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#ifdef MODULE_SOCK_SECURE
#include "net/sock/secure.h"
#include "od.h"

#define SECURE_CIPHER_PSK_IDS (0xC0A8)
#define SECURE_CIPHER_RPK_IDS (0xC0AE)
#define SECURE_CIPHER_LIST { SECURE_CIPHER_PSK_IDS, SECURE_CIPHER_RPK_IDS }

sock_secure_session_t secure_session = { .flag=0, .cb=NULL};
uint8_t *ext_buf;
size_t  ext_bufsize;

static void _dtls_server_data_app_handler(uint8_t *buf, size_t bufsize, void *sock)
{
    (void) sock;
    coap_pkt_t pkt;
    ssize_t res = bufsize;
    if (coap_parse(&pkt, buf, res) < 0) {
        DEBUG("error parsing packet\n");
        return;
    }
    if ((res = coap_handle_req(&pkt, ext_buf, res)) > 0) {
        if(ENABLE_DEBUG){
          DEBUG("%s: Sending %i bytes by means of secure channel...\n", __func__, res );
          od_hex_dump(ext_buf, res, OD_WIDTH_DEFAULT);
        }

        sock_secure_write(&secure_session, ext_buf, res);
    }
}

static coap_pkt_t client_pkt;

static void _dtls_client_data_app_handler(uint8_t *buf, size_t bufsize, void *sock)
{
    (void) sock;

    // size_t pdu_len = (client_pkt.payload - (uint8_t *)client_pkt.hdr) + client_pkt.payload_len;
    if (coap_parse(&client_pkt, buf, bufsize) < 0)  {
        DEBUG("nanocoap: error parsing packet\n");
        return;
    }
    /* FIXME */
    DEBUG("%s: CoAPS Hex raw bytes: \n", __func__);
    od_hex_dump(buf, bufsize, OD_WIDTH_DEFAULT);
}


#endif /* MODULE_SOCK_SECURE */

ssize_t nanocoap_request(coap_pkt_t *pkt, sock_udp_ep_t *local, sock_udp_ep_t *remote, size_t len)
{
    ssize_t res;
    size_t pdu_len = (pkt->payload - (uint8_t *)pkt->hdr) + pkt->payload_len;
    uint8_t *buf = (uint8_t*)pkt->hdr;
    sock_udp_t sock;

    if (!remote->port) {
#ifdef MODULE_SOCK_SECURE
      local->port = COAP_PORT +1; /* CoAPS default port 5684 */
#else
      local->port = COAP_PORT;
#endif
    }

#ifdef MODULE_SOCK_SECURE
    secure_session.flag = TLSMAN_FLAG_STACK_DTLS | TLSMAN_FLAG_SIDE_CLIENT;
    uint16_t ciphers[] = SECURE_CIPHER_LIST;
    /*  FIXME (This should be called only one time!) */
    res = sock_secure_load_stack(&secure_session, ciphers, sizeof(ciphers));
    if (res) {
      puts("ERROR: Unable to load (D)TLS stack ");
      return -1;
    }
    // ext_buf = buf;
    // ext_bufsize = bufsize;
#endif /* MODULE_SOCK_SECURE */

    res = sock_udp_create(&sock, local, remote, 0);
    if (res < 0) {
        return res;
    }

    /* TODO: timeout random between between ACK_TIMEOUT and (ACK_TIMEOUT *
     * ACK_RANDOM_FACTOR) */
    uint32_t timeout = COAP_ACK_TIMEOUT * (1000000U);
    int tries = 0;
#ifdef MODULE_SOCK_SECURE

    res = sock_secure_initialized(&secure_session,
                                  _dtls_client_data_app_handler,
                                  (void *)&sock,
                                  (sock_secure_ep_t*)local,
                                  (sock_secure_ep_t *)remote);

    if (res != 0) {
        puts("ERROR: Unable to init sock_secure!");
        return -1;
    }

    /* NOTE: This start the dtls handhsake. */
    sock_secure_update(&secure_session);

    (void) tries;
    (void) timeout;
    (void) len;

    /* FIXME Upgrade sock_secure for supporting */
    while (tries++ < 1) {
      if (!tries) {
          DEBUG("nanocoap: maximum retries reached.\n");
          res = -ETIMEDOUT;
          goto out;
      }
      /* TODO sock_secure_[write|read]() must accept custom timeouts */
      /* TODO sock_secure_[write|read]() should return written bytes? */
      sock_secure_write(&secure_session, buf, pdu_len);
      sock_secure_read(&secure_session);

    }
    goto out;
#else /* MODULE_SOCK_SECURE */
    while (tries++ < COAP_MAX_RETRANSMIT) {
        if (!tries) {
            DEBUG("nanocoap: maximum retries reached.\n");
            res = -ETIMEDOUT;
            goto out;
        }

        res = sock_udp_send(&sock, buf, pdu_len, NULL);
        if (res <= 0) {
            DEBUG("nanocoap: error sending coap request\n");
            goto out;
        }

        res = sock_udp_recv(&sock, buf, len, timeout, NULL);
        if (res <= 0) {
            if (res == -ETIMEDOUT) {
                DEBUG("nanocoap: timeout\n");

                timeout *= 2;
                continue;
            }
            DEBUG("nanocoap: error receiving coap request\n");
            break;
        }
        else {
            if (coap_parse(pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("nanocoap: error parsing packet\n");
                res = -EBADMSG;
            }
            break;
        }
    }
#endif /* MODULE_SOCK_SECURE */
out:
#ifdef MODULE_SOCK_SECURE
    sock_secure_release(&secure_session);
#endif
    sock_udp_close(&sock);

    return res;
}

ssize_t nanocoap_get(sock_udp_ep_t *remote, const char *path, uint8_t *buf, size_t len)
{
    ssize_t res;
    coap_pkt_t pkt;
    uint8_t *pktpos = buf;

    pkt.hdr = (coap_hdr_t*)buf;
    pktpos += coap_build_hdr(pkt.hdr, COAP_REQ, NULL, 0, COAP_METHOD_GET, 1);
    pktpos += coap_put_option_uri(pktpos, 0, path, COAP_OPT_URI_PATH);
    pkt.payload = pktpos;
    pkt.payload_len = 0;

    res = nanocoap_request(&pkt, NULL, remote, len);
    if (res < 0) {
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if (res != 205) {
            res = -res;
        }
        else {
            if (pkt.payload_len) {
                memmove(buf, pkt.payload, pkt.payload_len);
            }
            res = pkt.payload_len;
        }
    }
    return res;
}

int nanocoap_server(sock_udp_ep_t *local, uint8_t *buf, size_t bufsize)
{
    sock_udp_t sock;
    sock_udp_ep_t remote;

#ifdef MODULE_SOCK_SECURE
    secure_session.flag = TLSMAN_FLAG_STACK_DTLS | TLSMAN_FLAG_SIDE_SERVER;
    uint16_t ciphers[] = SECURE_CIPHER_LIST;
    sock_secure_load_stack(&secure_session, ciphers, sizeof(ciphers));
    ext_buf = buf;
    ext_bufsize = bufsize;
#endif /* MODULE_SOCK_SECURE */

    if (!local->port) {
#ifdef MODULE_SOCK_SECURE
        local->port = COAP_PORT +1; /* CoAPS default port 5684 */
#else
        local->port = COAP_PORT;
#endif
    }

    ssize_t res = sock_udp_create(&sock, local, NULL, 0);
    if (res == -1) {
        return -1;
    }

    DEBUG("Listening to udp port %u\n", local->port);

#ifdef MODULE_SOCK_SECURE
    res = sock_secure_initialized(&secure_session,
                                          _dtls_server_data_app_handler,
                                          (void *)&sock,
                                          (sock_secure_ep_t*)&local,
                                          (sock_secure_ep_t *)&remote);

    if (res != 0) {
        puts("ERROR: Unable to init sock_secure!");
        return -1;
    }

      while(sock_secure_read(&secure_session))
      {
        xtimer_usleep(500);
      }

      sock_secure_release(&secure_session);

#else /* MODULE_SOCK_SECURE */
    while (1) {
        res = sock_udp_recv(&sock, buf, bufsize, -1, &remote);
        if (res == -1) {
            DEBUG("error receiving UDP packet\n");
            return -1;
        }
        else {
            coap_pkt_t pkt;
            if (coap_parse(&pkt, (uint8_t *)buf, res) < 0) {
                DEBUG("error parsing packet\n");
                continue;
            }
            if ((res = coap_handle_req(&pkt, buf, bufsize)) > 0) {
                res = sock_udp_send(&sock, buf, res, &remote);
            }
        }
    }
#endif /* MODULE_SOCK_SECURE */

    return 0;
}
