#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "transceiver.h"

#include "tap.h"
#include "nativenet.h"
#include "nativenet_internal.h"
#include "cpu.h"

struct nativenet_callback_s {
    void (*func)(void);
};
static struct nativenet_callback_s _nativenet_callbacks[255];

struct rx_buffer_s _nativenet_rx_buffer[RX_BUF_SIZE];
volatile uint8_t rx_buffer_next;

uint8_t _native_net_chan;
uint16_t _native_net_pan;
uint8_t _native_net_monitor;
static int _native_net_tpid;
radio_address_t _native_net_addr;

/************************************************************************/
/* nativenet.h **********************************************************/
/************************************************************************/

void nativenet_init(int transceiver_pid)
{
    DEBUG("nativenet_init(transceiver_pid=%d)", transceiver_pid);
    _native_net_pan = 0;
    _native_net_chan = 0;
    _native_net_monitor = 0;
    _native_net_tpid = transceiver_pid;
}

void nativenet_powerdown()
{
    return;
}

void nativenet_set_monitor(uint8_t mode)
{
    DEBUG("nativenet_set_monitor(mode=%d)\n", mode);
    _native_net_monitor = mode;
}

int16_t nativenet_set_channel(uint8_t channel)
{
    _native_net_chan = channel;
    return _native_net_chan;
}

int16_t nativenet_get_channel()
{
    return _native_net_chan;
}

uint16_t nativenet_set_pan(uint16_t pan)
{
    _native_net_pan = pan;
    return _native_net_pan;
}

uint16_t nativenet_get_pan()
{
    return _native_net_pan;
}

int16_t nativenet_set_address(radio_address_t address)
{
    _native_net_addr = address;
    return _native_net_addr;
}

int16_t nativenet_get_address()
{
    return _native_net_addr;
}

uint8_t nativenet_send(radio_packet_t *packet)
{
    DEBUG("nativenet_send:  Sending packet of length %u to %u: %s\n", packet->length, packet->dst, (char*) packet->data);

    if (send_buf(packet) == -1) {
        warnx("nativenet_send: error sending packet");
    }
    return 0;
}

void nativenet_switch_to_rx()
{
    return;
}

/************************************************************************/
/* nativenet_internal.h *************************************************/
/************************************************************************/

int _nativenet_register_cb(int event, void *func)
{
    if (event > NNEV_MAXEV) {
        DEBUG("_nativenet_register_cb: event > NNEV_MAXEV");
        return -1;
    }

    _nativenet_callbacks[event].func = func;
    return 0;
}

int _nativenet_unregister_cb(int event)
{
    if (event > NNEV_MAXEV) {
        DEBUG("_nativenet_unregister_cb: event > NNEV_MAXEV");
        return -1;
    }

    _nativenet_callbacks[event].func = NULL;
    return 0;
}

void do_cb(int event)
{
    if (event > NNEV_MAXEV) {
        DEBUG("do_cb: event > NNEV_MAXEV");
        return;
    }

    if (_nativenet_callbacks[event].func != NULL) {
        _nativenet_callbacks[event].func();
    }
}

void _nativenet_handle_packet(radio_packet_t *packet)
{ 
    uint8_t dst_addr = packet->dst;

    /* address filter / monitor mode */
    if (_native_net_monitor == 1) {
        DEBUG("_nativenet_handle_packet: monitoring, not filtering address \n");
    }
    else {
        /* own addr check */
        if (dst_addr == _native_net_addr) {
            DEBUG("_nativenet_handle_packet: accept packet, addressed to us\n");
        }
        else if (dst_addr == 0) {
            DEBUG("_nativenet_handle_packet: accept packet, broadcast\n");
        }
        else {
            DEBUG("_nativenet_handle_packet: discard packet addressed to someone else\n");
            return;
        }
    }

    /* copy packet to rx buffer */
    memcpy(&_nativenet_rx_buffer[rx_buffer_next].data, packet->data, packet->length);
    memcpy(&_nativenet_rx_buffer[rx_buffer_next].packet, packet, sizeof(radio_packet_t));

    /* notify transceiver thread if any */
    if (_native_net_tpid) {
        DEBUG("_nativenet_handle_packet: notifying transceiver thread!\n");
        msg_t m;
        m.type = (uint16_t) RCV_PKT_NATIVE;
        m.content.value = rx_buffer_next;
        msg_send_int(&m, _native_net_tpid);
    }
    else {
        DEBUG("_nativenet_handle_packet: no one to notify =(\n");
    }

    /* shift to next buffer element */
    if (++rx_buffer_next == RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }
    
}
