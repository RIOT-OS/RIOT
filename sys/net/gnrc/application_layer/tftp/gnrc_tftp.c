/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup net_gnrc_tftp
 * @{
 *
 * @file
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "net/gnrc/tftp.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/ipv6.h"
#include "random.h"

#define ENABLE_DEBUG                (1)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

static kernel_pid_t _tftp_kernel_pid;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CT_HTONS(x)                 ((uint16_t)((          \
                                                    (((uint16_t)(x)) >> 8) & 0x00FF) |  \
                                                ((((uint16_t)(x)) << 8) & 0xFF00)))
#else
#define CT_HTONS(x)                 ((uint16_t)x)
#endif

#define MIN(a, b)                    ((a) > (b) ? (b) : (a))
#define ARRAY_LEN(x)                (sizeof(x) / sizeof(x[0]))

#define TFTP_TIMEOUT_MSG            0x4000
#define TFTP_STOP_SERVER_MSG        0x4001
#define TFTP_DEFAULT_DATA_SIZE      (GNRC_TFTP_MAX_TRANSFER_UNIT    \
                                     + sizeof(tftp_packet_data_t))

/**
 * @brief TFTP mode help support
 */
#define MODE(mode)                  { # mode, sizeof(# mode) }

typedef struct {
    char *name;
    uint8_t len;
} tftp_opt_t;

/**
 * @brief TFTP opcodes
 * @{
 */
typedef uint16_t tftp_opcodes_t;

#define TO_RRQ          CT_HTONS(1)         /**< Read Request */
#define TO_WRQ          CT_HTONS(2)         /**< Write Request */
#define TO_DATA         CT_HTONS(3)         /**< Data */
#define TO_ACK          CT_HTONS(4)         /**< Acknowledgment */
#define TO_ERROR        CT_HTONS(5)         /**< Error */
#define TO_OACK         CT_HTONS(6)         /**< Option ACK */
/**
 * @}
 */

/**
 * @brief TFTP Error Codes
 * @{
 */
typedef uint16_t tftp_err_codes_t;

#define TE_UN_DEF       CT_HTONS(0)         /**< Not defined, see error message */
#define TE_NO_FILE      CT_HTONS(1)         /**< File not found */
#define TE_ACCESS       CT_HTONS(2)         /**< Access violation */
#define TE_DFULL        CT_HTONS(3)         /**< Disk full or allocation exceeded */
#define TE_ILLOPT       CT_HTONS(4)         /**< Illegal TFTP operation */
#define TE_UNKOWN_ID    CT_HTONS(5)         /**< Unknown transfer ID */
#define TE_EXISTS       CT_HTONS(6)         /**< File already exists */
#define TE_UNKOWN_USR   CT_HTONS(7)         /**< No such user */
/**
 * @}
 */

/* ordered as @see tftp_mode_t */
tftp_opt_t _tftp_modes[] = {
    [TTM_ASCII] = MODE(netascii),
    [TTM_OCTET] = MODE(octet),
    [TTM_MAIL]  = MODE(mail),
};

/**
 * @brief TFTP Options
 */
typedef enum {
    TOPT_BLKSIZE,
    TOPT_TIMEOUT,
    TOPT_TSIZE,
} tftp_options_t;

/* ordered as @see tftp_options_t */
tftp_opt_t _tftp_options[] = {
    [TOPT_BLKSIZE] = MODE(blksize),
    [TOPT_TIMEOUT] = MODE(timeout),
    [TOPT_TSIZE]   = MODE(tsize),
};

/**
 * @brief The TFTP state
 */
typedef enum {
    TS_FAILED      = -1,
    TS_BUSY        = 0,
    TS_FINISHED    = 1
} tftp_state;

/**
 * @brief The type of the context used
 */
typedef enum {
    CT_SERVER,
    CT_CLIENT
} tftp_context_type;

/**
 * @brief The TFTP context for the current transfer
 */
typedef struct {
    char file_name[GNRC_TFTP_MAX_FILENAME_LEN];
    tftp_mode_t mode;
    tftp_opcodes_t op;
    ipv6_addr_t peer;
    xtimer_t timer;
    msg_t timer_msg;
    uint32_t timeout;
    uint16_t dst_port;
    uint16_t src_port;
    tftp_context_type ct;
    tftp_start_cb_t start_cb;
    tftp_data_cb_t data_cb;
    tftp_stop_cb_t stop_cb;
    gnrc_netreg_entry_t entry;

    /* transfer parameters */
    uint16_t block_nr;
    uint16_t block_size;
    size_t transfer_size;
    uint32_t block_timeout;
    uint32_t retries;
    bool use_options;
    bool enable_options;
    bool write_finished;
} tftp_context_t;

/**
 * @brief The default TFTP header
 */
typedef struct __attribute__((packed)) {
    tftp_opcodes_t opc;
    uint8_t data[];
} tftp_header_t;

/**
 * @brief The TFTP data packet
 */
typedef struct __attribute__((packed)) {
    tftp_opcodes_t opc;
    network_uint16_t block_nr;
    uint8_t data[];
} tftp_packet_data_t;

/**
 * @brief The TFTP error packet
 */
typedef struct __attribute__((packed)) {
    tftp_opcodes_t opc;
    tftp_err_codes_t err_code;
    char err_msg[];
} tftp_packet_error_t;

/* get the TFTP opcode */
static inline tftp_opcodes_t _tftp_parse_type(uint8_t *buf)
{
    return ((tftp_header_t *)buf)->opc;
}

/* initialize the context to it's default state */
static int _tftp_init_ctxt(ipv6_addr_t *addr, const char *file_name,
                           tftp_opcodes_t op, tftp_mode_t mode, tftp_context_type type,
                           tftp_start_cb_t start, tftp_stop_cb_t stop,
                           tftp_data_cb_t data, bool enable_options, tftp_context_t *ctxt);

/* set the default TFTP options */
static void _tftp_set_default_options(tftp_context_t *ctxt);

/* set the TFTP options to use */
static int _tftp_set_opts(tftp_context_t *ctxt, size_t blksize, uint32_t timeout, size_t total_size);

/* this function registers the UDP port and won't return till the TFTP transfer is finished */
static int _tftp_do_client_transfer(tftp_context_t *ctxt);

/* the state process of the TFTP transfer */
static tftp_state _tftp_state_processes(tftp_context_t *ctxt, msg_t *m);

/* send an start request if we run in client mode */
static tftp_state _tftp_send_start(tftp_context_t *ctxt, gnrc_pktsnip_t *buf);

/* send data or and ack depending if we are reading or writing */
static tftp_state _tftp_send_dack(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, tftp_opcodes_t op);

/* send and TFTP error to the client */
static tftp_state _tftp_send_error(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, tftp_err_codes_t err, const char *err_msg);

/* this function sends the actual packet */
static tftp_state _tftp_send(gnrc_pktsnip_t *buf, tftp_context_t *ctxt, size_t len);

/* decode the default TFTP start packet */
static int _tftp_decode_start(tftp_context_t *ctxt, uint8_t *buf, gnrc_pktsnip_t *outbuf);

/* decode the TFTP option extensions */
static int _tftp_decode_options(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, uint32_t start);

/* decode the received ACK packet */
static bool _tftp_validate_ack(tftp_context_t *ctxt, uint8_t *buf);

/* processes the received data packet and calls the callback defined by the user */
static int _tftp_process_data(tftp_context_t *ctxt, gnrc_pktsnip_t *buf);

/* decode the received error packet and calls the callback defined by the user */
static int _tftp_decode_error(uint8_t *buf, tftp_err_codes_t *err, const char * *err_msg);

/* TFTP super loop server */
static int _tftp_server(tftp_context_t *ctxt);

/* get the maximum allowed transfer unit to avoid 6Lo fragmentation */
static uint16_t _tftp_get_maximum_block_size(void)
{
    uint16_t tmp;
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t ifnum = gnrc_netif_get(ifs);

    if (ifnum > 0 && gnrc_netapi_get(ifs[0], NETOPT_MAX_PACKET_SIZE, 0, &tmp, sizeof(uint16_t)) >= 0) {
        /* TODO calculate proper block size */
        return tmp - sizeof(udp_hdr_t) - sizeof(ipv6_hdr_t) - 10;
    }

    return GNRC_TFTP_MAX_TRANSFER_UNIT;
}

int gnrc_tftp_client_read(ipv6_addr_t *addr, const char *file_name, tftp_mode_t mode,
                          tftp_data_cb_t data_cb, tftp_start_cb_t start_cb, tftp_stop_cb_t stop_cb, bool use_option_extensions)
{
    tftp_context_t ctxt;

    assert(data_cb);
    assert(start_cb);
    assert(stop_cb);

    /* prepare the context */
    if (_tftp_init_ctxt(addr, file_name, TO_RRQ, mode, CT_CLIENT, start_cb, stop_cb, data_cb, use_option_extensions, &ctxt) != TS_FINISHED) {
        return -EINVAL;
    }

    /* set the transfer options */
    uint16_t mtu = _tftp_get_maximum_block_size();
    if (!use_option_extensions ||
        _tftp_set_opts(&ctxt, mtu, GNRC_TFTP_DEFAULT_TIMEOUT, 0) != TS_FINISHED) {
        _tftp_set_default_options(&ctxt);

        if (use_option_extensions) {
            return -EINVAL;
        }
    }

    /* start the process */
    int ret = _tftp_do_client_transfer(&ctxt);

    /* remove possibly stale timer */
    xtimer_remove(&(ctxt.timer));

    return ret;
}

int gnrc_tftp_client_write(ipv6_addr_t *addr, const char *file_name, tftp_mode_t mode,
                           tftp_data_cb_t data_cb, size_t total_size, tftp_stop_cb_t stop_cb, bool use_option_extensions)
{
    tftp_context_t ctxt;

    assert(data_cb);
    assert(stop_cb);

    /* prepare the context */
    if (_tftp_init_ctxt(addr, file_name, TO_WRQ, mode, CT_CLIENT, NULL, stop_cb, data_cb, use_option_extensions, &ctxt) < 0) {
        return -EINVAL;
    }

    /* set the transfer options */
    uint16_t mtu = _tftp_get_maximum_block_size();
    if (!use_option_extensions ||
        _tftp_set_opts(&ctxt, mtu, GNRC_TFTP_DEFAULT_TIMEOUT, total_size) != TS_FINISHED) {

        _tftp_set_default_options(&ctxt);

        if (use_option_extensions) {
            return -EINVAL;
        }
    }

    /* start the process */
    int ret = _tftp_do_client_transfer(&ctxt);

    /* remove possibly stale timer */
    xtimer_remove(&(ctxt.timer));

    return ret;
}

int _tftp_init_ctxt(ipv6_addr_t *addr, const char *file_name,
                    tftp_opcodes_t op, tftp_mode_t mode, tftp_context_type type,
                    tftp_start_cb_t start, tftp_stop_cb_t stop,
                    tftp_data_cb_t data, bool enable_options, tftp_context_t *ctxt)
{

    if (!addr) {
        return TS_FAILED;
    }

    memset(ctxt, 0, sizeof(*ctxt));

    /* set the default context parameters */
    ctxt->op = op;
    ctxt->ct = type;
    ctxt->data_cb = data;
    ctxt->start_cb = start;
    ctxt->stop_cb = stop;
    memcpy(&(ctxt->peer), addr, sizeof(ctxt->peer));
    ctxt->mode = mode;
    if (file_name) {
        strncpy(ctxt->file_name, file_name, GNRC_TFTP_MAX_FILENAME_LEN);
    }
    ctxt->file_name[GNRC_TFTP_MAX_FILENAME_LEN - 1] = 0;
    ctxt->dst_port = GNRC_TFTP_DEFAULT_DST_PORT;
    ctxt->enable_options = enable_options;

    /* transport layer parameters */
    ctxt->block_size = GNRC_TFTP_MAX_TRANSFER_UNIT;
    ctxt->block_timeout = GNRC_TFTP_DEFAULT_TIMEOUT;
    ctxt->write_finished = false;

    /* generate a random source UDP source port */
    do {
        ctxt->src_port = (random_uint32() & 0xff) + GNRC_TFTP_DEFAULT_SRC_PORT;
    } while (gnrc_netreg_num(GNRC_NETTYPE_UDP, ctxt->src_port));

    return TS_FINISHED;
}

void _tftp_set_default_options(tftp_context_t *ctxt)
{
    ctxt->block_size = GNRC_TFTP_MAX_TRANSFER_UNIT;
    ctxt->timeout = GNRC_TFTP_DEFAULT_TIMEOUT;
    ctxt->block_timeout = GNRC_TFTP_DEFAULT_TIMEOUT;
    ctxt->transfer_size = 0;
    ctxt->use_options = false;
}

int _tftp_set_opts(tftp_context_t *ctxt, size_t blksize, uint32_t timeout, size_t total_size)
{
    if (blksize > GNRC_TFTP_MAX_TRANSFER_UNIT || !timeout) {
        return TS_FAILED;
    }

    ctxt->block_size = blksize;
    ctxt->timeout = timeout;
    ctxt->block_timeout = timeout;
    ctxt->transfer_size = total_size;
    ctxt->use_options = true;

    return TS_FINISHED;
}

int gnrc_tftp_server(tftp_data_cb_t data_cb, tftp_start_cb_t start_cb, tftp_stop_cb_t stop_cb, bool use_options)
{
    /* check if there is only one TFTP server running */
    if (_tftp_kernel_pid != KERNEL_PID_UNDEF) {
        DEBUG("tftp: only one TFTP server allowed\n");
        return -1;
    }

    /* context will be initialized when a connection is established */
    tftp_context_t ctxt;
    ctxt.data_cb = data_cb;
    ctxt.start_cb = start_cb;
    ctxt.stop_cb = stop_cb;
    ctxt.enable_options = use_options;

    /* validate our arguments */
    assert(data_cb);
    assert(start_cb);
    assert(stop_cb);

    /* save our kernel PID */
    _tftp_kernel_pid = thread_getpid();

    /* start the server */
    int ret = _tftp_server(&ctxt);

    /* remove possibly stale timer */
    xtimer_remove(&(ctxt.timer));

    /* reset the kernel PID */
    _tftp_kernel_pid = KERNEL_PID_UNDEF;

    return ret;
}

int gnrc_tftp_server_stop(void)
{
    /* check if there is a server running */
    if (_tftp_kernel_pid == KERNEL_PID_UNDEF) {
        DEBUG("tftp: no TFTP server running\n");
        return -1;
    }

    /* prepare the stop message */
    msg_t m = {
        thread_getpid(),
        TFTP_STOP_SERVER_MSG,
        { NULL }
    };

    /* send the stop message */
    msg_send(&m, _tftp_kernel_pid);

    return 0;
}

int _tftp_server(tftp_context_t *ctxt)
{
    msg_t msg;
    bool active = true;
    gnrc_netreg_entry_t entry = { NULL, GNRC_TFTP_DEFAULT_DST_PORT,
                                  thread_getpid() };

    while (active) {
        int ret = TS_BUSY;
        bool got_client = false;

        /* register the servers main listening port */
        if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &entry)) {
            DEBUG("tftp: error starting server.");
            return TS_FAILED;
        }

        /* main processing loop */
        while (ret == TS_BUSY) {
            /* wait for a message */
            msg_receive(&msg);

            /* check if the server stop message has been received */
            if (msg.type == TFTP_STOP_SERVER_MSG) {
                active = false;
                ret = TS_FAILED;
                break;
            }
            else {
                /* continue normal server opration */
                DEBUG("tftp: message incoming\n");
                ret = _tftp_state_processes(ctxt, &msg);

                /* release packet if we received one */
                if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
                    gnrc_pktbuf_release((gnrc_pktsnip_t *) msg.content.ptr);
                }
            }

            /* if we just accepted a client, disable the server main listening port */
            if (!got_client) {
                gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &entry);

                if (ret == TS_BUSY) {
                    got_client = true;
                    DEBUG("tftp: connection established\n");
                }
            }
        }

        /* remove any stall timers */
        xtimer_remove(&(ctxt->timer));

        /* if the server transfer has finished, unregister the client dst port */
        gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &(ctxt->entry));
        DEBUG("tftp: connection terminated\n");
    }

    /* unregister our UDP listener on this thread */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &entry);

    return 0;
}

int _tftp_do_client_transfer(tftp_context_t *ctxt)
{
    msg_t msg;
    tftp_state ret = TS_BUSY;

    /* register our DNS response listener */
    gnrc_netreg_entry_t entry = { NULL, ctxt->src_port, thread_getpid() };

    if (gnrc_netreg_register(GNRC_NETTYPE_UDP, &entry)) {
        DEBUG("tftp: error starting server.");
        return TS_FAILED;
    }

    /* try to start the TFTP transfer */
    ret = _tftp_state_processes(ctxt, NULL);
    if (ret != TS_BUSY) {
        DEBUG("tftp: transfer failed\n");
        /* if the start failed return */
        return ret;
    }

    /* main processing loop */
    while (ret == TS_BUSY) {
        /* wait for a message */
        msg_receive(&msg);
        DEBUG("tftp: message received\n");
        ret = _tftp_state_processes(ctxt, &msg);

        /* release packet if we received one */
        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            gnrc_pktbuf_release((gnrc_pktsnip_t *) msg.content.ptr);
        }
    }

    /* unregister our UDP listener on this thread */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &entry);

    return ret;
}

tftp_state _tftp_state_processes(tftp_context_t *ctxt, msg_t *m)
{
    gnrc_pktsnip_t *outbuf = gnrc_pktbuf_add(NULL, NULL, TFTP_DEFAULT_DATA_SIZE,
                                             GNRC_NETTYPE_UNDEF);

    /* check if this is an client start */
    if (!m) {
        DEBUG("tftp: starting transaction as client\n");
        return _tftp_send_start(ctxt, outbuf);
    }
    else if (m->type == TFTP_TIMEOUT_MSG) {
        DEBUG("tftp: timeout occured\n");
        if (++(ctxt->retries) > GNRC_TFTP_MAX_RETRIES) {
            /* transfer failed due to lost peer */
            DEBUG("tftp: peer lost\n");
            gnrc_pktbuf_release(outbuf);
            return TS_FAILED;
        }

        /* increase the timeout for congestion control */
        ctxt->block_timeout <<= 1;

        /* the send message timed out, re-sending */
        if (ctxt->dst_port == GNRC_TFTP_DEFAULT_DST_PORT) {
            DEBUG("tftp: sending timed out, re-sending\n");
            /* we are still negotiating resent, start */
            return _tftp_send_start(ctxt, outbuf);
        }
        else {
            DEBUG("tftp: last data or ack packet lost, resending\n");
            /* we are sending / receiving data */
            /* if we are reading resent the ACK, if writing the DATA */
            return _tftp_send_dack(ctxt, outbuf, (ctxt->op == TO_RRQ) ? TO_ACK : TO_DATA);
        }
    }
    else if (m->type != GNRC_NETAPI_MSG_TYPE_RCV) {
        DEBUG("tftp: unknown message");
        gnrc_pktbuf_release(outbuf);
        return TS_BUSY;
    }

    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *)(m->content.ptr);

    gnrc_pktsnip_t *tmp;
    tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
    udp_hdr_t *udp = (udp_hdr_t *)tmp->data;

    tmp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    ipv6_hdr_t *ip = (ipv6_hdr_t *)tmp->data;
    uint8_t *data = (uint8_t *)pkt->data;

    xtimer_remove(&(ctxt->timer));

    switch (_tftp_parse_type(data)) {
        case TO_RRQ:
        case TO_WRQ: {
            if (byteorder_ntohs(udp->dst_port) != GNRC_TFTP_DEFAULT_DST_PORT) {
                /* not a valid start packet */
                DEBUG("tftp: incoming packet on port %d dropped\n", byteorder_ntohs(udp->dst_port));
                gnrc_pktbuf_release(outbuf);
                return TS_FAILED;
            }

            /* reinitialize the context with the current client */
            tftp_opcodes_t op = _tftp_parse_type(data);
            _tftp_init_ctxt(&(ip->src), NULL, op, TTM_ASCII, CT_SERVER,
                            ctxt->start_cb, ctxt->stop_cb, ctxt->data_cb,
                            ctxt->enable_options, ctxt);

            /* get the context of the client */
            ctxt->dst_port = byteorder_ntohs(udp->src_port);
            DEBUG("tftp: client's port is %" PRIu16 "\n", ctxt->dst_port);

            int offset = _tftp_decode_start(ctxt, data, outbuf);
            DEBUG("tftp: offset after decode start = %i\n", offset);
            if (offset < 0) {
                DEBUG("tftp: there is no data?\n");
                gnrc_pktbuf_release(outbuf);
                return TS_FAILED;
            }

            /* register a listener for the UDP port */
            ctxt->entry.next = NULL;
            ctxt->entry.demux_ctx = ctxt->src_port;
            ctxt->entry.pid = thread_getpid();
            gnrc_netreg_register(GNRC_NETTYPE_UDP, &(ctxt->entry));

            /* try to decode the options */
            tftp_state state;
            tftp_opcodes_t opcode;
            if (ctxt->enable_options &&
                _tftp_decode_options(ctxt, pkt, offset) > offset) {
                DEBUG("tftp: send option ACK\n");

                /* the client send the TFTP options */
                opcode = TO_OACK;
            }
            else {
                DEBUG("tftp: send normal ACK\n");

                /* the client didn't send options, use ACK and set defaults */
                _tftp_set_default_options(ctxt);

                /* send the first data block */
                if (ctxt->op == TO_RRQ) {
                    ++(ctxt->block_nr);
                    opcode = TO_DATA;
                }
                else {
                    opcode = TO_ACK;
                }
            }

            /* validate if the application accepts the action, mode, filename and transfer_size */
            tftp_action_t action = (ctxt->op == TO_RRQ) ? TFTP_READ : TFTP_WRITE;
            if (!ctxt->start_cb(action, ctxt->mode, ctxt->file_name, &(ctxt->transfer_size))) {
                _tftp_send_error(ctxt, outbuf, TE_ACCESS, "Blocked by user application");
                DEBUG("tftp: callback not able to handle mode\n");
                return TS_FAILED;
            }

            /* the client send the TFTP options */
            state = _tftp_send_dack(ctxt, outbuf, opcode);

            /* check if the client negotiation was successful */
            if (state != TS_BUSY) {
                DEBUG("tftp: not able to send ACK");
            }
            return state;
        } break;

        case TO_DATA: {
            /* try to process the data */
            int proc = _tftp_process_data(ctxt, pkt);
            if (proc < 0) {
                DEBUG("tftp: data not accepted\n");
                /* the data is not accepted return */
                gnrc_pktbuf_release(outbuf);
                return TS_BUSY;
            }

            /* check if this is the first block */
            if (!ctxt->block_nr
                && ctxt->dst_port == GNRC_TFTP_DEFAULT_DST_PORT
                && !ctxt->use_options) {
                /* no OACK received, restore default TFTP parameters */
                _tftp_set_default_options(ctxt);
                DEBUG("tftp: restore default TFTP parameters\n");

                /* switch the destination port to the src port of the server */
                ctxt->dst_port = byteorder_ntohs(udp->src_port);
            }

            /* wait for the next data block */
            DEBUG("tftp: wait for the next data block\n");
            ++(ctxt->block_nr);
            _tftp_send_dack(ctxt, outbuf, TO_ACK);

            /* check if the data transfer has finished */
            if (proc < ctxt->block_size) {
                DEBUG("tftp: transfer finished\n");

                if (ctxt->stop_cb) {
                    ctxt->stop_cb(TFTP_SUCCESS, NULL);
                }

                return TS_FINISHED;
            }

            return TS_BUSY;
        } break;

        case TO_ACK: {
            /* validate if this is the ACK we are waiting for */
            if (!_tftp_validate_ack(ctxt, data)) {
                /* invalid packet ACK, drop */
                gnrc_pktbuf_release(outbuf);
                return TS_BUSY;
            }

            /* check if the write action is finished */
            if (ctxt->write_finished) {
                gnrc_pktbuf_release(outbuf);

                if (ctxt->stop_cb) {
                    ctxt->stop_cb(TFTP_SUCCESS, NULL);
                }

                return TS_FINISHED;
            }

            /* check if this is the first ACK */
            if (!ctxt->block_nr && ctxt->dst_port != byteorder_ntohs(udp->src_port)) {
                /* no OACK received restore default TFTP parameters */
                _tftp_set_default_options(ctxt);

                /* switch the destination port to the src port of the server */
                ctxt->dst_port = byteorder_ntohs(udp->src_port);
            }

            /* send the next data block */
            ++(ctxt->block_nr);

            return _tftp_send_dack(ctxt, outbuf, TO_DATA);
        } break;

        case TO_ERROR: {
            tftp_err_codes_t err;
            const char *err_msg;

            /* decode the received error */
            _tftp_decode_error(data, &err, &err_msg);

            /* inform the user application about the error */
            if (ctxt->stop_cb) {
                ctxt->stop_cb(TFTP_PEER_ERROR, err_msg);
            }

            DEBUG("tftp: ERROR: %s\n", err_msg);
            gnrc_pktbuf_release(outbuf);
            return TS_FAILED;
        } break;

        case TO_OACK: {
            /* only allow one OACK to be received */
            if (ctxt->dst_port != byteorder_ntohs(udp->src_port)) {
                DEBUG("tftp: TO_OACK received\n");

                /* decode the options */
                _tftp_decode_options(ctxt, pkt, 0);

                /* take the new source port */
                ctxt->dst_port = byteorder_ntohs(udp->src_port);

                /* we must send block one to finish the negotiation in send mode */
                if (ctxt->op == TO_WRQ) {
                    ++(ctxt->block_nr);
                }
            }
            else {
                DEBUG("tftp: dropping double TO_OACK\n");
            }

            return _tftp_send_dack(ctxt, outbuf, (ctxt->op == TO_WRQ) ? TO_DATA : TO_ACK);
        } break;
    }

    gnrc_pktbuf_release(outbuf);
    return TS_FAILED;
}

size_t _tftp_add_option(uint8_t *dst, tftp_opt_t *opt, uint32_t value)
{
    size_t offset;

    /* set the option name */
    memcpy(dst, opt->name, opt->len);
    offset = opt->len;

    /* set the option value */
    offset += sprintf((char *)(dst + opt->len), "%" PRIu32, value);

    /* finish option value */
    *(dst + offset) = 0;
    return ++offset;
}

uint32_t _tftp_append_options(tftp_context_t *ctxt, tftp_header_t *hdr, uint32_t offset)
{
    offset += _tftp_add_option(hdr->data + offset, _tftp_options + TOPT_BLKSIZE, ctxt->block_size);
    offset += _tftp_add_option(hdr->data + offset, _tftp_options + TOPT_TIMEOUT, (ctxt->timeout / SEC_IN_USEC));

    /**
     * Only set the transfer option if we are sending.
     * Or when we are reading in bin mode.
     */
    if ((ctxt->ct == CT_SERVER && ctxt->op == TO_RRQ) ||
        (ctxt->ct == CT_CLIENT && ctxt->op == TO_WRQ) ||
        ctxt->mode == TTM_OCTET) {
        offset += _tftp_add_option(hdr->data + offset, _tftp_options + TOPT_TSIZE, ctxt->transfer_size);
    }

    return offset;
}

tftp_state _tftp_send_start(tftp_context_t *ctxt, gnrc_pktsnip_t *buf)
{
    /* get required values */
    int len = strlen(ctxt->file_name) + 1;          /* we also want the \0 char */
    tftp_opt_t *m = _tftp_modes + ctxt->mode;

    /* start filling the header */
    tftp_header_t *hdr = (tftp_header_t *)(buf->data);

    hdr->opc = ctxt->op;
    memcpy(hdr->data, ctxt->file_name, len);
    memcpy(hdr->data + len, m->name, m->len);

    /* fill the options */
    uint32_t offset = (len + m->len);
    if (ctxt->use_options) {
        offset = _tftp_append_options(ctxt, hdr, offset);
    }

    /* send the data */
    return _tftp_send(buf, ctxt, offset + sizeof(tftp_header_t));
}

tftp_state _tftp_send_dack(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, tftp_opcodes_t op)
{
    size_t len = 0;

    assert(op == TO_DATA || op == TO_ACK || op == TO_OACK);

    /* fill the packet */
    tftp_packet_data_t *pkt = (tftp_packet_data_t *)(buf->data);
    pkt->block_nr = byteorder_htons(ctxt->block_nr);
    pkt->opc = op;

    if (op == TO_DATA) {
        DEBUG("tftp: getting data from callback\n");
        /* get the required data from the user */
        len = ctxt->data_cb(ctxt->block_size * (ctxt->block_nr - 1), pkt->data, ctxt->block_size);

        /* check if we are finished on ACK receive */
        ctxt->write_finished = (len < ctxt->block_size);

        /* enable timeout */
        ctxt->block_timeout = ctxt->timeout;
    }
    else if (op == TO_OACK) {
        /* append the options */
        len = _tftp_append_options(ctxt, (tftp_header_t *)pkt, 0);

        /* disable timeout*/
        ctxt->block_timeout = 0;
    }
    else if (op == TO_ACK) {
        /* disable timeout*/
        ctxt->block_timeout = 0;
    }

    /* send the data */
    return _tftp_send(buf, ctxt, sizeof(tftp_packet_data_t) + len);
}

tftp_state _tftp_send_error(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, tftp_err_codes_t err, const char *err_msg)
{
    int strl = err_msg
               ? strlen(err_msg) + 1
               : 0;

    (void) ctxt;

    /* fill the packet */
    tftp_packet_error_t *pkt = (tftp_packet_error_t *)(buf->data);
    pkt->opc = TO_ERROR;
    pkt->err_code = err;
    memcpy(pkt->err_msg, err_msg, strl);

    /* don't set a timeout on the error */
    ctxt->block_timeout = 0;

    /* return the size of the packet */
    _tftp_send(buf, ctxt, sizeof(tftp_packet_error_t) + strl);

    return TS_FAILED;
}

tftp_state _tftp_send(gnrc_pktsnip_t *buf, tftp_context_t *ctxt, size_t len)
{
    network_uint16_t src_port, dst_port;
    gnrc_pktsnip_t *udp, *ip;

    assert(len <= TFTP_DEFAULT_DATA_SIZE);

    /* down-size the packet to it's used size */
    if (len > TFTP_DEFAULT_DATA_SIZE) {
        DEBUG("tftp: can't reallocate to bigger packet, buffer overflowed");
        gnrc_pktbuf_release(buf);

        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "buffer overflowed");
        }

        return TS_FAILED;
    }
    else if (gnrc_pktbuf_realloc_data(buf, len) != 0) {
        assert(false);

        DEBUG("tftp: failed to reallocate data snippet");
        gnrc_pktbuf_release(buf);

        /* inform the user that we can't reallocate */
        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "no reallocate");
        }

        return TS_FAILED;
    }

    /* allocate UDP header, set source port := destination port */
    src_port.u16 = ctxt->src_port;
    dst_port.u16 = ctxt->dst_port;
    udp = gnrc_udp_hdr_build(buf, src_port.u16, dst_port.u16);
    if (udp == NULL) {
        DEBUG("tftp: error unable to allocate UDP header");
        gnrc_pktbuf_release(buf);

        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "no udp allocate");
        }

        return TS_FAILED;
    }

    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &(ctxt->peer));
    if (ip == NULL) {
        DEBUG("tftp: error unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);

        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "no ip allocate");
        }

        return TS_FAILED;
    }

    /* send packet */
    if (gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL,
                                  ip) == 0) {
        /* if send failed inform the user */
        DEBUG("tftp: error unable to locate UDP thread");
        gnrc_pktbuf_release(ip);

        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "no dispatch send");
        }

        return TS_FAILED;
    }

    /* only set timeout if enabled for this block */
    if (ctxt->block_timeout) {
        ctxt->timer_msg.type = TFTP_TIMEOUT_MSG;
        xtimer_set_msg(&(ctxt->timer), ctxt->block_timeout, &(ctxt->timer_msg), thread_getpid());
        DEBUG("tftp: set timeout %" PRIu32 " ms\n", ctxt->block_timeout / MS_IN_USEC);
    }

    return TS_BUSY;
}

bool _tftp_validate_ack(tftp_context_t *ctxt, uint8_t *buf)
{
    tftp_packet_data_t *pkt = (tftp_packet_data_t *) buf;

    return ctxt->block_nr == byteorder_ntohs(pkt->block_nr);
}

int _tftp_decode_start(tftp_context_t *ctxt, uint8_t *buf, gnrc_pktsnip_t *outbuf)
{
    /* decode the packet */
    tftp_header_t *hdr = (tftp_header_t *)buf;

    /* get the file name and copy terminating byte */
    size_t fnlen = strlen((char *)hdr->data) + 1;

    if (fnlen >= GNRC_TFTP_MAX_FILENAME_LEN) {
        _tftp_send_error(ctxt, outbuf, TE_ILLOPT, "Filename to long");

        if (ctxt->stop_cb) {
            ctxt->stop_cb(TFTP_INTERN_ERROR, "Filename to long");
        }

        return TS_FAILED;
    }
    memcpy(ctxt->file_name, hdr->data, fnlen);

    /* Get mode string by advancing pointer */
    char *str_mode = (char *)hdr->data + fnlen;
    DEBUG("tftp: incoming request '%s', mode: %s\n", ctxt->file_name, str_mode);

    /* decode the TFTP transfer mode */
    for (uint32_t idx = 0; idx < ARRAY_LEN(_tftp_modes); ++idx) {
        if (memcmp(_tftp_modes[idx].name, str_mode, _tftp_modes[idx].len) == 0) {
            ctxt->mode = (tftp_mode_t)idx;
            return (str_mode + _tftp_modes[idx].len) - (char *)hdr->data;
        }
    }

    return -EINVAL;
}

int _tftp_decode_options(tftp_context_t *ctxt, gnrc_pktsnip_t *buf, uint32_t start)
{
    tftp_header_t *pkt = (tftp_header_t *)buf->data;
    size_t offset = start;

    DEBUG("tftp: decode options\n");
    DEBUG("tftp:   buffer size = %lu\n", (unsigned long)buf->size);
    while ((offset + sizeof(uint16_t)) < (buf->size)) {
        DEBUG("tftp:   offset = %lu\n", (unsigned long)offset);
        /* get the option name */
        const char *name = (const char *)(pkt->data + offset);
        offset += strlen(name) + 1;
        /* get the value name */
        const char *value = (const char *)(pkt->data + offset);
        offset += strlen(value) + 1;

        /* check what option we are parsing */
        for (uint32_t idx = 0; idx < ARRAY_LEN(_tftp_options); ++idx) {
            if (memcmp(name, _tftp_options[idx].name, _tftp_options[idx].len) == 0) {
                /* set the option value of the known options */
                switch (idx) {
                    case TOPT_BLKSIZE:
                        ctxt->block_size = atoi(value);
                        DEBUG("tftp: got option TOPT_BLKSIZE = %" PRIu16 "\n", ctxt->block_size);
                        break;

                    case TOPT_TSIZE:
                        ctxt->transfer_size = atoi(value);
                        DEBUG("tftp: got option TOPT_TSIZE = %" PRIu32 "\n", (uint32_t)ctxt->transfer_size);

                        if (ctxt->start_cb && ctxt->ct == CT_CLIENT) {
                            ctxt->start_cb(TFTP_READ, ctxt->mode, ctxt->file_name, &(ctxt->transfer_size));
                        }
                        break;

                    case TOPT_TIMEOUT:
                        ctxt->timeout = atoi(value) * SEC_IN_USEC;
                        DEBUG("tftp: option TOPT_TIMEOUT = %" PRIu32 " ms\n", ctxt->timeout / MS_IN_USEC);
                        break;
                }

                break;
            }
        }
    }

    DEBUG("tftp:   return %lu\n", (unsigned long)offset);
    return offset;
}

int _tftp_process_data(tftp_context_t *ctxt, gnrc_pktsnip_t *buf)
{
    tftp_packet_data_t *pkt = (tftp_packet_data_t *) buf->data;

    DEBUG("tftp: processing data\n");

    uint16_t block_nr = byteorder_ntohs(pkt->block_nr);

    /* check if this is the packet we are waiting for */
    if (block_nr != (ctxt->block_nr + 1)) {
        DEBUG("tftp: not the packet we were wating for\n");
        return TS_BUSY;
    }

    /* send the user data trough to the user application */
    if (ctxt->data_cb(ctxt->block_nr * ctxt->block_size, pkt->data, buf->size - sizeof(tftp_packet_data_t)) < 0) {
        DEBUG("tftp: error in data callback\n");
        return TS_BUSY;
    }

    /* return the number of data bytes received */
    return buf->size - sizeof(tftp_packet_data_t);
}

int _tftp_decode_error(uint8_t *buf, tftp_err_codes_t *err, const char * *err_msg)
{
    tftp_packet_error_t *pkt = (tftp_packet_error_t *) buf;

    /* return the error code and message */
    *err = pkt->err_code;
    *err_msg = pkt->err_msg;

    return sizeof(tftp_packet_error_t) + strlen(pkt->err_msg) + 1;
}

/**
 * @}
 */
