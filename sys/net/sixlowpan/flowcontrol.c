#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vtimer.h>

#include "semaphore.h"
#include "bordermultiplex.h"
#include "flowcontrol.h"

flowcontrol_stat_t slwin_stat;
sem_t connection_established;
int16_t synack_seqnum = -1;


ipv6_addr_t init_threeway_handshake() {
    border_syn_packet_t *syn;
    msg_t m;
    m.content.ptr = NULL;
    msg_send(&m,border_get_serial_reader(),1);
    while(1) {
        msg_receive(&m);
        
        syn = (border_syn_packet_t *)m.content.ptr;
        border_conf_header_t *synack = (border_conf_header_t *)get_serial_out_buffer(0);
        ipv6_addr_t addr;
        memcpy(&addr, &(syn->addr), sizeof (ipv6_addr_t));
        
        slwin_stat.next_exp = syn->next_seq_num;
        slwin_stat.last_frame = syn->next_exp - 1;
        slwin_stat.last_ack = slwin_stat.last_frame;
        
        synack->reserved = 0;
        synack->type = BORDER_PACKET_CONF_TYPE;
        synack->conftype = BORDER_CONF_SYNACK;
        
        flowcontrol_send_over_uart((border_packet_t *)synack,sizeof (border_conf_header_t));
        
        synack_seqnum = synack->seq_num;
        
        return addr;
    }
}

ipv6_addr_t flowcontrol_init() {
    int i;
    
    slwin_stat.send_win_not_full = sem_init(BORDER_SWS);
    for(i = 0; i < BORDER_SWS; i++) {
        slwin_stat.send_win[i].frame_len = 0;
    }
    memset(&slwin_stat.send_win,0, sizeof(struct send_slot) * BORDER_SWS);
        
    for(i = 0; i < BORDER_RWS; i++) {
        slwin_stat.recv_win[i].received = 0;
        slwin_stat.recv_win[i].frame_len = 0;
    }
    memset(&slwin_stat.recv_win,0, sizeof(struct recv_slot) * BORDER_RWS);
    
    return init_threeway_handshake();
}

int set_timeout(vtimer_t *timeout, long useconds, void *args);

void timeout_callback (void *args) {
    uint8_t seq_num = *((uint8_t *)args);
    struct send_slot *slot = &(slwin_stat.send_win[seq_num % BORDER_SWS]);
    
    if (seq_num == ((border_packet_t *)(slot->frame))->seq_num) {
        writepacket(slot->frame,slot->frame_len);
        
        if (set_timeout(&slot->timeout, BORDER_SL_TIMEOUT, args) != 0) {
            printf("ERROR: Error invoking timeout timer\n");
            return;
        }
    } else {
        return;
    }
}

int set_timeout(vtimer_t *timeout, long useconds, void *args) {
    timex_t interval;
    interval.seconds = useconds / 1000000;
    interval.nanoseconds = (useconds % 1000000) * 1000;
    
    return vtimer_set_cb(timeout, interval, timeout_callback, args);
}

static int in_window(uint8_t seq_num, uint8_t min, uint8_t max) {
    uint8_t pos = seq_num - min;
    uint8_t maxpos = max - min + 1;
    return pos < maxpos;
}

void flowcontrol_send_over_uart(border_packet_t *packet, int len) {
    struct send_slot *slot;
    uint8_t args[] = {packet->seq_num};
    
    sem_wait(&(slwin_stat.send_win_not_full));
    packet->seq_num = ++slwin_stat.last_frame;
    slot = &(slwin_stat.send_win[packet->seq_num % BORDER_SWS]);
    memcpy(slot->frame, (uint8_t *)packet, len);
    slot->frame_len = len;
    if (set_timeout(&slot->timeout, BORDER_SL_TIMEOUT, (void *)args) != 0) {
        printf("ERROR: Error invoking timeout timer\n");
        return;
    }
    writepacket((uint8_t *)packet, len);
}

void send_ack(uint8_t seq_num) {
    border_packet_t *packet = (border_packet_t *)get_serial_out_buffer(0);
    packet->reserved = 0;
    packet->type = BORDER_PACKET_ACK_TYPE;
    packet->seq_num = seq_num;
    writepacket((uint8_t *)packet, sizeof (border_packet_t));
}

void flowcontrol_deliver_from_uart(border_packet_t *packet, int len) {
    if (packet->type == BORDER_PACKET_ACK_TYPE) {
        if (in_window(packet->seq_num, slwin_stat.last_ack+1, slwin_stat.last_frame)) {
            if (synack_seqnum == packet->seq_num) {
                synack_seqnum = -1;
                sem_signal(&connection_established);
            }
            do {
                struct send_slot *slot;
                slot = &(slwin_stat.send_win[++slwin_stat.last_ack % BORDER_SWS]);
                vtimer_remove(&slot->timeout);
                memset(&slot->frame,0,BORDER_BUFFER_SIZE);
                sem_signal(&slwin_stat.send_win_not_full);
            } while (slwin_stat.last_ack != packet->seq_num);
        }
    } else {
        struct recv_slot *slot;
        
        slot = &(slwin_stat.recv_win[packet->seq_num % BORDER_RWS]);
        if (    !in_window(packet->seq_num, 
                slwin_stat.next_exp, 
                slwin_stat.next_exp + BORDER_RWS - 1)) {
            return;
        }
        
        memcpy(slot->frame, (uint8_t *)packet, len);
        slot->received = 1;
        
        if (packet->seq_num == slwin_stat.next_exp) {
            while (slot->received) {
                demultiplex((border_packet_t *)slot->frame, slot->frame_len);
                memset(&slot->frame,0,BORDER_BUFFER_SIZE);
                slot->received = 0;
                slot = &slwin_stat.recv_win[++(slwin_stat.next_exp) % BORDER_RWS];
            }
        }
        
        send_ack(slwin_stat.next_exp - 1);
    }
}
