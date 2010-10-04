#ifndef __CHARDEV_THREAD_H
#define __CHARDEV_THREAD_H 

#include <ringbuffer.h>

void chardev_loop(ringbuffer *rb);

#endif /* __CHARDEV_THREAD_H */
