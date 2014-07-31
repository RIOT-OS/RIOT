/******************************************************************************
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 *  This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @ingroup     dev_cc110x
 * @{
 */

/**
 * @file
 * @internal
 * @brief       TI Chipcon CC110x physical radio driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @version     $Revision: 2130 $
 *
 * @note        $Id: cc1100_phy.c 2130 2010-05-12 13:19:07Z hillebra $
 */
#include <stdio.h>
#include <string.h>

#include "hwtimer.h"
#include "vtimer.h"

#include "cc1100.h"
#include "cc1100_spi.h"
#include "cc1100_phy.h"
#include "cc1100-defaultSettings.h"

#include "protocol-multiplex.h"

#include "kernel.h"
#include "thread.h"
#include "mutex.h"
#include "msg.h"
#include "debug.h"

#define PRIORITY_CC1100         PRIORITY_MAIN-1

#define MSG_POLL 12346

#define FLAGS_IDENTIFICATION             (0x01) ///< Bit mask for reading the identification out of the flags field
#define R_FLAGS_PROTOCOL(x)     ((x & 0x0E)>>1) ///< Macro for reading the protocol out of the flags field
#define W_FLAGS_PROTOCOL(x)     ((x<<1) & 0x0E) ///< Macro for writing the protocol in the flags field

/*---------------------------------------------------------------------------*/
/*                  RX/TX buffer data structures */
/*---------------------------------------------------------------------------*/

typedef struct {
    cc1100_packet_layer0_t packet;
    packet_info_t info;
} rx_buffer_t;

#define RX_BUFF_SIZE    (10)                    ///< Size of RX queue
static volatile uint8_t rx_buffer_head;         ///< RX queue head
static volatile uint8_t rx_buffer_tail;         ///< RX queue tail
static volatile uint8_t rx_buffer_size;         ///< RX queue size
static rx_buffer_t rx_buffer[RX_BUFF_SIZE];     ///< RX buffer
static cc1100_packet_layer0_t tx_buffer;        ///< TX buffer (for one packet)

/*---------------------------------------------------------------------------*/
/*              Process/Event management data structures */
/*---------------------------------------------------------------------------*/

#define MAX_PACKET_HANDLERS     (5)
static packet_monitor_t packet_monitor;
static handler_entry_t handlers[MAX_PACKET_HANDLERS];
static const pm_table_t handler_table;
static const char *cc1100_event_handler_name = "cc1100_event_handler";
static mutex_t cc1100_mutex = MUTEX_INIT;
volatile int cc1100_mutex_pid;
static vtimer_t cc1100_watch_dog;
static timex_t cc1100_watch_dog_period;

static uint16_t cc1100_event_handler_pid;
static void *cc1100_event_handler_function(void *);

static char event_handler_stack[KERNEL_CONF_STACKSIZE_MAIN];

/*---------------------------------------------------------------------------*/
/*              Sequence number buffer management data structures */
/*---------------------------------------------------------------------------*/

/**
 * @name Sequence Buffer
 * @{
 */
#define MAX_SEQ_BUFFER_SIZE   (20)  ///< Maximum size of the sequence number buffer

typedef struct {
    uint64_t m_ticks;           ///< 64-bit timestamp
    uint8_t source;             ///< Source address
    uint8_t identification;     ///< Identification (1-bit)
} seq_buffer_entry_t;

//* Sequence number buffer for this layer */
static seq_buffer_entry_t seq_buffer[MAX_SEQ_BUFFER_SIZE];

//* Next position to enter a new value into ::seqBuffer */
static uint8_t seq_buffer_pos = 0;

/**
 * @brief   Last sequence number this node has seen
 *
 * @note    (phySrc + flags.identification) - for speedup in ISR.
 */
static volatile uint16_t last_seq_num = 0;

/** @} */

/*---------------------------------------------------------------------------*/
//                  WOR configuration data structures
/*---------------------------------------------------------------------------*/

#define EVENT0_MAX          (60493) ///< Maximum RX polling interval in milliseconds
#define WOR_RES_SWITCH       (1891) ///< Switching point value in milliseconds between
                                    ///< WOR_RES = 0 and WOR_RES = 1
#define DUTY_CYCLE_SIZE         (7) ///< Length of duty cycle array

cc1100_wor_config_t cc1100_wor_config;  ///< CC1100 WOR configuration

uint16_t cc1100_burst_count;            ///< Burst count, number of packets in a burst transfer
uint8_t cc1100_retransmission_count_uc; ///< Number of retransmissions for unicast
uint8_t cc1100_retransmission_count_bc; ///< Number of retransmissions for broadcast

static const double duty_cycle[2][DUTY_CYCLE_SIZE] = {  ///< Duty cycle values from AN047
    {12.5, 6.25, 3.125, 1.563, 0.781, 0.391, 0.195},
    {1.95, 0.9765, 0.4883, 0.2441, 0.1221, 0.061035, 0.030518}
};

/*---------------------------------------------------------------------------*/
//                  Data structures for statistic
/*---------------------------------------------------------------------------*/

cc1100_statistic_t cc1100_statistic;

/*---------------------------------------------------------------------------*/
//                  Initialization of physical layer
/*---------------------------------------------------------------------------*/

void cc1100_phy_init(void)
{
    int i;

    rx_buffer_head = 0;
    rx_buffer_tail = 0;
    rx_buffer_size = 0;

    /* Initialize RX-Buffer (clear content) */
    for (i = 0; i < RX_BUFF_SIZE; i++) {
        rx_buffer->packet.length = 0;
    }

    /* Initialize handler table & packet monitor */
    packet_monitor = NULL;
    pm_init_table((pm_table_t *)&handler_table, MAX_PACKET_HANDLERS, handlers);

    /* Clear sequence number buffer */
    memset(seq_buffer, 0, sizeof(seq_buffer_entry_t) * MAX_SEQ_BUFFER_SIZE);

    /* Initialize mutex */
    cc1100_mutex_pid = -1;

    /* Allocate event numbers and start cc1100 event process */
    cc1100_event_handler_pid = thread_create(event_handler_stack, sizeof(event_handler_stack), PRIORITY_CC1100, CREATE_STACKTEST,
                               cc1100_event_handler_function, NULL, cc1100_event_handler_name);

    /* Active watchdog for the first time */
    if (radio_mode == CC1100_MODE_CONSTANT_RX) {
        cc1100_watch_dog_period = timex_set(CC1100_WATCHDOG_PERIOD, 0);

        if (timex_cmp(cc1100_watch_dog_period, timex_set(0, 0)) != 0) {
            vtimer_set_msg(&cc1100_watch_dog, cc1100_watch_dog_period, cc1100_event_handler_pid, NULL);
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                          CC1100 mutual exclusion */
/*---------------------------------------------------------------------------*/

void cc1100_phy_mutex_lock(void)
{
    if (sched_active_thread->pid != cc1100_mutex_pid) {
        mutex_lock(&cc1100_mutex);
        cc1100_mutex_pid = sched_active_thread->pid;
    }
}

void cc1100_phy_mutex_unlock(void)
{
    cc1100_mutex_pid = -1;
    mutex_unlock(&cc1100_mutex);
}

/*---------------------------------------------------------------------------*/
//                  Statistical functions
/*---------------------------------------------------------------------------*/

void cc1100_reset_statistic(void)
{
    cc1100_statistic.packets_in_up = 0;
    cc1100_statistic.acks_send = 0;
    cc1100_statistic.packets_out_acked = 0;
    cc1100_statistic.packets_in = 0;
    cc1100_statistic.packets_out = 0;
    cc1100_statistic.packets_out_broadcast = 0;
    cc1100_statistic.raw_packets_out_acked = 0;
    cc1100_statistic.raw_packets_out = 0;
    cc1100_statistic.packets_in_dups = 0;
    cc1100_statistic.packets_in_crc_fail = 0;
    cc1100_statistic.packets_in_while_tx = 0;
    cc1100_statistic.rx_buffer_max = 0;
    cc1100_statistic.watch_dog_resets = 0;
}

void cc1100_print_statistic(void)
{
    printf("\nStatistic on CC1100 interface\n\n");
    printf("Total packets send on layer 0.5 (broadcast): %lu\n", cc1100_statistic.packets_out_broadcast);
    printf("Total packets send on layer 0.5 (unicast): %lu\n", cc1100_statistic.packets_out);
    printf("Total packets Acked on layer 0.5: %lu (%.2f%%)\n", cc1100_statistic.packets_out_acked, cc1100_statistic.packets_out_acked * (100.0f / (float)cc1100_statistic.packets_out));
    printf("Total packets send on layer 0: %lu\n", cc1100_statistic.raw_packets_out);
    printf("Total packets send on layer 0 w. Ack on Layer 0.5: %lu (Avg. Ack after: %lu packets)\n", cc1100_statistic.raw_packets_out_acked, cc1100_statistic.raw_packets_out_acked  / cc1100_statistic.packets_out_acked);
    printf("Burst count on this node: %i (%.2f%%)\n", cc1100_burst_count, (100 / (float)cc1100_burst_count) * (cc1100_statistic.raw_packets_out_acked  / (float) cc1100_statistic.packets_out_acked));
    printf("Total packets In on layer 0: %lu\n", cc1100_statistic.packets_in);
    printf("Duped packets In on layer 0: %lu\n", cc1100_statistic.packets_in_dups);
    printf("Corrupted packets In on layer 0: %lu\n", cc1100_statistic.packets_in_crc_fail);
    printf("Packets In on layer 0 while in TX: %lu\n", cc1100_statistic.packets_in_while_tx);
    printf("Total packets In and up to layer 1: %lu (%.2f%%)\n", cc1100_statistic.packets_in_up, cc1100_statistic.packets_in_up * (100.0f / (float)cc1100_statistic.packets_in));
    printf("Total Acks send on layer 0.5: %lu\n", cc1100_statistic.acks_send);
    printf("RX Buffer max: %lu (now: %u)\n", cc1100_statistic.rx_buffer_max, rx_buffer_size);
    printf("State machine resets by cc1100 watchdog: %lu\n", cc1100_statistic.watch_dog_resets);
}

void cc1100_print_config(void)
{
    char buf[8];
    printf("Current radio mode:           %s\r\n", cc1100_mode_to_text(radio_mode));
    printf("Current radio state:          %s\r\n", cc1100_state_to_text(radio_state));
    printf("Current MARC state:           %s\r\n", cc1100_get_marc_state());
    printf("Current channel number:       %u\r\n", cc1100_get_channel());
    printf("Burst count:                  %u packet(s)\r\n", cc1100_burst_count);
    printf("Retransmissions (unicast):    %u - if no ACK\r\n", cc1100_retransmission_count_uc);
    printf("Retransmissions (broadcast):  %u - always\r\n", cc1100_retransmission_count_bc);
    printf("Output power setting:         %s\r\n", cc1100_get_output_power(buf));

    if (radio_mode == CC1100_MODE_WOR) {
        printf("RX polling interval:      %u ms\r\n", cc1100_wor_config.rx_interval);
        printf("WOR receive time:         0x%.2X (%f ms)\r\n", cc1100_wor_config.rx_time_reg,
               cc1100_wor_config.rx_time_ms);
        printf("CC1100 WOREVT0 register:  0x%.2X\r\n", cc1100_wor_config.wor_evt_0);
        printf("CC1100 WOREVT1 register:  0x%.2X\r\n", cc1100_wor_config.wor_evt_1);
        printf("CC1100 WOR_CTRL register: 0x%.2X\r\n", cc1100_wor_config.wor_ctrl);
        printf("CC1100 MAN_WOR flag:      %u\r\n", rflags.MAN_WOR);
    }
}

/*---------------------------------------------------------------------------*/
//                  Change of RX polling interval (T_EVENT0)
/*---------------------------------------------------------------------------*/

inline uint16_t iround(double d)
{
    return (uint16_t) (d + 0.5);
}

int cc1100_phy_calc_wor_settings(uint16_t millis)
{
    /* Get packet interval as milliseconds */
    double t_packet_interval = (double)((T_PACKET_INTERVAL) / 1000.0);

    /* Calculate minimal T_EVENT0:

       (1) t_rx_time > t_packet_interval
       (2) t_rx_time = T_EVENT0 / 2 ^ (RX_TIME + 3 + WOR_RES)
       ------------------------------------------------------
       with RX_TIME = 0 && WOR_RES = 0 => event0_min > t_packet_interval * 8

       t_packet_interval = 3.8 ms (@400kbit/s)

       => event0_min = Math.ceil(3.8 * 8) + 10 */
    uint16_t event0_min = (uint16_t)(t_packet_interval * 8) + 1 + 10;

    /* Check if given value is in allowed range */
    if (millis < event0_min || millis > EVENT0_MAX) {
        return -1;
    }

    /* Time resolution for EVENT0 and other WOR parameters, */
    /* possible values are 0 and 1 if WOR is used */
    uint8_t wor_res = millis < WOR_RES_SWITCH ? 0 : 1;

    /* Calculate new value for EVENT0 */
    double tmp = (millis * 26) / (double) 750;

    if (wor_res == 1) {
        tmp /= 32;
    }

    tmp *= 1000;
    uint16_t event0 = (uint16_t) iround(tmp);

    /* Calculate all possible RX timeouts */
    int i;
    double rx_timeouts[DUTY_CYCLE_SIZE];

    for (i = 0; i < DUTY_CYCLE_SIZE; i++) {
        rx_timeouts[i] = (millis * duty_cycle[wor_res][i]) / 100;
    }

    /* Calculate index for optimal rx_timeout (MCSM2.RX_TIME) (if possible) */
    int idx = -1;

    for (i = DUTY_CYCLE_SIZE - 1; i >= 0; i--) {
        if (rx_timeouts[i] > t_packet_interval) {
            idx = i;
            break;
        }
    }

    /* If no index found, exit here (configuration with given value is not possible) */
    if (idx == -1) {
        return -1;
    }

    /* Calculate burst count (secure burst calculation with 8 extra packets) */
    int burst_count = (int) iround(millis / t_packet_interval) + 8;

    /* All calculations successful, now its safe to store */
    /* final configuration values in global WOR configuration */
    cc1100_wor_config.rx_interval = millis;
    cc1100_wor_config.wor_ctrl = (wor_res == 0) ? 0x78 : 0x79;
    cc1100_wor_config.wor_evt_0 = (uint8_t) event0;
    cc1100_wor_config.wor_evt_1 = (uint8_t)(event0 >> 8);
    cc1100_wor_config.rx_time_reg = idx;
    cc1100_wor_config.rx_time_ms = rx_timeouts[idx];

    /* If successful, return number of packets in a burst transfer */
    return burst_count;
}

/*---------------------------------------------------------------------------*/
//                  Sequence number buffer management
/*---------------------------------------------------------------------------*/

static bool contains_seq_entry(uint8_t src, uint8_t id)
{
    int i;
    uint32_t cmp;
    timex_t now_timex;
    vtimer_now(&now_timex);

    for (i = 0; i < MAX_SEQ_BUFFER_SIZE; i++) {
        if ((seq_buffer[i].source == src) && (seq_buffer[i].identification == id)) {
            /* Check if time stamp is OK */
            cmp = (radio_mode == CC1100_MODE_WOR) ? cc1100_wor_config.rx_interval : 16000; /* constant RX ~16ms */

            if ((timex_uint64(now_timex) - seq_buffer[i].m_ticks < cmp)) {
                return true;
            }
            else {
                seq_buffer[i].source = 0; /* Reset */
            }
        }
    }

    return false;
}

static void add_seq_entry(uint8_t src, uint8_t id)
{
    /* Remove all entries with given source to avoid short time overflow
     * of one bit counter (of the source node). So a valid packet would get
     * lost (especially important in constant RX mode). */
    int i;

    for (i = 0; i < MAX_SEQ_BUFFER_SIZE; i++) {
        if (seq_buffer[i].source == src) {
            seq_buffer[i].source = 0; /* Reset */
        }
    }

    /* Add new entry */
    seq_buffer[seq_buffer_pos].source = src;
    seq_buffer[seq_buffer_pos].identification = id;
    timex_t now;
    vtimer_now(&now);
    seq_buffer[seq_buffer_pos].m_ticks = timex_uint64(now);

    /* Store 16 bit sequence number of layer 0 for speedup */
    last_seq_num = src;
    last_seq_num <<= 8;
    last_seq_num += id;

    seq_buffer_pos++;

    if (seq_buffer_pos == MAX_SEQ_BUFFER_SIZE) {
        seq_buffer_pos = 0;
    }
}

/*---------------------------------------------------------------------------*/
/*              CC1100 physical layer send functions */
/*---------------------------------------------------------------------------*/

static void send_link_level_ack(uint8_t dest)
{
    uint8_t oldState = radio_state;             /* Save old state */
    cc1100_packet_layer0_t ack;                 /* Local packet, don't overwrite */

    radio_state = RADIO_SEND_ACK;               /* Set state to "Sending ACK" */
    cc1100_spi_write_reg(CC1100_MCSM0, 0x08);   /* Turn off FS-Autocal */
    cc1100_spi_write_reg(CC1100_MCSM1, 0x00);   /* TX_OFFMODE = IDLE */
    ack.length = 3;                             /* possible packet in txBuffer!*/
    ack.address = dest;
    ack.phy_src = rflags.RSSI;
    ack.flags = (LAYER_1_PROTOCOL_LL_ACK << 1);
    cc1100_send_raw((uint8_t *)&ack,                /* IDLE -> TX (88.4 us) */
                    ack.length + 1);
    cc1100_spi_write_reg(CC1100_MCSM0, 0x18);   /* Turn on FS-Autocal */
    cc1100_spi_write_reg(CC1100_MCSM1, 0x03);   /* TX_OFFMODE = RX */
    radio_state = oldState;                     /* Restore state */
    cc1100_statistic.acks_send++;
}

static bool send_burst(cc1100_packet_layer0_t *packet, uint8_t retries, uint8_t rtc)
{
    int i;
    radio_state = RADIO_SEND_BURST;
    rflags.LL_ACK = false;

    for (i = 1; i <= cc1100_burst_count; i++) {
        /*
         * Number of bytes to send is:
         * length of phy payload (packet->length)
         * + size of length field (1 byte)
         */
        extern unsigned long hwtimer_now(void);
        timer_tick_t t = hwtimer_now() + RTIMER_TICKS(T_PACKET_INTERVAL);
        cc1100_send_raw((uint8_t *)packet, packet->length + 1); /* RX -> TX (9.6 us) */

        cc1100_statistic.raw_packets_out++;

        /* Delay until predefined "send" interval has passed */
        timer_tick_t now = hwtimer_now();

        if (t > now) {
            hwtimer_wait(t - now);
        }

        /**
         * After sending the packet the CC1100 goes automatically
         * into RX mode (21.5 us) (listening for an ACK).
         * Do not interrupt burst if send to broadcast address (a node may
         * have the broadcast address at startup and would stop the burst
         * by sending an ACK).
         */
        if (rflags.LL_ACK && packet->address != CC1100_BROADCAST_ADDRESS) {
            cc1100_statistic.raw_packets_out_acked += i;
            break;
        }
    }

    /* No link level ACK -> do retry if retry counter greater zero
     * Note: Event broadcast packets can be sent repeatedly if in
     *       constant RX mode. In WOR mode it is not necessary, so
     *       set retry count to zero.*/
    if (!rflags.LL_ACK && retries > 0) {
        return send_burst(packet, retries - 1, rtc + 1);
    }

    /* Store number of transmission retries */
    rflags.RETC = rtc;
    rflags.RPS = rtc * cc1100_burst_count + i;

    if (i > cc1100_burst_count) {
        rflags.RPS--;
    }

    rflags.TX = false;

    /* Go to mode after TX (CONST_RX -> RX, WOR -> WOR) */
    cc1100_go_after_tx();

    /* Burst from any other node is definitely over */
    last_seq_num = 0;

    if (packet->address != CC1100_BROADCAST_ADDRESS && !rflags.LL_ACK) {
        return false;
    }

    return true;
}

int cc1100_send(radio_address_t addr, protocol_t protocol, int priority, char *payload, int payload_len)
{
    (void) priority;

    bool result;
    int return_code;
    uint8_t address;
    uint8_t retries;

    /* Lock mutex, nobody else should send now */
    cc1100_phy_mutex_lock();

    /* TX state machine lock -> no timers (WOR), no packets (only ACKs) */
    rflags.TX = true;

    /* Set chip to idle state */
    cc1100_set_idle();

    /* CC1100 radio layer only supports 8-bit addresses */
    address = addr;

    /* Loopback not supported */
    if (address == cc1100_get_address()) {
        return_code = RADIO_ADDR_OUT_OF_RANGE;
        goto mode_before_final;
    }

    /* Check address */
    if (address > MAX_UID) {
        return_code = RADIO_ADDR_OUT_OF_RANGE;
        goto mode_before_final;
    }

    /* Packet too long */
    if (payload_len > MAX_DATA_LENGTH) {
        return_code = RADIO_PAYLOAD_TOO_LONG;
        goto mode_before_final;
    }

    if (radio_state == RADIO_PWD) {
        return_code = RADIO_WRONG_MODE;
        goto mode_before_final;
    }

    /* Set number of transmission retries */
    retries = (address == CC1100_BROADCAST_ADDRESS) ?
              cc1100_retransmission_count_bc : cc1100_retransmission_count_uc;

    memset(tx_buffer.data, 0, MAX_DATA_LENGTH);         /* Clean data */

    /* TODO: If packets are shorter than max packet size, WOR interval is too long.
     *       This must be solved in some way. */
    tx_buffer.length = 3 + payload_len;             /* 3 bytes (A&PS&F) + data length */
    tx_buffer.address = address;                        /* Copy destination address */
    tx_buffer.flags = 0x00;                             /* Set clean state */
    tx_buffer.flags = W_FLAGS_PROTOCOL(protocol);       /* Copy protocol identifier */
    tx_buffer.phy_src = (uint8_t) cc1100_get_address(); /* Copy sender address */

    /* Set identification number of packet */
    tx_buffer.flags |= rflags.SEQ;                      /* Set flags.identification (bit 0) */
    rflags.SEQ = !rflags.SEQ;                           /* Toggle value of layer 0 sequence number bit */

    memcpy(tx_buffer.data, payload, payload_len);       /* Copy data */

    /* Send the packet */
    cc1100_spi_write_reg(CC1100_MCSM0, 0x08);           /* Turn off FS-Autocal */
    result = send_burst(&tx_buffer, retries, 0);        /* Send raw burst */
    return_code = result ? payload_len : RADIO_OP_FAILED;

    /* Collect statistics */
    if (address != CC1100_BROADCAST_ADDRESS) {
        cc1100_statistic.packets_out++;

        if (result) {
            cc1100_statistic.packets_out_acked++;
        }
    }
    else {
        cc1100_statistic.packets_out_broadcast++;
    }

    goto final;

mode_before_final:
    rflags.TX = false;
    /* Definitely set secure mode (CONST_RX -> RX, WOR -> WOR) */
    cc1100_go_after_tx();

final:
    /* Release mutex and return */
    cc1100_phy_mutex_unlock();
    return return_code;
}

/*---------------------------------------------------------------------------*/
/*                          RX Event Handler */
/*---------------------------------------------------------------------------*/

bool cc1100_set_packet_monitor(packet_monitor_t monitor)
{
    packet_monitor = monitor;
    return true;
}

int cc1100_set_packet_handler(protocol_t protocol, packet_handler_t handler)
{
    if (protocol > 7) {
        return -1;    /* Only 3-bit value allowed */
    }

    return pm_set_handler(&handler_table, protocol, handler);
}

static void *cc1100_event_handler_function(void *arg)
{
    (void) arg;

    msg_t m;

    while (1) {
        if (timex_uint64(cc1100_watch_dog_period) != 0) {
            vtimer_remove(&cc1100_watch_dog);
        }

        /* Test if any resource error has occurred */
        if (rflags.KT_RES_ERR) {
            rflags.KT_RES_ERR = false;
            /* possibly do something, e.g. log error condition */
        }

        if (m.type == MSG_TIMER) {
            uint8_t state;

            if (radio_mode == CC1100_MODE_CONSTANT_RX) {
                state = cc1100_spi_read_status(CC1100_MARCSTATE) & MARC_STATE;

                if ((state < 13 || state > 15) && radio_state == RADIO_RX && !rflags.TX) {
                    cc1100_statistic.watch_dog_resets++;

                    if (state != 1) {
                        cc1100_spi_strobe(CC1100_SIDLE);
                    }

                    cc1100_spi_strobe(CC1100_SFRX);
                    cc1100_go_receive();
                }
            }
            else {
                /* Radio mode is WOR, cannot read current MARC state, will */
                /* always be IDLE. So do nothing here, e.g. disable watchdog. */
            }
        }

        while (rx_buffer_size > 0) {
            rx_buffer_t *packet = &rx_buffer[rx_buffer_head];
            protocol_t p =  R_FLAGS_PROTOCOL(packet->packet.flags);

            if (packet_monitor != NULL) {
                packet_monitor((void *)&packet->packet.data, packet->packet.length, p, &packet->info);
            }

            pm_invoke(&handler_table, p, (void *)&packet->packet.data, MAX_DATA_LENGTH, &packet->info);
            dINT();
            rx_buffer_size--;
            rx_buffer_head++;

            if (rx_buffer_head == RX_BUFF_SIZE) {
                rx_buffer_head = 0;
            }

            eINT();
        }

        dINT();

        if (rx_buffer_size == 0) {
            if (timex_uint64(cc1100_watch_dog_period) != 0) {
                vtimer_set_msg(&cc1100_watch_dog, cc1100_watch_dog_period,
                               cc1100_event_handler_pid, NULL);
            }

            msg_receive(&m);
        }

        eINT();
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/
/*                          CC1100 packet (RX) ISR */
/*---------------------------------------------------------------------------*/

void cc1100_phy_rx_handler(void)
{
    msg_t m;
    m.type = MSG_POLL;
    bool dup = false;
    bool res = false;

    /* Possible packet received, RX -> IDLE (0.1 us) */
    rflags.CAA = false;
    rflags.MAN_WOR = false;
    cc1100_statistic.packets_in++;

    /* If WOR timer set, delete it now (new one will be set at end of ISR) */
    if (wor_hwtimer_id != -1) {
        hwtimer_remove(wor_hwtimer_id);
        wor_hwtimer_id = -1;
    }

    /* Transfer packet into temporary buffer position */
    res = cc1100_spi_receive_packet((uint8_t *) & (rx_buffer[rx_buffer_tail].packet), sizeof(cc1100_packet_layer0_t));

    if (res) {
        /* Get packet pointer and store additional data in packet info structure */
        cc1100_packet_layer0_t *p = &(rx_buffer[rx_buffer_tail].packet);
        rx_buffer[rx_buffer_tail].info.phy_src = p->phy_src;
        rx_buffer[rx_buffer_tail].info.source = p->phy_src;
        rx_buffer[rx_buffer_tail].info.destination = p->address;
        rx_buffer[rx_buffer_tail].info.rssi = rflags.RSSI;
        rx_buffer[rx_buffer_tail].info.lqi = rflags.LQI;
        rx_buffer[rx_buffer_tail].info.promiscuous = false;

        /* Get protocol and id field out of flags field */
        uint8_t protocol = R_FLAGS_PROTOCOL(p->flags);
        uint8_t identification = (p->flags & FLAGS_IDENTIFICATION);

        /* If received packet was an ACK (here we must be in
         * TX lock state, otherwise we don't expect an ACK) */
        if (protocol == LAYER_1_PROTOCOL_LL_ACK && rflags.TX) {
            /* And packet was for us */
            if (p->address == cc1100_get_address()) {
                /* Stop the burst */
                rflags.LL_ACK = true;
                rflags.RSSI_SEND = p->phy_src;
                rflags.TCP = (uint32_t)((uint16_t *)p->data);
            }

            return;
        }
        else {
            /* No ACK received so TOF is unpredictable */
            rflags.TOF = 0;
        }

        /* If we are sending a burst, don't accept packets.
         * Only ACKs are processed (for stopping the burst).
         * Same if state machine is in TX lock. */
        if (radio_state == RADIO_SEND_BURST || rflags.TX) {
            cc1100_statistic.packets_in_while_tx++;
            return;
        }

        /* If buffer is currently full -> don't check sequence numbers, send
         * ACK and restore state (keep always one position free for temporary packets) */
        if (rx_buffer_size >= RX_BUFF_SIZE - 1) {
            goto send_ack;
        }

        /* Build 16 bit sequence number of layer 0 for fast check */
        uint16_t new_seq_num = p->phy_src;
        new_seq_num <<= 8;
        new_seq_num += identification;

        /* Duplicate packet detection */
        dup = true;

        /* If new and last sequence number are the same, then discard packet */
        if (last_seq_num != new_seq_num) {
            /* Do a more precise check (takes more time) with larger buffer */
            if (!contains_seq_entry(p->phy_src, identification)) {
                /* Sequence number is new, no duplicate packet */
                dup = false;

                /* Store sequence number */
                add_seq_entry(p->phy_src, identification);

                /* Make temporary packet in RX buffer to a "real" packet which is processed */
                rx_buffer_size++;

                if (rx_buffer_size > cc1100_statistic.rx_buffer_max) {
                    cc1100_statistic.rx_buffer_max = rx_buffer_size;
                }

                rx_buffer_tail++;

                if (rx_buffer_tail == RX_BUFF_SIZE) {
                    rx_buffer_tail = 0;
                }

                /* Send empty message to wake up receiver process.
                 * Receiver process could already be running (triggered by previous message),
                 * so function would return 0 and assume the receiver is not waiting but indeed
                 * all is working fine.*/
                msg_send_int(&m, cc1100_event_handler_pid);
                cc1100_statistic.packets_in_up++;
            }
        }

    send_ack:

        /* If packet was send directly to us, send an ACK packet back to sender.
         * But only not if the packet itself was a LL-ACK!*/
        if (p->address == cc1100_get_address() && protocol != LAYER_1_PROTOCOL_LL_ACK) {
            send_link_level_ack(p->phy_src);

            /* After LL-ACK burst is over, reset number */
            last_seq_num = 0;
        }

        /* If duplicate packet detected, clear rxBuffer position */
        if (dup) {
            cc1100_statistic.packets_in_dups++;
        }

        /* If packet interrupted this nodes send call,
         * don't change anything after this point. */
        if (radio_state == RADIO_AIR_FREE_WAITING) {
            cc1100_spi_strobe(CC1100_SRX);
            hwtimer_wait(IDLE_TO_RX_TIME);
            return;
        }

        /* Valid packet. After a wake-up, the radio should be in IDLE.
         * So put CC1100 to RX for WOR_TIMEOUT (have to manually put
         * the radio back to sleep/WOR).*/
        cc1100_spi_write_reg(CC1100_MCSM0, 0x08);   /* Turn off FS-Autocal */
        cc1100_spi_write_reg(CC1100_MCSM2, 0x07);   /* Configure RX_TIME (until end of packet) */

        if (radio_mode == CC1100_MODE_CONSTANT_RX) {
            cc1100_spi_strobe(CC1100_SRX);
            hwtimer_wait(IDLE_TO_RX_TIME);
            radio_state = RADIO_RX;
            /* Return here if mode is CONSTANT_RX_MODE */
            return;
        }
        else {
            cc1100_spi_strobe(CC1100_SPWD);
            radio_state = RADIO_PWD;
        }

        /* Set hwtimer to put CC1100 back to RX after WOR_TIMEOUT_1 */
        wor_hwtimer_id = hwtimer_set(WOR_TIMEOUT_1, cc1100_hwtimer_go_receive_wrapper, NULL);

        if (wor_hwtimer_id == -1) {
            /* Signal hwtimer resource error, radio stays in RX,
             * so no big problem, only energy is wasted. */
            rflags.KT_RES_ERR = true;
        }
    }
    else {
        /* No ACK received so TOF is unpredictable */
        rflags.TOF = 0;

        /* CRC false or RX buffer full -> clear RX FIFO in both cases */
        last_seq_num = 0;                   /* Reset for correct burst detection */
        cc1100_spi_strobe(CC1100_SIDLE);    /* Switch to IDLE (should already be)... */
        cc1100_spi_strobe(CC1100_SFRX);     /* ...for flushing the RX FIFO */

        /* If packet interrupted this nodes send call,
         * don't change anything after this point. */
        if (radio_state == RADIO_AIR_FREE_WAITING) {
            cc1100_spi_strobe(CC1100_SRX);
            hwtimer_wait(IDLE_TO_RX_TIME);
            return;
        }

        /* If currently sending, exit here (don't go to RX/WOR) */
        if (radio_state == RADIO_SEND_BURST) {
            cc1100_statistic.packets_in_while_tx++;
            return;
        }

        /* No valid packet, so go back to RX/WOR as soon as possible */
        cc1100_go_receive();
    }
}
