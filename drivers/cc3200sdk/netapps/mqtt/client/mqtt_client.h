
/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

/*
  mqtt_client.h
  
  This module enumerates the public interfaces / API of the MQTT Client
  Library.
*/


#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

/**@file mqtt_client.h
   This C library provisions the interface / API(s) for the MQTT Client.

   This is a light-weight library to enable the services of the MQTT protocol
   for one or more client applications (that would typically run on a resource
   constrained system). The key consideration in the design of small footprint
   library has been the abstraction of the low level details of the message
   transactions with the server and yet, provide rudimentary API(s), such that,
   the capabilities and features of the protocol are availalbe to be utilized
   by existing and new applications in an un-restrictive manner.

   The library is targeted to conform to MQTT 3.1.1 specification.

   The MQTT Client library is a highly portable software and implies a very
   limited set of dependencies on a platform. Importantly, these limited
   dependencies are the commonly used features used in the embedded and the
   networking world, and can be easily adapted to the target platforms.

   The services of the library are multi-task safe. Platform specific atomicity
   constructs are used, through abstractions, by the library to maintain data 
   coherency and synchronization. In addition, the library can be configured to
   support several in-flight messages.

   The client library supports multiple and simultaneous MQTT connections to one
   or more servers. In this client LIB, the reference to an individual connection
   in conjunction with the associated configuration parameters has been termed as
   a 'context'. Therefore, the client library supports multiple 'contexts'. An
   application that intends to make use of the client library must set-up or
   create a 'context' prior to establishing a connection with the server. The
   application can choose to destroy the 'context' after the connection with the
   server has been terminated. The client LIB can only support a finite set of
   'contexts' and the number can be configured by using a compiler line option /
   flag <b> -DCFG_CL_MQTT_CTXS </b>.

   Once, the 'context' is set-up, the application can send and receive the MQTT
   packets to / from the server. Among several features supported by the client
   LIB, the 'context' manages the keep-alive mechanism by automatically sending
   PING request to the server, if there has been no other packet send to the
   server with the keep-alive duration. 
   
   @note Any future extensions & development must follow the following guidelines.
   A new API or an extension to the existing API
   a) must be rudimentary
   b) must not imply a rule or policy (including a state machine)
   b) must ensure simple design and implementation
   
*/

#include "mqtt_common.h"

/*------------------------------------------------------------------------------
 *  MQTT Client Messaging Routines / Services
 *------------------------------------------------------------------------------
 */

/** @defgroup client_api The Client Library API(s)
    @{
*/

#define MQTT_CLIENT_VERSTR "1.0.3" /**< Version of Client LIB */

/** Provides a new MSG Identifier for a packet dispatch to server

    @return MSG / PKT Transaction identifier
*/
u16 mqtt_client_new_msg_id(void);

/** Ascertain whether connection / session with the server is active or not.
    Prior to sending out any information any message to server, the application
    can use this routine to check the status of the connection. If connection
    does not exist, then client should first CONNECT to the broker. 

    A connection to server could have been closed unsolicitedly either due to
    keep alive time-out or due to error in RX / TX transactions.
    
    @note this API does not refer to network layer connection

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create

    @return true if connection is active otherwise false.
*/
bool mqtt_client_is_connected(void *ctx);

/** Send the CONNECT message to the server (and don't wait for CONNACK).
    This routine accomplishes multiple sequences. As a first step, it tries
    to establish a network connection with the server. Then, it populates
    an internaly allocated packet buffer with all the previously provided
    payload data information, prepares the requisite headers and finally,
    dispatches the constructed message to the server.

    Prior to invoking this service, the client application should provision the
    intended payload contents of the CONNECT message by using the API(s) @ref
    mqtt_client_ctx_info_register and @ref mqtt_client_ctx_will_register. And
    information about the server of interest must be provided in the client LIB
    'context' creation (@ref mqtt_client_ctx_create).

    The client application must invoke an appropriate receive routine to know
    about the corresponding response CONNACK from the server. The client LIB will
    close the network connection to the server, if the server happens to refuse
    the CONNECT request.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] clean_session asserted to delete references to previous session
    at both server and client
    @param[in] ka_secs Keep Alive Time
    @return number of bytes sent or LIB defined errors (@ref lib_err_group)
*/
i32 mqtt_connect_msg_send(void *ctx, bool clean_session, u16 ka_secs);

/** Send a PUBLISH message to the server (don't wait for PUBACK / PUBREC).
    This routine creates a PUBLISH message in an internally allocated packet
    buffer by embedding the 'topic' and 'data' contents, then prepares the
    packet header and finally, dispatches the message to the server.

    After the packet has been sent to the server, if the associated QoS of the
    dispatched packet is ether level 1 or 2, the client LIB 'context' will then
    store the packet until the time, a corresponding PUB-ACK (for QoS1) or
    PUB-REC (QoS2) message is received from the server.

    If the client LIB 'context' has been configured to assert 'clean session',
    then the references to all the stored and unacknowledged PUBLISH messages
    are dropped at the time of MQTT disconnection (or network disconnection).
    Otherwise, these unacknowledged packets continue to be availalbe for the
    next iteration of the MQTT connection. However, if the client application
    asserts the 'clean session' parameter in the next iteration of the CONNECT
    operation, then references to all the stored PUBLISH messages, if any, are
    dropped.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] topic UTF8 based Topic Name for which data is being published.
    @param[in] data_buf The binary data that is being published for the topic.
    @param[in] data_len The length of the binary data.
    @param[in] qos quality of service of the message
    @param[in] retain should the server retain the message.
    @return on success, a transaction message id otherwise, LIB defined errors
    (@ref lib_err_group)
*/
i32 mqtt_client_pub_msg_send(void *ctx,
                             const  struct utf8_string *topic,
                             const u8 *data_buf, u32 data_len,
                             enum mqtt_qos qos,  bool retain);

/** Dispatch application constructed PUBLISH message to the server.
    Prior to sending the message to the server, this routine will prepare a fixed
    header to account for the size of the contents and the flags that have been
    indicated by the caller. 

    After the packet has been sent to the server, if the associated QoS of the
    dispatched packet is ether level 1 or 2, the client LIB 'context' will then
    store the packet until the time, a corresponding PUB-ACK (for QoS1) or
    PUB-REC (QoS2) message is received from the server.

    If the client LIB 'context' has been configured to assert 'clean session',
    then the references to all the stored and unacknowledged PUBLISH messages
    are dropped at the time of MQTT disconnection (or network disconnection).
    Otherwise, these unacknowledged packets continue to be availalbe for the
    next iteration of the MQTT connection. However, if the client application
    asserts the 'clean session' parameter in the next iteration of the CONNECT
    operation, then references to all the stored PUBLISH messages, if any, are
    dropped.

    The caller must populate the payload information with topic and data before
    invoking this service. 

    This service facilitates direct writing of topic and (real-time) payload data
    into the buffer, thereby, avoiding power consuming and wasteful intermediate
    data copies.

    In case, the routine returns an error, the caller is responsbile for freeing
    up or re-using the packet buffer. For all other cases, the client library
    will manage the return of the packet buffer to the pool.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] mqp app created PUBLISH message without the fixed header
    @param[in] qos QoS with which the message needs to send to server
    @param[in] retain Asserted if the message is to be retained by server.
    @return on success, the transaction Message ID, otherwise LIB defined errors
    (@ref lib_err_group)
*/
i32 mqtt_client_pub_dispatch(void *ctx, struct mqtt_packet *mqp,
                             enum mqtt_qos qos, bool retain);

/** Send a SUBSCRIBE message to the server (and don't wait for SUBACK).
    This routine creates a SUBSCRIBE message in an internally allocated packet
    buffer by embedding the 'qos_topics', then prepares the message header and
    finally, dispatches the packet to the server. 

    After the packet has been dispatched to the server, the library will store
    the packet until the time, a corresponding SUB-ACK has been received from
    the server. This mechanism enables the client LIB 'context' to trace the
    sequence of the message-ID and / or resend the SUB packets to the server.

    The client LIB 'context', if configured to operate in the MQTT 3.1.1 mode
    will drop or remove the un-acknowledged SUB messages at the time of the
    termination of the network connection.

    In the MQTT 3.1 mode, the client LIB 'context' will remove the unacknowledged
    SUB messages at the time of the termination of the network connection, if the
    'clean session' has been asserted. In case, the 'clean session' has not been
    asserted, the stored SUB messages will continue to be available for the next
    iteration of the MQTT connection. However, if the client application asserts
    the 'clean session' parameter in the next iteration of the CONNECT operation,
    then references to all the stored SUBSCRIBE messages, if any, are dropped.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] qos_topics an array of topic along-with its qos
    @param[in] count the number of elements in the array
    @return on success, the transaction Message ID, otherwise Lib defined errors
    (@ref lib_err_group)
*/
i32 mqtt_sub_msg_send(void *ctx, const struct utf8_strqos *qos_topics, u32 count);

/** Dispatch application constructed SUSBSCRIBE message to the server.
    Prior to sending the message to the server, this routine will prepare a fixed
    header to account for the size of the size of the contents. 

    After the packet has been dispatched to the server, the library will store
    the packet until the time, a corresponding SUB-ACK has been received from
    the server. This mechanism enables the client LIB 'context' to trace the
    sequence of the message-ID and / or resend the SUB packets to the server.

    The client LIB 'context', if configured to operate in the MQTT 3.1.1 mode
    will drop or remove the un-acknowledged SUB messages at the time of the
    termination of the network connection.

    In the MQTT 3.1 mode, the client LIB 'context' will remove the unacknowledged
    SUB messages at the time of the termination of the network connection, if the
    'clean session' has been asserted. In case, the 'clean session' has not been
    asserted, the stored SUB messages will continue to be available for the next
    iteration of the MQTT connection. However, if the client application asserts
    the 'clean session' parameter in the next iteration of the CONNECT operation,
    then references to all the stored SUBSCRIBE messages, if any, are dropped.

    The caller must populate the payload information of topic along with qos
    before invoking this service. 

    This service facilitates direct writing of topic and (real-time) payload data
    into the buffer, thereby, avoiding power consuming and wasteful intermediate
    data copies.

    In case, the routine returns an error, the caller is responsbile for freeing
    up or re-using the packet buffer. For all other cases, the client library
    will manage the return of the packet buffer to the pool.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] mqp app created SUBSCRIBE message without the fixed header.
    @return on success, the transaction Message ID, otherwise Lib defined errors
    (@ref lib_err_group)
*/
i32 mqtt_sub_dispatch(void *ctx, struct mqtt_packet *mqp);

/** Send an UNSUBSCRIBE message to the server (and don't wait for UNSUBACK).
    This routine creates an UNSUBSCRIBE message in an internally allocated packet
    buffer by embedding the 'topics', then prepares the message header and 
    finally, dispatches the packet to the server.

    After the packet has been dispatched to the server, the library will store
    the packet until the time, a corresponding UNSUB-ACK has been received from
    the server. This mechanism enables the client LIB 'context' to trace the
    sequence of the message-ID and / or resend the UNSUB packets to the server.

    The client LIB 'context', if configured to operate in the MQTT 3.1.1 mode
    will drop or remove the un-acknowledged SUB messages at the time of the
    termination of the network connection.

    In the MQTT 3.1 mode, the client LIB 'context' will remove the unacknowledged
    UNSUB messages at the time of the termination of the network connection, if
    the 'clean session' has been asserted. In case, the 'clean session' has not
    been asserted, the stored UNSUB messages will continue to be available for
    the next iteration of the MQTT connection. However, if the client application
    asserts the 'clean session' parameter in the next iteration of the CONNECT
    operation, then references to all the stored UNSUBSCRIBE messages, if any,
    are dropped.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] topics an array of topic to unsubscribe
    @param[in] count the number of elements in the array
    @return on success, the transaction Message ID, otherwise Lib defined errors
    (@ref lib_err_group)
*/
i32 mqtt_unsub_msg_send(void *ctx, const struct utf8_string *topics, u32 count);

/** Dispatch application constructed UNSUSBSCRIBE message to the server. 
    Prior to sending the message to the server, this routine will prepare a fixed
    header to account for the size of the size of the contents. 

    After the packet has been dispatched to the server, the library will store
    the packet until the time, a corresponding UNSUB-ACK has been received from
    the server. This mechanism enables the client LIB 'context' to trace the
    sequence of the message-ID and / or resend the UNSUB packets to the server.

    The client LIB 'context', if configured to operate in the MQTT 3.1.1 mode
    will drop or remove the un-acknowledged SUB messages at the time of the
    termination of the network connection.

    In the MQTT 3.1 mode, the client LIB 'context' will remove the unacknowledged
    UNSUB messages at the time of the termination of the network connection, if
    the 'clean session' has been asserted. In case, the 'clean session' has not
    been asserted, the stored UNSUB messages will continue to be available for
    the next iteration of the MQTT connection. However, if the client application
    asserts the 'clean session' parameter in the next iteration of the CONNECT
    operation, then references to all the stored UNSUBSCRIBE messages, if any,
    are dropped.

    The caller must populate the payload information of topics before invoking
    this service. 

    This service facilitates direct writing of topic and (real-time) payload data
    into the buffer, thereby, avoiding power consuming and wasteful intermediate
    data copies.

    In case, the routine returns an error, the caller is responsbile for freeing
    up or re-using the packet buffer. For all other cases, the client library
    will manage the return of the packet buffer to the pool.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] Packet Buffer that holds UNSUBSCRIBE message without a fixed header
    @return on success, the transaction Message ID, otherwise LIB defined errors
    (@ref lib_err_group)
*/
i32 mqtt_unsub_dispatch(void *ctx, struct mqtt_packet *mqp);

/** Send a PINGREQ message to the server (and don't wait for PINGRSP).

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @return number of bytes sent or Lib define errors (@ref lib_err_group)
*/

i32 mqtt_pingreq_send(void *ctx);

/** Send a DISCONNECT message to the server.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @return number of bytes sent or Lib define errors (@ref lib_err_group)
*/
i32 mqtt_disconn_send(void *ctx);


/** Send remaining data or contents of the scheduled message to the server.
    This routine tries to send the remaining data in an active transfer of a
    message to the server. This service is valid, only if the network layer of
    the platform is not able to send out the entire message in one TCP packet
    and has to "back-off" or "give up the control" before it can schedule or
    dispatch the next packet. In such a scenario, the network layer sends the
    first part (segment) of the scheduled message in the mqtt_xxx_send() API and
    the subsequent parts or the segments are sent using this routine.
    
    This routine is not applicable to the platforms or the scenarios, where the
    implementation of the platform can segment the MQTT message in a manner to
    schedule consercutive or back-to-back blocking socket transactions.
    Specifically, this API must be used by an application, only if the network
    layer can indicate in an asynchronous manner, its readiness to send the next
    packet to the server. And the mechanism to indicate readiness of the network
    layer for the next send transaction is out of band and out of scope for the
    Client LIB and depends on the platform.

    A platform that resorts to partial send of a message and has to back-off from
    transmission implies the following the considerations on to the application.
    (a) The next segment / part of the currently active MQTT packet can be sent
    or scheduled only after receiving the indication, to do so, from the network
    layer.
    (b) The next or new MQTT message (or its first segment) can be scheduled for
    transmission only after receiving the indication for completion of handling
    of the last segment of currently active message.

    @note The application developer should refer to the platform specific network
    implementation for details.

    The routine returns the number of remaining bytes in the message to be sent.
    However, as described earlier, the application is expected to wait for an
    indication about the readiness of the network layer prior to sending or
    scheduling another segment, if so available, to the server. Now, the new
    segment can be a next part of the currently active message or it can be the
    first segment of a new message. A return value of zero means that there is
    no more data left in the scheduled message to be sent to the server and the
    application should wait for an appropriate event to indicate the transmission
    of the last segment.

    In case of an error, the transfer of the remaining segments or parts of the
    scheduled message is aborted. Depending on the configuration of the 'clean
    session' in-conjunction with the revision of the MQTT protocol, the active
    message would be stored for re-transmission, MQTT connection is established
    again. To store a message for re-transmission, at least one segment of the
    message must have been successfully sent to the server.

    @note This API must be used by the application only if the platform has the
    capability to indicate the completion of the sending of an active segment.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @return the number of bytes remaining to be sent in the message. Otherwise,
    LIB defined errors (@ref lib_err_group)
*/
i32 mqtt_client_send_progress(void *ctx);

/** Block on the 'context' to receive a message type with-in specified wait time.
    This service is valid only for the configuration, where the application has
    not provided the callbacks to the client LIB 'context'. The caller must
    provide a packet buffer of adequate size to hold the expected message from
    the server. 

    The wait time implies the maximum intermediate duration between the reception
    of two successive messages from the server. If no message is received before
    the expiry of the wait time, the routine returns. However, the routine would
    continue to block, in case, messages are being received within the successive
    period of wait time and these messages are not the one that client is waiting
    for.

    For platforms that can receive a MQTT message over multiple TCP packets or in
    segments, it is neccessary for the application to provide the same packet
    packet buffer 'mqp' across all iterations of this routine that returns with a
    value of MQP_ERR_TIMEOUT. Such an arrangement enables the implementation to
    iteratively build from consecutive multiple RX network packets / segements, a
    MQTT message into the packet buffer 'mqp. The application can always choose
    to assign a new 'mqp' once, a complete MQTT message has been received.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] msg_type message type to receive. A value of 0 would imply that
    caller is ready to receive the next message, whatsoever, from the  server.
    @param[out] mqp packet buffer to hold the message received from the server.
    @param[in] wait_secs maximum Time to wait for a message from the server.
    @return On success, the number of bytes received for 'msg_type' from server,
    otherwise LIB defined error values (@ref lib_err_group)
*/
i32 mqtt_client_ctx_await_msg(void *ctx, u8 msg_type, struct mqtt_packet *mqp,
                              u32 wait_secs);

/** Helper function to receive any message from the server.
    Refer to mqtt_client_ctx_await_msg for the details.
    @see mqtt_client_ctx_await_msg
*/
static inline
i32 mqtt_client_ctx_recv(void *ctx, struct mqtt_packet *mqp, u32 wait_secs)
{
        /* Receive next and any MQTT Message from the broker */
        return mqtt_client_ctx_await_msg(ctx, 0x00, mqp, wait_secs);
}

/** Run the context for the specificed wait time.
    This service is valid only for the configuration, where the application has
    populated the callbacks that can be invoked by the client LIB 'context'.

    This routine yields the control back to the application after the duration
    of the wait time. Such an arrangement enable the application to make overall
    progress to meet its intended functionality. 

    The wait time implies the maximum intermediate duration between the reception
    of two successive messages from the server. If no message is received before
    the expiry of the wait time, the routine returns. However, the routine would
    continue to block, in case, messages are being received within the successive
    period of the wait time.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] wait_secs maximum time to wait for a message from the server

    @return MQP_ERR_NOTCONN if MQTT connection is closed by the application, 
    MQP_ERR_TIMEOUT if there was no MQTT transaction in the interval of wait time
    and other values (@ref lib_err_group)
*/
i32 mqtt_client_ctx_run(void *ctx, u32 wait_secs);

/** Block to receive any message for the grouped contexts within specified time.
    This service is valid only for the set-up, where the applicatiion has not
    configured the grouped contexts in the callback mode. The caller must provide
    a packet buffer of adequate size to hold the expected message from the server.

    The wait time implies the maximum intermediate duration between the reception
    of two successive messages from the server. If no message is received before
    the expiry of the wait time, the routine returns. However, the routine would
    continue to block, in case, messages are being received within the successive
    period of wait time.

    In this revision of the LIB, the underlying implementation will be provide a
    valid value in the 'app' field for the all returned values, including errors,
    other than the cases of 'MQP_ERR_TIMEOUT' and 'MQP_ERR_LIBQUIT'. The value in
    'app', when valid refers to a previously configured handle to an application
    and idenitifies the 'context' for which the routine has been returned.

    @param[out] mqp packet buffer to hold the message received from the server.
    @param[in]  wait_secs Maximum Time to wait for a message from the server.
    @param[out] app place holder to indicate application handle for the packet.
    @return On success, the number of bytes received for 'msg_type' from server,
    otherwise LIB defined error values (@ref lib_err_group)

    @note if the value of MQP_ERR_LIBQUIT is returned, then system must be
    restarted.
*/
i32 mqtt_client_await_msg(struct mqtt_packet *mqp, u32 wait_secs, void **app);

/** Run the LIB for the specified wait time.
    This service is valid only for the set-up of grouped contexts, where the
    application has populated the callbacks that can be invoked by the LIB.

    This routine yields the control back to the application after the specified
    duration of wait time. Such an arrangement enable the application to
    make overall progress to meet it intended functionality. 

    The wait time implies the maximum intermediate duration between the reception
    of two successive messages from the server. If no message is received before
    the expiry of the wait time, the routine returns. However, the routine would
    continue to block, in case, messages are being received within the successive
    period of wait time.

    @param[in] wait_secs maximum time to wait for a message from the server

    @return on connection close by client app, number of bytes received for the
    last msg from broker, otherwise LIB defined error values.

    @note if the value of MQP_ERR_LIBQUIT is returned, then system must be
    restarted.
*/
i32 mqtt_client_run(u32 wait_secs);

/*------------------------------------------------------------------------------
 *  MQTT Client Library: Packet Buffer Pool and its management
 *------------------------------------------------------------------------------
 */

/** Allocates a free MQTT Packet Buffer.
    The pool that will be used by the library to allocate a free MQP buffer
    must be configured (i.e. registered) a-priori by the app.

    The parameter 'offset' is used to specify the number of bytes that are
    reserved for the header in the buffer

    @param[in] msg_type Message Type for which MQP buffer is being assigned.
    @param[in] offset Number of bytes to be reserved for MQTT headers.
    @return A NULL on error, otherwise a reference to a valid packet holder.

    @see mqtt_client_register_buffers
*/
struct mqtt_packet *mqp_client_alloc(u8 msg_type, u8 offset);

/** Helper function to allocate a MQTT Packet Buffer for a message to be
    dispatched to server.

    @see to mqp_client_alloc( ) for details.
*/
static inline struct mqtt_packet *mqp_client_send_alloc(u8 msg_type)
{
        return mqp_client_alloc(msg_type, MAX_FH_LEN);
}

/** Helper function to allocate a MQTT Packet Buffer for a message to be
    received from the server.

    @see to mqp_client_alloc( ) for details.
*/
static inline struct mqtt_packet *mqp_client_recv_alloc(u8 msg_type)
{
        return mqp_client_alloc(msg_type, 0);
}

/** Create a pool of MQTT Packet Buffers for the client library. 
    This routine creates a pool of free MQTT Packet Buffers by attaching a buffer
    (buf) to a packet holder (mqp). The count of mqp elements and buf elements in
    the routine are same. And the size of the buffer in constant across all the
    elements.

    The MQTT Packet Buffer pool should support (a) certain number of in-flight and
    stored packets that await ACK(s) from the server (b) certain number of packets
    from server whose processing would be deferred by the client app (to another
    context) (c) a packet to create a CONNECT message to re-establish transaction
    with the server.

    A meaningful size of the pool is very much application specific and depends
    on the target functionality. For example, an application that intends to have
    only one in-flight message to the server would need atmost three MQP buffers
    (1 for TX (for Qos1 or 2 store), 1 for RX and 1 for CONNECT message). If the
    application sends only QoS0 messages to the server, then the number of MQP
    buffers would reduce to two (i.e. 1 Tx to support CONNECT / PUB out and 1 RX)

    @param[in] num_mqp Number or count of elements in mqp_vec and buf_vec.
    @param[in] mqp_vec An array of MQTT Packet Holder without a buffer.
    @param[in] buf_len The size or length of the buffer element in the 'buf_vec'
    @param[in] buf_vec An array of buffers.
    @retun  0 on success otherwise -1 on error.

    @note The parameters mqp_vec and buf_vec should be peristent entities.

    @see mqtt_client_await_msg
    @see mqtt_client_run
*/
i32 mqtt_client_buffers_register(u32 num_mqp, struct mqtt_packet *mqp_vec,
                                 u32 buf_len, u8 *buf_vec);

/*------------------------------------------------------------------------------
 *  MQTT Client Library: Register application, platform information and services.
 *------------------------------------------------------------------------------
 */

/** Register application info and its credentials with the client library.
    This routine registers information for all the specificed parameters,
    therefore, an upate to single element would imply re-specification of
    the other paramters, as well.

    @note Contents embedded in the parameters is not copied by the routine,
    and instead a reference to the listed constructs is retained. Therefore,
    the app must enable the parameter contents for persistency.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] client_id MQTT UTF8 identifier of the client. If set to NULL,
    then the client will be treated as zero length entity.
    @param[in] user_name MQTT UTF8 user name for the client. If not used,
    set it to NULL. If used, then it can't be of zero length.
    @param[in] pass_word MQTT UTF8 pass word for the client. If not used, set
    it to NULL, If used, then it can't be of zero length.
    @return 0 on success otherwise -1

    User name without a pass word is a valid configuration. A pass word won't
    be processed if it is not associated with a valid user name.
*/
i32 mqtt_client_ctx_info_register(void *ctx,
                                  const struct utf8_string *client_id,
                                  const struct utf8_string *user_name,
                                  const struct utf8_string *pass_word);

/** Register WILL information of the client application.
    This routine registers information for all the specificed parameters,
    therefore, an update to single element would imply re-specification
    of the other paramters, as well.

    @note Contents embedded in the parameters is not copied by the routine,
    and instead a reference to the listed constructs is retained. Therefore,
    the app must enable the parameter contents for persistency.

    @param[in] ctx handle to the underlying network context in the LIB
    @see mqtt_client_ctx_create
    @param[in] will_top UTF8 WILL Topic on which WILL message is to be published.
    @param[in] will_msg UTF8 WILL message.
    @param[in] will_qos QOS for the WILL message
    @param[in] retain asserted to indicate that published WILL must be retained
    @return 0 on success otherwise -1.

    Both will_top and will_msg should be either present or should be NULL.
    will_qos and retain are relevant only for a valid Topic and Message combo.
*/
i32 mqtt_client_ctx_will_register(void *ctx,
                                  const struct utf8_string  *will_top,
                                  const struct utf8_string  *will_msg,
                                  enum mqtt_qos will_qos, bool retain);

/** Abstraction for the device specific network services.
    Network services for communication with the server

    @param[in] net refers to network services supported by the platform
    @return on success, 0, otherwise -1

    @ref net_ops_group
    @note all entries in net must be supported by the platform.
*/
i32 mqtt_client_net_svc_register(const struct device_net_services *net);

/** Helper functions & macros to derive 16 bit CONNACK Return Code from broker.
 */
#define VHB_CONNACK_RC(vh_buf) (vh_buf[1])     /**< CONNACK  VH:: Return Code */
#define MQP_CONNACK_RC(mqp)    (mqp->buffer[3])/**< CONNACK MQP:: Return Code */

#define VHB_CONNACK_SP(vh_buf) (vh_buf[0] & 0x1)/**< CONNACK VH:: Session Bit */
#define MQP_CONNACK_SP(mqp)    (mqp->buffer[2] & 0x1) /**< CONNACK MQP:: \
                                                         Session Bit */

#define VHB_CONNACK_VH16(vh_buf)((vh_buf[0] << 8) | vh_buf[1]) 
#define MQP_CONNACK_VH16(mqp)   ((mqp->buffer[2] << 8) | mqp->buffer[3])

/** Callbacks to be invoked by MQTT Client library onto Client application */
struct mqtt_client_ctx_cbs {

        /** Provides a PUBLISH message from server to the client application.
            The application can utilize the associated set of helper macros to
            get references to the topic and the data information contained in
            the message. @ref rxpub_help_group

            Depending upon the QoS level of the message, the MQTT client library
            shall dispatch the correponding acknowlegement (PUBACK or PUBREC) to
            the server, thereby, relieving application of this support.

            If the application completes the processing of the packet within the
            implementation of this callback routine, then a value of 'true' must
            be returned to the client LIB 'context'. The library, in this case,
            does not handover the packet to application and instead, frees it up
            on return from this routine.

            If the application intends to defer the processing of the PUBLISH
            message to a different execution task, then it must takeover the
            owernship of the packet by returning a value of 'false' to the client
            LIB 'context. In this arrangement, the client LIB 'context' hands
            over the packet to the application. The responsibility of storing,
            managing and eventually freeing up the packet back to the pool, now,
            lies with the app.

            @param[in] app application to which this PUBLISH message is targeted
            @see mqtt_client_ctx_create
            @param[in] dup asserted to indicate a DUPLICATE PUBLISH
            @param[in] qos quality of Service of the PUBLISH message
            @param[in] retain Asserted to indicate message at new subscription
            @param[in] mqp Packet Buffer that holds the PUBLISH message

            @return true to indicate that processing of the packet has been
            completed and it can freed-up and returned back to the pool by
            the library. Otherwise, false.
        */
        bool (*publish_rx)(void *app,
                           bool dup, enum mqtt_qos qos, bool retain, 
                           struct mqtt_packet *mqp);

        /** Notifies the client application about an ACK or a response from the
            server. Following are the messages that are notified by the client
            LIB to the application.

            CONNACK, PINGRSP, PUBACK, PUBCOMP, SUBACK, UNSUBACK

            @param[in] app application to which this ACK message is targeted
            @see mqtt_client_ctx_create
            @param[in] msg_type Type of the MQTT messsage 
            @param[in] msg_id transaction identity of the message
            @param[in] buf refers to contents of message and depends on msg_type
            @param[in] len length of the buf
            @return none

            @note The size of the buf parameter i.e len is non-zero for the
            SUBACK and CONNACK messages. For SUBACK the buf carries an array of
            QOS responses provided by the server. For CONNACK, the buf carries
            variable header contents. Helper macro VHB_CONNACK_RC( ) and 
            VHB_CONNACK_SP( ) can be used to access contents provided by the 
            server. For all other messages, the value of len parameter is zero.

            @note The parameter msg_id is not relevant for the messages CONNACK
            and PINGRSP and is set to zero.
        */
        void (*ack_notify)(void *app, u8 msg_type, u16 msg_id, u8 *buf, u32 len);

        /** Notifies the client application about the termination of connection
            with the server. After servicing this callback, the application can
            destroy associated context if it no longer required 

            @param[in] app application whose connection got terminated
            @see mqtt_client_ctx_create
            @param[in] cause reason that created disconnection
            (@ref lib_err_group)
        */
        void (*disconn_cb)(void *app, i32 cause);
};

struct mqtt_client_ctx_cfg {

        /** @defgroup mqtt_ctx_cfg_opt_grp Options for application to config CTX
            @{
        */
#define MQTT_CFG_PROTOCOL_V31  0x0001 /**< Assert for ver3.1, default is 3.1.1 */
#define MQTT_CFG_APP_HAS_RTSK  0x0002 /**< Assert if APP has dedicated RX Task */
#define MQTT_CFG_MK_GROUP_CTX  0x0004 /**< Assert if task manages > 1 contexts */
        /** @} */

        u16   config_opts;  /**< Context config Opt, @ref mqtt_ctx_cfg_opt_grp */

        /** @defgroup mqtt_netconn_opt_grp Options for App to configure network
            @{
        */
#define MQTT_NETCONN_OPT_IP6  DEV_NETCONN_OPT_IP6 /**<@ref dev_netconn_opt_grp */
#define MQTT_NETCONN_OPT_URL  DEV_NETCONN_OPT_URL /**<@ref dev_netconn_opt_grp */
#define MQTT_NETCONN_OPT_SEC  DEV_NETCONN_OPT_SEC /**<@ref dev_netconn_opt_grp */
        /** @} */

        u32   nwconn_opts;  /**< Network Options,  @ref mqtt_netconn_opt_grp */

        c8   *server_addr;  /**< Reference to '\0' terminated address string */
        u16   port_number;  /**< Network Listening Port number of the server */
        struct secure_conn *nw_security;  /**< Refer to @ref mqtt_netsec_grp */
};

/** Create a Network Connection Context.
    This routine sets-up the parameters that are required to operate and manage
    the network connection with a MQTT server / broker. As part of the creation
    of a context, the implementation also records the handle, if provided, by
    the application. In addition, the invoker of the routine must facilitate a
    place holder to enable the client LIB to provision the reference to the 
    'context', so created.

    Specifically, this routine associates or ties-up, in an one-to-one manner,
    the caller provided handle 'app' and the client LIB provisioned handle 'ctx'.
    The parameter 'app' is returned by the client LIB in certain other routines
    to indicate the underlying 'context' with which network transaction or event
    is associated. Similarly, the caller must specify the context handle 'ctx'
    for which the services are being invoked.

    A user or a task prior to utilizing the services of the library to schedule
    MQTT transactions must create a 'context'. A client LIB 'context' can be
    operated in two modes: (a) sync-wait or explicit receive mode and (b) the
    callback mode. Provisioning or absence of the callback parameter in this
    routine defines the mode of operation of the 'context'.

    Explicit receive mode is analogous to the paradigm of the socket programming
    in which an application utilize the recv() function call. It is anticipated
    that applications which would make use of limited set of MQTT messages may
    find this mode of operation useful. Applications which intend to operate the
    'context' in this mode must not provision any callbacks.

    On the other hand, certain applications, may prefer an asynchronous mode of
    operation and would want the client LIB 'context' to raise callbacks into
    the application, as and when, packets arrive from the server. And such
    applications must provide the callback routines.

    @param[in] ctx_cfg configuration information for the Network Context. 
    @param[in] ctx_cbs callback routines. Must be set to NULL, if the application
    intends to operate the context in the sync-wait / explicit receive mode.
    @param[in] app     handle to application. Returned by LIB in other routines
    to refer to the underlying context.
    @param[out] ctx reference to the context created and is provisioned by the
    implementation. (Valid only if routine returns a success)

    @return 0 on success otherwise -1.
*/
i32 mqtt_client_ctx_create(const struct mqtt_client_ctx_cfg *ctx_cfg,
                           const struct mqtt_client_ctx_cbs *ctx_cbs,
                           void *app, void **ctx);

/** Delete a Network Connection Context.
    This routines destroys the previously created network 'context' and releases
    resources that would be assigned for maintaining the information about the
    'context'.

    A user or a task prior to deleting the 'context' must ensure that there is no
    active MQTT connection on this context.

    @param[in] ctx handle to network context to be deleted. The context must
    have been previously created.

    @return 0 on success otherwise -1
*/

i32 mqtt_client_ctx_delete(void *ctx);

/** Contruct / Data to initialize MQTT Client Library */
struct mqtt_client_lib_cfg {

        /** If an application has more than one contexts (i.e. grouped contexts)
            to manage in a single task, then a non-zero value must be provided.
            Otherwise, this parameter must be set to zero.
        */
        u16   loopback_port;
        bool  grp_uses_cbfn;  /**< Assert if grouped contexts use call-backs */

        /** For a multi-task environment, provide a handle to platform mutex */
        void  *mutex;
        void (*mutex_lockin)(void *mutex); /**< Take platform mutex function */
        void (*mutex_unlock)(void *mutex); /**< Give platform mutex function */

        i32  (*debug_printf)(const c8 *format, ...);   /**< Debug, mandatory */
        bool  aux_debug_en;    /**< Assert to indicate additional debug info */
};

/** Initialize the MQTT client library.
    This routine initializes all the common constructs that are required to
    manage the multiple network connetions. The client LIB must be initialized
    prior to invoking of any other routine or service.

    @note This routine must be invoked only once in an run of the system.

    Depending upon the deployment needs, this routine can be invoked either as
    part of the platform initialization sequence or as part of the application.
    Deployments that have more than one application utilizing the services of
    the client LIB should try to invoke the routine from the initialization
    sequence of the platform.

    In addition, if an application has to manage more than one network
    connections (i.e. in other words, if the application has to handle
    a group of connections), then certain configuration must be set in
    the LIB @see struct mqtt_client_lib_cfg

    @note There must be only one group of network connetions in the system.

    @param[in] cfg Configuration information for the MQTT client Library.

    @return 0 on success otherwise -1.
*/

i32 mqtt_client_lib_init(const struct mqtt_client_lib_cfg  *cfg);

/** Exit the MQTT client library.
    @return 0 on success otherwise -1.
*/
i32 mqtt_client_lib_exit(void);

/** @} */ /* End group client_api */

#endif
