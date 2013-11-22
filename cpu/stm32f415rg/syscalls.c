/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

/**
 * @file
 * @ingroup		lpc2387
 * @brief		LPC2387 NewLib system calls implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		Michael Baar <michael.baar@fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: syscalls.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>
// core
#include "kernel.h"
// sys
#include "lpm.h"

/* When using the HAL standard in and out are handled by HAL
   devices. */
#if FEUERWARE_CONF_ENABLE_HAL
#include "hal.h"
#include "interface-chardevice.h"
#endif

#define DEBUG_SYSCALLS			0
#if DEBUG_SYSCALLS
#define	PRINTF(...)				printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef MODULE_FAT
#include "ff_ansi.h"
#endif

#include "stm32f415rg-uart.h"

/**
 * @name Heaps (defined in linker script)
 * @{
 */
#define NUM_HEAPS	1//2

extern uintptr_t __heap1_start;		///< start of heap memory space
extern uintptr_t __heap1_max;		///< maximum for end of heap memory space
/*
extern uintptr_t __heap2_start;		///< start of heap memory space
extern uintptr_t __heap2_max;		///< maximum for end of heap memory space
extern uintptr_t __heap3_start;		///< start of heap memory space
extern uintptr_t __heap3_max;		///< maximum for end of heap memory space
*/


/// current position in heap
static caddr_t heap[NUM_HEAPS] = {(caddr_t)&__heap1_start};//,(caddr_t)&__heap3_start,(caddr_t)&__heap2_start}; // add heap3 before heap2 cause Heap3 address is lower then addr of heap2
/// maximum position in heap
static const caddr_t heap_max[NUM_HEAPS] = {(caddr_t)&__heap1_max};//,(caddr_t)&__heap3_max,(caddr_t)&__heap2_max};
// start position in heap
static const caddr_t heap_start[NUM_HEAPS] = {(caddr_t)&__heap1_start};//,(caddr_t)&__heap3_start,(caddr_t)&__heap2_start};
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
#if SYSLOG_CONF_ASSERT
	trace_number(TRACELOG_EV_ASSERTION, line);
	syslog(SL_EMERGENCY, "assert", "%s() in %s:%u\n", func, file, line);
#endif
	printf("#! assertion %s failed\n\t%s() in %s:%u\n", failedexpr, func, file, line );
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

		#ifdef MODULE_TRACELOG
		trace_pointer(TRACELOG_EV_MEMORY, heap[iUsedHeap]);
		#endif
		if( new_heap <= heap_max[iUsedHeap] ) {
			caddr_t prev_heap = heap[iUsedHeap];
			#ifdef MODULE_TRACELOG
			trace_pointer(TRACELOG_EV_MEMORY, new_heap);
			#endif
			heap[iUsedHeap] = new_heap;

			r->_errno = 0;
			restoreIRQ(cpsr);
			return prev_heap;
		}
	}
	restoreIRQ(cpsr);
	#ifdef MODULE_TRACELOG
	trace_string(TRACELOG_EV_MEMORY, "heap!");									// heap full
	#endif

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
#ifdef MODULE_FAT
    result = ff_lseek_r(r, fd, pos, whence);
#endif

	PRINTF("lseek returned %li (0 is success)\n", result);
	return result;
}
/*---------------------------------------------------------------------------*/
int _open_r(struct _reent *r, const char *name, int mode)
{
	int ret = -1;
	PRINTF("open '%s' mode %#x\n", name, mode);

	r->_errno = ENODEV; // no such device
#ifdef MODULE_FAT
	ret = ff_open_r(r,name,mode);
#endif

	PRINTF("open [%i] errno %i\n", ret, r->_errno);
	return ret;
}
/*---------------------------------------------------------------------------*/
int _stat_r(struct _reent *r, char *name, struct stat *st)
{
	int ret = -1;
	PRINTF("_stat_r '%s' \n", name);
	r->_errno = ENODEV; // no such device
#ifdef MODULE_FAT
	ret = ff_stat_r(r,name,st);
#endif
	PRINTF("_stat_r [%i] errno %i\n", ret, r->_errno);
	return ret;
}
/*---------------------------------------------------------------------------*/
int _fstat_r(struct _reent *r, int fd, struct stat * st)
{
	int ret = -1;

	r->_errno = 0;
	memset(st, 0, sizeof(*st));
	if( fd == STDOUT_FILENO || fd == STDERR_FILENO ) {
		st->st_mode = S_IFCHR;
		ret = 0;
	} else {

#ifdef MODULE_FAT
		PRINTF("_fstat_r '%i' \n", fd);

	    ret = ff_fstat_r(r,fd,st);
	    PRINTF("_fstat_r [%i] errno %i\n", ret, r->_errno);

#else
	    r->_errno = ENODEV;
#endif
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
			#if FEUERWARE_CONF_ENABLE_HAL
				if( stdio != NULL )
					result = chardevice_write(stdio, (char*)data, count);
				else if( hal_state == HAL_NOT_INITIALIZED )
					result = fw_puts((char*)data, count);
			#else
				//FIXME impl fw_puts


				char* chars = data;
				for(int i = 0;i < count;i++) {
					uart_writec(chars[i]);
				}

				return count;
				//result = fw_puts((char*)data, count);
			#endif
		}
			break;

		default:
			#ifdef MODULE_FAT
				result = ff_write_r(r, fd, data, count);
			#endif
			break;
	}

	return result;
}
/*---------------------------------------------------------------------------*/
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
	int result = -1;
	r->_errno = EBADF;


	switch(fd) {
		case STDIN_FILENO:
			{
				char* chars = buffer;
				for(int i = 0;i < count;i++) {
					chars[i] = uart_readc();
				}
			}break;
		case STDERR_FILENO:
			{
			}
			break;
		default:
			#ifdef MODULE_FAT
					result = ff_read_r(r, fd, buffer, count);
			#endif
			break;
		}

	return result;
}
/*---------------------------------------------------------------------------*/
int _close_r(struct _reent *r, int fd)
{
	int ret = -1;
	r->_errno = EBADF;
	#ifdef MODULE_FAT
		ret = ff_close_r(r, fd);
	#endif
	return ret;
}
/*---------------------------------------------------------------------------*/
int _unlink_r(struct _reent *r, char* path)
{
	int ret = -1;
	r->_errno = ENODEV;
#ifdef MODULE_FAT
	ret = ff_unlink_r(r, path);
#endif
	return ret;
}
/*---------------------------------------------------------------------------*/
void _exit(int n)
{
#ifdef MODULE_TRACELOG
	trace_number(TRACELOG_EV_EXIT, n);
#endif
	printf("#! exit %i: resetting\n", n);

	//FIXME write out all peripherie buffers stdout flush
	NVIC_SystemReset();
	while(1);
}
/*---------------------------------------------------------------------------*/
int _getpid(void)
{
	return active_thread->pid;
}
/*---------------------------------------------------------------------------*/
int _kill_r(struct _reent *r, int pid, int sig)
{
	/* not implemented */
	r->_errno = ESRCH;		// no such process
	return -1;
}
/*---------------------------------------------------------------------------*/
void __no_operation(void)
{
	asm("nop");
}
/*---------------------------------------------------------------------------*/
void _init(void){}
void _fini(void){}
