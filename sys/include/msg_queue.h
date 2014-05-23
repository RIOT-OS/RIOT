#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H 

#ifndef MODULE_MSG_QUEUE
#error "You are including msg_queue.h, but didn't compile in the MSG_QUEUE module. Maybe add \"USEMODULE += msg_queue lib\" to your Makefile."
#endif

#include "msg.h"
#include "ringbuffer.h"

typedef struct msg_queue {
    ringbuffer_t rb;
    int          maxsize;
} msg_queue_t;

int msg_queue_init(msg_queue_t *queue, char *buf, int size, int maxsize);
int msg_queue_add(msg_queue_t *queue, msg_hdr_t *src);
int msg_queue_get(msg_queue_t *queue, msg_hdr_t *dest);

#endif /* __MSG_QUEUE_H */
