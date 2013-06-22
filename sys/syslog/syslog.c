/******************************************************************************
Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.

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
 * @addtogroup	syslog
 * @{
 */

/**
 * @file
 * @brief		System Logging Service implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Michael Baar <baar@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: syslog.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfg-feuerware.h"
#include "cmdengine.h"
#include "device-rs232.h"
#include "syslog.h"
#include "sysmon.h"
#include "tracelog.h"

static const char *syslog_level_stringtable =	"emergency\0"
												"critical\0"
												"warn\0"
												"notice\0"
												"info\0"
												"debug\0"
												"\3";

__attribute__((section(".noinit")))
static volatile unsigned int syslog_msgnum;

static unsigned short syslog_flagtolevel(const uint8_t level);
static int find_interface_index_for_name(const char *name);

extern struct syslog_interface syslog_interfaces[];

/*-----------------------------------------------------------------------------------*/
void syslog_init(void)
{
    if(sysmon_initial_boot()) {
        syslog_msgnum = 0;
    }
}
/*-----------------------------------------------------------------------------------*/
static bool testlevel(uint8_t filter_level, const uint8_t level)
{
    return ((filter_level & level) != 0);
}
/*-----------------------------------------------------------------------------------*/
void vsyslog(const uint8_t level, const char(*const strModule),
             const char *strFmt, va_list argp)
{
    int i;
    struct syslog_interface	*slif;				// syslog interface
    struct syslog_args args;						// output function arguments
    char   message[SYSLOG_CONF_BUFSIZE];			// message buffer
    int    msglen = 0;

    args.message = NULL;

    /* check each syslog interface */
    for(i = 0; i < SYSLOG_CONF_NUM_INTERFACES; i++) {
        slif = &syslog_interfaces[i];

        /* run interface filter */
        if(slif->name != NULL && testlevel(cfg_feuerware.level[i], level)) {
            /* filter matched, produce output */
            if(args.message == NULL) {
                /* initialize structure one time, when actually needed */
                args.msgnum = syslog_msgnum++;
                args.level = syslog_flagtolevel(level);
                args.module = strModule;
                args.message = message;
                msglen = vsnprintf(message, SYSLOG_CONF_BUFSIZE - 1, strFmt, argp);
            }

            args.interface = (const struct syslog_interface *)slif;

            /* invoke first link of ouput chain */
            if(slif->chain->fpout != NULL) {
                slif->chain->fpout(&args, slif->chain->next);
            }
        }
    }
}
/*-----------------------------------------------------------------------------------*/
void syslog_printlevel(char *buffer, size_t bufsize, uint8_t level)
{
    uint8_t l = level;
    int intlevel;
    int bufpos;
    int len = 0;

    bufpos = snprintf(buffer, bufsize, "%#.2x=", level);
    bufsize -= bufpos;

    for(intlevel = 0; intlevel < SYSLOG_LEVELS_COUNT; intlevel++) {
        if(l & 0x0001) {
            const char *string = string_table_get(syslog_level_stringtable, intlevel);

            if(string) {
                if(bufsize < 0) {
                    bufsize = 0;
                }

                len = snprintf(&buffer[bufpos], bufsize, "%s%s", ((len > 0) ? "|" : ""), (char *)string);
                bufsize -= len;
                bufpos += len;
            }
        }

        l >>= 1;
    }
}
/*-----------------------------------------------------------------------------------*/
uint8_t syslog_set_level(const char *name, uint8_t level)
{
    uint8_t result;
    int index = find_interface_index_for_name(name);

    if(index < 0) {
        return 0;
    }

    result = cfg_feuerware.level[index];
    cfg_feuerware.level[index] = level;
    cfg_feuerware_spec.state->modified = 1;

    return result;
}
/*-----------------------------------------------------------------------------------*/
const char *syslog_leveltostring(unsigned int level)
{
    return string_table_get(syslog_level_stringtable, level);
}
/*-----------------------------------------------------------------------------------*/

#if CMD_ISLEVEL(CMD_LEVEL_HUMAN_USER)
/**
 * @brief		Syslog-level property: <code>syslog [interface] [newlevel]</code>
 * @ingroup		commands
 * @param[in]	interface		Name of the syslog interface to change (optional)
 * @param[in]	newlevel		New loglevel value (see uint8_t)
 * @return		Current level
 *
 * \p Returns and optionally sets the syslog log-level. For available combinations see
 * uint8_t.
 *
 * \p Usage:
 * \li \c syslog	Prints syslog configuration
 * \li \c syslog \c 0x3f	Sets loglevel of syslog node 0 (usually stdout) to 0x3f
 * \li \c syslog \c stdout \c 0x3f	Sets loglevel of syslog node "stdout" to 0x3f
 */
ASCCMD(syslog, 0, "[interface] [byte val]: syslog-level property");
CMD_FUNCTION(syslog, cmdargs)
{
    int i;

    if(cmdargs->arg_size > 0) {
        unsigned int argc;
        const char *arg;
        const char *name = NULL;
        unsigned int value;

        argc = cmd_split_arguments(cmdargs->args);
        arg = cmdargs->args;

        if(*arg >= 'A') {
            // first argument is a string
            name = arg;
            // move to next argument
            arg = cmd_get_next_argument(arg);
        }

        if(*arg == '\0') {
            return CMD_ERROR;
        }

        value = strtoul(arg, NULL, 0);
        syslog_set_level(name, value);
    }

    for(i = 0; i < SYSLOG_CONF_NUM_INTERFACES; i++) {
        if(syslog_interfaces[i].name != NULL) {
            char buf[SYSLOG_PRINTLEVEL_MAXLEN];
            syslog_printlevel(buf, sizeof(buf), cfg_feuerware.level[i]);
            cmdargs->fresponse(cmdargs,
                               CMD_RESPONSE_MULTILINE,
                               "%s:%s",
                               syslog_interfaces[i].name,
                               buf
                              );
        }
    }

    return CMD_SUCCESS;
}
#endif
/*-----------------------------------------------------------------------------------*/
/**
 * @brief	Convert bit flag to bit number
 */
static unsigned short syslog_flagtolevel(const uint8_t level)
{
    uint8_t l = level;
    unsigned short intlevel;

    for(intlevel = 0; intlevel < SYSLOG_LEVELS_COUNT; intlevel++) {
        if((l & 1) == 1) {
            break;
        }

        l >>= 1;
    }

    return intlevel;
}
/*-----------------------------------------------------------------------------------*/
/**
 * @brief	Find an interface for a given name
 */
static int find_interface_index_for_name(const char *name)
{
    int i;

    for(i = 0; i < SYSLOG_CONF_NUM_INTERFACES; i++) {
        if(strcmp(syslog_interfaces[i].name, name) == 0) {
            return i;
        }
    }

    return -1;
}

/** @} */
