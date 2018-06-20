/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sock
 * @{
 *
 * @file
 * @brief       sock_secure utility functions implementation
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#include "net/sock/secure.h"
#include "net/tlsman.h"


#define ENABLE_DEBUG (1)
#include "debug.h"

/* TODO FIXME universal buffer (which in turn must be enough bigh for payload) */

/* FIXME Maximum size is defined by (D)TLS library stack (example DTLS_MAX_BUFF for tinydtls) */
/* TODO: Switch to something similar for TCP
 * NOTE: Probably not feasible because exteneral packages handles the
 *       assembly of the DTLS packets)
 */
static uint8_t dtls_header[100];

static ssize_t _connect(sock_secure_session_t *sock_secure, uint8_t *data,
                 size_t data_size){
  sock_secure->session.tlsman_create_channel(&sock_secure->session.session,
     sock_secure->flag, data, data_size);

  sock_secure->state = SOCK_SECURE_STATE_STARTED;
  return 0;
}

ssize_t sock_secure_load_stack(sock_secure_session_t *sock_secure,
                             uint16_t *ciphers, size_t ciphers_size)
 {

   /* TODO Add DEBUG lines for display technical info? */
  ssize_t res = tlsman_load_stack(&sock_secure->session, ciphers, ciphers_size,
                    sock_secure->flag);

  if (res == 0) {
    sock_secure->state = SOCK_SECURE_STATE_NON_STARTED;
    return 0;
  } else {
    sock_secure->state =  SOCK_SECURE_STATE_FATAL;
    return SOCK_SECURE_STATE_FATAL;
  }

}

ssize_t sock_secure_initialized(sock_secure_session_t *sock_secure,
                              tlsman_resp_handler_t callback,
                              void *sock,
                              sock_secure_ep_t *local,
                              sock_secure_ep_t *remote)
{
   ssize_t res;

   sock_secure->session.session.sock = sock;
   sock_secure->cb = callback;

  /*
   *  FIXME: TLSMAN Should handling this part
   *  HOWEVER, TLSMAN should be agnostic to sock, socket or whatever library is being used
   */
   sock_secure->session.session.local = (sock_udp_ep_t *) local;
   sock_secure->session.session.remote = (sock_udp_ep_t *) remote;


   /* TODO: SOCK_HAS_ASYNC suppport */

   res = sock_secure->session.tlsman_init_context(&sock_secure->session.session,
                              callback,
                              sock_secure->flag);

    if (res == 0) {
       if (sock_secure->flag & TLSMAN_FLAG_SIDE_SERVER) {
          sock_secure->state = SOCK_SECURE_STATE_SERVER_LISTENING;
       }
       else {
          sock_secure->state = SOCK_SECURE_STATE_NON_STARTED;
        }
    }

   return 0;
}

ssize_t sock_secure_update(sock_secure_session_t *sock_secure)
{
  /* Check the status, if necessary coonnect to the peeer (create the secure session)*/

  /* FIXME: Verify that not new resource is required */

  switch (sock_secure->state) {
    case SOCK_SECURE_STATE_NON_INITIALIZED:
      /* FIXME  What type of error? Is */
      DEBUG("%s: Secure session parameters not initialized!\n", __func__);
      return -1;

    case SOCK_SECURE_STATE_NON_STARTED:
      DEBUG("%s: Creating the secure channel between locan and remote!\n", __func__);
      _connect(sock_secure, dtls_header, sizeof(dtls_header));
    break;

    case SOCK_SECURE_STATE_STARTED:
      DEBUG("%s: (D)TLS Handshake started\n", __func__);
      /* TODO: Identify the current status of the handshake */
    break;

    case SOCK_SECURE_STATE_FATAL:
      DEBUG("%s: Unable to advnace!\n", __func__);
      return -1;
    break;

    case SOCK_SECURE_STATE_SERVER_LISTENING:
      DEBUG("%s: Server is in permanent listening mode\n", __func__);
    break;

    default:
      DEBUG("%s: Unknown sock_secure session status (%u)\n", __func__, sock_secure->state );
    break;
  }

  return 0;
}

ssize_t sock_secure_write(sock_secure_session_t *sock_secure,
                          const void* data, size_t len)
{

  if (sock_secure->flag & TLSMAN_FLAG_SIDE_SERVER){
    DEBUG("%s: Server side sending data app\n",__func__);
    return sock_secure->session.tlsman_send_data_app(&sock_secure->session.session,
                                                     data, len);
  }

  sock_secure_update(sock_secure);

  /* TODO: Identify the current status of the handshake   */

  /* TODO: Create events or timeouts here */
  if (sock_secure->state != SOCK_SECURE_STATE_STARTED) {
    sock_secure->state = SOCK_SECURE_STATE_ERROR;
    return -1;
  }
  DEBUG("%s: Client side sending data app\n",__func__);
  sock_secure->session.tlsman_send_data_app(&sock_secure->session.session,
                      data, len);

  return 0;
}

ssize_t sock_secure_read(sock_secure_session_t *sock_secure)
{

  if (sock_secure->flag & TLSMAN_FLAG_SIDE_SERVER){

    return sock_secure->session.tlsman_listening(&sock_secure->session.session,
                                                 sock_secure->flag,
                                                 dtls_header,
                                                 sizeof(dtls_header));
  }

  /* TODO: Identify the current status of the handshake   */
  /* NOTE: If channel is not ready then exit? (Check TCP behavior) */
  sock_secure_update(sock_secure);
  sock_secure->session.tlsman_retrieve_data_app(&sock_secure->session.session,
                        dtls_header, sizeof(dtls_header));

  return 0;
}

void sock_secure_release(sock_secure_session_t *sock_secure)
{
  /* TODO is worth to check the current status? */

  sock_secure->session.tlsman_release_resources(&sock_secure->session.session);
  /*
   * WARNING: Some (D)TLS stacks will send back a DTLS alert record
   * If not handled properly (remove it from the gnrc queue) will have a negative
   * impact for the next time time a secure session is attempted.
   */
  sock_secure->session.tlsman_retrieve_data_app(&sock_secure->session.session,
                        dtls_header, sizeof(dtls_header));

  /* This is ready for a new secure session */
  sock_secure->state = SOCK_SECURE_STATE_NON_STARTED;

  /* TODO DISCUSSION should be in its own function?  */
  /* NOTE: Probably tinydtls?sock.c must rework this part  if tlsman_release_resources()
  *  was called previously */
  sock_secure->session.tlsman_close_channel(&sock_secure->session.session);

}
