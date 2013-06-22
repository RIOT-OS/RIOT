/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
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
 * @ingroup	tracelog
 * @{
 */

/**
 * @file
 * @brief		Tracelog implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Michael Baar <baar@inf.fu-berlin.de>
 *
 * @note		$Id: tracelog.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configure.h"
// core
#include "sysmon.h"
#include "cmdengine.h"
#include "tracelog.h"
#include "stringlib.h"
#include "syslog.h"

#if TRACELOG_CONF_NUM_ENTRIES > 0
__attribute__((section(".noinit")))
struct tracelog tracelog;

/// tells if tracelog can accept input
static bool tracelog_initialized = 0;
#endif

#if defined(SYSLOG_CONF_LEVEL) && ((SYSLOG_CONF_LEVEL & SL_DEBUG) == SL_DEBUG)
static const char symon_event_names[] =
    "\0"
    "start\0"
    "exit\0"
    "assert\0"
    "event\0"
    "timer\0"
    "irqdis\0"
    "irqen\0"
    "irq\0"
    "switch\0"
    "\0"
    "memory\0"
    "memaccess\0"
    "opfault\0"
    "panic\0"
    "userdef\0"
    "\3";
#endif

/*-----------------------------------------------------------------------------------*/
#if TRACELOG_CONF_NUM_ENTRIES > 0
static void tracelog_snprint(char *buf, int bufsz, int i)
{
    struct tracelog_entry *trace = &tracelog.traces[i];
    int length = 0;
    bufsz -= 1;																	// save one for zero

#if (SYSLOG_CONF_LEVEL & SL_DEBUG) == SL_DEBUG
    /* when running in debug level, event names are available and can be printed */
    char *name = (char *)string_table_get(symon_event_names, trace->event);
    length = snprintf(buf, bufsz, "%#.2x (%s): ", trace->event, name);
#else
    length = snprintf(buf, bufsz, "%#.2x: ", trace->event);
#endif
    bufsz -= length;
    buf += length;

    switch(trace->type) {
        case TRACE_NUMBER: {
            tracelog_number_t uldata;
            memcpy(&uldata, trace->data, sizeof(tracelog_number_t));
            length += snprintf(buf, bufsz, "%#10lx (%lu)", uldata, uldata);
            break;
        }

        case TRACE_POINTER: {
            void *uldata;
            memcpy(&uldata, trace->data, sizeof(void *));
            length += snprintf(buf, bufsz, "%p", uldata);
            break;
        }

        case TRACE_STRING:
            length += snprintf(buf, bufsz, "%.*s", TRACELOG_CONF_ENTRY_SIZE, trace->data);
            break;

        default:
            break;
    }

    buf[length] = '\0';
}
#endif
/*-----------------------------------------------------------------------------------*/
/**
 * @brief	Writes a new tracelog entry
 * @internal
 */
static void
trace(
    enum tracelog_event	event,
    const enum tracelog_type t,
    const void (* const data),
    const int size
)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
    int length = size;

    if(tracelog_initialized == false) {
        return;
    }

    struct tracelog_entry *trace = &tracelog.traces[tracelog.tail];				// get current tail element

    /* increase tail */
    if((tracelog.tail + 1) < TRACELOG_CONF_NUM_ENTRIES) {
        tracelog.tail++;
    }
    else {
        tracelog.tail = 0;
    }

    /* fill meta data */
    trace->event = event & 0x7F;
    trace->type = t;

    /* calculate size */
    if(length == 0) {
        if(t == TRACE_STRING) {
            length = strlen((char *)data);
        }
    }

    length = (TRACELOG_CONF_ENTRY_SIZE < length) ? TRACELOG_CONF_ENTRY_SIZE : length;

    memcpy(trace->data, data, length);										// copy description
#endif
}
/*-----------------------------------------------------------------------------------*/
void trace_reset(void)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
#if SYSLOG_ISLEVEL(SL_DEBUG)
    char buffer[12];
    sysmon_write_reset_info(buffer, 12, sysmon.reset_code);
    trace_string(TRACELOG_EV_START, buffer);
#else
    trace_number(TRACELOG_EV_START, sysmon.reset_code);
#endif
#endif
}
/*-----------------------------------------------------------------------------------*/
void
tracelog_init(void)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0

    if(tracelog_initialized != 0) {
        return;
    }

    if(sysmon_initial_boot()) {
        memset(&tracelog, 0, sizeof(struct tracelog));						// clear tracelog buffer on initial boot only
    }

    tracelog_initialized = true;												// accept traces

    trace_reset();																// trace reason for last reset
#endif
}
void
tracelog_dump(void)
{
    printf("[trace] {\n");
#if TRACELOG_CONF_NUM_ENTRIES > 0
    char buf[30 + TRACELOG_CONF_ENTRY_SIZE];
    int i = tracelog.tail;														// tracelog tail holds next index

    do {
        i--;

        if(i < 0) {
            i = TRACELOG_CONF_NUM_ENTRIES - 1;
        }

        tracelog_snprint(buf, sizeof(buf), i);
        printf("\t %.2i: %s\n", i, buf);
    }
    while(i != tracelog.tail);

#endif

    printf("}\n");
}
/*-----------------------------------------------------------------------------------*/
void
trace_event(enum tracelog_event event)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
    trace(event, TRACE_NULL, NULL, 0);
#endif
}
/*-----------------------------------------------------------------------------------*/
void
trace_number(enum tracelog_event event, tracelog_number_t number)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
    trace(event, TRACE_NUMBER, &number, sizeof(tracelog_number_t));
#endif
}
/*-----------------------------------------------------------------------------------*/
void
trace_pointer(enum tracelog_event event, void *pointer)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
    trace(event, TRACE_POINTER, &pointer, sizeof(void *));
#endif
}
/*-----------------------------------------------------------------------------------*/
void
trace_string(enum tracelog_event event, char *string)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0
    trace(event, TRACE_STRING, string, strlen(string));
#endif
}
/*-----------------------------------------------------------------------------------*/
#if CMD_ISLEVEL(CMD_LEVEL_SYSTEM_DEBUG | CMD_LEVEL_HUMAN_USER)
/**
 * @brief	Trace command: <code>trace [event] [string]</code>
 * @ingroup	commands
 * @param	event	Event to add to trace (optional)
 * @param	string	String commend to include with event (optional)
 *
 * If no parameters are given dumps the current system event trace.
 */
ASCCMD(trace, CMDFLAG_SERIAL, "[event] [text]: print tracelog / trace event [num] with [text]");
CMD_FUNCTION(trace, cmdargs)
{
#if TRACELOG_CONF_NUM_ENTRIES > 0

    if(cmdargs->arg_size > 0) {
        enum tracelog_event event;
        char *c = (char *)cmdargs->args;
        event = (enum tracelog_event)strtoul(c, &c, 0);						// read event number

        if(event > 0) {
            c++;															// skip expected whitespace
            trace_string(event, c);											// generate event with argument as text
            return true;
        }
    }

#endif
    tracelog_dump();

    return CMD_SUCCESS;
}
#endif
/*-----------------------------------------------------------------------------------*/
/** @} */
