/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_sim900
 * @{
 *
 * @file
 * @brief       Implementation of SIM900 driver
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "sim900.h"

#define PPPINITFCS16    0xffff
#define PPPGOODFCS16    0xf0b8

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define STREAM_CR (0x0D0A)
#define STREAM_OK (0x0D0A4F4B)
#define STREAM_ERROR (0x4552524F)
#define STREAM_CONN (0x434F4E4E)

#define HAS_OK (1)
#define HAS_ERROR (2)
#define HAS_CONN (4)

#define MSG_AT_FINISHED (1)
#define MSG_AT_TIMEOUT (2)
#define PDP_UP (3)
#define RX_FINISHED (4)

#define HDLC_FLAG_CHAR (0x7e)
#define HDLC_ESCAPE_CHAR (0x7d)
#define HDLC_SIX_CMPL (0x20)

#define SIM900_TIMEOUT_DEV_ALIVE (5000000U)
#define SIM900_TIMEOUT_NETATTACH (3000000U)
#define SIM900_DATAMODE_DELAY (1000000U)
#define SIM900_LINKDOWN_DELAY (1000000U)


void pdp_netattach_timeout(sim900_t *dev);
void pdp_netattach(sim900_t *dev);
void pdp_check_netattach(sim900_t *dev);

static inline void _reset_at_status(sim900_t *dev)
{
    dev->state = AT_STATE_IDLE;
}

static inline void _isr_at_command(sim900_t *dev, char data)
{
    msg_t msg;

    /* Detect stream of characters and add flag to at_status*/
    dev->_stream |= data;
    dev->at_status |= (dev->_stream == STREAM_OK) * HAS_OK;
    dev->at_status |= (dev->_stream == STREAM_ERROR) * HAS_ERROR;
    dev->at_status |= (dev->_stream == STREAM_CONN) * HAS_CONN;
    dev->_num_esc -= (dev->_stream & 0xFFFF) == STREAM_CR;

    /* if the AT command finished, send msg to driver */
    if (!dev->_num_esc) {
        _reset_at_status(dev);
        msg.type = PPPDEV_MSG_TYPE_EVENT;
        msg.content.value = MSG_AT_FINISHED;
        msg_send_int(&msg, dev->mac_pid);
    }

    dev->_stream = (dev->_stream << 8);
}

static inline void _rx_ready(sim900_t *dev)
{
    msg_t msg;

    msg.type = PPPDEV_MSG_TYPE_EVENT;
    msg.content.value = RX_FINISHED;

    dev->ppp_rx_state = PPP_RX_IDLE;
    dev->rx_count = dev->int_count;
    dev->int_count = 0;
    dev->escape = 0;

    /* if PPP pkt is sane, send to thread */
    if (dev->int_fcs == PPPGOODFCS16 && dev->escape == 0 && dev->rx_count >= 4) {
        msg_send_int(&msg, dev->mac_pid);
    }

    dev->int_fcs = PPPINITFCS16;
}

static inline void _rx_escape_next(sim900_t *dev)
{
    dev->ppp_rx_state = PPP_RX_STARTED;
    dev->escape = HDLC_SIX_CMPL;
}

static inline void _write_rx_char(sim900_t *dev, uint8_t data)
{
    uint8_t c;

    if (!(data <= HDLC_SIX_CMPL && dev->rx_accm & (1 << data))) {
        dev->ppp_rx_state = PPP_RX_STARTED;
        c = data ^ dev->escape;
        dev->int_fcs = fcs16_bit(dev->int_fcs, c);
        dev->rx_buf[dev->int_count++] = c;
        dev->escape = 0;
    }
}

static inline void _isr_rx(sim900_t *dev, char data)
{
    switch (data) {
        case HDLC_FLAG_CHAR:
            if (!dev->ppp_rx_state == PPP_RX_IDLE) {
                _rx_ready(dev);
            }
            break;
        case HDLC_ESCAPE_CHAR:
            _rx_escape_next(dev);
            break;
        default:
            _write_rx_char(dev, data);
    }
}
static void rx_cb(void *arg, uint8_t data)
{
    sim900_t *dev = (sim900_t *) arg;

    /* if state is in Command mode or Data Mode */
    switch (dev->state) {
        case AT_STATE_CMD:
            _isr_at_command(dev, data);
            break;
        case AT_STATE_RX:
            _isr_rx(dev, data);
            break;
        default:
            break;
    }
}

void _send_driver_event(msg_t *msg, uint8_t driver_event)
{
    msg->type = PPPDEV_MSG_TYPE_EVENT;
    msg->content.value = driver_event;
    msg_send(msg, thread_getpid());
}

int send_at_command(sim900_t *dev, char *cmd, size_t size, uint8_t ne, void (*cb)(sim900_t *dev))
{
    if (dev->state == AT_STATE_CMD) {
        DEBUG("AT device busy\n");
        return -EBUSY;
    }
    dev->state = AT_STATE_CMD;

    /* Lock thread in order to prevent multiple writes */
    mutex_lock(&dev->out_mutex);

    dev->_num_esc = ne;
    dev->at_status = 0;
    dev->_cb = cb;
    uart_write(dev->uart, (uint8_t *) cmd, size);
    mutex_unlock(&dev->out_mutex);
    return 0;
}


void _remove_timer(sim900_t *dev)
{
    xtimer_remove(&dev->xtimer);
}

void sim900_putchar(uart_t uart, uint8_t c)
{
    uint8_t *p = &c;

    uart_write(uart, p, 1);
}

int sim900_recv(pppdev_t *ppp_dev, char *buf, int len, void *info)
{
    sim900_t *dev = (sim900_t *) ppp_dev;
    int payload_length = dev->rx_count - 2;

    /* if buf given, copy rx buf to buf */
    if (buf) {
        memcpy(buf, dev->rx_buf, payload_length);
    }
    return payload_length;
}

int sim900_send(pppdev_t *ppp_dev, const struct iovec *vector, int count)
{
    sim900_t *dev = (sim900_t *) ppp_dev;
    uint16_t fcs = PPPINITFCS16;

    /* Lock thread in order to prevent multiple writes */
    mutex_lock(&dev->out_mutex);

    /* Send opening flag */
    sim900_putchar(dev->uart, (uint8_t) HDLC_FLAG_CHAR);
    uint8_t c;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < vector[i].iov_len; j++) {
            c = *(((uint8_t *)vector[i].iov_base) + j);
            /* if flag, escape character or current char in ACCM, add escape character plus XOR HDLC_SIX_CMPL*/
            if (c == HDLC_FLAG_CHAR || c == HDLC_ESCAPE_CHAR || dev->tx_accm & (1 << c)) {
                sim900_putchar(dev->uart, (uint8_t) HDLC_ESCAPE_CHAR);
                sim900_putchar(dev->uart, (uint8_t) c ^ HDLC_SIX_CMPL);
            }
            else {
                sim900_putchar(dev->uart, c);
            }
            fcs = fcs16_bit(fcs, c);
        }
    }

    /* Write checksum */
    fcs ^= 0xffff;
    sim900_putchar(dev->uart, (uint8_t) fcs & 0x00ff);
    sim900_putchar(dev->uart, (uint8_t) (fcs >> 8) & 0x00ff);

    /* Write closing flag */
    sim900_putchar(dev->uart, (uint8_t) HDLC_FLAG_CHAR);

    mutex_unlock(&dev->out_mutex);
    return 0;
}


void at_timeout(sim900_t *dev, uint32_t ms, void (*cb)(sim900_t *dev))
{
    dev->msg.type = PPPDEV_MSG_TYPE_EVENT;
    dev->msg.content.value = MSG_AT_TIMEOUT;
    dev->_timer_cb = cb;
    xtimer_set_msg(&dev->xtimer, ms, &dev->msg, dev->mac_pid);
}

void pdp_netattach(sim900_t *dev)
{
    send_at_command(dev, "AT+CGATT=1\r\n", 12, 3, &pdp_check_netattach);
}

void check_data_mode(sim900_t *dev)
{
    if (dev->at_status & HAS_CONN) {
        puts("Successfully entered data mode");
        dev->state = AT_STATE_RX;
        dev->ppp_rx_state = PPP_RX_IDLE;
        _send_driver_event(&dev->msg, PDP_UP);
    }
    else {
        puts("Failed to enter data mode");

        /* Force dial-up again */
        _send_driver_event(&dev->msg, PPPDEV_LINK_DOWN_EVENT);
    }
}

void pdp_enter_data_mode(sim900_t *dev)
{
    send_at_command(dev, "ATD*99***1#\r\n", 13, 3, &check_data_mode);
}

void set_apn(sim900_t *dev)
{
    uint8_t command[22 + SIM900_APN_SIZE];

    memcpy(command, "AT+CGDCONT=1,\"IP\",\"", 19);
    memcpy(command + 19, dev->apn, dev->apn_len);
    memcpy(command + dev->apn_len + 19, "\"\r\n", 3);
    send_at_command(dev, (char *) command, 22 + dev->apn_len, 3, &pdp_enter_data_mode);
}
void pdp_check_netattach(sim900_t *dev)
{
    if (dev->at_status &  HAS_ERROR) {
        puts("Still don't get network attach... let's try again");
        at_timeout(dev, SIM900_TIMEOUT_NETATTACH, &pdp_netattach);
    }
    else {
        puts("Network attach!.");
        set_apn(dev);
    }
}

void hang_out(sim900_t *dev)
{
    _remove_timer(dev);

    /* Hang out current call, for not getting errors calling *99# */
    send_at_command(dev, "ATH0\r\n", 5, 2, &pdp_netattach);
}
void check_device_status(sim900_t *dev)
{
    /*If AT has response, hang_out will be called */
    _reset_at_status(dev);
    send_at_command(dev, "AT\r\n", 4, 3, &hang_out);

    /* if not, call this function again with given timeout */
    at_timeout(dev, SIM900_TIMEOUT_DEV_ALIVE, &check_device_status);
}

void dial_up(sim900_t *dev)
{
    /*Exit data mode and call check_device_status*/
    uart_write(dev->uart, (uint8_t *) "+++", 3);
    at_timeout(dev, SIM900_DATAMODE_DELAY, &check_device_status);
}

int sim900_set(pppdev_t *dev, uint8_t opt, void *value, size_t value_len)
{
    sim900_t *d = (sim900_t *) dev;
    network_uint32_t *nu32;

    nu32 = (network_uint32_t *) value;
    switch (opt) {
        case PPPOPT_ACCM_RX:
            d->rx_accm = byteorder_ntohl(*nu32);
            break;
        case PPPOPT_ACCM_TX:
            d->tx_accm = byteorder_ntohl(*nu32);
            break;
        case PPPOPT_APN_NAME:
            if (value_len > SIM900_APN_SIZE) {
                return -1;
            }
            memcpy(d->apn, value, value_len);
            d->apn_len = value_len;
            break;
    }
    return 0;
}

int sim900_init(pppdev_t *d)
{
    sim900_t *dev = (sim900_t *) d;

    dev->rx_count = 0;
    dev->int_count = 0;
    dev->int_fcs = PPPINITFCS16;
    dev->_num_esc = 0;
    dev->_stream = 0;
    dev->rx_accm = 0;
    dev->tx_accm = 0;
    dev->mac_pid = thread_getpid();
    memset(dev->apn, 0, SIM900_APN_SIZE);

    /* Initialize UART */
    uint8_t res;
    res = uart_init(dev->uart, 9600, rx_cb, dev);
    if (res < 0) {
        return 1;
    }


    /*Start dial up */
    xtimer_usleep(100);
    return 0;
}

void driver_events(pppdev_t *d, uint8_t event)
{
    sim900_t *dev = (sim900_t *) d;

    /*Driver event*/
    switch (event) {
        case MSG_AT_FINISHED:
            dev->_cb(dev);
            break;
        case MSG_AT_TIMEOUT:
            dev->_timer_cb(dev);
            break;
        case PDP_UP:
            gnrc_ppp_link_down(&dev->msg, dev->mac_pid);
            gnrc_ppp_link_up(&dev->msg, dev->mac_pid);
            break;
        case RX_FINISHED:
            gnrc_ppp_dispatch_pkt(&dev->msg, dev->mac_pid);
            break;
        default:
            DEBUG("Unrecognized driver msg\n");
            break;
    }
}

int sim900_get(pppdev_t *dev, uint8_t opt, void *value, size_t max_lem)
{
    return 0;
}

int sim900_dialup(pppdev_t *dev)
{
    dial_up((sim900_t *) dev);
    return 0;
}

int link_down(pppdev_t *dev)
{
    sim900_t *d = (sim900_t *) dev;

    _reset_at_status(d);
    at_timeout(d, SIM900_LINKDOWN_DELAY, &dial_up);
    return 0;
}

const static pppdev_driver_t pppdev_driver_sim900 =
{
    .send = sim900_send,
    .recv = sim900_recv,
    .driver_ev = driver_events,
    .init = sim900_init,
    .set = sim900_set,
    .get = sim900_get,
    .dial_up = sim900_dialup,
    .link_down = link_down
};

void sim900_setup(sim900_t *dev, const sim900_params_t *params)
{
    dev->netdev.driver = &pppdev_driver_sim900;
    dev->uart = (uart_t) params->uart;
    mutex_init(&dev->out_mutex);
}
