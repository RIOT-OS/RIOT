/**
 * Syscall implementation for NXP LPC1768
 *
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup  lpc1768
 * @{
 * @file    syscalls.c
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 * @author Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include "kernel.h"
#include "irq.h"
#ifdef MODULE_VTIMER
#include "vtimer.h"
#endif

/**
 * @name Heaps (defined in linker script)
 * @{
 */
#define NUM_HEAPS	1

#define DEBUG_SYSCALLS			0
#if DEBUG_SYSCALLS
#define	PRINTF(...)				printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

extern uintptr_t __heap_start;		///< start of heap memory space
extern uintptr_t __heap_max;		///< maximum for end of heap memory space

/// current position in heap
static caddr_t heap[NUM_HEAPS] = {(caddr_t)&__heap_start};
/// maximum position in heap
static const caddr_t heap_max[NUM_HEAPS] = {(caddr_t)&__heap_max};
// start position in heap
static const caddr_t heap_start[NUM_HEAPS] = {(caddr_t)&__heap_start};
// current heap in use
volatile static uint8_t iUsedHeap = 0;

/** @} */

/*-----------------------------------------------------------------------------------*/
void heap_stats(void) {
	for(int i = 0; i < NUM_HEAPS; i++)
		printf("# heap %i: %p -- %p -> %p (%li of %li free)\n", i, heap_start[i], heap[i], heap_max[i],
			(uint32_t)heap_max[i] - (uint32_t)heap[i], (uint32_t)heap_max[i] - (uint32_t)heap_start[i]);
}
/*-----------------------------------------------------------------------------------*/
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
	printf("#! assertion %s failed\n\t%s() in %s:%d\n", failedexpr, func, file, line );
	_exit(3);
}
/*-----------------------------------------------------------------------------------*/
void __assert(const char *file, int line, const char *failedexpr)
{
	__assert_func(file, line, "?", failedexpr);
}
/*-----------------------------------------------------------------------------------*/
caddr_t _sbrk_r(struct _reent *r, size_t incr)
{
    if(incr < 0)
    {
        puts("[syscalls] Negative Values for _sbrk_r are not supported");
        r->_errno = ENOMEM;
        return NULL;
    }

    uint32_t cpsr = disableIRQ();

    /* check all heaps for a chunk of the requested size */
	for( ; iUsedHeap < NUM_HEAPS; iUsedHeap++ ) {
		caddr_t new_heap = heap[iUsedHeap] + incr;

		if( new_heap <= heap_max[iUsedHeap] ) {
			caddr_t prev_heap = heap[iUsedHeap];
			heap[iUsedHeap] = new_heap;

			r->_errno = 0;
			restoreIRQ(cpsr);
			return prev_heap;
		}
	}
	restoreIRQ(cpsr);

	r->_errno = ENOMEM;
	return NULL;
}
/*---------------------------------------------------------------------------*/
int _isatty_r(struct _reent *r, int fd)
{
	r->_errno = 0;
	if( fd == STDOUT_FILENO || fd == STDERR_FILENO )
		return 1;
	else
		return 0;
}
/*---------------------------------------------------------------------------*/
_off_t _lseek_r(struct _reent *r, int fd, _off_t pos, int whence)
{
	_off_t result = -1;
	PRINTF("lseek [%i] pos %li whence %i\n", fd, pos, whence);

	r->_errno = ENODEV;

	PRINTF("lseek returned %li (0 is success)\n", result);
	return result;
}
/*---------------------------------------------------------------------------*/
int _open_r(struct _reent *r, const char *name, int mode)
{
	int ret = -1;
	PRINTF("open '%s' mode %#x\n", name, mode);

	r->_errno = ENODEV; // no such device

	PRINTF("open [%i] errno %i\n", ret, r->_errno);
	return ret;
}
/*---------------------------------------------------------------------------*/
int _stat_r(struct _reent *r, char *name, struct stat *st)
{
	int ret = -1;
	PRINTF("_stat_r '%s' \n", name);
	r->_errno = ENODEV; // no such device
	PRINTF("_stat_r [%i] errno %i\n", ret, r->_errno);
	return ret;
}
/*---------------------------------------------------------------------------*/
int _fstat_r(struct _reent *r, int fd, struct stat * st)
{
	int ret = -1;

	r->_errno = 0;
	memset(st, 0, sizeof(*st));
	if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		ret = 0;
	}
    else {
        r->_errno = ENODEV;
	}
	return ret;
}
/*---------------------------------------------------------------------------*/
int _write_r(struct _reent *r, int fd, const void *data, unsigned int count)
{
	int result = EOF;
	r->_errno = EBADF;

	switch(fd) {
		case STDOUT_FILENO:
		case STDERR_FILENO:
		{
				//FIXME impl fw_puts
				//char* chars = (char*) data;
				for(int i = 0;i < count;i++) {
				  //USART_SendData(USART2, chars[i]);

				  /* Loop until the end of transmission */
				  //while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
				}

				return count;
				//result = fw_puts((char*)data, count);
		}
			break;

		default:
			break;
	}

	return result;
}
/*---------------------------------------------------------------------------*/
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
	int result = -1;
	r->_errno = EBADF;
	return result;
}
/*---------------------------------------------------------------------------*/
int _close_r(struct _reent *r, int fd)
{
	int ret = -1;
	r->_errno = EBADF;
	return ret;
}
/*---------------------------------------------------------------------------*/
int _unlink_r(struct _reent *r, char* path)
{
	int ret = -1;
	r->_errno = ENODEV;
	return ret;
}
/*---------------------------------------------------------------------------*/
void _exit(int n)
{
	printf("#! exit %i: resetting\n", n);

	//FIXME write out all peripherie buffers stdout flush
	NVIC_SystemReset();
	while(1);
}
/*---------------------------------------------------------------------------*/
int _getpid(void)
{
	return sched_active_thread->pid;
}
/*---------------------------------------------------------------------------*/
int _kill_r(struct _reent *r, int pid, int sig)
{
	/* not implemented */
	r->_errno = ESRCH;		// no such process
	return -1;
}
/*---------------------------------------------------------------------------*/
#ifdef MODULE_VTIMER
int _gettimeofday(struct timeval *tp, void *restrict tzp) {
    (void) tzp;
    vtimer_gettimeofday(tp);
    return 0;
}
#endif

void _init(void){}
void _fini(void){}
