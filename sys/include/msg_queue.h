#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H 

#ifndef MODULE_MSG_QUEUE
#error "You are including msg_queue.h, but didn't compile in the MSG_QUEUE module. Maybe add \"USEMODULE += msg_queue lib\" to your Makefile."
#endif

#include "msg.h"
#include "ringbuffer.h"

#define MSG_PULSE_QUEUE_SIZE (sizeof(msg_hdr_t)+sizeof(msg_pulse_t))
#define MSG_QUEUE_SPACE(N) (N*MSG_PULSE_QUEUE_SIZE + sizeof(msg_queue_t))

typedef struct msg_queue {
    ringbuffer_t rb;
    int          maxsize;
} msg_queue_t;

struct msg_hdr;

int msg_queue_init(msg_queue_t *queue, char *buf, int size, int maxsize);
int thread_msg_queue_init(char *buf, int size, int maxsize);
int msg_queue_add(msg_queue_t *queue, struct msg_hdr *src);
int msg_queue_get(msg_queue_t *queue, struct msg_hdr *dest);

#endif /* __MSG_QUEUE_H */
