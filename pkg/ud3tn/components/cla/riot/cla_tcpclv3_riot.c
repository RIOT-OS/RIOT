// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#include "cla/cla.h"
#include "cla/cla_contact_tx_task.h"
#include "cla/posix/cla_tcpclv3.h"
#include "cla/posix/cla_tcpclv3_proto.h"
#include "cla/posix/cla_tcp_common.h"

#include "bundle6/parser.h"
#include "bundle6/sdnv.h"
#include "bundle7/parser.h"
#include "net/ipv4/addr.h"

#include "platform/hal_io.h"
#include "platform/hal_queue.h"
#include "platform/hal_semaphore.h"
#include "platform/hal_task.h"

#include "ud3tn/bundle_processor.h"
#include "ud3tn/cmdline.h"
#include "ud3tn/common.h"
#include "ud3tn/eid.h"
#include "ud3tn/result.h"
#include "ud3tn/simplehtab.h"

#include "net/sock/tcp.h"
#include "net/sock/util.h"
#include "net/af.h"
#include "net/gnrc/tcp.h"


#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TCPCLV3_RX_TIMEOUT_MS  (100U)
#define TCPCLV3_ACCEPT_POLL_MS (100U)
static const char *CLA_NAME = "tcpclv3-riot";


/* Wrapper structure for sock_tcp operations */
struct tcpclv3_socket {
    sock_tcp_t *sock;
    sock_tcp_t storage;
    bool valid;
};

/* Main configuration structure */
struct tcpclv3_config {
    struct cla_config base;

    /* Listening queue for accepting incoming connections */
    sock_tcp_queue_t listen_queue;
    sock_tcp_t listen_queue_array[4];
    bool listening;

    /* Hash table for managing active connections by EID */
    struct htab_entrylist *param_htab_elem[32];
    struct htab param_htab;
    Semaphore_t param_htab_sem;
};

/* Connection state enumeration */
enum TCPCLV3_STATE {
    TCPCLV3_INACTIVE,
    TCPCLV3_CONNECTING,
    TCPCLV3_CONNECTED,
    TCPCLV3_ESTABLISHED,
};

/* Per-contact/connection parameters */
struct tcpclv3_contact_parameters {
    struct cla_tcp_link link;

    struct tcpclv3_config *config;
    Semaphore_t param_semphr;

    char *eid;
    char *cla_addr;

    struct tcpclv3_socket socket;

    enum TCPCLV3_STATE state;
    bool opportunistic;

    struct tcpclv3_parser tcpclv3_parser;
    struct cla_tcp_rate_limit_config rl_config;
};

/*
 * Helper functions for sock_tcp operations
 */

static void tcpclv3_socket_close(struct tcpclv3_socket *s)
{
    if (!s->valid) {
        return;
    }

    UD3TN_LOG_INFO("TCPCLv3: Disconnecting socket");
    sock_tcp_disconnect(s->sock);
    s->valid = false;
    UD3TN_LOG_INFO("TCPCLv3: Socket disconnected; GNRC close completed");
}

static ssize_t tcpclv3_send_all(struct tcpclv3_socket *sock,
                                const void *buffer, size_t length)
{
    if (!sock || !sock->valid) {
        LOGF_ERROR("%s", "TCPCLv3: Invalid socket for sending data");
        return -1;
    }

    const uint8_t *data = (const uint8_t *)buffer;
    size_t total_sent = 0;
    // printf("Sending data: %zu bytes\n", length);
    while (total_sent < length) {
        // printf("Writing data: %zu bytes\n", length - total_sent);
        ssize_t sent = sock_tcp_write(sock->sock,
                                      data + total_sent,
                                      length - total_sent);
        // printf("Sent value: %zd\n", sent);
        if (sent < 0) {
            // printf("Error sending data: %zd\n", sent);
            return sent;
        }
        if (sent == 0) {
            break;
        }
        total_sent += sent;
    }
    // printf("Sending Success:Total bytes sent: %zu\n", total_sent);
    return total_sent;
}

static ssize_t tcpclv3_recv_all(struct tcpclv3_socket *sock,
                                void *buffer, size_t length)
{
    if (!sock || !sock->valid) {
        return -1;
    }

    uint8_t *data = (uint8_t *)buffer;
    size_t total_read = 0;

    while (total_read < length) {
        ssize_t read_bytes = sock_tcp_read(sock->sock,
                                           data + total_read,
                                           length - total_read,
                                           TCPCLV3_RX_TIMEOUT_MS);
        if (read_bytes < 0) {
            return read_bytes;
        }
        if (read_bytes == 0) {
            break;
        }
        total_read += read_bytes;
    }

    return total_read;
}

/*
 * MGMT
 */

static enum ud3tn_result cla_tcpclv3_perform_handshake(
    struct tcpclv3_contact_parameters *const param)
{
    UD3TN_LOG_INFO("TCPCLv3: Starting handshake");
    const char *const local_eid =
        param->config->base.bundle_agent_interface->local_eid;
    size_t header_len;
    char *const header = cla_tcpclv3_generate_contact_header(
        local_eid,
        &header_len
        );

    if (!header) {
        return UD3TN_FAIL;
    }

    hal_semaphore_release(param->param_semphr);

    if (tcpclv3_send_all(&param->socket, header, header_len) < 0) {
        free(header);
        LOGF_ERROR("%s", "TCPCLv3: send(header) failed");
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    free(header);

    char header_buf[8];

    if (tcpclv3_recv_all(&param->socket, header_buf, 8) != 8 ||
        memcmp(header_buf, "dtn!", 4) != 0 ||
        header_buf[4] < 0x03) {
        UD3TN_LOG_WARN("TCPCLv3: Did not receive proper \"dtn!\" magic!");
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    uint8_t cur_byte;
    struct sdnv_state sdnv_state;
    uint32_t peer_eid_len = 0;

    sdnv_reset(&sdnv_state);
    while (sdnv_state.status == SDNV_IN_PROGRESS) {
        ssize_t read_res = tcpclv3_recv_all(&param->socket, &cur_byte, 1);
        if (read_res != 1) {
            break;
        }
        sdnv_read_u32(&sdnv_state, &peer_eid_len, cur_byte);
    }

    if (sdnv_state.status != SDNV_DONE) {
        UD3TN_LOG_WARN("TCPCLv3: Error receiving EID length SDNV!");
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    if (peer_eid_len > EID_MAX_LEN) {
        UD3TN_LOG_WARN("TCPCLv3: Peer specified an EID length that is too big!");
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    char *eid_buf = malloc(peer_eid_len + 1);

    if (!eid_buf) {
        LOGF_ERROR(
            "TCPCLv3: Error allocating memory (%u byte(s)) for EID!",
            peer_eid_len
            );
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    if (tcpclv3_recv_all(&param->socket, eid_buf, peer_eid_len)
        != (ssize_t)peer_eid_len) {
        free(eid_buf);
        LOGF_WARN(
            "TCPCLv3: Error receiving peer EID of len %u byte(s)",
            peer_eid_len
            );
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    eid_buf[peer_eid_len] = '\0';
    if (validate_eid(eid_buf) != UD3TN_OK) {
        LOGF_WARN(
            "TCPCLv3: Received invalid peer EID of len %u: \"%s\"",
            peer_eid_len,
            eid_buf
            );
        free(eid_buf);
        hal_semaphore_take_blocking(param->param_semphr);
        return UD3TN_FAIL;
    }

    hal_semaphore_take_blocking(param->param_semphr);

    LOGF_INFO(
        "TCPCLv3: Handshake performed with \"%s\", reports EID \"%s\"",
        param->cla_addr ? param->cla_addr : "<incoming>",
        eid_buf
        );
    if (param->eid) {
        if (strcmp(param->eid, eid_buf) != 0) {
            LOGF_WARN(
                "TCPCLv3: EID \"%s\" differs from configured EID \"%s\", using own configuration",
                eid_buf,
                param->eid
                );
        }
        free(eid_buf);
    }
    else {
        param->eid = eid_buf;
    }

    return UD3TN_OK;
}

static enum ud3tn_result handle_established_connection(
    struct tcpclv3_contact_parameters *const param)
{
    struct tcpclv3_config *const tcpclv3_config = param->config;

    hal_semaphore_take_blocking(tcpclv3_config->param_htab_sem);

    struct tcpclv3_contact_parameters *const other =
        htab_get(&tcpclv3_config->param_htab, param->eid);

    if (other && other != param) {
        hal_semaphore_take_blocking(other->param_semphr);

        LOGF_INFO(
            "TCPCLv3: Taking over management of connection with \"%s\"",
            param->eid
            );
        htab_remove(&tcpclv3_config->param_htab, param->eid);
        if (!other->opportunistic) {
            other->opportunistic = true;
            param->opportunistic = false;
            if (!param->cla_addr) {
                param->cla_addr = other->cla_addr;
                other->cla_addr = NULL;
            }
        }

        hal_semaphore_release(other->param_semphr);
    }

    htab_add(&tcpclv3_config->param_htab, param->eid, param);

    param->state = TCPCLV3_ESTABLISHED;

    /* Initialize CLA link with the socket information */
    param->link.base.config = &tcpclv3_config->base;
    param->link.base.cla_addr = param->cla_addr;
    param->link.connection_socket = -1; /* Not used with sock_tcp */

    if (cla_link_init(&param->link.base, &tcpclv3_config->base,
                      param->cla_addr, true, true) != UD3TN_OK) {
        UD3TN_LOG_ERROR("TCPCLv3: Error initializing CLA link!");
        param->state = TCPCLV3_CONNECTING;
        hal_semaphore_release(tcpclv3_config->param_htab_sem);
        return UD3TN_FAIL;
    }

    hal_semaphore_release(tcpclv3_config->param_htab_sem);
    hal_semaphore_release(param->param_semphr);
    cla_link_wait(&param->link.base);
    hal_semaphore_take_blocking(param->config->param_htab_sem);
    hal_semaphore_take_blocking(param->param_semphr);
    cla_link_cleanup(&param->link.base);
    hal_semaphore_release(param->config->param_htab_sem);

    param->state = TCPCLV3_CONNECTING;
    return UD3TN_OK;
}

static void tcpclv3_link_management_task(void *p)
{
    struct tcpclv3_contact_parameters *const param = p;

    /* Invariant: param_semphr is held here and released only around I/O. */
    for (;;) {
        if (param->state == TCPCLV3_CONNECTING) {

            hal_semaphore_release(param->param_semphr);

            if (cla_tcp_rate_limit_connection_attempts(&param->rl_config)) {
                hal_semaphore_take_blocking(param->param_semphr);
                break;
            }

            hal_semaphore_take_blocking(param->param_semphr);

            if (param->opportunistic || !param->cla_addr ||
                param->cla_addr[0] == '\0') {
                UD3TN_LOG_WARN(
                    "TCPCLv3: No CLA address present, not initiating connection attempt"
                    );
                break;
            }

            char *cla_addr = strdup(param->cla_addr);

            if (!cla_addr) {
                UD3TN_LOG_ERROR("TCPCLv3: Error copying memory");
                break;
            }

            hal_semaphore_release(param->param_semphr);

            sock_tcp_ep_t remote = SOCK_IPV6_EP_ANY;
            int conn_ret = -1;

            remote.port = 4556; /* Default TCPCLv3 port */
            if (sock_tcp_str2ep(&remote, cla_addr) < 0) {
                LOGF_WARN("TCPCLv3: Could not parse remote addr: %s", cla_addr);
            }
            else {
                if (remote.port == 0) {
                    remote.port = 4556;
                }

                LOGF_INFO("TCPCLv3: Connecting to remote CLA addr \"%s\"", cla_addr);
                conn_ret = sock_tcp_connect(
                    &param->socket.storage, &remote, 0, 0);
                if (conn_ret != 0) {
                    LOGF_WARN("TCPCLv3: Connection to \"%s\" failed with code %d",
                              cla_addr, conn_ret);
                }
            }

            hal_semaphore_take_blocking(param->param_semphr);

            if (conn_ret != 0) {
                param->socket.valid = false;
                free(cla_addr);
                continue;
            }

            LOGF_INFO("TCPCLv3: Connected successfully to \"%s\"",
                      cla_addr);
            free(cla_addr);
            param->socket.sock = &param->socket.storage;
            param->socket.valid = true;
            param->state = TCPCLV3_CONNECTED;
        }
        else if (param->state == TCPCLV3_CONNECTED) {
            ASSERT(param->socket.valid);
            if (cla_tcpclv3_perform_handshake(param) == UD3TN_OK) {
                handle_established_connection(param);
            }
            tcpclv3_socket_close(&param->socket);
            if (param->opportunistic) {
                UD3TN_LOG_INFO(
                    "TCPCLv3: Link marked as opportunistic, not initiating reconnection attempt");
                break;
            }
            if (!param->cla_addr || param->cla_addr[0] == '\0') {
                UD3TN_LOG_INFO(
                    "TCPCLv3: No CLA address present, not initiating reconnection attempt");
                break;
            }
            param->state = TCPCLV3_CONNECTING;
        }
        else {
            abort();
        }

    }

    LOGF_INFO(
        "TCPCLv3: Terminating contact link manager%s%s%s",
        param->eid ? " for \"" : "",
        param->eid ? param->eid : "",
        param->eid ? "\"" : ""
        );

    param->state = TCPCLV3_INACTIVE;
    tcpclv3_socket_close(&param->socket);
    hal_semaphore_release(param->param_semphr);

    if (param->eid) {
        hal_semaphore_take_blocking(param->config->param_htab_sem);
        if (htab_get(&param->config->param_htab, param->eid) == param) {
            htab_remove(&param->config->param_htab, param->eid);
        }
        hal_semaphore_release(param->config->param_htab_sem);
    }

    hal_semaphore_take_blocking(param->param_semphr);

    tcpclv3_parser_reset(&param->tcpclv3_parser);
    free(param->eid);
    free(param->cla_addr);
    hal_semaphore_delete(param->param_semphr);
    free(param);
}

static void launch_connection_management_task(
    struct tcpclv3_config *const tcpclv3_config,
    sock_tcp_t *sock,
    const char *eid, const char *cla_addr)
{
    struct tcpclv3_contact_parameters *contact_params =
        malloc(sizeof(struct tcpclv3_contact_parameters));

    if (!contact_params) {
        UD3TN_LOG_ERROR("TCPCLv3: Failed to allocate memory!");
        if (sock) {
            sock_tcp_disconnect(sock);
        }
        return;
    }

    contact_params->config = tcpclv3_config;
    contact_params->socket.sock = &contact_params->socket.storage;
    contact_params->socket.valid = false;

    if (!sock) {
        ASSERT(eid && cla_addr);
        if (!eid || !cla_addr) {
            UD3TN_LOG_ERROR("TCPCLv3: Invalid parameters!");
            free(contact_params);
            return;
        }
        contact_params->eid = strdup(eid);

        if (!contact_params->eid) {
            UD3TN_LOG_ERROR("TCPCLv3: Failed to copy EID!");
            free(contact_params);
            return;
        }

        contact_params->cla_addr = cla_get_connect_addr(cla_addr, "tcpclv3_riot");

        if (!contact_params->cla_addr) {
            UD3TN_LOG_ERROR("TCPCLv3: Invalid address");
            free(contact_params->eid);
            free(contact_params);
            return;
        }

        contact_params->socket.valid = false;
        contact_params->state = TCPCLV3_CONNECTING;
        contact_params->opportunistic = false;
    }
    else {
        ASSERT(!eid && !cla_addr);
        contact_params->eid = NULL;
        contact_params->cla_addr = NULL;
        contact_params->socket.sock = sock;
        contact_params->socket.valid = true;
        contact_params->state = TCPCLV3_CONNECTED;
        contact_params->opportunistic = true;
    }

    contact_params->rl_config.last_connection_attempt_ms = 0;
    contact_params->rl_config.last_connection_attempt_no = 1;

    /* Starts taken: the management task holds it until it blocks on I/O. */
    contact_params->param_semphr = hal_semaphore_init_binary();
    ASSERT(contact_params->param_semphr != NULL);
    if (contact_params->param_semphr == NULL) {
        UD3TN_LOG_ERROR("TCPCLv3: Failed to create semaphore!");
        goto fail;
    }

    if (!tcpclv3_parser_init(&contact_params->tcpclv3_parser)) {
        UD3TN_LOG_ERROR("TCPCLv3: Error initializing parser!");
        goto fail_sem;
    }

    struct htab_entrylist *htab_entry = NULL;

    if (contact_params->eid) {
        htab_entry = htab_add(
            &tcpclv3_config->param_htab,
            contact_params->eid,
            contact_params
            );
        if (!htab_entry) {
            UD3TN_LOG_ERROR("TCPCLv3: Error creating htab entry!");
            goto fail_sem;
        }
    }

    const enum ud3tn_result task_creation_result = hal_task_create(
        tcpclv3_link_management_task,
        contact_params,
        true,
        NULL
        );

    if (task_creation_result != UD3TN_OK) {
        UD3TN_LOG_ERROR("TCPCLv3: Error creating management task!");
        if (htab_entry) {
            ASSERT(contact_params->eid);
            ASSERT(htab_remove(
                       &tcpclv3_config->param_htab,
                       contact_params->eid
                       ) == contact_params);
        }
        goto fail_sem;
    }

    return;

fail_sem:
    hal_semaphore_delete(contact_params->param_semphr);
fail:
    if (sock) {
        sock_tcp_disconnect(sock);
    }
    free(contact_params->eid);
    free(contact_params->cla_addr);
    free(contact_params);
}

static struct tcpclv3_contact_parameters *get_contact_parameters(
    struct cla_config *config, const char *eid)
{
    struct tcpclv3_config *const tcpclv3_config =
        (struct tcpclv3_config *)config;

    return htab_get(&tcpclv3_config->param_htab, eid);
}

static void tcpclv3_listener_task(void *p)
{
    struct tcpclv3_config *const tcpclv3_config = p;
    bool accept_queue_full = false;

    for (;;) {
        sock_tcp_t *sock = NULL;
        /* Finite timeout: gnrc_tcp_accept() can miss an ESTABLISHED notification; each call rescans the queue. */
        const int accept_ret = sock_tcp_accept(
            &tcpclv3_config->listen_queue, &sock, TCPCLV3_ACCEPT_POLL_MS);

        if (accept_ret == -ETIMEDOUT) {
            continue;
        }

        if (accept_ret < 0) {
            if (accept_ret == -ECONNABORTED) {
                continue;
            }
            if (accept_ret == -ENOMEM) {
                if (!accept_queue_full) {
                    UD3TN_LOG_WARN("TCPCLv3: Accept queue full; retrying");
                }
                accept_queue_full = true;
                hal_task_delay(250);
                continue;
            }
            LOGF_WARN("TCPCLv3: Accept failed with code %d", accept_ret);
            return;
        }

        accept_queue_full = false;
        UD3TN_LOG_INFO("TCPCLv3: Accepted incoming socket");
        hal_semaphore_take_blocking(tcpclv3_config->param_htab_sem);

        launch_connection_management_task(
            tcpclv3_config,
            sock,
            NULL,
            NULL
            );
        hal_semaphore_release(tcpclv3_config->param_htab_sem);
    }
    UD3TN_LOG_ERROR("TCPCLv3: Unexpected failure to accept connection - abort!");
    abort();
}

/*
 * API
 */

static enum ud3tn_result tcpclv3_launch(struct cla_config *const config)
{
    return hal_task_create(
        tcpclv3_listener_task,
        config,
        true,
        NULL
        );
}

static enum ud3tn_result tcpclv3_terminate(struct cla_config *const config)
{
    (void)config;
    UD3TN_LOG_DEBUG("TCPCLv3: Terminated gracefully");
    return UD3TN_OK;
}

static const char *tcpclv3_name_get(void)
{
    return CLA_NAME;
}

static size_t tcpclv3_mbs_get(struct cla_config *const config)
{
    (void)config;
    return SIZE_MAX;
}

static struct cla_tx_queue tcpclv3_get_tx_queue(
    struct cla_config *config, const char *eid, const char *cla_addr)
{
    (void)cla_addr;
    struct tcpclv3_config *const tcpclv3_config =
        (struct tcpclv3_config *)config;

    hal_semaphore_take_blocking(tcpclv3_config->param_htab_sem);
    struct tcpclv3_contact_parameters *const param = get_contact_parameters(
        config,
        eid
        );

    if (param != NULL) {
        hal_semaphore_take_blocking(param->param_semphr);

        if (param->state != TCPCLV3_ESTABLISHED) {
            hal_semaphore_release(param->param_semphr);
            hal_semaphore_release(tcpclv3_config->param_htab_sem);
            return (struct cla_tx_queue){ NULL, NULL };
        }

        hal_semaphore_take_blocking(param->link.base.tx_queue_sem);
        hal_semaphore_release(param->param_semphr);
        hal_semaphore_release(tcpclv3_config->param_htab_sem);

        if (!param->link.base.tx_queue_handle) {
            return (struct cla_tx_queue){ NULL, NULL };
        }

        return (struct cla_tx_queue){
                   .tx_queue_handle = param->link.base.tx_queue_handle,
                   .tx_queue_sem = param->link.base.tx_queue_sem,
        };
    }

    hal_semaphore_release(tcpclv3_config->param_htab_sem);
    return (struct cla_tx_queue){ NULL, NULL };
}

static enum ud3tn_result tcpclv3_start_scheduled_contact(
    struct cla_config *config, const char *eid, const char *cla_addr)
{
    struct tcpclv3_config *const tcpclv3_config =
        (struct tcpclv3_config *)config;

    hal_semaphore_take_blocking(tcpclv3_config->param_htab_sem);
    struct tcpclv3_contact_parameters *const param = get_contact_parameters(
        config,
        eid
        );

    if (param != NULL) {
        hal_semaphore_take_blocking(param->param_semphr);
        if (param->state != TCPCLV3_INACTIVE) {
            LOGF_INFO(
                "TCPCLv3: Associating open connection with \"%s\" to new contact",
                eid
                );
            param->opportunistic = false;
            if (param->cla_addr) {
                free(param->cla_addr);
            }
            param->cla_addr = cla_get_connect_addr(cla_addr, "tcpclv3_riot");

            if (!param->cla_addr) {
                UD3TN_LOG_WARN("TCPCLv3: Invalid address");
                hal_semaphore_release(param->param_semphr);
                hal_semaphore_release(tcpclv3_config->param_htab_sem);
                return UD3TN_FAIL;
            }

            const struct bundle_agent_interface *bai =
                config->bundle_agent_interface;
            const bool link_active = (
                param->state == TCPCLV3_ESTABLISHED
                );

            hal_semaphore_release(param->param_semphr);
            if (link_active) {
                bundle_processor_inform(
                    bai->bundle_signaling_queue,
                    (struct bundle_processor_signal) {
                    .type = BP_SIGNAL_NEW_LINK_ESTABLISHED,
                    .peer_cla_addr = strdup(
                        cla_addr
                        ),
                }
                    );
            }

            hal_semaphore_release(tcpclv3_config->param_htab_sem);
            return UD3TN_OK;
        }
        hal_semaphore_release(param->param_semphr);
        htab_remove(&tcpclv3_config->param_htab, eid);
    }

    launch_connection_management_task(tcpclv3_config, NULL, eid, cla_addr);
    hal_semaphore_release(tcpclv3_config->param_htab_sem);

    return UD3TN_OK;
}

static enum ud3tn_result tcpclv3_end_scheduled_contact(
    struct cla_config *config, const char *eid, const char *cla_addr)
{
    (void)cla_addr;
    struct tcpclv3_config *const tcpclv3_config =
        (struct tcpclv3_config *)config;

    hal_semaphore_take_blocking(tcpclv3_config->param_htab_sem);
    struct tcpclv3_contact_parameters *const param = get_contact_parameters(
        config,
        eid
        );

    if (param != NULL) {
        hal_semaphore_take_blocking(param->param_semphr);
        if (!param->opportunistic) {
            LOGF_INFO(
                "TCPCLv3: Marking active contact with \"%s\" as opportunistic",
                eid
                );
            param->opportunistic = true;
        }
        hal_semaphore_release(param->param_semphr);
    }

    hal_semaphore_release(tcpclv3_config->param_htab_sem);

    return UD3TN_OK;
}

/*
 * RX
 */

static void tcpclv3_reset_parsers(struct cla_link *link)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    tcpclv3_parser_reset(&param->tcpclv3_parser);

    if (param->state == TCPCLV3_ESTABLISHED) {
        rx_task_reset_parsers(&link->rx_task_data);
        link->rx_task_data.cur_parser = &param->tcpclv3_parser.basedata;
    }
}

static size_t tcpclv3_forward_to_specific_parser(struct cla_link *link,
                                                 const uint8_t *buffer,
                                                 size_t length)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;
    struct rx_task_data *const rx_data = &link->rx_task_data;

    ASSERT(param->state == TCPCLV3_ESTABLISHED);
    if (param->tcpclv3_parser.stage != TCPCLV3_FORWARD_BUNDLE) {
        return tcpclv3_parser_read(
            &param->tcpclv3_parser,
            buffer,
            length
            );
    }

    if (length > param->tcpclv3_parser.fragment_size) {
        length = param->tcpclv3_parser.fragment_size;
    }

    size_t result = 0;

    switch (rx_data->payload_type) {
    case PAYLOAD_UNKNOWN:
        result = select_bundle_parser_version(rx_data, buffer, length);
        if (result == 0) {
            tcpclv3_reset_parsers(link);
        }
        break;
    case PAYLOAD_BUNDLE6:
        rx_data->cur_parser = rx_data->bundle6_parser.basedata;
        result = bundle6_parser_read(
            &rx_data->bundle6_parser,
            buffer,
            length
            );
        break;
    case PAYLOAD_BUNDLE7:
        rx_data->cur_parser = rx_data->bundle7_parser.basedata;
        result = bundle7_parser_read(
            &rx_data->bundle7_parser,
            buffer,
            length
            );
        break;
    default:
        tcpclv3_reset_parsers(link);
        return 0;
    }

    ASSERT(result <= param->tcpclv3_parser.fragment_size);
    param->tcpclv3_parser.fragment_size -= result;

    if (!param->tcpclv3_parser.fragment_size) {
        tcpclv3_reset_parsers(link);
    }

    return result;
}

/*
 * GNRC TCP uses a per-connection exclusive mutex (function_lock) that is held
 * for the entire duration of both gnrc_tcp_recv AND gnrc_tcp_send. Using
 * SOCK_NO_TIMEOUT here would cause the RX task to hold this lock indefinitely,
 * blocking any concurrent sock_tcp_write in the TX task. To avoid this
 * deadlock, we use a finite timeout and retry on -ETIMEDOUT, releasing the
 * lock periodically so the TX task gets windows to write.
 * NOTE: gnrc_sock_tcp passes the value directly to gnrc_tcp_recv which treats
 * it as milliseconds.
 */


static enum ud3tn_result tcpclv3_read(struct cla_link *link,
                                      uint8_t *buffer, size_t length,
                                      size_t *bytes_read)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    ssize_t ret;

    do {
        if (!param->socket.valid) {
            link->config->vtable->cla_disconnect_handler(link);
            return UD3TN_FAIL;
        }
        ret = sock_tcp_read(param->socket.sock, buffer, length,
                            TCPCLV3_RX_TIMEOUT_MS);
    } while (ret == -ETIMEDOUT);

    if (ret < 0) {
        LOGF_ERROR("%s", "TCPCLv3: recv() failed");
        link->config->vtable->cla_disconnect_handler(link);
        return UD3TN_FAIL;
    }
    else if (ret == 0) {
        LOGF_INFO("TCPCLv3: A peer (via CLA %s) has disconnected gracefully!",
                  link->config->vtable->cla_name_get());
        link->config->vtable->cla_disconnect_handler(link);
        return UD3TN_FAIL;
    }

    if (bytes_read) {
        *bytes_read = ret;
    }
    return UD3TN_OK;
}

/*
 * TX
 */

static void tcpclv3_begin_packet(struct cla_link *link, size_t length, char *cla_addr)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    ASSERT(param->state == TCPCLV3_ESTABLISHED);
    (void)cla_addr;

    uint8_t header_buffer[1 + MAX_SDNV_SIZE];

    header_buffer[0] = (
        TCPCLV3_TYPE_DATA_SEGMENT |
        TCPCLV3_FLAG_S |
        TCPCLV3_FLAG_E
        );

    int sdnv_len = sdnv_write_u32(&header_buffer[1], length);
    // LOGF_INFO(
    //  "TCPCLv3: Sending segment header for %lu byte bundle to %s",
    //  (unsigned long)length,
    //  param->cla_addr ? param->cla_addr : "<unknown>"
    // );
    // LOGF_INFO("TCPCLv3: Segment header: type=0x%02X, length=%d", header_buffer[0], sdnv_len);
    if (tcpclv3_send_all(&param->socket, header_buffer, sdnv_len + 1) < 0) {
        LOGF_ERROR("%s", "TCPCLv3: send(segment_header) failed");
        link->config->vtable->cla_disconnect_handler(link);
    }
    else {
        // LOGF_INFO("%s", "TCPCLv3: Segment header sent successfully");
    }
}

static void tcpclv3_end_packet(struct cla_link *link)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    ASSERT(param->state == TCPCLV3_ESTABLISHED);
    (void)param;
}

static void tcpclv3_send_packet_data(
    struct cla_link *link, const void *data, const size_t length)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    ASSERT(param->state == TCPCLV3_ESTABLISHED);

    // LOGF_INFO(
    //  "TCPCLv3: Sending %lu payload bytes to %s",
    //  (unsigned long)length,
    //  param->cla_addr ? param->cla_addr : "<unknown>"
    // );
    if (tcpclv3_send_all(&param->socket, data, length) < 0) {
        LOGF_ERROR("%s", "TCPCLv3: send() failed");
        link->config->vtable->cla_disconnect_handler(link);
    }
    else {
        // LOGF_INFO("%s", "TCPCLv3: Payload sent successfully");
    }
}

static void tcpclv3_disconnect_handler(struct cla_link *link)
{
    struct tcpclv3_contact_parameters *const param =
        (struct tcpclv3_contact_parameters *)link;

    tcpclv3_socket_close(&param->socket);
    cla_generic_disconnect_handler(link);
}

/*
 * INIT
 */

const struct cla_vtable tcpclv3_riot_vtable = {
    .cla_name_get = tcpclv3_name_get,

    .cla_launch = tcpclv3_launch,
    .cla_terminate = tcpclv3_terminate,

    .cla_mbs_get = tcpclv3_mbs_get,
    .cla_get_tx_queue = tcpclv3_get_tx_queue,

    .cla_start_scheduled_contact = tcpclv3_start_scheduled_contact,
    .cla_end_scheduled_contact = tcpclv3_end_scheduled_contact,

    .cla_begin_packet = tcpclv3_begin_packet,
    .cla_end_packet = tcpclv3_end_packet,
    .cla_send_packet_data = tcpclv3_send_packet_data,

    .cla_rx_task_reset_parsers = tcpclv3_reset_parsers,
    .cla_rx_task_forward_to_specific_parser =
        &tcpclv3_forward_to_specific_parser,

    .cla_read = tcpclv3_read,

    .cla_disconnect_handler = tcpclv3_disconnect_handler,
};

static enum ud3tn_result tcpclv3_init(
    struct tcpclv3_config *config,
    const char *node, const char *service,
    const struct bundle_agent_interface *bundle_agent_interface)
{
    /* Initialize base_config */
    if (cla_config_init(&config->base, bundle_agent_interface) != UD3TN_OK) {
        return UD3TN_FAIL;
    }

    /* Set base_config vtable */
    config->base.vtable = &tcpclv3_riot_vtable;

    config->param_htab_sem = hal_semaphore_init_binary();
    if (!config->param_htab_sem) {
        return UD3TN_FAIL;
    }
    hal_semaphore_release(config->param_htab_sem);

    htab_init(&config->param_htab, 32, config->param_htab_elem);

    /* Setup listening endpoint */
    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;

    /* Parse port from service */
    uint16_t port = atoi(service);
    if (port == 0) {
        port = 4556; /* Default TCPCLv3 port */
    }
    local.port = port;

    /* Parse address from node */
    if (node && strcmp(node, "*") != 0) {
        if (ipv6_addr_from_str((ipv6_addr_t *)&local.addr, node) == NULL) {
            LOGF_ERROR("TCPCLv3: Invalid IP address \"%s\" - aborting!", node);
            hal_semaphore_take_blocking(config->param_htab_sem);
            htab_trunc(&config->param_htab);
            hal_semaphore_delete(config->param_htab_sem);
            return UD3TN_FAIL;
        }
    }

    /* Start listening */
    int listen_ret = sock_tcp_listen(&config->listen_queue, &local,
                                     config->listen_queue_array,
                                     sizeof(config->listen_queue_array) /
                                     sizeof(config->listen_queue_array[0]),
                                     0);
    if (listen_ret < 0) {
        LOGF_ERROR("TCPCLv3: sock_tcp_listen failed with code %d", listen_ret);
        hal_semaphore_take_blocking(config->param_htab_sem);
        htab_trunc(&config->param_htab);
        hal_semaphore_delete(config->param_htab_sem);
        return UD3TN_FAIL;
    }

    config->listening = true;

    return UD3TN_OK;
}

struct cla_config *tcpclv3_riot_create(
    const char *const options[], const size_t option_count,
    const struct bundle_agent_interface *bundle_agent_interface)
{
    if (option_count != 2) {
        UD3TN_LOG_ERROR("TCPCLv3: Options format has to be: <IP>,<PORT>");
        return NULL;
    }

    struct tcpclv3_config *config = malloc(sizeof(struct tcpclv3_config));

    if (!config) {
        UD3TN_LOG_ERROR("TCPCLv3: Memory allocation failed!");
        return NULL;
    }

    if (tcpclv3_init(config, options[0], options[1],
                     bundle_agent_interface) != UD3TN_OK) {
        free(config);
        UD3TN_LOG_ERROR("TCPCLv3: Initialization failed!");
        return NULL;
    }

    return &config->base;
}
