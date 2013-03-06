#ifndef _CPU_H
#define _CPU_H

#include <ucontext.h>

#include "kernel_intern.h"
#include "sched.h"
#include "debug.h"

#include "cpu-conf.h"

/* TODO: choose better value? */
#define F_CPU 1000000

void dINT(void);
void eINT(void);

int register_interrupt(int sig, void *handler);
int unregister_interrupt(int sig);
void thread_yield(void);

#endif //_CPU_H
