#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef ENABLE_DEBUG_OLSR
#ifndef RIOT
#include <time.h>
#include <execinfo.h>
#endif /* not RIOT */
#include <stdio.h>
#include <stdlib.h>
#include "common/netaddr.h"

struct netaddr_str nbuf[4];
int debug_ticks;

#define DEBUG_TICK		debug_ticks++

#ifdef RIOT
#define DEBUG(fmt, ...) printf((fmt "\n"), ##__VA_ARGS__)
#define TRACE_FUN(fmt, ...) printf(("%s(" fmt ")\n"), __FUNCTION__, ##__VA_ARGS__)

#else

time_t _t_tmr;
char _t_buf[9];
#define DEBUG(fmt, ...) {	\
	_t_tmr = time_now();			\
	strftime(_t_buf, sizeof _t_buf, "%H:%M:%S", localtime(&_t_tmr));	\
	printf(("[%d, %s] " fmt "\n"), debug_ticks, _t_buf, ##__VA_ARGS__);	\
	}

#define TRACE_FUN(fmt, ...)	{	\
	_t_tmr = time_now();			\
	strftime(_t_buf, sizeof _t_buf, "%H:%M:%S", localtime(&_t_tmr));	\
	printf(("[%d, %s] %s(" fmt ")\n"), debug_ticks, _t_buf, __FUNCTION__, ##__VA_ARGS__);				\
	}

static inline void print_trace(void) {
	void *array[10];
	size_t size;
	char **strings;
	size_t i;

	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);

	printf("Obtained %zd stack frames.\n", size);

	for (i = 0; i < size; i++)
		printf ("%s\n", strings[i]);

	free(strings);
}

#endif /* not RIOT */

#else	/* no ENABLE_DEBUG_OLSR */

#define DEBUG(...)
#define DEBUG_TICK
#define TRACE_FUN(...)

#endif
#endif
