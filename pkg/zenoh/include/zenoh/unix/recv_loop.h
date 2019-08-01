#ifndef ZENOH_C_RECV_LOOP_UNIX_H_
#define ZENOH_C_RECV_LOOP_UNIX_H_

#include <pthread.h>
#include <unistd.h>

#include "zenoh/types.h"

typedef struct {
    pthread_t recv_loop;
    volatile int running;
} z_runtime_t;

void* z_recv_loop(void* arg);

#endif /* ZENOH_C_RECV_LOOP_UNIX_H_ */
