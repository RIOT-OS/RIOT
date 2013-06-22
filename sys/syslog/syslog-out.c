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
 * @file
 * @ingroup		syslog
 * @brief		System Logging Service output implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @version     $Revision: 3854 $
 *
 * @note		$Id: syslog-out.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include <stdio.h>
#include <string.h>
#ifdef MODULE_FAT
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#endif
// sys
#include "cfg-feuerware.h"
#include "syslog.h"
#include "clock.h"
#include "device-rs232.h"
#if FEUERWARE_CONF_CORE_SUPPORTS_TIME
#include "timelib.h"
#endif
// net
#include "net-types.h"
#ifdef MODULE_TRACELOG
#include "tracelog.h"
#endif

/*-----------------------------------------------------------------------------------*/
void syslog_format_ascii(struct syslog_args	*args, struct syslog_chainlink *chainlink)
{
    char buffer[SYSLOG_CONF_BUFSIZE + 25];
    int msglen = 0;

    if(args->message[0] != '\t') {
        const char *strlevel = syslog_leveltostring(args->level);
        msglen = snprintf(buffer, SYSLOG_CONF_BUFSIZE + 23,
                          "#[%u.%u:%u=%s] %s:",
                          NETWORK_ADDR_NET(NET_LOCAL_ADDRESS), NETWORK_ADDR_HOST(NET_LOCAL_ADDRESS),
                          args->level, strlevel, args->module
                         );
    }

    msglen += snprintf(buffer + msglen, SYSLOG_CONF_BUFSIZE + 23 - msglen, "%s", args->message);
    buffer[msglen++] = '\n';
    buffer[msglen] = '\0';

    args->message = buffer;

    if(chainlink != NULL && chainlink->fpout != NULL) {
        chainlink->fpout(args, chainlink->next);
    }
}
/*-----------------------------------------------------------------------------------*/
void syslog_format_xml(struct syslog_args *args, struct syslog_chainlink *chainlink)
{
    const size_t bufsize = SYSLOG_CONF_BUFSIZE + 80;
    char buffer[bufsize];
    char tbuf[20];
    int msglen = 0;

#if FEUERWARE_CONF_CORE_SUPPORTS_TIME
    time_get_string(tbuf, sizeof(tbuf));
#else
    sprintf(tbuf, "%lu", clock_time(NULL));
#endif

    msglen = snprintf(buffer, bufsize,
                      "<log lvl=%u src=\"%u.%u\" id=%u ts=\"%s\" mod=\"%s\">%s</log>\n",
                      args->level, NETWORK_ADDR_NET(NET_LOCAL_ADDRESS), NETWORK_ADDR_HOST(NET_LOCAL_ADDRESS),
                      args->msgnum, tbuf, args->module, args->message
                     );

    args->message = buffer;

    if(chainlink != NULL && chainlink->fpout != NULL) {
        chainlink->fpout(args, chainlink->next);
    }
}
/*-----------------------------------------------------------------------------------*/
void syslog_copy_stdout(struct syslog_args	*args, struct syslog_chainlink *chainlink)
{
    if(args->message[0] != '\t') {
        const char *strlevel = syslog_leveltostring(args->level);
        printf("#[%u.%u:%u=%s] %s:",
               NETWORK_ADDR_NET(NET_LOCAL_ADDRESS), NETWORK_ADDR_HOST(NET_LOCAL_ADDRESS),
               args->level, strlevel, args->module
              );
    }

    printf("%s\n", args->message);

    if(chainlink != NULL && chainlink->fpout != NULL) {
        chainlink->fpout(args, chainlink->next);
    }
}
/*-----------------------------------------------------------------------------------*/
void syslog_out_stdout(struct syslog_args	*args, struct syslog_chainlink *chainlink)
{
    printf(args->message);

    if(chainlink != NULL && chainlink->fpout != NULL) {
        chainlink->fpout(args, chainlink->next);
    }
}
/*-----------------------------------------------------------------------------------*/
#ifdef MODULE_FAT
static int syslog_file = -1;

static int fat_open_logfile(const char *path);
static int fat_open_logfile(const char *path)
{
    char t[20];
    int file;

    file = open(path, O_WRONLY | O_APPEND | O_CREAT);

    time_get_string(t, sizeof(t));

    if(file >= 0) {
        syslog_notice("sys", "%s log %s opened", t, path);
    }
    else {
        syslog_warn("sys", "%s log %s failed", t, path);
    }

    return file;
}

void syslog_out_file(struct syslog_args *args, struct syslog_chainlink *chainlink)
{
    if(syslog_file >= 0) {
        size_t length = (size_t)strlen(args->message);
        int ret = write(syslog_file, args->message, length);

        if(ret == 1) {
#ifdef MODULE_TRACELOG
            trace_string(TRACELOG_EV_MEMORY, "fat");
#endif
        }
    }

    if(chainlink != NULL && chainlink->fpout != NULL) {
        chainlink->fpout(args, chainlink->next);
    }
}

bool syslog_open_file(void)
{
    syslog_file = fat_open_logfile("SYSLOG.LOG");
    return (syslog_file >= 0);
}
void syslog_close_file(void)
{
    close(syslog_file);
    syslog_file = -1;
}
#endif
