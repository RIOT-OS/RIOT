#ifndef __PROTOCOL_MSG_GATEWAY_H
#define __PROTOCOL_MSG_GATEWAY_H 

#define PACKET_BUFFER_SIZE 32
#define MAXIMUM_PAYLOAD_SIZE 64

typedef struct {
    packet_info_t packet_info;
    unsigned int msg_size;
    char payload[MAXIMUM_PAYLOAD_SIZE];
} packet_t;

void init_protocol_msg_gateway();
int set_protocol_handler_thread(int pid);

extern packet_t packet_buffer[PACKET_BUFFER_SIZE];

#endif /* __PROTOCOL_MSG_GATEWAY_H */
