#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <thread.h>
#include <msg.h>

#include <transceiver.h>

#define PAYLOAD_SIZE  (0)

/* supported transceivers */
#ifdef MODULE_CC110X_NG
#include <cc1100_ng.h>
#if (CC1100_MAX_DATA_LENGTH > PAYLOAD_SIZE)
    #undef PAYLOAD_SIZE
    #define PAYLOAD_SIZE (CC1100_MAX_DATA_LENGTH)
#endif
#endif

#define ENABLE_DEBUG (1)
#include <debug.h>

/* used transceiver types */
transceiver_type_t transceivers = TRANSCEIVER_NONE;

registered_t reg[TRANSCEIVER_MAX_REGISTERED];

/* packet buffers */
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
uint8_t data_buffer[TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE];

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

/* transceiver stack */
char transceiver_stack[TRANSCEIVER_STACK_SIZE];

/* function prototypes */
static void run(void);
static void receive_packet(uint16_t type, uint8_t pos);
static void receive_cc1100_packet(radio_packet_t *trans_p);
static uint8_t send_packet(transceiver_type_t t, radio_packet_t *pkt);

void transceiver_init(transceiver_type_t t) {
    uint8_t i;
    for (i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        reg[i].transceivers = TRANSCEIVER_NONE;
        reg[i].pid          = 0;
    }
    if (t & TRANSCEIVER_CC1100) {
        transceivers |= t;
    }
    else {
        puts("Invalid transceiver type");
    }
}

int transceiver_start(void) {
    int pid = thread_create(transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN-3, CREATE_STACKTEST, run, "Transceiver");
    if (pid < 0) {
        puts("Error creating transceiver thread");
    }
    else if (transceivers & TRANSCEIVER_CC1100) {
        DEBUG("Transceiver started for CC1100\n");
        cc1100_init(pid);
    }
    return pid;
}

uint8_t transceiver_register(transceiver_type_t t, int pid) {
   uint8_t i;
   for (i = 0; ((reg[i].pid != pid) && 
               (i < TRANSCEIVER_MAX_REGISTERED) && 
               (reg[i].transceivers != TRANSCEIVER_NONE)); i++);

   if (i >= TRANSCEIVER_MAX_REGISTERED) {
       return ENOMEM;
   }
   else {
       reg[i].transceivers |= t;
       reg[i].pid         = pid;
       DEBUG("Thread %i registered for %i\n", reg[i].pid, reg[i].transceivers);
       return 1;
   }
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/
void run(void) {
    msg m;
    send_packet_t *spkt;

    while (1) {
        DEBUG("Waiting for messages\n");
        /* TODO: check here if message was lost, while handling the last */
        msg_receive(&m);
        DEBUG("Transceiver: Message received\n");
        switch (m.type) {
            case RCV_PKT_CC1020:
            case RCV_PKT_CC1100:
                receive_packet(m.type, m.content.value);
                break;
            case SND_PKT:
                spkt = (send_packet_t*) m.content.ptr;
                send_packet(spkt->transceivers, spkt->packet);
                break;
            default:
                DEBUG("Unknown message received\n");
                break;
        }
    }
}


static void receive_packet(uint16_t type, uint8_t pos) {
    uint8_t i = 0;
    transceiver_type_t t;
    rx_buffer_pos = pos;
    msg m;
   
    switch (type) {
        case RCV_PKT_CC1020:
            t = TRANSCEIVER_CC1020;
            break;
        case RCV_PKT_CC1100:
            t = TRANSCEIVER_CC1100;
            break;
        default:
            t = TRANSCEIVER_NONE;
            break;
    }

    /* search first free position in transceiver buffer */
    while ((transceiver_buffer[transceiver_buffer_pos].processing) && (transceiver_buffer_pos < TRANSCEIVER_BUFFER_SIZE))
    {
        transceiver_buffer_pos++;
    }
    /* no buffer left */
    if (transceiver_buffer_pos >= TRANSCEIVER_BUFFER_SIZE) {
        /* inform upper layers of lost packet */
        m.type = ENOBUFFER;
        m.content.value = t;
    }
    /* copy packet and handle it */
    else {
        radio_packet_t trans_p = transceiver_buffer[transceiver_buffer_pos];
        m.type = PKT_PENDING;

        if (type == RCV_PKT_CC1100) {
            receive_cc1100_packet(&trans_p); 
        }
        else {
            puts("Invalid transceiver type");
            return;
        }
    }

    /* finally notify waiting upper layers
     * this is done non-blocking, so packets can get lost */
    while (reg[i].transceivers != TRANSCEIVER_NONE) {
        if (reg[i].transceivers & t) {
            m.content.value = transceiver_buffer_pos;
            msg_send(&m, reg[i].pid, false);
        }
        i++;
    }
}

static void receive_cc1100_packet(radio_packet_t *trans_p) {
    /* disable interrupts while copying packet */
    dINT();
    cc1100_packet_t p = cc1100_rx_buffer[rx_buffer_pos].packet;
    
    trans_p->src = p.phy_src;
    trans_p->dst = p.address;
    trans_p->rssi = cc1100_rx_buffer[rx_buffer_pos].rssi;
    trans_p->lqi = cc1100_rx_buffer[rx_buffer_pos].lqi;
    trans_p->length = p.length;
    memcpy((void*) &(data_buffer[transceiver_buffer_pos]), p.data, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t*) &(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
}
 

static uint8_t send_packet(transceiver_type_t t, radio_packet_t *pkt) {
    uint8_t res = 0;
    return res;
}
