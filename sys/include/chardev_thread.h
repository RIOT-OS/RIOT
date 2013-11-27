
/**
 * @defgroup 	sys_chardevthread Chardev Thread
 * @ingroup		sys
 * @brief		Chardev thread
 */

#ifndef __CHARDEV_THREAD_H
#define __CHARDEV_THREAD_H

#include <ringbuffer.h>

void chardev_loop(ringbuffer_t *rb);

#endif /* __CHARDEV_THREAD_H */
