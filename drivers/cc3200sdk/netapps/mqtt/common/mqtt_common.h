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
  mqtt_common.h

  This module outlines the interfaces that are common to both client amd
  server components. The applications are not expected to utilize the
  services outlined in this module.
*/

#ifndef __MQTT_COMMON_H__
#define __MQTT_COMMON_H__

/** @file mqtt_common.h 
    This file incorporates constructs that are common to both client and server
    implementation. 

    The applications are not expected to utlize the routines made available in
    this module module. 

    @note the routines in this module do not check for availability and 
    correctness of the input parameters

    @warning The module is expected to under-go changes whilst incorporating
    support for the server. Therefore, it is suggested that applications do
    not rely on the services provided in this module.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "platform.h"

#define MQTT_COMMON_VERSTR "1.1.1" /**< Version of Common LIB */

#define MIN(a, b) ((a > b)? b : a)

/** MQTT Message Types */
#define MQTT_CONNECT      0x01
#define MQTT_CONNACK      0x02
#define MQTT_PUBLISH      0x03
#define MQTT_PUBACK       0x04
#define MQTT_PUBREC       0x05
#define MQTT_PUBREL       0x06
#define MQTT_PUBCOMP      0x07
#define MQTT_SUBSCRIBE    0x08
#define MQTT_SUBACK       0x09
#define MQTT_UNSUBSCRIBE  0x0A
#define MQTT_UNSUBACK     0x0B
#define MQTT_PINGREQ      0x0C
#define MQTT_PINGRSP      0x0D
#define MQTT_DISCONNECT   0x0E

#define MAX_FH_LEN        0x05    /**< MAX Length of Fixed Header */

/** Max number of bytes in remaining length field */
#define MAX_REMLEN_BYTES  (MAX_FH_LEN - 1)  

#define MAKE_FH_BYTE1(msg_type,  flags) (u8)((msg_type << 4) | flags)

#define MAKE_FH_FLAGS(bool_dup, enum_qos, bool_retain)                  \
        (u8)(((bool_dup << 3) | (enum_qos << 1) | bool_retain) & 0xF)

#define QID_VMASK           0x3
#define QOS_VALUE(enum_qos) (u8)(enum_qos & QID_VMASK)
#define QFL_VALUE           0x80  /**< QOS Failure value (SUBACK) */

#define DUP_FLAG_VAL(bool_val) (u8)(bool_val << 3)

#define BOOL_RETAIN(fh_byte1)  ((fh_byte1 & 0x1)? true : false)
#define BOOL_DUP(fh_byte1)     ((fh_byte1 & 0x8)? true : false)
#define ENUM_QOS(fh_byte1)     (enum mqtt_qos)((fh_byte1 & 0x6) >> 1)

#define MSG_TYPE(fh_byte1)  (u8)((fh_byte1 & 0xf0) >> 4)

static inline u32 buf_wr_nbytes(u8 *dst, const u8 *src, u32 n)
{
        u32 c = n; 
        while(c--)
                *dst++ = *src++;

        return n;
}

static inline u32 buf_set(u8 *dst, u8 val, u32 n)
{
        u32 c = n; 
        while(c--)
                *dst++ = val;

        return n;
}

/** Writing 2 bytes entity in network byte order */
static inline u32 buf_wr_nbo_2B(u8 *buf, u16 val)
{
        buf[0] = (u8)((val >> 8) & 0xFF); /* MSB */
        buf[1] = (u8)((val)      & 0xFF); /* LSB */
        return 2;
}

/** Reading 2 bytes entity in network byte order */
static inline u32 buf_rd_nbo_2B(const u8 *buf, u16 *val)
{
        *val = (u16)((buf[0] << 8) | (buf[1]));
        return 2;
}

/** @defgroup mqp_group MQTT Packet (MQP) Buffer structure
    The core construct to encapsulate, construct and process a message

    @{
*/
struct mqtt_packet {

        u8                     msg_type;   /**< MQTT  Message  Type */
        u8                     fh_byte1;   /**< Fixed Header: Byte1 */

        u16                    msg_id;     /**< Msg transaction  ID */

        u8                     n_refs;     /**< # users of this msg */
        u8                     pad[3];

        u8                     offset;     /**< Start of data index */
        u8                     fh_len;     /**< Fix Header Length   */
        u16                    vh_len;     /**< Var Header Length   */
        u32                    pl_len;     /**< Pay Load   Length   */

        u32                    private;

        u32                    maxlen;     /**< Maximum buffer size */
        u8                    *buffer;     /**< The attached buffer */

        /** Method to free this packet to a particular pool */
        void                  (*free)(struct mqtt_packet *mqp);

        struct mqtt_packet    *next;
};

/** @} */

#define MQP_FHEADER_BUF(mqp)  (mqp->buffer + mqp->offset)
#define MQP_VHEADER_BUF(mqp)  (MQP_FHEADER_BUF(mqp) + mqp->fh_len)
#define MQP_PAYLOAD_BUF(mqp)  (MQP_VHEADER_BUF(mqp) + mqp->vh_len)

#define MQP_CONTENT_LEN(mqp)  (mqp->fh_len + mqp->vh_len + mqp->pl_len)
#define MQP_FREEBUF_LEN(mqp)  (mqp->maxlen - mqp->offset -      \
                               MQP_CONTENT_LEN(mqp))

#define MQP_FHEADER_VAL(mqp)  (mqp->fh_byte1)
#define MQP_FHEADER_MSG(mqp)  (MSG_TYPE(MQP_FHEADER_VAL(mqp)))
#define MQP_FHEADER_FLG(mqp)  (MSG_FLAGS(MQP_FHEADER_VAL(mqp)))

#define DEFINE_MQP_VEC(num_mqp, mqp_vec)                \
        static struct mqtt_packet mqp_vec[num_mqp];

#define DEFINE_MQP_BUF_VEC(num_mqp, mqp_vec, buf_len, buf_vec)        \
        DEFINE_MQP_VEC(num_mqp, mqp_vec);                             \
        static u8 buf_vec[num_mqp][buf_len];

/*---------------------------------------------------------------------
 * Heleper MACROS for PUBLISH-RX Message Processing
 *---------------------------------------------------------------------
 */

/** @defgroup rxpub_help_group Helper Macros for RX PUBLISH
    @{
*/

/** Yields pointer to topic content */
#define MQP_PUB_TOP_BUF(mqp) (MQP_VHEADER_BUF(mqp) + 2)

/** Length or size of topic content */
#define MQP_PUB_TOP_LEN(mqp) (mqp->vh_len - 2 - (mqp->msg_id? 2 : 0))

/** Yields pointer to payload data */
#define MQP_PUB_PAY_BUF(mqp) (mqp->pl_len? MQP_PAYLOAD_BUF(mqp) : NULL)

/** Length or size of payload data */
#define MQP_PUB_PAY_LEN(mqp) (mqp->pl_len)

/** @} */

/** @cond
    CONNECT Message Flags, as outined in the MQTT Specification
*/
#define WILL_RETAIN_VAL 0x20
#define WILL_CONFIG_VAL 0x04
#define CLEAN_START_VAL 0x02
#define USER_NAME_OPVAL 0x80
#define PASS_WORD_OPVAL 0x40
/**
    @endcond
*/

/** @cond
    CONNACK 8bit Return Code, as outlined in the MQTT Specification
*/
#define CONNACK_RC_REQ_ACCEPT   0x00
#define CONNACK_RC_BAD_PROTOV   0x01
#define CONNACK_RC_CLI_REJECT   0x02
#define CONNACK_RC_SVR_UNAVBL   0x03
#define CONNACK_RC_BAD_USRPWD   0x04
#define CONNACK_RC_NOT_AUTHED   0x05
/**
   @endcond
*/

/** @defgroup lib_err_group LIBRARY Generated Error Codes
    Library provides these codes as return values in several routines

    @{
*/
#define MQP_ERR_NETWORK   (-1)  /**< Problem in network (sock err) */
#define MQP_ERR_TIMEOUT   (-2)  /**< Net transaction has timed out */
#define MQP_ERR_NET_OPS   (-3)  /**< Platform Net Ops un-available */
#define MQP_ERR_FNPARAM   (-4)  /**< Invalid parameter(s) provided */
#define MQP_ERR_PKT_AVL   (-5)  /**< No pkts are available in pool */
#define MQP_ERR_PKT_LEN   (-6)  /**< Inadequate free buffer in pkt */
#define MQP_ERR_NOTCONN   (-7)  /**< Lib isn't CONNECTED to server */
#define MQP_ERR_BADCALL   (-8)  /**< Irrelevant call for LIB state */
#define MQP_ERR_CONTENT   (-9)  /**< MSG / Data content has errors */
#define MQP_ERR_LIBQUIT  (-10)  /**< Needs reboot library has quit */


#define MQP_ERR_NOT_DEF  (-32)  /**< Value other than defined ones */

/** @} */

/*---------------------------------------------------------------------
 * Common Operations
 *---------------------------------------------------------------------
 */

/** Free a MQTT Packet Buffer 
    Puts back the packet buffer in to the appropriate pool.
    
    @param[in] mqp packet buffer to be freed
    @return none
*/
void mqp_free(struct mqtt_packet *mqp);

/** Resets the attributes of MQTT Packet Holder to its init state
    Not all fields are reset - entities such as offset, n_refs in addition
    to buffer information are not updated.

    @param[in] mqp packet buffer to be reset
    @return none

    @see mqp_init
*/
void mqp_reset(struct mqtt_packet *mqp);

/** Initializes attributes of the MQTT Packet Holder.
    This routine sets number of users of the MQTT Packet Holder to 1. However,
    it leaves, if already provisioned, the reference to buffer and its size
    un-altered.

    @param[in] mqp packet buffer to be initialized
    @param[in] offset index in buffer to indicate start of the contents
    @return none
*/
void mqp_init(struct mqtt_packet *mqp, u8 offset);

/** Initialize MQTT Packet Holder and attach the buffer */
static 
inline void mqp_buffer_attach(struct mqtt_packet *mqp, u8 *buffer, u32 length,
                              u8 offset)
{
        mqp_init(mqp, offset);

        mqp->buffer = buffer;
        mqp->maxlen = length;
        mqp->free   = NULL;

        return;
}

/** Description of UTF8 information as used by MQTT Library. */
struct utf8_string {
        
        const c8 *buffer;   /**< Refers to UTF8 content */
        u16       length;   /**< Length of UTF8 content */
};

/** Write UTF8 information into the buffer. 
    The UTF8 information includes content and its length.

    @warning The routine does not check for correctness of the paramters.

    @param[in] buf refers to memory to write UTF8 information into
    @param[in] utf8 contains UTF8 information to be written
    @return on success, number of bytes written, otherwise -1 on error.
*/
i32 mqp_buf_wr_utf8(u8 *buf, const struct utf8_string *utf8);

/** Write the MQTT construct 'Remaining Length' into trailing end of buffer.
    The 'remaining length' is written in the format as outlined in the MQTT
    specification. 

    The implementation assumes availability of at-least 4 bytes in the buffer.
    Depending on the value of 'Remaining Length' appropriate trailing bytes in
    the buffer would be used.
    
    @param[in] buf refers to memory to tail-write 'Remaining Length' into
    @param[in] remlen The 'Remaining Length' value
    @return in success, number of trailing bytes used, otherwise -1 on error
*/
i32 mqp_buf_tail_wr_remlen(u8 *buf, u32 remlen);

/** Read MQTT construct 'Remaining Length' from leading bytes of the buffer.
    The 'remaining length' is written in the format as outlined in the MQTT
    specification.
     
    @param[in] buf refers to memory to head-read 'Remaining Length' from
    @param[in] remlen place-holder for The 'Remaining Length' value
    @return in success, number of header bytes read, otherwise -1 on error
*/   
i32 mqp_buf_rd_remlen(u8 *buf, u32 *remlen);

/** Include variable header Topic as part of PUB Message construction.
    Inclusion of a Topic also encompasses incorporation of the message ID.

    The topic refers to the subject for which data will be published by
    the client or the server. The topic entity must be appended into the
    packet buffer prior to the inclusion of the payload (data).

    @warning This routine does not check for correctness of the input
    parameters.

    @param[in] mqp packet buffer in which topic must be included.
    @param[in] topic UTF8 information
    @param[in] msg_id Message or Packet transaction ID
    @return on success, number of bytes appended, otherwise -1 on error.

    @note A 'topic' must be appended prior to inclusion of pulished data.
*/
i32 
mqp_pub_append_topic(struct mqtt_packet *mqp, const struct utf8_string *topic,
                     u16 msg_id);

/** Include payload data for publishing
    The payload data is associated with a topic.
    
    @warning This routine does not check for correctness of the input
    parameters.

    @param[in] mqp packet buffer in which payload data must be included.
    @param[in] data_buf data to be included in the packet buffer
    @param[in] data_len length of the data to be included in the packet.
    @return on success, number of bytes appended, otherwise -1 on error.

    @note A 'topic' must be appended prior to inclusion of pulished data.
*/
i32 mqp_pub_append_data(struct mqtt_packet *mqp, const u8 *data_buf,
                        u32 data_len);

/** Construct a packet for Message ID enabled ACK received from network
    Process the raw ACK message information to update the packet holder.
   
    @warning This routine does not check for correctness of the input
    parameters.
       
    @param[in] mqp_raw holds a raw buffer from the network
    @param[in] has_payload asserted, if ACK message should have a payload
    @return on success, true, otherwise false
*/
bool mqp_proc_msg_id_ack_rx(struct mqtt_packet *mqp_raw, bool has_payload);

/** Construct a packet for PUBLISH message received from the network
    Process the raw PUB message information to update the packet holder.

    @warning This routine does not check for correctness of the input
    parameters.

    @param[in] mqp_raw holds a raw buffer from the network
    @return on success, true, other wise false
*/
bool mqp_proc_pub_rx(struct mqtt_packet *mqp_raw);

/*
   Wait-List of MQTT Messages for which acknoledge is pending from remote node.
*/
struct mqtt_ack_wlist {
        
        struct mqtt_packet *head;  /* Points to head of single linked-list. */
        struct mqtt_packet *tail;  /* Points to tail of single linked-list. */
};

static inline bool mqp_ack_wlist_is_empty(struct mqtt_ack_wlist *list)
{
        return (NULL == list->head) ? true : false;
}

/*
   Add specified element into trailing end of list.

   Returns, on success, true, otherwise false.
*/
bool mqp_ack_wlist_append(struct mqtt_ack_wlist *list,
                          struct mqtt_packet    *elem);

/*
   Removes element that has specified msg_id from list. 

   Returns, on success, pointer to removed element, otherwise NULL.
*/
struct mqtt_packet *mqp_ack_wlist_remove(struct mqtt_ack_wlist *list,
                                          u16 msg_id);
/* 
   Removes and frees all elements in list. 
*/
void mqp_ack_wlist_purge(struct mqtt_ack_wlist *list);

static inline bool is_wlist_empty(const struct mqtt_ack_wlist *list)
{
        return list->head? false : true;
}

/** Prepare the Fixed-Header of the MQTT Packet (before being sent to network)
    Based on the contents of the mqtt packet and the combination of DUP, QoS
    and Retain flags as outlined the MQTT specification, the routine updates,
    among others, significant internal fields such as 'remaining length' and
    'fixed header length' in the packet construct and embeds the fixed header,
    so created, in the packet buffer.
    
    This service must be utilized on a packet that has been already populated
    with all the payload data, topics and other contents. The fixed header
    must be the final step in the compostion of MQTT packet prior to its
    dispatch to the server.
    
    Returns size, in bytes, of the fixed-header, otherwise -1 on error.
*/
i32 mqp_prep_fh(struct mqtt_packet *mqp, u8 flags);

/** MQTT Quality of Service */
enum mqtt_qos {
        
        MQTT_QOS0,  /**< QoS Level 0 */
        MQTT_QOS1,  /**< QoS Level 1 */
        MQTT_QOS2   /**< QoS Level 2 */
};

/** Construct to create Topic to SUBSCRIBE */
struct utf8_strqos {

        const c8       *buffer;  /**< Refers to UTF8 content */
        u16             length;  /**< Length of UTF8 content */
        enum mqtt_qos   qosreq;  /**< QoS Level  for content */
};


/** @defgroup mqtt_netsec_grp Information to establish a secure connection.
    This is implementation specific and is targeted for the network services.

    Specifically, the MQTT implementation makes no assumption or use of this
    construct. The client library merely passes information from the app to
    the network service layer.
    Note: value of n_file can vary from 1 to 4, with corresponding pointers to
    the files in files field. Value of 1(n_file) will assume the corresponding
    pointer is for CA File Name. Any other value of n_file expects the files to
    be in following order:
    1.  Private Key File
    2.  Certificate File Name
    3.  CA File Name
    4.  DH Key File Name

example: 
If you want to provide only CA File Name, following are the two way of doing it:
for n_file = 1
char *security_file_list[] = {"/cert/testcacert.der"};
for n_file = 4
char *security_file_list[] = {NULL, NULL, "/cert/testcacert.der", NULL};

where files = security_file_list
    @{
*/
struct secure_conn {
        
        void *method;  /**< Reference to information about protocol or methods */
        void *cipher;  /**< Reference to information about cryptograph ciphers */
        u32   n_file;  /**< Count of secure connection related files, certs... */
        c8   **files;  /**< Reference to array of file-names used for security */
};

/** @} */

/* Initialize the struct secure_conn data */
void secure_conn_struct_init(struct secure_conn *nw_security);

/** @defgroup net_ops_group Abstraction of Network Services on a platform
    Services to enable the MQTT Client-Server communication over network
   
    These services are invoked by the MQTT Library.
    
    @{
*/ 
struct device_net_services {

       /** @defgroup dev_netconn_opt_grp Options for platform to configure network
           @{
        */
#define DEV_NETCONN_OPT_TCP  0x01  /**< Assert to indicate TCP net connection  */
#define DEV_NETCONN_OPT_UDP  0x02  /**< Assert to create a local UDP port bind */
#define DEV_NETCONN_OPT_IP6  0x04  /**< Assert for IPv6, otherwise it is IPv4  */
#define DEV_NETCONN_OPT_URL  0x08  /**< Assert if the network address is a URL */
#define DEV_NETCONN_OPT_SEC  0x10  /**< Assert to indicate a secure connection */
        /** @} */

        /** Set up a communication channel with a server or set up a local port.
            This routine opens up either a "connection oriented" communication
            channel with the specified server or set up a local configuration for
            "connectionless" transactions.

            @param[in] nwconn_opts Implementation specific construct to enumerate
            server address and / or connection related details
            @param[in] server_addr URL or IP address (string) or other server
            reference. For setting up a local (UDP) port, set it to NULL.
            @param[in] port_number Network port number, typically, 1883 or 8883
            for remote severs. For setting up a local (UDP) port, use an intended
            port number.
            @param[in] nw_security Information to establish a secure connection
            with server. Set it to NULL, if not used. @ref mqtt_netsec_grp
            @return a valid handle to connection, otherwise NULL
        */
        i32 (*open)(u32 nwconn_opts, const c8 *server_addr, u16 port_number,
                    const struct secure_conn *nw_security);

        /** Send data onto the "connection oriented" network.
            The routine blocks till the time, the data has been copied into the
            network stack for dispatch on to the "connection oriented" network.

            @param[in] comm handle to network connection as returned by open().
            @param[in] buf refers to the data that is intended to be sent
            @param[in] len length of the data
            @param[in] ctx reference to the MQTT connection context
            @return on success, the number of bytes sent, 0 on connection reset,
            otherwise -1
        */
        i32   (*send)(i32 comm, const u8 *buf, u32 len, void *ctx);

        /** Receive data from the "connection oriented" channel.
            The routine blocks till the time, there is either a data that has
            been received from the server or the time to await data from the
            server has expired.

            @param[in] comm Handle to network connection as returned by
            accept().
            @param[out] buf place-holder to which data from network should be
            written into.
            @param[in] len maximum length of 'buf'
            @param[in] wait_secs maximum time to await data from network. If
            exceeded, the routine returns error with the err_timeo flag set
            as true.
            @param[out] err_timeo if set, indicates that error is due to
            timeout.
            @param[in] ctx reference to the MQTT connection context
            @return on success, number of bytes received, 0 on connection reset,
            otherwise -1 on error. In case, error (-1) is due to the time-out,
            then the implementation should set flag err_timeo as true.
        */
        i32   (*recv)(i32 comm, u8 *buf, u32 len, u32 wait_secs,
                      bool *err_timeo, void *ctx);

        /** Send data to particular port on the specified network element.
            The routine blocks till the time, the data has been copied into the
            network stack for dispatch to the "specified" network entity.

            @param[in] comm handle to network connection as returned by open().
            @param[in] buf refers to data that is intended to be sent
            @param[in] len length of the data
            @param[in] dest_port network port to which data is to be sent.
            @param[in] dest_ip IP address of the entity to which data is to be
            sent.
            @param[in] ip_len length of the destination IP address.
            @return on success, the number of bytes sent, 0 on connection reset,
            otherwise -1.
        */
        i32   (*send_dest)(i32 comm, const u8 *buf, u32 len, u16 dest_port,
                           const u8 *dest_ip, u32 ip_len);

        /** Receive data on a local port sent by any network element.
            The routine blocks till the time, data has been received on the local
            port from any remote network element.

            @param[in] comm handle to network connection as return by open().
            @param[in] buf place-holder to which data from network should be
            written into.
            @param[in] len maximum lengh of 'buf'
            @param[out] from_port place-holder for the port of the sender network
            entity
            @param[out] from_ip place-holder to retrieve the IP address of the
            sender network entity. The memory space must be provisioned to store
            atleast 16 bytes.
            @param[in, out] ip_len length of IP address. It is provided by
            the caller to declare the length of the place holder and updated by
            routine to indicate the length of the remote network entity's IP
            address.
            @return on success, number of bytes received, 0 on connection reset,
            otherwise -1 on errir. 
        */
        i32   (*recv_from)(i32 comm, u8 *buf, u32 len, u16 *from_port,
                           u8 *from_ip, u32 *ip_len);

        /** Close communication connection */
        i32   (*close)(i32 comm);

        /** Listen to incoming connection from clients.
            This routine prepares the system to listen on the specified port
            for the incoming network connections from the remote clients.

            @param[in] nwconn_opts Implementation specific construct to
            enumerate server address and / or connection related details
            @param[in] port_number Network port number, typically, 1883 or 8883
            @param[in] nw_security Information to establish a secure connection
            with client. Set it to NULL, if not used. @ref mqtt_netsec_grp
            @return a valid handle to listening contruct, otherwise NULL
        */
        i32 (*listen)(u32 nwconn_opts, u16 port_number,
                      const struct secure_conn *nw_security);

        /** Accept an incominng connection.
            This routine creates a new communication channel for the (remote)
            requesting client.

            @param[in] listen handle to listen for the incoming connection
            requests from the remote clients
            @param[out] client_ip IP address of the connected client. This value
            is valid only on successful return of the routine. The place holder
            must provide memory to store atleast 16 bytes.
            @param[in, out] ip_length Length of IP address. It is provided by
            the caller to declare the length of the place holder and updated by
            routine to indicate the length of the connected client's IP address.
            @return on success, a valid handle to the new connection, otherwise
            NULL
        */
        i32 (*accept)(i32 listen, u8 *client_ip, u32 *ip_length);

        /** Monitor activity on communication handles.
            The routine blocks for the specified period of time to monitor
            activity, if any, on each of the communication handle that has
            been provided in one or more vector sets. At the expiry of the
            wait time, this function must identify the handles, on which,
            acitvities were observed.

            A particular collection of communication handles are packed as
            an array or in a vector and is passed to the routine. A NULL
            handle in the vector indicates the termination of the vector
            and can effectively used to account for the size of the vector.

            Similarly, at end the end of the wait period, the routine must
            provide a vector of the handles for which activity was observed.

            @param[in, out] recv_hvec a vector of handles which must be
            monitored for receive activities. 
            @param[in, out] send_hvec a vector of handles which must be
            monitored for send activities.
            @param[in, out] rsvd_hvec reserved for future use.
            @param[in] wait_secs time to wait and monitor activity on
            communication handles provided in one or more sets. If set 
            to 0, the routine returns immediately.
            @return on success, the total number of handles for which activity
            was observed. This number can be 0, if no activity was observed on
            any of the provided handle in the specified time. Otherwise, -1 on
            error.
        */
        i32   (*io_mon)(i32 *recv_cvec, i32 *send_cvec,
                        i32 *rsvd_cvec,  u32 wait_secs);

        /** Get Time (in seconds).
            Provides a monotonically incrementing value of a time  service in
            unit of seconds. The implementation should ensure that associated
            timer hardware or the clock module remains active through the low
            power states of the system. Such an arrangement ensures that MQTT
            Library is able to track the Keep-Alive time across the cycles of
            low power states. It would be typical of battery operated systems
            to transition to low power states during the period of inactivity
            or otherwise to conserve battery. 

            In the absence of a sustained time reference across the low power
            states, if the system transitions away from the active state, the
            MQTT Library, then may not be able to effectively monitor the Keep
            Alive duration.

            It is the responsbililty of the implementation to manage the roll-
            over problem of the hardware and ensure the integrity of the time
            value is maintained.

           @return time in seconds
        */
        u32   (*time)(void);
};

/** @} */ /* device_net_services */

/* Receive data from the specified network and read into the 'mqp' */
i32 mqp_recv(i32  net,     const struct device_net_services *net_ops,
             struct mqtt_packet *mqp, u32 wait_secs, bool *timed_out,
             void *ctx);

/*-----------------------------------------------------------------------------
 * Data structure for managing the QoS2 PUB RX packets and follow-ups
 *---------------------------------------------------------------------------*/

#define MAX_PUBREL_INFLT 8 /* Must be kept as a value of 2^n */

struct pub_qos2_cq { /* Circular Queue CQ to track QOS2 PUB RX messages */

        u16 id_vec[MAX_PUBREL_INFLT];  /* Vector to store RX Message-IDs */
        u8  n_free;                    /* Num of free elements in vector */
        u8  rd_idx;                    /* Index to Read  next Message-ID */
        u8  wr_idx;                    /* Index to Write next Message-ID */
};

/* Reset the specified Circular Queue (CQ) */
void qos2_pub_cq_reset(struct pub_qos2_cq *cq);

/* Append the message-id into the CQ tail. Return true on success, else false */
bool qos2_pub_cq_logup(struct pub_qos2_cq *cq, u16 msg_id);

/* Remove the message-id from the CQ head. Return true on success, else false */
bool qos2_pub_cq_unlog(struct pub_qos2_cq *cq, u16 msg_id);

/* Is the message-id available in the CQ ? Return true on success, else false */
bool qos2_pub_cq_check(struct pub_qos2_cq *cq, u16 msg_id);

/* Get the count of message-ID(s) availalbe in the CQ */
static inline i32 qos2_pub_cq_count(struct pub_qos2_cq *cq)
{
        return MAX_PUBREL_INFLT - cq->n_free;
}

struct client_ctx {

        void        *usr;  /* Client Usr */
        i32          net;  /* Socket HND */

        u8           remote_ip[16];
        u32          ip_length;

        u32          timeout;
        u16          ka_secs;

        u32          flags;

        struct client_ctx *next;
};

void cl_ctx_reset(struct client_ctx *cl_ctx);
void cl_ctx_timeout_insert(struct client_ctx **head,
                           struct client_ctx *elem);

void cl_ctx_remove(struct client_ctx **head,
                   struct client_ctx *elem);

#define KA_TIMEOUT_NONE 0xffffffff  /* Different than KA SECS = 0 */
void cl_ctx_timeout_update(struct client_ctx *cl_ctx, u32 now_secs);

#endif
