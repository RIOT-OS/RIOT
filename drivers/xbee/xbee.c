/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_xbee
 * @{
 *
 * @file
 * @brief       High-level driver implementation for the XBee 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 *
 * @}
 */

#include "xbee.h"

#include "crash.h"
#include "mutex.h"

/* lower-level drivers needed to control the XBee module */
#include "periph/uart.h"
#include "periph/gpio.h"

/* hardware-dependent configuration */
#include "xbee-config.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"


typedef enum xbee_at_cmd_response_kind {
    XBEE_AT_CMD_OK = 0,        /* success */
    XBEE_AT_CMD_ERROR,         /* unexpected runtime error */
    XBEE_AT_CMD_INVALID,       /* invalid command */
    XBEE_AT_CMD_BAD_PARAM      /* invalid parameter */
} xbee_at_cmd_response_kind_t;

typedef enum xbee_tx_status {
    XBEE_TX_SUCCESS = 0,       /* success*/
    XBEE_TX_NOACK,             /* no ACK received for the TXed packet */
    XBEE_TX_MEDIUM_BUSY,       /* impossible to have a successful CCA */
    XBEE_TX_ERROR              /* other/unknown error */
} xbee_tx_status_t;

typedef enum xbee_event {
    XBEE_EVENT_HW_RESET = 0,   /* XBee module hardware reset */
    XBEE_EVENT_WDT_RESET,      /* watchdog timer reset */
    /* the following events should normally not be interesting to us */
    XBEE_EVENT_ASSOCIATED,
    XBEE_EVENT_DISASSOCIATED,
    XBEE_EVENT_SYNC_LOST,
    XBEE_EVENT_COORD_REALIGN,
    XBEE_EVENT_COORD_START,
} xbee_status_t;


/* response to an AT command from XBee modem */
typedef struct xbee_at_cmd_response {
    xbee_at_cmd_response_kind_t status;
    uint8_t frame_num;
    uint16_t command;   // 2 chars
    uint32_t parameter;
} xbee_at_cmd_response_t;

/* reported status for a TXed packet */
typedef struct xbee_tx_status_report {
    xbee_tx_status_t status;
    uint8_t frame_num;
} xbee_tx_status_report_t;


/*****************************************************************************/
/*                    Driver's internal utility functions                    */
/*****************************************************************************/

/* callback function to upper layers for RXed packets */
static receive_802154_packet_callback_t rx_callback;

/* next ID for sent frames */
static uint8_t frame_id;

/* AT command response management data */
static struct mutex_t mutex_wait_at_reponse;
static xbee_at_cmd_response_t latest_at_response;

/* TX status response management data */
static struct mutex_t mutex_wait_tx_status;
static xbee_tx_status_report_t latest_tx_report;


/* send a character, via UART, to the XBee module */
static void send_xbee(const uint8_t c)
{
    int res = uart_write_blocking(XBEE_UART_LINK, c);
    if (res < 0) {
        core_panic(0x0bee,
                   "Could not send char to XBee module");
    }
}

/* handle the reception of a data packet from XBee modem */
static void xbee_process_rx_packet(ieee802154_node_addr_t src_addr,
                                   bool use_long_addr,
                                   int8_t rssi,
                                   void *buf,
                                   unsigned int len)
{
    /* nothing more to do than relay to upper layers, if possible */
    if (rx_callback != NULL) {
        /* Since we don't know LQI, we pass just 0;
           packet validity is to be verified by the XBee modem,
           so we assume CRC is always correct */
        rx_callback(buf, len, rssi, 0, true);
    }
}

/* forward declaration of the driver's init function */
void xbee_initialize(void);

/* handle XBee event arrival */
static void xbee_process_modem_status_event(uint8_t status)
{
    switch (status) {
    case XBEE_EVENT_HW_RESET:
    case XBEE_EVENT_WDT_RESET:
        /* mode has resetted: reinit our driver */
        xbee_initialize();
        break;
    default:
        /* other events should not happen, and don't interest us */
        __asm__ ("nop");
    }
}

/* handle AT command response from XBee */
static void xbee_process_AT_command_response(uint8_t st,
                                             uint8_t fnum,
                                             uint16_t cmd,
                                             uint32_t param)
{
    switch (st) {
    case XBEE_AT_CMD_OK:
    case XBEE_AT_CMD_INVALID:
    case XBEE_AT_CMD_BAD_PARAM:
        /* record the received info from XBee module */
        latest_at_response.status = st;
        latest_at_response.frame_num = fnum;
        latest_at_response.command = cmd;
        latest_at_response.parameter = param;
        /* flag the info arrival for send_AT_command() function */
        mutex_unlock(&mutex_wait_at_reponse);
        break;
    default:
        /* other/unknown error */
        core_panic(0x0bee,
                   "Unexpected error during AT command on XBee");
    }
}

/* handle TX status response from XBee */
static void xbee_process_tx_status(uint8_t st,
                                   uint8_t fnum)
{
    switch (st) {
    case XBEE_TX_SUCCESS:
    case XBEE_TX_NOACK:
    case XBEE_TX_MEDIUM_BUSY:
        /* record the received info from XBee module */
        latest_tx_report.status = st;
        latest_tx_report.frame_num = fnum;
        /* flag the info arrival for do_send() function */
        mutex_unlock(&mutex_wait_tx_status);
        break;
    default:
        /* other/unknown error */
        core_panic(0x0bee,
                   "Unexpected error when TXing packet to XBee");
    }
}

#define INC_CKSUM(x)   (sum = (sum + (x)) & 0xff)

/* send an "API" (i.e. low-level) command to XBee modem */
static void xbee_send_API_command(uint8_t cmd_id,
                                  uint16_t payload_len,
                                  const uint8_t *payload)
{
    /* send the start byte on UART */
    send_xbee(0x7e);
    /* send the length of the payload */
    payload_len++;   /* cmd_id is part of the payload */
    send_xbee((uint8_t)(payload_len >> 8));
    send_xbee((uint8_t)(payload_len & 0xff));
    /* send the payload, while computing the checksum */
    send_xbee(cmd_id);
    uint8_t sum = cmd_id;
    for (int i = 0; i < payload_len; i++) {
        send_xbee(payload[i]);
        INC_CKSUM(payload[i]);
    }
    /* finish computing the checksum, then send it */
    sum = 0xff - sum;
    send_xbee(sum);
}

#define MAX_AT_CMD_LEN  32
  /* should be sufficient, since currently, largest parameter known
      for existing AT commands is 20 bytes long */

/* send an "AT" (i.e. high-level) command to the XBee modem,
    and wait for its response */
static xbee_at_cmd_response_t xbee_send_AT_command(unsigned int len,
                                                   const uint8_t *str)
{
    /* encode AT command into an API frame */
    static uint8_t buf[MAX_AT_CMD_LEN];
    buf[0] = frame_id;
    frame_id++;
    for (int i = 0; i < len; i++) {
        buf[i + 1] = str[i];
    }
    xbee_send_API_command(0x08, len + 1, buf);
    /* wait for response */
    mutex_lock(&mutex_wait_at_reponse);
    mutex_lock(&mutex_wait_at_reponse);
    mutex_unlock(&mutex_wait_at_reponse);  // release lock once response is here
    /* process response */
    if ( (latest_at_response.frame_num != buf[0])
      || (latest_at_response.command != ((buf[1] << 8) | buf[2])) ) {
        /* response doesn't correspond to sent AT command ! */
        core_panic(0x0bee,
                   "AT response doesn't correspond to sent command");
    }
    xbee_at_cmd_response_t ret_val;
    ret_val.status = latest_at_response.status;
    ret_val.parameter = latest_at_response.parameter;
    return ret_val;
}


/*****************************************************************************/
/*                          Handling incoming data                           */
/*****************************************************************************/

unsigned int xbee_rx_error_count = 0;

#define XBEE_RECV_BUF_SIZE   128

/* We implement a finite state machine (FSM) to handle the incoming data,
   reconstruct the packets, extract their payload, and handle its
   processing and/or forward it to the upper layers.
   The following enum lits the states of this FSM. */
static enum {
    RECV_FSM_IDLE,                /* no data stored for now */
    RECV_FSM_SIZE1,               /* waiting for the first byte (MSB)
                                     of the size of the packet to come */
    RECV_FSM_SIZE2,               /* waiting for the second byte (LSB)
                                     of the size of the packet to come */
    RECV_FSM_PACKET_INCOMPLETE,   /* a packet (whose size we know) has begun
                                     to arrive, but isn't complete yet */
    RECV_FSM_PACKET_COMPLETE,     /* we now have a complete packet,
                                     wait for checksum byte */
} recv_fsm_state;

/* reception buffer for incoming data from XBee module */
static uint8_t recv_buf[XBEE_RECV_BUF_SIZE];
/* size of data currently stored in the reception buffer */
static uint16_t recv_data_len;
/* expected size of the currently incoming packet */
static uint16_t expect_data_len;
/* current checksum of the incoming data */
static uint8_t cksum;

/**
 * @brief Handler for incoming data from the XBee module.
 * 
 * @details The low-level UART driver must call this function when receiving
 *          a character from the XBee module via the UART, passing
 *          the received character as a parameter.
 *
 * @param[in] c character received from the XBee module via UART.
 */
void xbee_incoming_char(char c)
{
    uint8_t in = (uint8_t) c;
    uint16_t cmd;
    uint32_t param;
    switch (recv_fsm_state) {
    case RECV_FSM_IDLE:
        if (in == 0x7e) {
            /* a new packet is beginning to arrive: wait for its size */
            recv_fsm_state = RECV_FSM_SIZE1;
        }
        break;
    case RECV_FSM_SIZE1:
        expect_data_len = (in << 8);
        recv_fsm_state = RECV_FSM_SIZE2;
        break;
    case RECV_FSM_SIZE2:
        expect_data_len |= in;
        /* we now wait for the payload of the packet */
        recv_fsm_state = RECV_FSM_PACKET_INCOMPLETE;
        recv_data_len = 0;
        cksum = 0;
        break;
    case RECV_FSM_PACKET_INCOMPLETE:
        /* a new byte of payload has arrived */
        recv_buf[recv_data_len] = in;
        recv_data_len++;
        cksum = (cksum + in) & 0xff;
        /* is the packet now complete? */
        if (recv_data_len >= expect_data_len) {
            recv_fsm_state = RECV_FSM_PACKET_COMPLETE;
        }
        break;
    case RECV_FSM_PACKET_COMPLETE:
        /* check the received packet */
        cksum = (cksum + in) & 0xff;
        if (cksum != 0xff) {
            puts("XBee driver: bad checksum for incoming data!");
            xbee_rx_error_count++;
            recv_data_len = 0;
            recv_fsm_state = RECV_FSM_IDLE;
            return;
        }
        ieee802154_node_addr_t src_node;
        /* process the verified payload, according to its API type */
        switch (recv_buf[0]) {
        case 0x8a:
            xbee_process_modem_status_event(recv_buf[1]);
            break;
        case 0x88:
            cmd = ((recv_buf[2] << 8) | recv_buf[3]);
            /* turn the parameter into a 32-bit unsigned integer */
            param = 0;
            for (uint8_t idx = 5; idx <= 8; idx++) {
                if (idx >= recv_data_len) break;
                param = (param << 8) | recv_buf[idx];
            }
            xbee_process_AT_command_response(recv_buf[4],
                                             recv_buf[1],
                                             cmd,
                                             param);
            break;
        case 0x89:
            xbee_process_tx_status(recv_buf[2],
                                   recv_buf[1]);
            break;
        case 0x80:
        case 0x82:
            src_node.long_addr = ((uint64_t)recv_buf[1] << 56)
                               | ((uint64_t)recv_buf[2] << 48)
                               | ((uint64_t)recv_buf[3] << 40)
                               | ((uint64_t)recv_buf[4] << 32)
                               | ((uint64_t)recv_buf[5] << 24)
                               | ((uint64_t)recv_buf[6] << 16)
                               | ((uint64_t)recv_buf[7] << 8)
                               | ((uint64_t)recv_buf[8]);
            if (recv_buf[10] & 0x02) {
                /* address broadcast for source */
                src_node.long_addr = 0x0000FFFFull;
            }
            xbee_process_rx_packet(src_node,
                                   true,
                                   recv_buf[9],
                                   &(recv_buf[11]),
                                   recv_data_len - 11);
            break;
        case 0x81:
        case 0x83:
            src_node.pan.addr = ((uint64_t)recv_buf[1] << 8)
                              | ((uint64_t)recv_buf[2]);
            if (recv_buf[4] & 0x02) {
                /* address broadcast for source */
                src_node.long_addr = 0xFFFFu;
            }
            xbee_process_rx_packet(src_node,
                                   false,
                                   recv_buf[3],
                                   &(recv_buf[5]),
                                   recv_data_len - 5);
            break;
        }
        break;
        recv_data_len = 0;
        recv_fsm_state = RECV_FSM_IDLE;
        DEBUG("Received and processed packet from XBee module.\n");
    }
}


/*****************************************************************************/
/*                        Driver's "public" functions                        */
/*****************************************************************************/

void xbee_initialize(void)
{
    int res;
    /* no RX callback function by default */
    rx_callback = NULL;
    /* UART initialization */
    res = uart_init(xbee_uart_link,
                    9600U,
                    xbee_incoming_char,
                    NULL);
    switch (res) {
    case 0:     /* OK */
        break;
    case -1:    /* wrong rate */
        core_panic(0x0bee,
                   "Invalid baud rate for XBee-link UART");
    default:    /* unexpected error! */
        core_panic(0x0bee,
                   "UART initialization failure for XBee link");
    }
    /* GPIO initialization (for sleep/weke-up...) */
    if (XBEE_SLEEP_RQ_GPIO != GPIO_UNDEFINED) {
        /* don't try to initialize GPIOs if undefined */
        res = gpio_init_out(XBEE_SLEEP_RQ_GPIO,
                            GPIO_NOPULL);
        switch (res) {
        case 0: /* OK */
            break;
        case -1:    /* undefined/unavilable device */
            core_panic(0x0bee,
                       "XBEE_SLEEP_RQ_GPIO pin unavailable");
        case -2:    /* unvalid pull-up/down mode */
            core_panic(0x0bee,
                       "invalid pull mode for XBEE_SLEEP_RQ_GPIO pin");
        default:    /* unexpected error! */
            core_panic(0x0bee,
                       "XBEE_SLEEP_RQ_GPIO initialization failure");
        }
    }
    if (XBEE_ON_STATUS_GPIO != GPIO_UNDEFINED) {
        res = gpio_init_in(XBEE_ON_STATUS_GPIO,
                           GPIO_NOPULL);
        switch (res) {
        case 0: /* OK */
            break;
        case -1:    /* undefined/unavilable device */
            core_panic(0x0bee,
                       "XBEE_ON_STATUS_GPIO pin unavailable");
        case -2:    /* unvalid pull-up/down mode */
            core_panic(0x0bee,
                       "invalid pull mode for XBEE_ON_STATUS_GPIO pin");
        default:    /* unexpected error! */
            core_panic(0x0bee,
                       "XBEE_ON_STATUS_GPIO initialization failure");
        }
    }
    /* initialize mutexes */
    mutex_init(&mutex_wait_tx_status);
    mutex_init(&mutex_wait_at_reponse);
    /* reset sent frames counter */
    frame_id = 1;   /* 0 would mean send no response */
    /* mark reception buffer as empty */
    recv_data_len = 0;
    /* put reception fsm in idle mode */
    recv_fsm_state = RECV_FSM_IDLE;

    /* send initial AT commands */
    send_xbee('+');send_xbee('+');send_xbee('+');
    /* reset XBee module */
    send_xbee('A');send_xbee('T');send_xbee('F');send_xbee('R');
    send_xbee('\r');
    /* disable non-802.15.4 extensions */
    send_xbee('A');send_xbee('T');send_xbee('M');send_xbee('M');send_xbee('2');
    send_xbee('\r');
    /* put XBee module in "API mode" */
    send_xbee('A');send_xbee('T');send_xbee('A');send_xbee('P');send_xbee('1');
    send_xbee('\r');
    /* apply AT commands */
    send_xbee('A');send_xbee('T');send_xbee('A');send_xbee('C');
    send_xbee('\r');
    /* print firmware version of the XBee module */
    uint8_t buf[2];
    buf[0] = 'V';
    buf[1] = 'R';
    xbee_at_cmd_response_t res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query firware version from XBee modem");
    }
    printf("XBee modem initialized. Firmare version = %X.%X.%X.%X \n",
           (uint8_t)((res.parameter >> 12) & 0x0f),
           (uint8_t)((res.parameter >> 8) & 0x0f),
           (uint8_t)((res.parameter >> 4) & 0x0f),
           (uint8_t)(res.parameter & 0x0f) );
}

bool xbee_on(void)
{
    if (XBEE_SLEEP_RQ_GPIO == GPIO_UNDEFINED) {
        /* if GPIO isn't defined, do nothing */
        return;
    }
    /* turn XBee sleep mode off, by clearing SLEEP_RQ */
    gpio_clear(XBEE_SLEEP_RQ_GPIO);
    /* never fails, but must wait some milliseconds... */
    do {
    } while (!xbee_is_on());
    return true;
}

void xbee_off(void)
{
    if (XBEE_SLEEP_RQ_GPIO == GPIO_UNDEFINED) {
        /* if GPIO isn't defined, do nothing */
        return;
    }
    /* put XBee into sleep mode, by asserting SLEEP_RQ */
    gpio_set(XBEE_SLEEP_RQ_GPIO);
}

bool xbee_is_on(void)
{
    if (XBEE_ON_STATUS_GPIO == GPIO_UNDEFINED) {
        /* STATUS pin of XBee modem not accessible... */
        if (XBEE_SLEEP_RQ_GPIO == GPIO_UNDEFINED) {
            /* if /SLEEP_RQ pin is not accessible,
               then modem is always on */
            return true;
        }
        /* XBee is on if SLEEP_RQ pin is not asserted */
        return (gpio_read(XBEE_SLEEP_RQ_GPIO) == 0);
    }
    /* XBee is on if pin ON/SLEEP is asserted */
    return (gpio_read(XBEE_ON_STATUS_GPIO) != 0);
}

radio_tx_status_t xbee_load_tx_buf(ieee802154_packet_kind_t kind,
                                   ieee802154_node_addr_t dest,
                                   bool use_long_addr,
                                   bool wants_ack,
                                   void *buf,
                                   unsigned int len)
{
    /* XBee modems doesn't allow to load TX buffer
       without sending... :( */
    core_panic(0x0bee,
               "Cannot load TX buf without sending with XBee modules");
}

radio_tx_status_t xbee_transmit_tx_buf(void)
{
    /* XBee modems doesn't allow to transmit
       a previously loaded TX buffer... :( */
    core_panic(0x0bee,
               "Cannot transmit TX buf without loading with XBee modules");
}

radio_tx_status_t xbee_do_send(ieee802154_packet_kind_t kind,
                               ieee802154_node_addr_t dest,
                               bool use_long_addr,
                               bool wants_ack,
                               void *buf,
                               unsigned int len)
{
    if (len > 100) {
        return RADIO_TX_PACKET_TOO_LONG;
    }

    /* prepare TX packet header */
    uint8_t cmd_id;
    uint16_t payload_len = len;
    if (use_long_addr) {
        cmd_id = 0x00;
        payload_len += 11;
    } else {
        cmd_id = 0x01;
        payload_len += 5;
    }

    /* send the start byte on UART */
    send_xbee(0x7e);
    /* send the length of the payload */
    send_xbee((uint8_t)(payload_len >> 8));
    send_xbee((uint8_t)(payload_len & 0xff));
    /* send the payload, while computing the checksum */
    send_xbee(cmd_id);
    uint8_t sum = cmd_id;
    send_xbee(frame_id);
    INC_CKSUM(frame_id);
    if (use_long_addr) {
        send_xbee(dest.long_addr >> 56);
        INC_CKSUM(dest.long_addr >> 56);
        send_xbee(dest.long_addr >> 48);
        INC_CKSUM(dest.long_addr >> 48);
        send_xbee(dest.long_addr >> 40);
        INC_CKSUM(dest.long_addr >> 40);
        send_xbee(dest.long_addr >> 32);
        INC_CKSUM(dest.long_addr >> 32);
        send_xbee(dest.long_addr >> 24);
        INC_CKSUM(dest.long_addr >> 24);
        send_xbee(dest.long_addr >> 16);
        INC_CKSUM(dest.long_addr >> 16);
        send_xbee(dest.long_addr >> 8);
        INC_CKSUM(dest.long_addr >> 8);
        send_xbee(dest.long_addr & 0xff);
        INC_CKSUM(dest.long_addr & 0xff);
    } else {
        send_xbee(dest.pan.addr >> 8);
        INC_CKSUM(dest.pan.addr >> 8);
        send_xbee(dest.pan.addr & 0xff);
        INC_CKSUM(dest.pan.addr & 0xff);
    }
    if (wants_ack) {
        send_xbee(0x00);
    } else {
        send_xbee(0x01);
        INC_CKSUM(0x01);
    }
    uint8_t *payload = (uint8_t *)buf;
    for (int i = 0; i < len; i++) {
        send_xbee(payload[i]);
        INC_CKSUM(payload[i]);
    }
    /* finish computing the checksum, then send it */
    sum = 0xff - sum;
    send_xbee(sum);

    /* wait for TX status response */
    mutex_lock(&mutex_wait_tx_status);
    mutex_lock(&mutex_wait_tx_status);
    mutex_unlock(&mutex_wait_tx_status); // release lock once status is here

    /* ensure status received is for TXed packet */
    if (latest_tx_report.frame_num != frame_id) {
        /* status doesn't correspond to TXed packet ! */
        core_panic(0x0bee,
                   "TX status doesn't correspond to sent packet");
    }
    frame_id++;
    /* return status for the TXed packet */
    switch (latest_tx_report.status) {
    case XBEE_TX_SUCCESS:
        return RADIO_TX_OK;
    case XBEE_TX_NOACK:
        return RADIO_TX_NOACK;
    case XBEE_TX_MEDIUM_BUSY:
        return RADIO_TX_MEDIUM_BUSY;
    default:
        return RADIO_TX_ERROR;
    }
}

void xbee_set_recv_callback(receive_802154_packet_callback_t recv_func)
{
    rx_callback = recv_func;
}

void xbee_switch_to_rx(void)
{
    /* XBee modem automatically manages its modes of operation,
       so this function can only be a NOP */
}

void xbee_set_channel(unsigned int chan)
{
    uint8_t buf[4];
    buf[0] = 'C';
    buf[1] = 'H';
    buf[2] = (chan & 0xff);
    buf[3] = 0;
    xbee_at_cmd_response_t res = xbee_send_AT_command(3, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to set RF channel on XBee modem");
    }
}

unsigned int xbee_get_channel(void)
{
    uint8_t buf[2];
    buf[0] = 'C';
    buf[1] = 'H';
    xbee_at_cmd_response_t res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query RF channel from XBee modem");
    }
    return (unsigned int) res.parameter;
}

void xbee_set_address(uint16_t addr)
{
    uint8_t buf[4];
    buf[0] = 'M';
    buf[1] = 'Y';
    buf[2] = (addr >> 8);
    buf[3] = (addr & 0xff);
    xbee_at_cmd_response_t res = xbee_send_AT_command(4, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to set 16-bit address on XBee modem");
    }
}

uint16_t xbee_get_address(void)
{
    uint8_t buf[2];
    buf[0] = 'M';
    buf[1] = 'Y';
    xbee_at_cmd_response_t res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query 16-bit address from XBee modem");
    }
    return (uint16_t) res.parameter;
}

void xbee_set_long_address(uint64_t addr)
{
    /* 64-bit long address is strictly read-only on XBee modems */
    core_panic(0x0bee,
               "cannot change long (64-bit) address on XBee modems");
}

uint64_t xbee_get_long_address(void)
{
    uint64_t addr;
    uint8_t buf[2];
    xbee_at_cmd_response_t res;

    buf[0] = 'S';
    buf[1] = 'H';
    res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query long (64-bit) address from XBee modem");
    }
    addr = ((uint64_t)res.parameter << 32);

    buf[0] = 'S';
    buf[1] = 'L';
    res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query long (64-bit) address from XBee modem");
    }
    addr |= res.parameter;

    return addr;
}

void xbee_set_pan_id(uint16_t pan)
{
    uint8_t buf[4];
    buf[0] = 'I';
    buf[1] = 'D';
    buf[2] = (pan >> 8);
    buf[3] = (pan & 0xff);
    xbee_at_cmd_response_t res = xbee_send_AT_command(4, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to set PAN ID on XBee modem");
    }
}

uint16_t xbee_get_pan_id(void)
{
    uint8_t buf[2];
    buf[0] = 'I';
    buf[1] = 'D';
    xbee_at_cmd_response_t res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query PAN ID from XBee modem");
    }
    return (uint16_t) res.parameter;
}

void xbee_set_tx_power(int pow)
{
    uint8_t pl = 0;
    if ((pow > -10) && (pow < -4)) {
        pl = 1;
    } else if ((pow >= -4) && (pow < -2)) {
        pl = 2;
    } else if ((pow >= -2) && (pow < 0)) {
        pl = 3;
    } else {   /* pow >= 0 */
        pl = 4;
    }
    uint8_t buf[4];
    buf[0] = 'P';
    buf[1] = 'L';
    buf[2] = pl;
    buf[3] = 0;
    xbee_at_cmd_response_t res = xbee_send_AT_command(3, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to set TX power level on XBee modem");
    }
}

int xbee_get_tx_power(void)
{
    uint8_t buf[2];
    buf[0] = 'P';
    buf[1] = 'L';
    xbee_at_cmd_response_t res = xbee_send_AT_command(2, buf);
    if (res.status != XBEE_AT_CMD_OK) {
        core_panic(0x0bee,
                   "failed to query TX power level from XBee modem");
    }
    switch (res.parameter) {
    case 0:
        return -10;
    case 1:
        return -6;
    case 2:
        return -4;
    case 3:
        return -2;
    case 4:
        return 0;
    default:
        core_panic(0x0bee,
                   "bad answer from ATPL command from XBee modem");
    }
}

bool xbee_channel_clear(void)
{
    /* XBee modems don't allow user to perform an "independent" CCA:
       they perform automatically CCA before each packet emission;
       thus, we "falsely" always return 'true' in this method,
       so that upper layers of the network stack feel free to
       start packet transmissions at any time */
    return true;
}

void xbee_set_monitor(bool monitor)
{
    /* XBee modems don't offer a monitor/promiscuous mode */
    if (monitor) {
        core_panic(0x0bee,
                   "cannot activate monitor/promiscuous mode on XBee modems");
    }
}

bool xbee_get_monitor(void)
{
    /* XBee modems don't offer a monitor/promiscuous mode */
    return false;
}


/* XBee low-level radio driver definition */
const ieee802154_radio_driver_t xbee_radio_driver = {
    .init = xbee_initialize,
    .on = xbee_on,
    .off = xbee_off,
    .is_on = xbee_is_on,
    .load_tx = xbee_load_tx_buf,
    .transmit = xbee_transmit_tx_buf,
    .send = xbee_do_send,
    .set_receive_callback = xbee_set_recv_callback,
    .switch_to_rx = xbee_switch_to_rx,
    .set_channel = xbee_set_channel,
    .get_channel = xbee_get_channel,
    .set_address = xbee_set_address,
    .get_address = xbee_get_address,
    .set_long_address = xbee_set_long_address,
    .get_long_address = xbee_get_long_address,
    .set_pan_id = xbee_set_pan_id,
    .get_pan_id = xbee_get_pan_id,
    .set_tx_power = xbee_set_tx_power,
    .get_tx_power = xbee_get_tx_power,
    .channel_is_clear = xbee_channel_clear,
    .set_promiscuous_mode = xbee_set_monitor,
    .in_promiscuous_mode = xbee_get_monitor
};
