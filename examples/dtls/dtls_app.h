#ifndef DTLS_APP_H
#define DTLS_APP_H

#include "msg.h"
#include "dtls.h"

#define READER_QUEUE_SIZE (8)
extern msg_t _reader_queue[READER_QUEUE_SIZE];

void *dtls_reader(void *args);

#endif /* DTLS_APP_H */
