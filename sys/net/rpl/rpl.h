/**
 * RPL constants and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl.h
 * @brief   RPL header
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include <transceiver.h>
#include "ipv6.h"
#include "rpl_dodag.h"

#undef CC1100_RADIO_MODE
#define CC1100_RADIO_MODE CC1100_MODE_WOR

#define RPL_PKT_RECV_BUF_SIZE 20
#define RPL_PROCESS_STACKSIZE 4096

uint8_t rpl_init(transceiver_type_t trans, uint16_t rpl_address);
void rpl_init_root(void);
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp);

void send_DIO(ipv6_addr_t *destination);
void send_DIS(ipv6_addr_t *destination);
void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index);
void send_DAO_ACK(ipv6_addr_t *destination);

void send_TVO(ipv6_addr_t *destination, struct rpl_tvo_t * tvo,  rpl_tvo_signature_t * signature); //trail
void send_TVO_ACK(ipv6_addr_t *destination, uint8_t sequence_number); //trail
void recv_rpl_tvo(void); //trail
void recv_rpl_tvo_ack(void); // trail
void enable_tvo_auto_send(void); //trail
void rpl_tvo_sign_data(void); //trail
void forward_rpl_tvo(ipv6_addr_t *destination); //trail
uint32_t global_zero_timestamp; //trail
void join_dodag(rpl_dodag_t * dio_dodag, ipv6_addr_t * src_addr, uint16_t parent_rank, uint8_t parent_dtsn); // trail
uint8_t include_parent_into_trail_buffer(void); //trail
uint8_t get_parent_from_trail_buffer(ipv6_addr_t * src_addr); // trail
uint8_t is_parent_verified(ipv6_addr_t * src_addr, uint16_t dio_rank); //trail
void disable_attacker(void); //trail: safest demo
void reset_rpl(void); //trail safest demo
void reset_trail_buffers(); // trail
void resend_tvos(); //trail
void configure_as_attacker(uint16_t rank); //trail
struct rpl_tvo_local_t * has_tvo_been_received(ipv6_addr_t * source, uint8_t tvo_seq); //trail
void save_tvo_locally(struct rpl_tvo_local_t * tvo_copy); //trail
void reset_tvo_timer(); //trail
void disable_attacker(void); //trail
void start_as_attacker(uint16_t rank); //trail
void disable_trail(void); //trail
void start_with_trail(void); //trail



void rpl_process(void);
void recv_rpl_dio(void);
void recv_rpl_dis(void);
void recv_rpl_dao(void);
void recv_rpl_dao_ack(void);
void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header, void *tcp_socket);
ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr);
void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime);
void rpl_del_routing_entry(ipv6_addr_t *addr);
rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr);
void rpl_clear_routing_table(void);
rpl_routing_entry_t *rpl_get_routing_table(void);
