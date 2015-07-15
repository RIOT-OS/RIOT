/*
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup MAC
 * @{
 * @file    iqueue_MAC.c
 * @brief   Implementation of the CoSenS MAC protocol for RIOT OS
 * @details Implementation of the CoSenS MAC/radio duty cycling protocol;
 *          designed by Shuguo Zhuo and Ye-Qiong Song, see:
 *          Ad Hoc Networks 10 (2012) 680-695.
 * @author  Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include "shuguo.h"

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

#include "hwtimer.h"
#include "thread.h"
#include "msg.h"
#include "sched.h"
#include "crash.h"

#include "random.h"

#include "board.h"
//#include "radio_helper.h"

#define ENABLE_DEBUG  0  ///remove all the printing by setting this to 0
#include "debug.h"

#include "beacon.h"
//#include "ieee802154_frame.h"

#define PERFORM_EXTENDED_CALLBACKS 0

/****************************************************************************/
/*     Constants/config. parameters of the iQueue-MAC protocol's MAC driver     */
/****************************************************************************/

/* tune this value to fit your mote!!! */
#define IQUEUE_THREAD_STACK_SIZE   256
#define IQUEUE_THREAD_PRIORITY   2
#define IQUEUE_MSG_QUEUE_DEPTH   4  // 4 ou 8

#define MAX_OUTGOING_PACKETS_NUMBER   25

#define MAX_DATA_LENGTH 110

#define MAC_MIN_BE     2
#define MAC_MAX_BE     5
#define A_UNIT_BACKOFF_PERIOD_uS      320

#define MAC_MAX_CSMA_BACKOFFS       5

#define MAC_MAX_FRAME_RETRIES       7

/*iQueue-MAC paramenter setting */
#define A_UNIT_SLEEP_PERIOD_uS      14000
#define A_UNIT_WAKEUP_PERIOD_uS     1000
#define A_UNIT_SLEEP_INTERVAL_uS    15000


#define A_UNIT_SLEEP_PERIOD_t     HWTIMER_TICKS(A_UNIT_SLEEP_PERIOD_uS)
#define A_UNIT_WAKEUP_PERIOD_t     HWTIMER_TICKS(A_UNIT_WAKEUP_PERIOD_uS)
#define A_UNIT_CP_PERIOD_t     HWTIMER_TICKS(A_UNIT_CP_PERIOD_uS)

#define MAX_PREAMBLE_MUN   10 
#define A_UNIT_PREAMBLE_INTERVAL_uS      1000
#define A_UNIT_PREAMBLE_INTERVAL_t     HWTIMER_TICKS(A_UNIT_PREAMBLE_INTERVAL_uS)

#define MAC_BEACON_PERIOD   1 
#define MAC_VTDMA_PERIOD    2 
#define MAC_SP_PERIOD   3 
#define MAC_CP_PERIOD   4 
#define MAC_PREAMBLE_PERIOD   5 
#define MAC_TP_PERIOD   6 

#define MAX_NEIGHBOR_ROUTER_NUM   20

#define ROUTER_NEXT_HOP_ID  4

/*iQueue-MAC simple nodes paramenter setting */
#define MAC_SIMPLE_CHECK_PERIOD   1 
#define MAC_WAIT_BEACON_PERIOD    2 
#define MAC_SIMPLE_VTDMA_PERIOD   3 
#define MAC_SIMPLE_SP_PERIOD   4 
#define MAC_SIMPLE_CP_PERIOD   5 


#define ROUTER_MODE   1 
#define SIMPLE_MODE   2

#define PACKET_KIND_PREAMBLE 0x55
#define PACKET_KIND_PREAMBLE_ACK 0x66

/****************************************************************************/
/*              Variables of the iQueue-MAC protocol's MAC driver               */
/****************************************************************************/

typedef unsigned long hwtimer_ticks_t;


/* iQueue-MAC thread ID */
static volatile int iqueue_mac_tid;

/* iQueue-MAC thread's stack */
static char iqueue_mac_stack[IQUEUE_THREAD_STACK_SIZE];

/* iQueue-MAC thread's message queue */
static msg_t iqueue_mac_msg_queue[IQUEUE_MSG_QUEUE_DEPTH];

/* iQueue-MAC activity flag */
static volatile bool iqueue_run;

/* internal message types for iQueue-MAC */
typedef enum {     
    IQUEUE_BEACON_MANAGE,  
    IQUEUE_VTDMA_MANAGE,   
    IQUEUE_SP_MANAGE,   
    IQUEUE_CP_MANAGE,   
    IQUEUE_PREAMBLE_MANAGE,
    IQUEUE_TP_MANAGE,   
    
    IQUEUE_SIMPLE_GOT_BEACON,
    IQUEUE_SIMPLE_TDMA_MANAGE,
    IQUEUE_SIMPLE_SP_MANAGE,
    IQUEUE_SIMPLE_CP_MANAGE,
    IQUEUE_GET_NEW_PACKET,
    IQUEUE_EMIT_PACKET,
    IQUEUE_WAIT,    
    //IQUEUE_SIMPLE_CHECK_DATA,
    IQUEUE_STOP
} iqueue_msg_t;


/* queue of packets to be send during next transmission period */
typedef struct out_pkt {
    struct out_pkt *next;    
    void *pkt_data;
    unsigned int data_len;
#if PERFORM_EXTENDED_CALLBACKS
    mac_tx_status_callback_t callback;
    void *callback_param;
#endif /* PERFORM_EXTENDED_CALLBACKS */
} out_pkt_t;
static out_pkt_t outgoing_queue[MAX_OUTGOING_PACKETS_NUMBER];
static out_pkt_t * volatile first_outgoing_packet;

typedef struct out_pkt_buffer {    
   u8 pkt_data_buf[MAX_DATA_LENGTH]    
} out_pkt_t_buffer;

static out_pkt_t_buffer outgoing_queue_buf[MAX_OUTGOING_PACKETS_NUMBER];
static u8 queue_buffer_tail=0;


/* Callback function to relay incoming packets to other net layers or apps */
static mac_rx_packet_notif_t rx_callback_func = NULL;


/* flag indicating whether we can transmit in burst (fast) mode,
   or whether we must use classical CSMA/CA method. */
static bool in_burst_mode = false;

/* number of backoffs waited for accessing channel; used in CSMA/CA mode */
static int num_backoffs = 0;

/* number of attempted transmission for the packet currently emitted;
   used in CSMA/CA mode */
static int num_tx = 0;


/* iQueue-MAC parameters */
u8 iqueue_mac_mode = SIMPLE_MODE; ///  ROUTER_MODE; //                                

u8 *rx_data_buf;

//////Beacon manage parameters
extern u8 beacon_buf[120];
//////Subframe manage parameters
extern u32 subframeduration;
extern u32 cp_duration_us;
extern u32 tdma_duration;
extern u32 sp_duration;
extern bool vtdma_exist;
extern node_regis_struc node_regis_list[MAX_LOCAL_NODE_NUM];

hwtimer_ticks_t sp_start_time_t = 0;
//////SP manage parameters
bool sp_turn_to_sleep_state = false;
bool sp_end = false;
//////CP manage parameters
volatile bool cp_got_new_packet = false;
//////TP manage parameters
u8 tp_preamble_counter = MAX_PREAMBLE_MUN;
bool tp_get_preamble_ack = false;
bool tp_start_trans_preamble = false;
bool channel_busy = false;
u8 preamble_buf[50];

unsigned short neighbor_router_ID[MAX_NEIGHBOR_ROUTER_NUM]; 


static volatile u8 mac_state = 0;

ieee802154_node_addr_t iq_dest;
ieee802154_node_addr_t router_dest;


/* iQueue-MAC parameters - simple node*/
volatile u8 mac_queue_length = 0;
u32 simple_slots_start_time_us = 0;
static bool simple_node_get_beacon = false;
static bool simple_node_get_slots = false;

u16 mac_short_address = 0;
u8 simple_slots_num = 0;
volatile bool end_csma_backoff = false;


double Drand(void);
static void mac_send_data_in_iqueue(ieee802154_node_addr_t dest,                                     
                                    bool use_long_addr,         
                                    bool wants_ack,                              
                                    bool csma_enable, bool insist);
                                    
mac_return_t iqueue_send_packet(void *buf,
                                unsigned int len,
                                mac_tx_status_callback_t tx_status_callback,
                                void *callback_param);

/****************************************************************************/
/*                         iQueue-MAC utility functions                      /
/****************************************************************************/

static inline void send_iqueue_mac_message(iqueue_msg_t msg_type, uint32_t param)
{
    msg_t new_msg;
    new_msg.type = msg_type;
    new_msg.content.value = param;
    int res = msg_send(&new_msg, iqueue_mac_tid, false);
    if (res < 1) {
        core_panic(res, "Couldn't send message to IQUEUE thread");
    }
}

/* choose an adequate backoff period */
static inline hwtimer_ticks_t choose_backoff_period(int num_tx)
{
    int be = MAC_MIN_BE + num_tx - 1;
    if (be < MAC_MIN_BE) {
        be = MAC_MIN_BE;
    }
    if (be > MAC_MAX_BE) {
        be = MAC_MAX_BE;
    }
    hwtimer_ticks_t max_backoff =
            HWTIMER_TICKS(((1 << be) - 1) * A_UNIT_BACKOFF_PERIOD_uS);

    hwtimer_ticks_t period = genrand_uint32() % max_backoff;
    if (period < 1) period = 1;
    return period;
}


/* enqueue a packet for transmission */
bool push_out_packet(void *buf, unsigned int len,
                            mac_tx_status_callback_t cb, void *param)
{
  u8 *rec_buf;
  rec_buf = buf;
    /* look for an empty out_pkt_t descriptor */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        if (outgoing_queue[i].pkt_data == NULL) {
            /* found: initialize the out_pkt_t descriptor accordingly... */
            outgoing_queue[i].next = NULL;
            
            /* shuguo revise */
            int abc;
            for(abc=0;abc<len;abc++)
            {
              outgoing_queue_buf[queue_buffer_tail].pkt_data_buf[abc] = rec_buf[abc];            
            } 
            
            //printf("The pushed first byte is %2x.\n",  rec_buf[0]);
            //printf("The pushed second byte is %2x.\n",  rec_buf[1]);
            //printf("The pushed third byte is %2x.\n",  rec_buf[2]);        
            //printf("The pushed fourth byte is %2x.\n",  outgoing_queue_buf[queue_buffer_tail].pkt_data_buf[3]);
                       
            outgoing_queue[i].pkt_data = outgoing_queue_buf[queue_buffer_tail].pkt_data_buf;                
            outgoing_queue[i].data_len = len;
            
            queue_buffer_tail ++;
            if(queue_buffer_tail >= MAX_OUTGOING_PACKETS_NUMBER){
              queue_buffer_tail = 0;
            }
            
#if PERFORM_EXTENDED_CALLBACKS
            outgoing_queue[i].callback = cb;
            outgoing_queue[i].callback_param = param;
#endif /* PERFORM_EXTENDED_CALLBACKS */
            /* ... and put it at the end of the list */
            if (first_outgoing_packet == NULL) {
                first_outgoing_packet = &(outgoing_queue[i]);
            } else {
                out_pkt_t *node = first_outgoing_packet;
                while (node->next != NULL) {
                    node = node->next;
                }
                node->next = &(outgoing_queue[i]);
            }            
            DEBUG("*** iQueue-MAC: Successfully inserted a packet!!!\n");  ////need \n to finish the DEBUG, otherwise, stops!
            
            if((iqueue_mac_mode == SIMPLE_MODE)&&(mac_state == MAC_SIMPLE_CHECK_PERIOD))
            {
              /* enable receiving beacon */
              //mac_turnon_radio();  
              
              mac_state = MAC_WAIT_BEACON_PERIOD;
              simple_node_get_beacon = false;
              simple_node_get_slots = false;
              DEBUG("*** iQueue-MAC: switch to wait beacon!!!\n");
              //send_iqueue_mac_message(IQUEUE_GET_NEW_PACKET, 0L); 
            }
            
            if(mac_queue_length <= MAX_OUTGOING_PACKETS_NUMBER)
            {
              mac_queue_length ++;
            }else{
              mac_queue_length = MAX_OUTGOING_PACKETS_NUMBER;
            }
            
            /*
            if(iqueue_mac_mode == ROUTER_MODE)
            {
              printf("The router queue-length is %2x.\n", mac_queue_length);
            }*/
            
            return true;
        }
    }
    /* if we arrive here, there is no more slots
       available for outgoing packets */
    return false;
}

/* free a slot of the outgoing packets queue */
static void pop_packet_from_queue(void)
{
    if (first_outgoing_packet == NULL) {
        /* no enqueued packet */
        return;
    }

    out_pkt_t *node = first_outgoing_packet;

    first_outgoing_packet = node->next;
    
    mac_queue_length --; 

    node->next = NULL;
    node->pkt_data = NULL;
    node->data_len = 0;
#if PERFORM_EXTENDED_CALLBACKS
    node->callback = NULL;
    node->callback_param = NULL;
#endif
}


/****************************************************************************/
/*                    iQueue-MAC period-management functions                    */
/****************************************************************************/

/* executing transmission period
   i.e.: handle the emission of outgoing packets, one after another */
static void do_transmit_2(void)
{
   hwtimer_spin(500);
   send_iqueue_mac_message(IQUEUE_WAIT, 0L);   ////send_iqueue_mac_message(IQUEUE_EMIT_PACKET, 0L);
   DEBUG("IQUEUE: No buffered packet, quit do_transmit!!\n");
   return;
}
static void do_transmit(void)
{
    /* outcome of the radio transmission */
    int result;
    /* backoff duration, before next emission attempt */
    hwtimer_ticks_t backoff_delay;
    /* next packet to send */
    out_pkt_t *curr_pkt = first_outgoing_packet;
    
    DEBUG("IQUEUE: Begin do_transmit!!\n");
    
    /* if all transmissions have been done, switch to waiting mode */
    if (curr_pkt == NULL) {
        send_iqueue_mac_message(IQUEUE_WAIT, 0L);   ////send_iqueue_mac_message(IQUEUE_EMIT_PACKET, 0L);
        DEBUG("IQUEUE: No buffered packet, quit do_transmit!!\n");
        return;
    }

    /* if next transmission must use the CSMA/CA method... */
    if (!in_burst_mode) {
        num_backoffs++;
        /* check if channel is clear, otherwise back off */
        if (!(BOARD_RADIO_DRIVER.channel_is_clear())) {   // XXX provisoire
            /* too much failed attempts: give up! */
            if (num_backoffs > MAC_MAX_CSMA_BACKOFFS) {
                DEBUG("IQUEUE: UNABLE TO ACCESS CHANNEL"
                      " AFTER %d BACKOFFS!\n", num_tx);
                DEBUG("IQUEUE: GIVING UP TRANSMISSION\n");
#if PERFORM_EXTENDED_CALLBACKS
                /* warn higher layers of netstack of failure */
                curr_pkt->callback(curr_seq_num,
                                   curr_pkt->callback_param,
                                   MAC_COLLISION,
                                   0);
#endif /* PERFORM_EXTENDED_CALLBACKS */
                /* remove the failed packet from queue
                   and free its corresponding slot */
                pop_packet_from_queue();
            }
            /* back off */
            backoff_delay = choose_backoff_period(num_backoffs);
            hwtimer_spin(backoff_delay);  ///hwtimer_wait()
            send_iqueue_mac_message(IQUEUE_EMIT_PACKET, 0L);
            return;
        }
    }
    num_backoffs = 0;

    LED_BLUE_ON;
    /* we go directly here if transmission can be done in burst mode */
    ieee802154_node_addr_t dest;   
    dest.pan.addr = 0xFFFF;
    dest.pan.id = BOARD_RADIO_DRIVER.get_pan_id();    // XXX provisoire
    DEBUG("IQUEUE: now emitting 802.15.4 packet...\n");
    //result = BOARD_RADIO_DRIVER.send(PACKET_KIND_DATA,dest,false,true, 
    //                    curr_pkt->pkt_data, curr_pkt->data_len);   // XXX provisoire
    LED_BLUE_OFF;
    
    /* check how transmission went */
    if (result >= RADIO_TX_OK) {
        /* success */
        DEBUG("IQUEUE-MAC: packet successfully transmitted.\n");
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();
        
#if PERFORM_EXTENDED_CALLBACKS
        /* ... warn higher layers that transmission is done... */
        curr_pkt->callback(curr_seq_num,
                           curr_pkt->callback_param,
                           MAC_OK,
                           num_tx);
#endif /* PERFORM_EXTENDED_CALLBACKS */
        /* ... reset counter of attempted transmissions... */
        num_tx = 0;
        /* ... can switch to "faster" burst mode... */
        in_burst_mode = true;
        DEBUG("IQUEUE-MAC: switching to burst transmission mode.\n");
        /* ... continue with next transmission ASAP... */
        backoff_delay = 0;

    } else {
        num_tx++;
        /* too much failed attempts: give up! */
        if (num_tx > MAC_MAX_FRAME_RETRIES) {
            DEBUG("IQUEUE-MAC: UNABLE TO EMIT PACKET"
                  " AFTER %d ATTEMPTS!\n", num_tx);
            DEBUG("IQUEUE-MAC: GIVING UP TRANSMISSION\n");
#if PERFORM_EXTENDED_CALLBACKS
            /* warn higher layers of netstack of failure */
            curr_pkt->callback(curr_seq_num,
                               curr_pkt->callback_param,
                               MAC_ERROR,
                               num_tx);
#endif /* PERFORM_EXTENDED_CALLBACKS */
            /* remove the failed packet from queue
               and free its corresponding slot */
            pop_packet_from_queue();
            /* reset counter of attempted transmissions... */
            num_tx = 0;
            /* ... and continue with next packet ASAP */
            backoff_delay = 0;
        }

        if (result == RADIO_TX_COLLISION) {
            /* collision */
            DEBUG("IQUEUE-MAC: collision during emission!"
                  " Retrying in CSMA/CA mode.\n");
#if PERFORM_EXTENDED_CALLBACKS
            /* ... warn higher layers of collision... */
            curr_pkt->callback(curr_seq_num,
                               curr_pkt->callback_param,
                               MAC_COLLISION,
                               num_tx);
#endif /* PERFORM_EXTENDED_CALLBACKS */
            /* ... then retry, reverting to "safer" CSMA/CA mode... */
            in_burst_mode = false;
            /* ... after backing off for a while... */
            backoff_delay = choose_backoff_period(num_tx);

        } else if(result == RADIO_TX_NOACK) {
            /* no response from peer... */
            DEBUG("IQUEUE-MAC: no ACK received from peer..."
                  " Retrying in CSMA/CA mode.\n");
#if PERFORM_EXTENDED_CALLBACKS
            /* ... warn higher layers of absence of ack... */
            curr_pkt->callback(curr_seq_num,
                               curr_pkt->callback_param,
                               MAC_TX_NO_ACK,
                               num_tx);
#endif /* PERFORM_EXTENDED_CALLBACKS */
            /* ... revert to "safer" CSMA/CA mode... */
            in_burst_mode = false;
            /* ... and retry ASAP */
            backoff_delay = 0;

        } else { /* RADIO_TX_ERR */
#if PERFORM_EXTENDED_CALLBACKS
            /* warn higher layers of netstack of failure */
            curr_pkt->callback(curr_seq_num,
                               curr_pkt->callback_param,
                               MAC_ERROR,
                               num_tx);
#endif /* PERFORM_EXTENDED_CALLBACKS */
            /* halt system */
            core_panic(0xdead,
                       "*** FATAL! ERROR OCCURED DURING PACKET TRANSMISSION"
                       " (RADIO FAILURE)! ***");
            // we won't go past here!!
        }
    }

    /* start next transmission, after the appropriate backoff delay */
    hwtimer_spin(50);  
    send_iqueue_mac_message(IQUEUE_BEACON_MANAGE, 0L);
}


/* ask the switch to the transmit period -- function called by hwtimer */
static void ask_start_superframe(void *unused)
{
      
    if(iqueue_mac_mode == ROUTER_MODE)
    { 
      send_iqueue_mac_message(IQUEUE_BEACON_MANAGE, 0L);
      mac_state = MAC_BEACON_PERIOD;
    }
    else
    {      
      //send_iqueue_mac_message(IQUEUE_SIMPLE_CHECK_DATA, 0L);
      //mac_state = MAC_SIMPLE_CHECK_PERIOD;
    }
    //LED_BLUE_OFF;
}


static void begin_wait(void)
{ 
    if (!iqueue_run) return;   /* don't start a new cycle if off */ 
    
    DEBUG("iQueue-MAC: switching to Beaocn/Checking period...\n");     
    
    
    /* defines the timer that will switch to transmission period */
    int res = hwtimer_set(500, ask_start_superframe, NULL);
    if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }

}

/*
static void wait_process(void)
{
    if (first_outgoing_packet == NULL) {
         send_iqueue_mac_message(IQUEUE_WAIT, 0L);
       }else{
        send_iqueue_mac_message(IQUEUE_EMIT_PACKET, 0L);
       }    
}
*/


void mac_send_beacon(void)
{   
    /* next waiting period duration in us */  
    iq_dest.pan.addr = 0xFFFF; 
    
    LED_BLUE_ON;
    DEBUG("iQueue-MAC: Broadcasting beacon packet... \n");
    //ieee802154_node_t dest;
    //dest.addr = 0xFFFF;
    //dest.pan_id = cc2420_get_pan();    // XXX provisoire
    int result = BOARD_RADIO_DRIVER.send(PACKET_KIND_BEACON,iq_dest,false,false,
                            beacon_buf, beacon_buf[0]);   // XXX provisoire
                            
    LED_BLUE_OFF;                               
    
    DEBUG("iQueue-MAC: Finished broadcast beacon packet... \n");
}



static void mac_beacon_manage(void)
{   
    BOARD_RADIO_DRIVER.set_promiscuous_mode(false); 
    
    DEBUG("iQueue-MAC: Enter Beacon manage period\n");
    mac_creat_beacon();    
    
    DEBUG("iQueue-MAC: Start to send Beacon\n");
    mac_send_beacon();    

    //vtdma_exist=true; 
   
    if(vtdma_exist==true)
    {
      DEBUG("iQueue-MAC: Slots allocated, going to vTDMA period\n");
      send_iqueue_mac_message(IQUEUE_VTDMA_MANAGE, 0L);  
      mac_state = MAC_VTDMA_PERIOD;
    } 
    else
    {
      DEBUG("iQueue-MAC: No Slots allocated, going to SP period\n");
      send_iqueue_mac_message(IQUEUE_SP_MANAGE, 0L); 
      mac_state = MAC_SP_PERIOD;
    }
    //hwtimer_wait(200);

}

static void mac_ask_end_vTDMA(void)
{
  /* Switch the channel to the subchannel*/
  //mac_switch_to_mainchannel();
  
  //hwtimer_wait(200);
  DEBUG("iQueue-MAC: End vTDMA manage period\n");
  send_iqueue_mac_message(IQUEUE_SP_MANAGE, 0L);   
  mac_state = MAC_SP_PERIOD;

}

static void mac_vTDMA_manage(void)
{   
    hwtimer_ticks_t tdma_duration_t=0;
    DEBUG("iQueue-MAC: Enter vTDMA manage period\n");
        
    /* Switch the channel to the subchannel*/
    //mac_switch_to_subchannel();
        
    tdma_duration_t = HWTIMER_TICKS(tdma_duration);
    
    /* defines the timer that will switch to SP period */  //
    int res = hwtimer_set(tdma_duration_t, mac_ask_end_vTDMA, NULL);
    if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }   
    
}

static void mac_SP_init(void)   
{     
   sp_duration = subframeduration - tdma_duration;

   DEBUG("iQueue-MAC: The vTDMA duration is %lu\n",tdma_duration);
   DEBUG("iQueue-MAC: The SP duration is %lu\n",sp_duration);
   
   sp_start_time_t = hwtimer_now();
   
   sp_turn_to_sleep_state = true;
   sp_end = false;

}

static void sp_switch_to_cp(void)
{
  //mac_turn_on_radio();
  LED_BLUE_OFF;
  DEBUG("iQueue-MAC: Leaving SP \n");
  send_iqueue_mac_message(IQUEUE_CP_MANAGE, 0L); 
  mac_state = MAC_CP_PERIOD;
}

static void mac_SP_action_judge(void)
{
  hwtimer_ticks_t sp_duration_t;
  hwtimer_ticks_t sp_remain_time_t;
  hwtimer_ticks_t sp_past_time_t;
  
  sp_duration_t = HWTIMER_TICKS(sp_duration);
  sp_past_time_t = hwtimer_now()-sp_start_time_t;
  
  if(sp_duration_t >sp_past_time_t)
    sp_remain_time_t = sp_duration_t - sp_past_time_t;
  else
  {
    sp_end = true;    
  }
  
  if(sp_end)  ///do not delete this judge
  {
    sp_switch_to_cp();
    return;
  }
  
  if(sp_turn_to_sleep_state==true)
  {
    LED_BLUE_OFF;
    sp_turn_to_sleep_state=false;
    //mac_turn_off_radio();
    //DEBUG("iQueue-MAC: Enter SP sleeping period\n");
    
    if(sp_remain_time_t > A_UNIT_SLEEP_PERIOD_t)
    {      
      int res = hwtimer_set(A_UNIT_SLEEP_PERIOD_t, mac_SP_action_judge, NULL);
       if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
      }        
    }
    else
    {
      int res = hwtimer_set(sp_remain_time_t, mac_SP_action_judge, NULL);
       if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
      } 
      sp_end = true;     
    }    
  }
  else
  {
    LED_BLUE_ON;
    sp_turn_to_sleep_state=true;   ///if the wakeup period needs to be extended, just modify this variable.
    //mac_turn_on_radio();    
    //DEBUG("iQueue-MAC: Enter SP waking period\n");
    
    if(sp_remain_time_t <= A_UNIT_WAKEUP_PERIOD_t)
    {
      sp_end = true;
      sp_switch_to_cp();
      return;
    }
    else
    {
      int res = hwtimer_set(A_UNIT_WAKEUP_PERIOD_t, mac_SP_action_judge, NULL);
       if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
      }    
    }
  }

}

static void mac_SP_manage(void)
{   

    DEBUG("iQueue-MAC: Enter SP manage period\n");
    mac_SP_init();
        
    mac_SP_action_judge();    
}


static void mac_ask_end_CP(void)
{
  if(cp_got_new_packet)
  {
    cp_got_new_packet = false;
    DEBUG("iQueue-MAC: Got new packet, continue CP \n");
    int res = hwtimer_set(A_UNIT_CP_PERIOD_t, mac_ask_end_CP, NULL);
    if (res < 0) {
      core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }      
  }
  else
  {  
    LED_BLUE_OFF;
    DEBUG("iQueue-MAC: No more packet, Leaving CP \n");
    
    if(mac_queue_length > 0)
    {
      send_iqueue_mac_message(IQUEUE_PREAMBLE_MANAGE, 0L);
      mac_state = MAC_PREAMBLE_PERIOD;
    }else{
      send_iqueue_mac_message(IQUEUE_BEACON_MANAGE, 0L);
      mac_state = MAC_BEACON_PERIOD;    
    }
    
  }
}

static void mac_CP_manage(void)
{   

    DEBUG("iQueue-MAC: Enter CP manage period\n");
    LED_BLUE_ON;
    cp_got_new_packet = false;
    
    BOARD_RADIO_DRIVER.set_promiscuous_mode(true);  
        
    int res = hwtimer_set(A_UNIT_CP_PERIOD_t, mac_ask_end_CP, NULL);
    if (res < 0) {
      core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }            
          
}


static void mac_preamble_init(void)
{
  tp_preamble_counter = MAX_PREAMBLE_MUN;
  tp_get_preamble_ack = false;
  tp_start_trans_preamble = true;
  channel_busy = false;
  iq_dest.pan.addr = ROUTER_NEXT_HOP_ID;  //0xFFFF;  
  
  preamble_buf[0] = PACKET_KIND_PREAMBLE;     /* identifies a "beacon" for S-CoSens */
  /* next subframeduration period duration in us */
  preamble_buf[1] = 0x55;
  preamble_buf[2] = 0x55;
  preamble_buf[3] = 0x55;
  preamble_buf[4] = 0x55;
  /* next waiting period duration in us */  

}

static void mac_tp_send_preamble_2(void)
{    
  if(mac_state != MAC_PREAMBLE_PERIOD)  //if the MAC is already in TP sending period, quit further operations of preamble!
  {
    return;
  }  
  
  if(channel_busy)
  {
    DEBUG("iQueue-MAC: Channel busy, go back to CP \n"); 
    send_iqueue_mac_message(IQUEUE_CP_MANAGE, 0L);
    mac_state = MAC_CP_PERIOD;
    return;
  }
  else
  {
    if(tp_get_preamble_ack)
    {
      BOARD_RADIO_DRIVER.set_promiscuous_mode(false); 
      DEBUG("iQueue-MAC: Get Preamble-ACK, go to TP \n");      
      send_iqueue_mac_message(IQUEUE_TP_MANAGE, 0L);
      mac_state = MAC_TP_PERIOD;
      return;
    }
    else
    {
      if(tp_preamble_counter==0)
      {
        BOARD_RADIO_DRIVER.set_promiscuous_mode(false); 
        DEBUG("iQueue-MAC: No Preamble-ACK, Enter next cycle-ShuGuo \n");
        send_iqueue_mac_message(IQUEUE_BEACON_MANAGE, 0L);
        mac_state = MAC_BEACON_PERIOD;      
        return;
      }
    }
  }   
    
  LED_BLUE_ON;
  //DEBUG("iQueue-MAC: Broadcasting preamble packet... \n");  
  int result = BOARD_RADIO_DRIVER.send(PACKET_KIND_DATA,iq_dest,false,false,
                          preamble_buf, 2);   // XXX provisoire
                                                   
  LED_BLUE_OFF;
  tp_preamble_counter --;       
    
  int res = hwtimer_set(A_UNIT_PREAMBLE_INTERVAL_t, mac_tp_send_preamble_2, NULL);
  if (res < 0) {
      core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
  }                       
}

static void mac_preamble_manage(void)
{  
   DEBUG("iQueue-MAC: Enter Preamble manage period\n"); 
   mac_preamble_init();
   
   mac_tp_send_preamble_2();

}


static void mac_burst_relay_data(void)
{

  DEBUG("iQueue-MAC: Start Burst relaying data \n");  
  
  bool want_ack = true;
  bool use_long_add = false;
  bool use_csma = false;
  bool use_inist_send = false;  
  
  //router_dest.pan.addr = ROUTER_NEXT_HOP_ID;  
  
  iq_dest.pan.addr = ROUTER_NEXT_HOP_ID; 
  //iq_dest.pan.id = BOARD_RADIO_DRIVER.get_pan_id();
  
  while(mac_queue_length > 0)
  {
    if(mac_queue_length > 0)
    { 
      DEBUG("iQueue-MAC: Router Send packets in TP.\n");  
      mac_send_data_in_iqueue(iq_dest,use_long_add,want_ack,use_csma,use_inist_send);                
    }
    else{
      break;
    }
  }
}

static void mac_TP_manage(void)
{   

    DEBUG("iQueue-MAC: Enter TP manage period\n");   
    
    mac_burst_relay_data();
    send_iqueue_mac_message(IQUEUE_BEACON_MANAGE, 0L);
    mac_state = MAC_BEACON_PERIOD;
      
}

static bool is_neighbor_router(u16 received_node_ID)
{
  int i;
  for(i=0;i<MAX_NEIGHBOR_ROUTER_NUM;i++)
  {
    if(neighbor_router_ID[i] == received_node_ID)
    {      
      //printf("Registing router %2x.\n", received_node_ID);
      return true;
    }    
  }  
  return false;   
}

static void record_neighbor_routerID(u16 received_node_ID)
{
  //u16 received_node_ID = 0;  
  //u16 received_dest_ID = 0;
  
  /*
  received_dest_ID = rx_data_buf[6];
  received_dest_ID = received_dest_ID <<8;
  received_dest_ID |= rx_data_buf[5];   
  
  if((mac_short_address != received_dest_ID)&&(mac_short_address != 0xFF))
  {  
  }  
  */ 
  int i;
  for(i=0;i<MAX_NEIGHBOR_ROUTER_NUM;i++)
  {
    if(neighbor_router_ID[i] == 0)
    {
      neighbor_router_ID[i] = received_node_ID;
      //printf("Registing router %2x.\n", received_node_ID);
      break;
    }else{
      if(neighbor_router_ID[i] == received_node_ID)
      {
        //printf("Router %2x already existed\n", received_node_ID);
        break;
      }
    }
  }
}

static void regist_simple_node_ID(u16 received_node_ID)
{
  int i;
  /* register the node  */
  for(i=0;i<MAX_LOCAL_NODE_NUM;i++)
  {
    if((node_regis_list[i].node_ID == 0)||(node_regis_list[i].node_ID == received_node_ID))
    {
      node_regis_list[i].node_ID = received_node_ID;      
      node_regis_list[i].queue_length = rx_data_buf[9]-1;
      break;
    }     
  }  
}

static void router_recive_data_process(void *buf, unsigned int len)
{
  /*Recording Queue-length information */
  u16 received_node_ID = 0;
  u16 received_dest_ID = 0;  
    
  received_dest_ID = rx_data_buf[6];
  received_dest_ID = received_dest_ID <<8;
  received_dest_ID |= rx_data_buf[5];   
  
  received_node_ID = rx_data_buf[8];
  received_node_ID = received_node_ID <<8;
  received_node_ID |= rx_data_buf[7];  
    
  /* If it is in the CP, extend the CP duration  */
  if(mac_state == MAC_CP_PERIOD)
  {
    cp_got_new_packet = true;
    LED_BLUE_ON;
  }else{   /* If it is in the Preamble period, retreat to the CP */
    if(mac_state == MAC_PREAMBLE_PERIOD)
    {
      channel_busy = true;
      send_iqueue_mac_message(IQUEUE_CP_MANAGE, 0L); 
      mac_state = MAC_CP_PERIOD;
    }
  }   
  
  //uint8_t data[110];
    
    //unsigned pkt_num = 0; 
    
  uint8_t pkt_r_data[60];
  
  if(mac_short_address == received_dest_ID)
  {  
    if(!(is_neighbor_router(received_node_ID)))
    {
      regist_simple_node_ID(received_node_ID);
    }    
    
    int i =0;
    for(i=0;i<len-8;i++)   //(len-8)
    {
      pkt_r_data[i] = rx_data_buf[i+9];            
    }
    
     push_out_packet(pkt_r_data, len-9,    
                                NULL, 0);                                         
                                         
    //push_out_packet(data,len-9,NULL,0);   //buf+9    iqueue_send_packet
  } 
    
  //printf("The received queue-length is %2x.\n",  pkt_r_data[0]);
  //printf("The first byte is %2x.\n",  pkt_r_data[1]);
  
}

static void router_recive_preamble_process(void)
{
  u8 preamble_ack[8];
  u16 received_node_ID = 0;
  u16 received_dest_ID = 0;
  
  received_dest_ID = rx_data_buf[6];
  received_dest_ID = received_dest_ID <<8;
  received_dest_ID |= rx_data_buf[5];   
    
  received_node_ID = rx_data_buf[8];
  received_node_ID = received_node_ID <<8;
  received_node_ID |= rx_data_buf[7];    
  iq_dest.pan.addr = received_node_ID;  
  
  if(mac_short_address == received_dest_ID)
  {
    preamble_ack[0] = PACKET_KIND_PREAMBLE_ACK;
  
    LED_BLUE_ON;
    //DEBUG("iQueue-MAC: Broadcasting preamble packet... \n");  
    int result = BOARD_RADIO_DRIVER.send(PACKET_KIND_DATA,iq_dest,false,false,
                          preamble_ack, 2);   // XXX provisoire
                                                   
    LED_BLUE_OFF;
  
    record_neighbor_routerID(received_node_ID);
  }
  
  /* If it is in the CP, extend the CP duration  */
  if(mac_state == MAC_CP_PERIOD)
  {
    cp_got_new_packet = true;
    LED_BLUE_ON;
  }else{   /* If it is in the Preamble period, retreat to the CP */
    if(mac_state == MAC_PREAMBLE_PERIOD)
    {
      channel_busy = true;
      send_iqueue_mac_message(IQUEUE_CP_MANAGE, 0L); 
      mac_state = MAC_CP_PERIOD;
    }
  }  
}

static void router_recive_preamble_ACK_process(void)
{
  u16 received_node_ID = 0;
  u16 received_dest_ID = 0;
  
  received_dest_ID = rx_data_buf[6];
  received_dest_ID = received_dest_ID <<8;
  received_dest_ID |= rx_data_buf[5];  
  
  
  received_node_ID = rx_data_buf[8];
  received_node_ID = received_node_ID <<8;
  received_node_ID |= rx_data_buf[7];  

  if((mac_state == MAC_PREAMBLE_PERIOD)&&(mac_short_address == received_dest_ID))
  {
      tp_get_preamble_ack = true;    
      //printf("iQueue-MAC: node %u Get Preamble-ACK, go to TP \n", mac_short_address);      
      send_iqueue_mac_message(IQUEUE_TP_MANAGE, 0L);
      mac_state = MAC_TP_PERIOD;      
  }else{
    if(mac_state == MAC_CP_PERIOD)
    {
      cp_got_new_packet = true;
    }else{   /* If it is in the Preamble period, retreat to the CP */
      if(mac_state == MAC_PREAMBLE_PERIOD)
      {
        channel_busy = true;
        send_iqueue_mac_message(IQUEUE_CP_MANAGE, 0L); 
        mac_state = MAC_CP_PERIOD;
      }
    }    
  }  
  record_neighbor_routerID(received_node_ID);
}

/* The code of the simple node mode */
static void mac_simple_wait_beacon_initiate(void)
{
  simple_node_get_beacon = false;
  simple_node_get_slots = false;
}



static void mac_simple_get_new_packet_process(void)
{
  switch(mac_state)
  {
    case MAC_SIMPLE_CHECK_PERIOD: {
     /*initialization */
     mac_simple_wait_beacon_initiate();
      
     //send_iqueue_mac_message(IQUEUE_SIMPLE_WAIT_BEACON, 0L);  
     //mac_turnon_radio(); 
     mac_state = MAC_WAIT_BEACON_PERIOD; 
     DEBUG("iQueue-MAC: go to wait beacon period\n");
    }break;   
    
    default: break;      
  }   
}



/*
static void mac_simple_check_data(void)
{

  DEBUG("iQueue-MAC: checking bufferd data ...\n"); 
  
  while(mac_queue_length == 0)
  {
    if(mac_queue_length > 0)
    {      
      break;
    }  
    hwtimer_wait(500);
  }
  
  DEBUG("iQueue-MAC: simple node gets packets to send.\n");   
  
  send_iqueue_mac_message(IQUEUE_SIMPLE_WAIT_BEACON, 0L);
  
  mac_state = MAC_WAIT_BEACON_PERIOD;
  
}
*/



static void mac_analyze_beacon(void)
{
  DEBUG("iQueue-MAC: Analyzing the received beacon.\n"); 
  
   
  simple_slots_num = 0;
  simple_slots_start_time_us = 0;
  
  sp_start_time_t = hwtimer_now();  
  simple_node_get_slots = false;
  
  
  router_dest.pan.addr = 0;
  router_dest.pan.addr = beacon_buf[8];
  router_dest.pan.addr = router_dest.pan.addr <<8;
  router_dest.pan.addr |= beacon_buf[7]; 
  
  
  subframeduration = 0;
  subframeduration = beacon_buf[13];
  subframeduration = subframeduration <<8;
  subframeduration |= beacon_buf[12]; 
  subframeduration = subframeduration <<8;
  subframeduration |= beacon_buf[11]; 
  subframeduration = subframeduration <<8;
  subframeduration |= beacon_buf[10]; 
  
  
  cp_duration_us = 0;
  cp_duration_us = beacon_buf[18];
  cp_duration_us = cp_duration_us <<8;
  cp_duration_us |= beacon_buf[17]; 
  cp_duration_us = cp_duration_us <<8;
  cp_duration_us |= beacon_buf[16]; 
  cp_duration_us = cp_duration_us <<8;
  cp_duration_us |= beacon_buf[15]; 
  
   int i;
   for(i=1;i<=beacon_buf[21];i++)
   {
     if((beacon_buf[22+i] == mac_short_address)&&(beacon_buf[22+beacon_buf[21]+i] > 0))
     {
       simple_node_get_slots = true;
       
       simple_slots_num = beacon_buf[22+beacon_buf[21]+i];
       //tdma_send_try_counter = got_allocated_slot_number;
       break;
     }
     else
     {
       simple_slots_start_time_us += beacon_buf[22+beacon_buf[21]+i]; //////////  
     }
   }
     
    
  DEBUG("iQueue-MAC: The subframe is is %lu\n",subframeduration);  
  
  //simple_slots_start_time_us = ...
  
  //beacon_buf[i] 
}

static void mac_simple_beacon_process(void)
{
  DEBUG("iQueue-MAC: start process beacon\n");     
  
  /*
  while(!simple_node_get_beacon) 
  {
    if(simple_node_get_beacon)
    {      
      DEBUG("iQueue-MAC: get_beacon is true.\n"); 
      break;
    } 
    else{
      DEBUG("iQueue-MAC: get_beacon is not true.\n"); 
    }
    
    //hwtimer_wait(5000);
  }*/
      
  mac_analyze_beacon();
  
  if(simple_node_get_slots)
  {
    simple_slots_start_time_us = simple_slots_start_time_us*SLOT_DURATION;
    
    send_iqueue_mac_message(IQUEUE_SIMPLE_TDMA_MANAGE, 0L);
    mac_state = MAC_SIMPLE_VTDMA_PERIOD;
  }
  else{
    send_iqueue_mac_message(IQUEUE_SIMPLE_SP_MANAGE, 0L);
    mac_state = MAC_SIMPLE_SP_PERIOD;
  }

}


static void mac_simple_send_packet_in_vtdma(void)
{
  
  bool want_ack = true;
  bool use_long_add = false;
  bool use_csma = false;
  bool use_inist_send = false;  
  
  //mac_turnon_radio();
  //DEBUG("iQueue-MAC: sending packets in vTDMA.\n");  
  int i =0;
  for(i=0;i<simple_slots_num;i++)
  {
    mac_send_data_in_iqueue(router_dest,use_long_add,want_ack,use_csma,use_inist_send);   
  }
   
  
  /* at the end of the slots period*/
  if(mac_queue_length > 0)
  {
    simple_node_get_slots = true;
  }
  else
  {
    simple_node_get_slots = false;
  }
  
  send_iqueue_mac_message(IQUEUE_SIMPLE_SP_MANAGE, 0L);
  mac_state = MAC_SIMPLE_SP_PERIOD;
}


static void mac_simple_tdma_manage(void)
{
  /*Set simple node sleep time before sending in vTDMA */
  hwtimer_ticks_t simple_slots_start_time_t =0;     
  
  if(simple_slots_start_time_us == 0)
  {
    mac_simple_send_packet_in_vtdma();
  }
  else{
    simple_slots_start_time_t = HWTIMER_TICKS(simple_slots_start_time_us);
    int res = hwtimer_set(simple_slots_start_time_t, mac_simple_send_packet_in_vtdma, NULL);
    if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }              
    //mac_turnoff_radio(); 
  }
}

static void mac_simple_ask_end_sp(void)
{  
  if(mac_queue_length > 0)
  {
    if(simple_node_get_slots)
    {
      //mac_turnon_radio();
      //send_iqueue_mac_message(IQUEUE_SIMPLE_WAIT_BEACON, 0L);
      mac_simple_wait_beacon_initiate();
      
      mac_state = MAC_WAIT_BEACON_PERIOD;
    }
    else
    {
      //mac_turnon_radio();
      send_iqueue_mac_message(IQUEUE_SIMPLE_CP_MANAGE, 0L);
      mac_state = MAC_SIMPLE_CP_PERIOD;
    }  
  }
  else
  {
    //mac_turnoff_radio();    
    //send_iqueue_mac_message(IQUEUE_SIMPLE_CHECK_DATA, 0L);
    mac_state = MAC_SIMPLE_CHECK_PERIOD;
    mac_simple_wait_beacon_initiate();
  }
  
}

static void mac_simple_sp_manage(void)
{
  DEBUG("iQueue-MAC: Simple node enter SP period.\n");
  
  /*Set simple node sleep time in SP */
  hwtimer_ticks_t subframeduration_t;
  hwtimer_ticks_t subfram_remain_time_t;
  hwtimer_ticks_t subfram_past_time_t;
  
  subframeduration_t = HWTIMER_TICKS(subframeduration);
  subfram_past_time_t = hwtimer_now()-sp_start_time_t; 

  if(subframeduration_t > subfram_past_time_t)
  { 
    subfram_remain_time_t = subframeduration_t - subfram_past_time_t;
    //mac_turnoff_radio();
    
    int res = hwtimer_set(subfram_remain_time_t, mac_simple_ask_end_sp, NULL);  //
    if (res < 0) {
        core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    }     
  }
  else /* This is unlikely to happen!! */
  {    
    mac_simple_ask_end_sp();
  }   
}

static void mac_end_csma_backoff_period(void)
{
  end_csma_backoff = true;
}

static void mac_send_data_csma_in_iqueue(ieee802154_node_addr_t dest, 
                                         bool use_long_addr,    
                                         bool wants_ack,                                      
                                         bool insist)
{  
  /* outcome of the radio transmission */
  int result;
  hwtimer_ticks_t backoff_delay;    
  out_pkt_t *curr_pkt = first_outgoing_packet;
  double r;
  
  /* if all transmissions have been done, switch to waiting mode */
  if (curr_pkt == NULL) {      
      DEBUG("IQUEUE: No buffered packet, quit transmit!!\n");
      return;
  }
  
  num_backoffs = 0;
  backoff_delay = 0;  
  
  u8 *ptr = (u8 *)(curr_pkt->pkt_data);      
  ptr[0] = mac_queue_length;
  
  //curr_pkt->pkt_data[0] = mac_queue_length;
  //num_tx = 0;
  while(1)
  {       
    r=Drand();
    backoff_delay = r*300;    
    //backoff_delay = choose_backoff_period(num_backoffs);  //hwtimer_spin(backoff_delay);        ///hwtimer_wait   
    
    end_csma_backoff = false;
    
    int res = hwtimer_set(backoff_delay, mac_end_csma_backoff_period, NULL);
    if (res < 0) {
       core_panic(0x1234, "Unable to allocate hwtimer for iQueue-MAC");
    } 
    ////////// block the func util csmabackoff ends
    while(!end_csma_backoff)
    {
      if(end_csma_backoff)
      {
        break;
      }
    }
         
    /* check if channel is clear, otherwise back off */
    if (!(BOARD_RADIO_DRIVER.channel_is_clear()))    // XXX provisoire
    { /* too much failed attempts: give up! */  
                 
      if (num_backoffs > MAC_MAX_CSMA_BACKOFFS)
      {
        if(!insist)
        {
           DEBUG("IQUEUE: UNABLE TO ACCESS CHANNEL"
                 " AFTER %d BACKOFFS!\n", num_tx);
           DEBUG("IQUEUE: GIVING UP TRANSMISSION\n");
           /* remove the failed packet from queue and free its corresponding slot */
           pop_packet_from_queue();
           return;
        }
      } /*if the max cca retries is not reached yet */
      else{
        num_backoffs++;      
      }
    }
    else{  /*clear channel, start transmission */      
         
      ptr[0] = mac_queue_length;
            
      LED_BLUE_ON;
      /* we go directly here if transmission can be done in burst mode */      
      //DEBUG("IQUEUE: now emitting 802.15.4 packet...\n");
      result = BOARD_RADIO_DRIVER.send(PACKET_KIND_DATA,dest,use_long_addr,wants_ack,
                          curr_pkt->pkt_data, curr_pkt->data_len);   // XXX provisoire                          
                          
      LED_BLUE_OFF;         
  
      /* check how transmission went */
       if (result >= RADIO_TX_OK) {               
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();     
        
        /* ... reset counter of attempted transmissions... */
        num_backoffs = 0;        
        return;
       } 
       else { //start of transmit failure
         num_backoffs++;         
       } //end of transmit failure     
      
    } //end of transmit process  
  } //end of while loop 
                                 
}


static void mac_send_packet_nocsma_in_iqueue(ieee802154_node_addr_t dest,
                                             bool use_long_addr,          
                                             bool wants_ack,                                
                                             bool insist)
{  
  /* outcome of the radio transmission */
  int result; 
  out_pkt_t *curr_pkt = first_outgoing_packet;
  
  /* if all transmissions have been done, switch to waiting mode */
  if (curr_pkt == NULL) {      
      DEBUG("IQUEUE: No buffered packet, quit transmit!!\n");
      return;
  }
  
  num_backoffs = 0;
  u8 *ptr = (u8 *)(curr_pkt->pkt_data);        
  
  while(1)
  {    
    ptr[0] = mac_queue_length;
    //curr_pkt->pkt_data[0] = mac_queue_length;
    
    
    LED_BLUE_ON;
    /* we go directly here if transmission can be done in burst mode */      
    //DEBUG("IQUEUE: now emitting 802.15.4 packet...\n");
    result = BOARD_RADIO_DRIVER.send(PACKET_KIND_DATA,dest,use_long_addr,wants_ack,
                        curr_pkt->pkt_data, curr_pkt->data_len);   // XXX provisoire
    LED_BLUE_OFF;    
    
     ////////////////
      /* check how transmission went */
       if (result >= RADIO_TX_OK) {
       
        /* success */        
        /* remove the packet successfully sent from queue
           and free the packet descriptor... */
        pop_packet_from_queue();        
        return;
       } 
       else 
       { //start of transmit failure                       
         /* too much failed attempts: give up! */
         if (num_backoffs > MAC_MAX_FRAME_RETRIES) 
         {
           if(!insist)
           {
             DEBUG("IQUEUE-MAC: UNABLE TO EMIT PACKET"
                   " AFTER %d ATTEMPTS!\n", num_tx);
             DEBUG("IQUEUE-MAC: GIVING UP TRANSMISSION\n");            
             /* remove the failed packet from queue
                and free its corresponding slot */
             pop_packet_from_queue();  
             return;
            }                   
            //continue;         
         }    
         else{
           num_backoffs ++; 
         }
       } //end of transmit failure
      ///////////////
  }//end of while loop

}
                                      
                                      
static void mac_send_data_in_iqueue(ieee802154_node_addr_t dest,                                     
                                    bool use_long_addr,         
                                    bool wants_ack,                              
                                    bool csma_enable, bool insist)     
{   
  if(csma_enable)
  {
    mac_send_data_csma_in_iqueue(dest,use_long_addr,wants_ack,insist);
  }
  else{
    mac_send_packet_nocsma_in_iqueue(dest,use_long_addr,wants_ack,insist);
  }
}

static void mac_simple_cp_manage(void)
{
  DEBUG("iQueue-MAC: Enter CP period.\n"); //use loop structure to send the packet in CSMA manner!
  bool want_ack = true;
  bool use_long_add = false;
  bool use_csma = true;
  bool use_inist_send = false;
  
  want_ack = true;
  use_csma = true;
  
  if(mac_queue_length > 0)
  { 
    DEBUG("iQueue-MAC: Send packets in CP.\n");  
    mac_send_data_in_iqueue(router_dest,use_long_add,want_ack,use_csma,use_inist_send);                
  }
  else{
    DEBUG("iQueue-MAC: No packets to send in CP.\n");  
  }
  
  /* After successfully sending the packet in CP!! */ 
  if(mac_queue_length > 0)
  {
    //send_iqueue_mac_message(IQUEUE_SIMPLE_WAIT_BEACON, 0L);    
    mac_state = MAC_WAIT_BEACON_PERIOD;
  }
  else
  {
    //mac_turnoff_radio();
    //send_iqueue_mac_message(IQUEUE_SIMPLE_CHECK_DATA, 0L);
    mac_state = MAC_SIMPLE_CHECK_PERIOD;
  }
  
  mac_simple_wait_beacon_initiate();
   
}

static void router_rx_packet_process(void *buf,
                                     unsigned int len,
                                     uint8_t rssi,
                                     uint8_t lqi,
                                     bool crc_ok)
{
  //uint8_t *data;
  rx_data_buf = buf;
  
  switch(rx_data_buf[0])
  {
    case 0x40 : break; ///beacon
    
    case 0x61 : ///////Data want ACK
    {
      //DEBUG("iQueue-MAC: Receive data packets is %2x,%2x.\n",  data[0],data[1]);      
      
      /*
      int i =0;
      for(i=0;i<(rx_data_buf[9]+9);i++)
      {
        data[i] = rx_data_buf[i];
        
        //rx_data_buf[i] = data[i];  
      } */
      
      router_recive_data_process(rx_data_buf, len);
    
    }break;    
    
    case 0x41 : ///Data donot want ACK like preamble and preamble-ACK 0x55 
    {
      DEBUG("iQueue-MAC: Receive data packets is %2x,%2x.\n",  rx_data_buf[0],rx_data_buf[1]);      
      //rx_data_buf[8] = data[8];
      //rx_data_buf[7] = data[7];
        
      
      if(rx_data_buf[9] == PACKET_KIND_PREAMBLE)
      {       
        router_recive_preamble_process();
      }
      else{
        if(rx_data_buf[9] == PACKET_KIND_PREAMBLE_ACK)
        {           
          router_recive_preamble_ACK_process();
        }      
      }
           
      //router_recive_data_process(buf, len);
    
    }break;    
    
       
    default: DEBUG("iQueue-MAC rx type error.\n");      
  }    
 
  
}

static void simple_rx_packet_process(void *buf,
                                     unsigned int len,
                                     uint8_t rssi,
                                     uint8_t lqi,
                                     bool crc_ok)
{
  uint8_t *data = buf;
  
  switch(data[0])
  {
    case 0x40 : 
    {  //if the node receives a BEACON;       
      printf("iQueue-MAC: simple node receive a beacon \n");
      switch(mac_state)
      {
        case MAC_WAIT_BEACON_PERIOD: 
        {                      
          if(simple_node_get_beacon)
          {
            return;
          }
          
          int i =0;
          for(i=0;i<(data[9]+9);i++)
          {
            beacon_buf[i] = data[i];  
          }            
          
          simple_node_get_beacon = true;         
          
          send_iqueue_mac_message(IQUEUE_SIMPLE_GOT_BEACON, 0L);     
          
        } break;
        
        default: break;      
      }    
    } break;        
    
    case 0x61 :  printf("iQueue-MAC: simple node receive a data from another node \n"); break;   
    case 0x41 : {    
      
    }break;   
      
    default: DEBUG("iQueue-MAC rx type error.\n");      
  }          
 
}

void mac_rx_packet_process(void *buf,
                 unsigned int len,
                 uint8_t rssi,
                 uint8_t lqi,
                 bool crc_ok)
{
        
    switch(iqueue_mac_mode)
    {
      case ROUTER_MODE: router_rx_packet_process(buf, len, rssi, lqi, crc_ok); break;
      case SIMPLE_MODE: simple_rx_packet_process(buf, len, rssi, lqi, crc_ok); break;    
      
      default: DEBUG("iQueue-MAC rx type error.\n");      
    } 
    
}


/****************************************************************************/
/*                      Main function of iQueue-MAC thread                  */
/****************************************************************************/
static void iQueue_MAC_init_router(void)
{ 
   DEBUG("iQueue-MAC router mode initialization.\n");
   sp_start_time_t = 0;
   //////SP manage parameters
   sp_turn_to_sleep_state = false;
   sp_end = false;
   //////CP manage parameters
   cp_got_new_packet = false;
   
   iq_dest.pan.addr = 0xFFFF;  
   iq_dest.pan.id = BOARD_RADIO_DRIVER.get_pan_id();   // XXX provisoire
   
   mac_short_address = BOARD_RADIO_DRIVER.get_address();

}


static void iQueue_MAC_init_simple(void)
{
   DEBUG("iQueue-MAC simple mode initialization.\n");
   sp_start_time_t = 0;
   //////SP manage parameters
   sp_turn_to_sleep_state = false;
   sp_end = false;
   //////CP manage parameters
   cp_got_new_packet = false;
   
   mac_state = MAC_SIMPLE_CHECK_PERIOD;
   
   iq_dest.pan.addr = 0xFFFF;  
   iq_dest.pan.id = BOARD_RADIO_DRIVER.get_pan_id();   // XXX provisoire

   router_dest.pan.addr = 0xFFFF;
   router_dest.pan.id = BOARD_RADIO_DRIVER.get_pan_id();
   
   mac_short_address = BOARD_RADIO_DRIVER.get_address();   
   
   //BOARD_RADIO_DRIVER.set_promiscuous_mode(true);   
}


static void iqueue_thread_func(void)
{
    /* init the thread's message queue */  
    int res = msg_init_queue(iqueue_mac_msg_queue, IQUEUE_MSG_QUEUE_DEPTH);
    if (res < 0) {
        core_panic(0x1234, "Couldn't init iQueue-MAC thread's message queue");
    }
    
    switch(iqueue_mac_mode)
    {
      case ROUTER_MODE: iQueue_MAC_init_router(); break;
      case SIMPLE_MODE: iQueue_MAC_init_simple(); break;    
      
      default: DEBUG("iQueue-MAC initiation error.\n");      
    }   
    
    srand(mac_short_address);
    
    DEBUG("iQueue-MAC thread starting.\n");

    begin_wait(); 
    msg_t current_message;
    
    
    switch(iqueue_mac_mode)
    {
      case ROUTER_MODE: 
      {    
         DEBUG("iQueue-MAC: start runing iQueue-MAC router mode.\n");
         
        /* loop = wait for messages and handle them */
        do {
          msg_receive(&current_message);  /// blocks (keep!!!!!) the thread as long as a message has not arrvied

          switch (current_message.type) {  
   
          case IQUEUE_BEACON_MANAGE:{
            mac_beacon_manage();
          }break;

          case IQUEUE_VTDMA_MANAGE:{
            mac_vTDMA_manage();
          }break;

          case IQUEUE_SP_MANAGE:{
            mac_SP_manage();
          }break;

          case IQUEUE_CP_MANAGE:{
            mac_CP_manage();
          }break;
        
          case IQUEUE_PREAMBLE_MANAGE:{
            mac_preamble_manage();
          }break;

          case IQUEUE_TP_MANAGE:{
            mac_TP_manage();
          }break;

          case IQUEUE_WAIT:
             begin_wait();   /////wait_process();
             break;            
          case IQUEUE_EMIT_PACKET:
            do_transmit_2();
            break;
          case IQUEUE_STOP:
            ////printf("iqueue_run=%d\n", iqueue_run); 
            iqueue_run = false;
            break;
          default:
            core_panic(current_message.type,
                       "iQueue MAC thread received an unknown message");
        }
       } while (iqueue_run || (first_outgoing_packet != NULL));
                        
      }break;  // ending of router mac thread!
         
      case SIMPLE_MODE: 
      {    
         DEBUG("iQueue-MAC: start runing iQueue-MAC simple mode.\n");
       
        /* loop = wait for messages and handle them */
        do {
        
          msg_receive(&current_message);  /// blocks (keep!!!!!) the thread as long as a message has not arrvied

          switch (current_message.type) {  
            /*
            case IQUEUE_SIMPLE_CHECK_DATA:{
              mac_simple_check_data();
            }break;  */

            case IQUEUE_SIMPLE_GOT_BEACON:{
              mac_simple_beacon_process();   
            }break;

            case IQUEUE_SIMPLE_TDMA_MANAGE:{
              mac_simple_tdma_manage();
            }break;

            case IQUEUE_SIMPLE_SP_MANAGE:{
              mac_simple_sp_manage();
            }break;
        
            case IQUEUE_SIMPLE_CP_MANAGE:{
              mac_simple_cp_manage();
            }break;
            
            case IQUEUE_GET_NEW_PACKET:{
              mac_simple_get_new_packet_process();            
            }
          
            case IQUEUE_STOP:{
              ////printf("iqueue_run=%d\n", iqueue_run); 
              iqueue_run = false;
              }break;
            
            default:
              core_panic(current_message.type,
                         "iQueue MAC thread received an unknown message");
           }///end of switch
        } while (iqueue_run || (first_outgoing_packet != NULL));
         
      }break;    // ending of simple mac thread!
      
      default: DEBUG("iQueue-MAC runing mode error.\n");      
    }     

    DEBUG("iQueue-MAC thread terminating.\n");
    iqueue_mac_tid = -1;
}

/****************************************************************************/
/*          Functions exported by the iQueue-MAC protocol's MAC driver          */
/****************************************************************************/

bool iqueue_active(void)
{
    return iqueue_run;
}

/* initialization function */
mac_return_t iqueue_init(void)
{
    DEBUG("*** iQueue-MAC is initializing... \n");

    /* initialize random number generator */
    genrand_init(hwtimer_now());   // XXX find a better way to get a random seed?

    /* iQueue-MAC doesn't start without being asked to */
    iqueue_run = false;

    /* intialize the outgoing packets' queue */
    for (int i = 0; i < MAX_OUTGOING_PACKETS_NUMBER; i++) {
        outgoing_queue[i].next = NULL;
        outgoing_queue[i].pkt_data = NULL;
        outgoing_queue[i].data_len = 0;
#if PERFORM_EXTENDED_CALLBACKS
        outgoing_queue[i].callback = NULL;
        outgoing_queue[i].callback_param = NULL;
#endif /* PERFORM_EXTENDED_CALLBACKS */
    }
    first_outgoing_packet = NULL;

    iqueue_driver.set_input_func(mac_rx_packet_process);
    
    return MAC_OK;
}

/* turn-on function */
mac_return_t iqueue_on(void)
{
    if (iqueue_active()) {
        /* already on */
        return MAC_OK;
    }

    DEBUG("*** iQueue-MAC: going ON...\n");

    /* set CoSenS activity flag */
    iqueue_run = true;

    /* create CoSenS thread */
    iqueue_mac_tid = thread_create(iqueue_mac_stack,
                            IQUEUE_THREAD_STACK_SIZE,
                            IQUEUE_THREAD_PRIORITY,
                            0,                          /* no flags */
                            iqueue_thread_func,
                            NULL,
                            "iQueue");
    if (iqueue_mac_tid < 0) {
        switch (iqueue_mac_tid) {
        case -EINVAL:
            core_panic(0x1234, "Unable to create iQueue-MAC thread"
                               " (bad priority parameter for thread_create)");
        case -EOVERFLOW:
            core_panic(0x1234, "Unable to create iQueue-MAC thread"
                               " (too much threads already running)");
        default:
            core_panic(iqueue_mac_tid, "Unable to create iQueue-MAC thread"
                                   " (unexpected error)");
        }
    }

    return MAC_OK;
}

/* turn-off function */
mac_return_t iqueue_off(void)
{
    if (!iqueue_active()) {
        /* already off */
        return MAC_OK;
    }

    DEBUG("*** iQueue-MAC: going OFF!...\n");
    DEBUG("NOTE: packets already enqueued will still be transmitted.\n");

    /* reset iQueue-MAC activity flag */
    iqueue_run = false;

    /* ask the iQueue-MAC thread to stop */
    send_iqueue_mac_message(IQUEUE_STOP, 0L);

    return MAC_OK;
}

/* packet TX function */
mac_return_t iqueue_send_packet(void *buf,
                                unsigned int len,
                                mac_tx_status_callback_t tx_status_callback,
                                void *callback_param)
{
    if (!iqueue_active()) {
        /* we don't accept packets when off line */
        return MAC_DRIVER_OFF;
    }

    /* try to enqueue packet for transmission */
    bool ok = push_out_packet(buf, len,
                              tx_status_callback, callback_param);
    if (!ok) {
        /* we exceeded the limit of TX packets */
        return MAC_TX_QUEUE_FULL;
    }

    /* Packet enqueued, will be TXed ASAP */
    return MAC_TX_DEFERRED;
}

/* packet RX (from radio transceiver) function */
void iqueue_receive_packet(void *buf,
                           unsigned int len,
                           uint8_t rssi,
                           uint8_t lqi,
                           bool crc_ok)
{
    /* only relay RX packets if on */
    if (!iqueue_active()) return;

    //LED_GREEN_ON;
    if (rx_callback_func != NULL) {
      rx_callback_func(buf, len, rssi, lqi, crc_ok);
    }
    //LED_GREEN_OFF;
}

/* definition of packet RX callback function */
void iqueue_set_input_func(mac_rx_packet_notif_t rx_func)
{
    rx_callback_func = rx_func;
}

/****************************************************************************/
/*              Definition of the iQueue-MAC protocol's MAC driver              */
/****************************************************************************/

const mac_driver_t iqueue_driver = {
    .name = "iQueue",
    .init = iqueue_init,
    .on = iqueue_on,
    .off = iqueue_off,
    .is_active = iqueue_active,
    .send = iqueue_send_packet,
    .receive = iqueue_receive_packet,
    .set_input_func = iqueue_set_input_func,
};

/**
 * @}
 */
