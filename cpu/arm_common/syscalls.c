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
 * @version     $Revision$
 *
 * @note		$Id$
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdint.h>

#include <board.h>

// core
#include "kernel.h"

#define DEBUG_SYSCALLS			0
#if DEBUG_SYSCALLS
#define	PRINTF(...)				printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/**
 * @name Heaps (defined in linker script)
 * @{
 */
#define NUM_HEAPS	2

extern uintptr_t __heap1_start;		///< start of heap memory space
extern uintptr_t __heap1_max;		///< maximum for end of heap memory space
extern uintptr_t __heap2_start;		///< start of heap memory space
extern uintptr_t __heap2_max;		///< maximum for end of heap memory space

/// current position in heap
static caddr_t heap[NUM_HEAPS] = {(caddr_t)&__heap1_start,(caddr_t)&__heap2_start};
/// maximum position in heap
static const caddr_t heap_max[NUM_HEAPS] = {(caddr_t)&__heap1_max,(caddr_t)&__heap2_max};
/** @} */

/*-----------------------------------------------------------------------------------*/
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
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
	/* check all heaps for a chunk of the requested size */
	for( int i = 0; i < NUM_HEAPS; i++ ) {
		caddr_t new_heap = heap[i] + incr;

		if( new_heap <= heap_max[i] ) {
			caddr_t prev_heap = heap[i];
			heap[i] = new_heap;

			r->_errno = 0;
			return prev_heap;
		}

	}
	
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

	r->_errno = ENODEV;

	PRINTF("open [%i] errno %i\n", ret, r->_errno);
	return ret;
}
/*---------------------------------------------------------------------------*/
int _stat_r(struct _reent *r, char *name, struct stat *st)
{
	int ret = -1;

	r->_errno = ENODEV;

	return ret;
}
/*---------------------------------------------------------------------------*/
int _fstat_r(struct _reent *r, int fd, struct stat * st)
{
	int ret = -1;

	memset(st, 0, sizeof(*st));
	if( fd == STDOUT_FILENO || fd == STDERR_FILENO ) {
		st->st_mode = S_IFCHR;
		ret = 0;
	} else {
	    r->_errno = ENODEV;
	}
	return ret;
}

extern int fw_puts(char* data, int count);

/*---------------------------------------------------------------------------*/
int _write_r(struct _reent *r, int fd, const void *data, unsigned int count)
{
	int result = EOF;
	r->_errno = 0;

	switch(fd) {
		case STDOUT_FILENO:
		case STDERR_FILENO:
				result = fw_puts((char*)data, count);
			break;

		default:
			PRINTF("write [%i] data @%p count %i\n", fd, data, count);

			PRINTF("write [%i] returned %i errno %i\n", fd, result, r->_errno);
			break;
	}

	return result;
}
/*---------------------------------------------------------------------------*/
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
	int result = EOF;
	r->_errno = 0;
	
    PRINTF("read [%i] buffer @%p count %i\n", fd, buffer, count);
	PRINTF("read [%i] returned %i\n", fd, result);
	
    return result;
}
/*---------------------------------------------------------------------------*/
int _close_r(struct _reent *r, int fd)
{
	int result = 0;
	r->_errno = 0;
	
    PRINTF("close [%i]\n", fd);
	PRINTF("close returned %i errno %i\n", result, errno);
	
    return result;
}
/*---------------------------------------------------------------------------*/
int _unlink_r(struct _reent *r, char* path)
{
	int result = -1;
	r->_errno = ENODEV;
	
    PRINTF("unlink '%s'\n", path);
	PRINTF("unlink returned %i errno %i\n", result, errno);
	
    return result;
}
/*---------------------------------------------------------------------------*/
void _exit(int n)
{
	printf("#! exit %i: resetting\n", n);

	stdio_flush();

    while(1);
    
	arm_reset();
}
/*---------------------------------------------------------------------------*/
int _getpid(void)
{
	return fk_thread->pid;
}
/*---------------------------------------------------------------------------*/
int _kill_r(struct _reent *r, int pid, int sig)
{
	r->_errno = ESRCH;		// no such process
	return -1;
}
/*---------------------------------------------------------------------------*/
