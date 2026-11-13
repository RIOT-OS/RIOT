// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0

// Next version of the Application Agent Protocol, supporting forwarding modules
// and FIB control in extension to regular bundle reception/delivery.
// NOTE: This is still experimental - use with caution!
#include "aap2/aap2_agent.h"
#include "aap2/aap2.pb.h"

#include "bundle6/create.h"

#include "bundle7/create.h"

#include "platform/hal_io.h"
#include "platform/hal_queue.h"
#include "platform/hal_semaphore.h"
#include "platform/hal_task.h"
#include "platform/hal_time.h"
#include "platform/hal_types.h"

#include "ud3tn/common.h"
#include "ud3tn/bundle.h"
#include "ud3tn/bundle_processor.h"
#include "ud3tn/eid.h"

#include <pb_decode.h>
#include <pb_encode.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// RIOT networking
#include "net/sock/tcp.h"
#include "net/ipv6/addr.h"
#include "net/af.h"

struct aap2_agent_config {
    const struct bundle_agent_interface *bundle_agent_interface;

    uint8_t bp_version;
    uint64_t lifetime_ms;

    sock_tcp_queue_t listen_queue;
    sock_tcp_t listen_queue_array[AAP2_AGENT_BACKLOG];
};

struct aap2_agent_comm_config {
    struct aap2_agent_config *parent;
    sock_tcp_t *client_sock;
    pb_istream_t pb_istream;
    // Staged byte for non-destructive peek (replaces POSIX poll/select)
    uint8_t staged_byte;
    bool has_staged_byte;
    // here use simple queue instead for communication with bundle processor
    Queue_t *bundle_queue;

    bool is_subscriber;
    char *registered_eid;
    char *secret;
    int keepalive_timeout_ms;

    // TODO: Move to common multiplexer/demux state in BP. For now, allow
    // only one sender per ID.
    uint64_t last_bundle_timestamp_ms;
    uint64_t last_bundle_sequence_number;
};

// RIOT write param (replaces POSIX tcp_write_to_socket_param)
struct aap2_write_param {
    sock_tcp_t *sock;
    int errno_;
};

// Returns: 1 = data available, 0 = timeout, -1 = error/disconnect
static int aap2_peek_with_timeout(struct aap2_agent_comm_config *config,
                                  int timeout_ms)
{
    if (config->has_staged_byte) {
        return 1;
    }
    uint32_t to = (timeout_ms < 0) ? SOCK_NO_TIMEOUT : (uint32_t)timeout_ms;
    ssize_t ret = sock_tcp_read(config->client_sock,
                                &config->staged_byte, 1, to);
    if (ret == 1) {
        config->has_staged_byte = true;
        return 1;
    }
    else if (ret == -ETIMEDOUT) {
        return 0;
    }
    UD3TN_LOG_ERROR("AAP2Agent: Connection error while waiting for data.");
    return -1;
}

// forward declaration
static void aap2_agent_comm_task(void *const param);

static bool pb_recv_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    if (count == 0) {
        return true;
    }

    struct aap2_agent_comm_config *const config = (
        (struct aap2_agent_comm_config *)stream->state
        );
    size_t idx = 0;

    // Consume staged byte first (from peek operation)
    if (config->has_staged_byte) {
        buf[0] = config->staged_byte;
        config->has_staged_byte = false;
        idx = 1;
    }

    while (idx < count) {
        ssize_t ret = sock_tcp_read(config->client_sock,
                                    buf + idx,
                                    count - idx,
                                    SOCK_NO_TIMEOUT);
        if (ret <= 0) {
            if (ret == 0) {
                stream->bytes_left = 0;
            }
            else {
                UD3TN_LOG_ERROR("AAP2Agent: recv() failed");
            }
            return false;
        }
        idx += (size_t)ret;
    }
    return true;
}

static void aap2_agent_listener_task(void *const param)
{
    struct aap2_agent_config *const config = (
        (struct aap2_agent_config *)param
        );

    for (;;) {
        sock_tcp_t *conn_sock = NULL;

        if (sock_tcp_accept(&config->listen_queue, &conn_sock,
                            SOCK_NO_TIMEOUT) < 0) {
            UD3TN_LOG_ERROR("AAP2Agent: accept() failed");
            continue;
        }

        UD3TN_LOG_INFO("AAP2Agent: Accepted new TCP connection.");

        struct aap2_agent_comm_config *child_config = malloc(
            sizeof(struct aap2_agent_comm_config)
            );
        if (!child_config) {
            UD3TN_LOG_ERROR("AAP2Agent: Error allocating memory for config!");
            sock_tcp_disconnect(conn_sock);
            continue;
        }
        child_config->parent = config;
        child_config->client_sock = conn_sock;
        child_config->has_staged_byte = false;
        child_config->last_bundle_timestamp_ms = 0;
        child_config->last_bundle_sequence_number = 0;
        child_config->is_subscriber = false;
        child_config->registered_eid = NULL;
        child_config->secret = NULL;
        child_config->keepalive_timeout_ms = -1; // infinite

        child_config->pb_istream = (pb_istream_t){
            &pb_recv_callback,
            (void *)child_config,
            SIZE_MAX,
            NULL,
        };
        // here create the queue
        child_config->bundle_queue = queueCreate(10, sizeof(struct bundle_adu));

        const enum ud3tn_result task_creation_result = hal_task_create(
            aap2_agent_comm_task,
            child_config,
            true,
            NULL
            );

        if (task_creation_result != UD3TN_OK) {
            UD3TN_LOG_ERROR("AAP2Agent: Error starting comm. task!");
            sock_tcp_disconnect(conn_sock);
            free(child_config);
        }
    }
}

static bool write_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count)
{
    struct aap2_write_param *const wsp = stream->state;

    ssize_t ret = sock_tcp_write(wsp->sock, buf, count);

    if (ret < 0) {
        wsp->errno_ = (int)(-ret);
        return false;
    }
    return true;
}

static int send_message(sock_tcp_t *const sock,
                        const pb_msgdesc_t *const fields,
                        const void *const src_struct)
{
    struct aap2_write_param wsp = {
        .sock = sock,
        .errno_ = 0,
    };

    pb_ostream_t stream = {
        &write_callback,
        &wsp,
        SIZE_MAX,
        0,
        NULL,
    };

    const bool ret = pb_encode_ex(
        &stream,
        fields,
        src_struct,
        PB_ENCODE_DELIMITED
        );

    if (wsp.errno_) {
        LOG_ERRNO("AAP2Agent", "send()", wsp.errno_);
    }
    else if (!ret) {
        LOGF_ERROR(
            "AAP2Agent: Protobuf encode error: %s",
            PB_GET_ERROR(&stream)
            );
        wsp.errno_ = EIO;
    }

    return -wsp.errno_;
}

static void agent_msg_recv(struct bundle_adu data, void *param,
                           const void *bp_context)
{
    (void)bp_context;

    struct aap2_agent_comm_config *const config = (
        (struct aap2_agent_comm_config *)param
        );

    LOGF_DEBUG(
        "AAP2Agent: Got Bundle for EID \"%s\" from \"%s\", forwarding.",
        config->registered_eid,
        data.source
        );
    // here add to queue
    if (queuePush(config->bundle_queue, &data, -1, 0) != EXIT_SUCCESS) {
        LOG_ERRNO("AAP2Agent", "write()", errno);
        bundle_adu_free_members(data);
    }
    /*if (pipeq_write_all(config->bundle_pipe_fd[1],
                &data, sizeof(struct bundle_adu)) <= 0) {
        LOG_ERRNO("AAP2Agent", "write()", errno);
        bundle_adu_free_members(data);
       }*/
}

static int register_sink(const char *sink_identifier, bool is_subscriber,
                         const char *secret,
                         struct aap2_agent_comm_config *config)
{
    return bundle_processor_perform_agent_action(
        config->parent->bundle_agent_interface->bundle_signaling_queue,
        (
            is_subscriber
            ? BP_SIGNAL_AGENT_REGISTER
            : BP_SIGNAL_AGENT_REGISTER_RPC
        ),
        (struct agent) {
        // NOTE that both sink_identifier and secret have to be
        // valid for the full duration of the registration!
        .sink_identifier = sink_identifier,
        .secret = secret,
        .callback = is_subscriber ? agent_msg_recv : NULL,
        .param = is_subscriber ? config : NULL,
    },
        true
        );
}

static void deregister_sink(struct aap2_agent_comm_config *config)
{
    if (config->registered_eid == NULL) {
        return;
    }

    const char *agent_id = get_agent_id_ptr(
        config->registered_eid
        );

    LOGF_INFO(
        "AAP2Agent: De-registering agent ID \"%s\".",
        agent_id
        );

    ASSERT(bundle_processor_perform_agent_action(
               config->parent->bundle_agent_interface->bundle_signaling_queue,
               (
                   config->is_subscriber
            ? BP_SIGNAL_AGENT_DEREGISTER
            : BP_SIGNAL_AGENT_DEREGISTER_RPC
               ),
               (struct agent){ .sink_identifier = agent_id },
               true
               ) == 0);

    free(config->registered_eid);
    config->registered_eid = NULL;
    free(config->secret);
    config->secret = NULL;
}

static uint64_t allocate_sequence_number(
    struct aap2_agent_comm_config *const config,
    const uint64_t time_ms)
{
    // If a previous bundle was sent less than one millisecond ago, we need
    // to increment the sequence number.
    if (config->last_bundle_timestamp_ms == time_ms) {
        return ++config->last_bundle_sequence_number;
    }

    config->last_bundle_timestamp_ms = time_ms;
    config->last_bundle_sequence_number = 1;

    return 1;
}

static aap2_ResponseStatus process_configure_msg(
    struct aap2_agent_comm_config *const config,
    aap2_ConnectionConfig *const msg)
{
    LOGF_INFO(
        "AAP2Agent: Received request to %s for EID \"%s\".",
        msg->is_subscriber ? "subscribe" : "register",
        msg->endpoint_id
        );

    // Clean up a previous registration in case there is one.
    deregister_sink(config);

    if (validate_eid(msg->endpoint_id) != UD3TN_OK) {
        LOGF_INFO(
            "AAP2Agent: Invalid EID provided: \"%s\"",
            msg->endpoint_id
            );
        return aap2_ResponseStatus_RESPONSE_STATUS_INVALID_REQUEST;
    }

    const char *sink_id = get_agent_id_ptr(
        msg->endpoint_id
        );

    if (!sink_id) {
        LOGF_WARN(
            "AAP2Agent: Cannot obtain sink for EID: \"%s\"",
            msg->endpoint_id
            );
        return aap2_ResponseStatus_RESPONSE_STATUS_INVALID_REQUEST;
    }

    if (register_sink(sink_id, msg->is_subscriber, msg->secret, config)) {
        UD3TN_LOG_INFO("AAP2Agent: Registration request declined.");
        return aap2_ResponseStatus_RESPONSE_STATUS_UNAUTHORIZED;
    }

    config->registered_eid = msg->endpoint_id;
    msg->endpoint_id = NULL;            // take over freeing
    config->secret = msg->secret;
    msg->secret = NULL;                 // take over freeing

    config->keepalive_timeout_ms = -1;  // infinite by default
    if (msg->keepalive_seconds != 0) {
        if (msg->keepalive_seconds >= (INT32_MAX / 1000 / 2)) {
            LOGF_WARN(
                "AAP2Agent: Keepalive timeout of %d sec is too large, ignoring.",
                msg->keepalive_seconds
                );
        }
        else {
            config->keepalive_timeout_ms = (
                msg->keepalive_seconds * 1000
                );
            // For active clients we wait for twice the specified
            // timeout until we terminate the connection.
            if (!msg->is_subscriber) {
                config->keepalive_timeout_ms *= 2;
            }
        }
    }

    config->is_subscriber = msg->is_subscriber;
    if (config->is_subscriber) {
        UD3TN_LOG_INFO("AAP2Agent: Switching control flow!");
    }

    return aap2_ResponseStatus_RESPONSE_STATUS_SUCCESS;
}

static aap2_ResponseStatus process_adu_msg(
    struct aap2_agent_comm_config *const config,
    aap2_BundleADU *const msg,
    uint8_t *payload_data,
    aap2_AAPResponse *response)
{
    LOGF_DEBUG(
        "AAP2Agent: Received %s (l = %zu) for %s via AAP.",
        (
            (msg->adu_flags & aap2_BundleADUFlags_BUNDLE_ADU_BPDU)
            ? "BIBE BPDU"
            : "bundle"
        ),
        msg->payload_length,
        msg->dst_eid
        );

    if (msg->creation_timestamp_ms != 0 || msg->sequence_number != 0) {
        UD3TN_LOG_WARN("AAP2Agent: User-defined creation timestamps are unsupported!");
        free(payload_data);
        return aap2_ResponseStatus_RESPONSE_STATUS_INVALID_REQUEST;
    }

    if (!config->registered_eid) {
        UD3TN_LOG_WARN("AAP2Agent: No agent ID registered, dropping!");
        free(payload_data);
        return aap2_ResponseStatus_RESPONSE_STATUS_NOT_FOUND;
    }

    if (!payload_data) {
        UD3TN_LOG_WARN("AAP2Agent: Cannot handle ADU without payload data!");
        return aap2_ResponseStatus_RESPONSE_STATUS_ERROR;
    }

    enum bundle_proc_flags flags = BUNDLE_FLAG_NONE;
    size_t payload_length = msg->payload_length;

    if (msg->adu_flags & aap2_BundleADUFlags_BUNDLE_ADU_BPDU) {
        UD3TN_LOG_DEBUG("AAP2Agent: ADU is a BPDU, prepending AR header!");

        #ifdef BIBE_CL_DRAFT_1_COMPATIBILITY
        const uint8_t typecode = 7;
        #else
        const uint8_t typecode = 3;
        #endif

        const size_t ar_size = payload_length + 2;
        uint8_t *const ar_bytes = malloc(ar_size);

        memcpy(
            ar_bytes + 2,
            payload_data,
            payload_length
            );
        ar_bytes[0] = 0x82;     // CBOR array of length 2
        ar_bytes[1] = typecode; // Integer (record type)

        free(payload_data);
        payload_data = ar_bytes;
        payload_length = ar_size;
        flags |= BUNDLE_FLAG_ADMINISTRATIVE_RECORD;
    }

    const uint64_t time_ms = hal_time_get_timestamp_ms();
    const uint64_t seqnum = allocate_sequence_number(
        config,
        time_ms
        );

    struct bundle *bundle;

    if (config->parent->bp_version == 6) {
        bundle = bundle6_create_local(
            payload_data,
            payload_length,
            config->registered_eid,
            msg->dst_eid,
            time_ms, seqnum,
            config->parent->lifetime_ms,
            flags
            );
    }
    else {
        bundle = bundle7_create_local(
            payload_data,
            payload_length,
            config->registered_eid,
            msg->dst_eid,
            time_ms,
            seqnum,
            config->parent->lifetime_ms,
            flags
            );
    }

    if (!bundle) {
        UD3TN_LOG_WARN("AAP2Agent: Bundle creation failed!");
        return aap2_ResponseStatus_RESPONSE_STATUS_ERROR;
    }

    bundle_processor_inform(
        config->parent->bundle_agent_interface->bundle_signaling_queue,
        (struct bundle_processor_signal){
        .type = BP_SIGNAL_BUNDLE_LOCAL_DISPATCH,
        .bundle = bundle,
    }
        );

    LOGF_DEBUG("AAP2Agent: Injected new bundle %p.", bundle);

    response->bundle_headers.src_eid = strdup(config->registered_eid);
    response->bundle_headers.dst_eid = msg->dst_eid;
    msg->dst_eid = NULL;
    response->bundle_headers.payload_length = payload_length;
    response->bundle_headers.creation_timestamp_ms = time_ms;
    response->bundle_headers.sequence_number = seqnum;
    response->bundle_headers.lifetime_ms = config->parent->lifetime_ms;
    response->has_bundle_headers = true;

    return aap2_ResponseStatus_RESPONSE_STATUS_SUCCESS;
}

static int process_aap_message(
    struct aap2_agent_comm_config *const config,
    aap2_AAPMessage *const msg,
    uint8_t *const payload_data)
{
    aap2_AAPResponse response = aap2_AAPResponse_init_default;
    int result = -1;

    // Default response
    response.response_status = (
        aap2_ResponseStatus_RESPONSE_STATUS_INVALID_REQUEST
        );

    switch (msg->which_msg) {
    case aap2_AAPMessage_config_tag:
        response.response_status = process_configure_msg(
            config,
            &msg->msg.config
            );
        break;
    case aap2_AAPMessage_adu_tag:
        response.response_status = process_adu_msg(
            config,
            &msg->msg.adu,
            payload_data,
            &response
            );
        break;
    case aap2_AAPMessage_keepalive_tag:
        LOGF_DEBUG(
            "AAP2Agent: Received KEEPALIVE from \"%s\"",
            config->registered_eid
            ? config->registered_eid
            : "<not registered>"
            );
        response.response_status = (
            aap2_ResponseStatus_RESPONSE_STATUS_ACK
            );
        break;
    default:
        LOGF_WARN(
            "AAP2Agent: Cannot handle AAP messages of tag type %d!",
            msg->which_msg
            );
        break;
    }

    result = send_message(
        config->client_sock,
        aap2_AAPResponse_fields,
        &response
        );

    // Free message contents
    pb_release(aap2_AAPResponse_fields, &response);
    return result;
}

static uint8_t *receive_payload(pb_istream_t *istream, size_t payload_length)
{
    if (payload_length > BUNDLE_MAX_SIZE) {
        UD3TN_LOG_WARN("AAP2Agent: Payload too large!");
        return NULL;
    }

    uint8_t *payload = malloc(payload_length);

    if (!payload) {
        UD3TN_LOG_ERROR("AAP2Agent: Payload alloc error!");
        return NULL;
    }


    const bool success = pb_read(istream, payload, payload_length);

    if (!success) {
        free(payload);
        payload = NULL;
        UD3TN_LOG_ERROR("AAP2Agent: Payload read error!");
    }

    return payload;
}

static int send_bundle_from_pipe(struct aap2_agent_comm_config *const config)
{
    struct bundle_adu data;

    // here take from queue
    if (queuePop(config->bundle_queue, &data, -1) != EXIT_SUCCESS) {
        LOG_ERRNO("AAP2Agent", "read()", errno);
        return -1;
    }
    /*if (pipeq_read_all(config->bundle_pipe_fd[0],
               &data, sizeof(struct bundle_adu)) <= 0) {
        LOG_ERRNO("AAP2Agent", "read()", errno);
        return -1;
       }*/

    aap2_AAPMessage msg = aap2_AAPMessage_init_default;

    msg.which_msg = aap2_AAPMessage_adu_tag;
    msg.msg.adu.dst_eid = data.destination;
    msg.msg.adu.src_eid = data.source;
    msg.msg.adu.payload_length = data.length;
    msg.msg.adu.creation_timestamp_ms = data.bundle_creation_timestamp_ms;
    msg.msg.adu.sequence_number = data.bundle_sequence_number;
    // BIBE
    if (data.proc_flags == BUNDLE_FLAG_ADMINISTRATIVE_RECORD) {
        msg.msg.adu.adu_flags = aap2_BundleADUFlags_BUNDLE_ADU_BPDU;
    }

    sock_tcp_t *const client_sock = config->client_sock;
    int send_result = send_message(
        client_sock,
        aap2_AAPMessage_fields,
        &msg
        );

    // NOTE: We do not "release" msg as there is nothing to deallocate here.

    if (send_result < 0) {
        bundle_adu_free_members(data);
        return send_result;
    }

    struct aap2_write_param wsp = {
        .sock = client_sock,
        .errno_ = 0,
    };
    pb_ostream_t stream = {
        &write_callback,
        &wsp,
        SIZE_MAX,
        0,
        NULL,
    };

    const bool ret = pb_write(&stream, data.payload, data.length);

    if (wsp.errno_) {
        LOG_ERRNO("AAP2Agent", "send()", wsp.errno_);
        send_result = -1;
    }
    else if (!ret) {
        LOGF_WARN(
            "AAP2Agent: pb_write() error: %s",
            PB_GET_ERROR(&stream)
            );
        send_result = -1;
    }

    if (send_result < 0) {
        bundle_adu_free_members(data);
        return send_result;
    }

    // Receive status from client.
    aap2_AAPResponse response = aap2_AAPResponse_init_default;

    if (aap2_peek_with_timeout(config, AAP2_AGENT_TIMEOUT_MS) <= 0) {
        UD3TN_LOG_WARN("AAP2Agent: No response received, closing connection.");
        send_result = -1;
        goto done;
    }

    const bool success = pb_decode_ex(
        &config->pb_istream,
        aap2_AAPResponse_fields,
        &response,
        PB_DECODE_DELIMITED
        );

    if (!success) {
        LOGF_WARN(
            "AAP2Agent: Protobuf decode error: %s",
            PB_GET_ERROR(&config->pb_istream)
            );
        send_result = -1;
        goto done;
    }

    if (response.response_status !=
        aap2_ResponseStatus_RESPONSE_STATUS_SUCCESS) {
        // TODO: Implement configurable policy to re-route/keep somehow.
        UD3TN_LOG_WARN("AAP2Agent: Client reported error for bundle, dropping.");
        // NOTE: Do not return -1 here as this would close the socket.
    }

done:
    pb_release(aap2_AAPResponse_fields, &response);
    bundle_adu_free_members(data);
    return send_result;
}
/*
   static void shutdown_bundle_pipe(int bundle_pipe_fd[2])
   {
    struct bundle_adu data;

    while (poll_recv_timeout(bundle_pipe_fd[0], 0) > 0) {
        if (pipeq_read_all(bundle_pipe_fd[0],
                   &data, sizeof(struct bundle_adu)) <= 0) {
            LOG_ERRNO("AAP2Agent", "read()", errno);
            break;
        }

        LOGF_WARN(
            "AAP2Agent: Dropping unsent bundle from '%s'.",
            data.source
        );
        bundle_adu_free_members(data);
    }

    close(bundle_pipe_fd[0]);
    close(bundle_pipe_fd[1]);
   }
 */
static int send_keepalive(struct aap2_agent_comm_config *const config)
{
    aap2_AAPMessage msg = aap2_AAPMessage_init_default;

    UD3TN_LOG_DEBUG("AAP2Agent: Sending Keepalive message to Client.");
    msg.which_msg = aap2_AAPMessage_keepalive_tag;

    sock_tcp_t *const client_sock = config->client_sock;
    int send_result = send_message(
        client_sock,
        aap2_AAPMessage_fields,
        &msg
        );

    // NOTE: We do not "release" msg as there is nothing to deallocate here.

    if (send_result < 0) {
        return send_result;
    }

    // Receive status from client.
    aap2_AAPResponse response = aap2_AAPResponse_init_default;

    if (aap2_peek_with_timeout(config, AAP2_AGENT_TIMEOUT_MS) <= 0) {
        UD3TN_LOG_WARN("AAP2Agent: No response received, closing connection.");
        send_result = -1;
        goto done;
    }

    const bool success = pb_decode_ex(
        &config->pb_istream,
        aap2_AAPResponse_fields,
        &response,
        PB_DECODE_DELIMITED
        );

    if (!success) {
        LOGF_WARN(
            "AAP2Agent: Protobuf decode error: %s",
            PB_GET_ERROR(&config->pb_istream)
            );
        send_result = -1;
        goto done;
    }

    if (response.response_status !=
        aap2_ResponseStatus_RESPONSE_STATUS_ACK) {
        UD3TN_LOG_WARN("AAP2Agent: Keepalive not acknowledged, closing connection.");
        send_result = -1;
        goto done;
    }

done:
    pb_release(aap2_AAPResponse_fields, &response);
    return send_result;
}

static void aap2_agent_comm_task(void *const param)
{
    struct aap2_agent_comm_config *const config = (
        (struct aap2_agent_comm_config *)param
        );

    // Send indicator for invalid version.
    const uint8_t aap2_version_indicator = 0x2F;

    if (sock_tcp_write(config->client_sock, &aap2_version_indicator, 1) != 1) {
        UD3TN_LOG_ERROR("AAP2Agent: send() failed");
        goto done;
    }

    char *local_eid = config->parent->bundle_agent_interface->local_eid;
    aap2_AAPMessage msg = aap2_AAPMessage_init_default;

    msg.which_msg = aap2_AAPMessage_welcome_tag;
    msg.msg.welcome.node_id = local_eid;

    if (send_message(config->client_sock, aap2_AAPMessage_fields, &msg)) {
        goto done;
    }
    // NOTE: We do not "release" msg as there is nothing to deallocate here.

    aap2_AAPMessage request;
    // Here use select instead
    for (;;) {
        if (config->is_subscriber) {
            if (send_bundle_from_pipe(config) < 0) {
                break;
            }
        }
        else {
            const int select_result = aap2_peek_with_timeout(
                config,
                config->keepalive_timeout_ms
                );

            if (select_result == 0) {
                UD3TN_LOG_WARN("AAP2Agent: Client exceeded keepalive timeout, terminating.");
                break;
            }
            else if (select_result < 0) {
                // An error message was already logged here.
                break;
            }

            bool success = pb_decode_ex(
                &config->pb_istream,
                aap2_AAPMessage_fields,
                &request,
                PB_DECODE_DELIMITED
                );

            if (!success) {
                LOGF_WARN(
                    "AAP2Agent: Protobuf decode error: %s",
                    PB_GET_ERROR(&config->pb_istream)
                    );
                break;
            }

            uint8_t *payload = NULL;

            // Read payload for ADU messages. Note that even if this
            // fails we process the message with payload == NULL, to
            // send a proper response.
            if (request.which_msg == aap2_AAPMessage_adu_tag) {
                payload = receive_payload(
                    &config->pb_istream,
                    request.msg.adu.payload_length
                    );
            }

            success = !process_aap_message(
                config,
                &request,
                payload
                );

            pb_release(aap2_AAPMessage_fields, &request);
            if (!success) {
                break;
            }
        }
    }

done:
    deregister_sink(config);
    //shutdown_bundle_pipe(config->bundle_pipe_fd);
    queueDelete(config->bundle_queue);
    sock_tcp_disconnect(config->client_sock);
    free(config);
    UD3TN_LOG_INFO("AAP2Agent: Closed connection.");
}

struct aap2_agent_config *aap2_agent_setup(
    const struct bundle_agent_interface *bundle_agent_interface,
    const char *socket_path,
    const char *node, const char *service,
    const uint8_t bp_version, uint64_t lifetime_ms)
{
    struct aap2_agent_config *const config = malloc(
        sizeof(struct aap2_agent_config)
        );

    if (!config) {
        UD3TN_LOG_ERROR("AAP2Agent: Error allocating memory for task config!");
        return NULL;
    }

    if (!node || !service) {
        UD3TN_LOG_ERROR("AAP2Agent: node and service must be provided!");
        free(config);
        return NULL;
    }

    sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = (uint16_t)atoi(service);
    if (local.port == 0) {
        UD3TN_LOG_ERROR("AAP2Agent: Invalid port in service string!");
        free(config);
        return NULL;
    }

    // Bind to specific IPv6 address if provided; "::" or "*" = all interfaces
    if (node && strcmp(node, "::") != 0 && strcmp(node, "*") != 0) {
        if (ipv6_addr_from_str((ipv6_addr_t *)&local.addr.ipv6, node) == NULL) {
            LOGF_ERROR("AAP2Agent: Invalid IPv6 address: \"%s\"", node);
            free(config);
            return NULL;
        }
    }

    int listen_ret = sock_tcp_listen(&config->listen_queue, &local,
                                     config->listen_queue_array,
                                     sizeof(config->listen_queue_array) /
                                     sizeof(config->listen_queue_array[0]),
                                     0);
    if (listen_ret < 0) {
        LOGF_INFO("AAP2Agent IP_Adress: %s", node);
        LOGF_INFO("AAP2Agent Port: %s", service);
        LOGF_INFO("AAP2Agent Queue size: %zu",
                  sizeof(config->listen_queue_array) / sizeof(config->listen_queue_array[0]));
        LOGF_ERROR("AAP2Agent: sock_tcp_listen failed with code %d", listen_ret);
        UD3TN_LOG_ERROR("AAP2Agent: Error binding to provided address!");
        free(config);
        return NULL;
    }

    LOGF_INFO("AAP2Agent: Listening on [%s]:%s", node, service);

    config->bundle_agent_interface = bundle_agent_interface;
    config->bp_version = bp_version;
    config->lifetime_ms = lifetime_ms;

    const enum ud3tn_result task_creation_result = hal_task_create(
        aap2_agent_listener_task,
        config,
        true,
        NULL
        );

    if (task_creation_result != UD3TN_OK) {
        UD3TN_LOG_ERROR("AAP2Agent: Error creating listener task!");
        free(config);
        return NULL;
    }

    return config;
}
