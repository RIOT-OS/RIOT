/**
 * Syslog daemon 
 *
 * Copyright (C) 2009-2013 Freie Universitaet Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup syslog 
 * @{
 * @file        syslog-api.c
 * @author      Freie Universitaet Berlin
 * @}
 */

*******************************************************************************/

#include <stdarg.h>
#include "syslog.h"
#include "tracelog.h"

#undef  VSYSLOG
#define VSYSLOG(level, strModule, strFmt, argp)		vsyslog(level, strModule, strFmt, argp)

void
syslog(const uint8_t level, const char(*const strModule), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(level, strModule, strFmt, argp);
    va_end(argp);
}
/*-----------------------------------------------------------------------------------*/
#if (SYSLOG_CONF_LEVEL & SL_EMERGENCY) == SL_EMERGENCY
void syslog_emergency(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_EMERGENCY, mod, strFmt, argp);
    va_end(argp);
}
#endif
/*-----------------------------------------------------------------------------------*/
#if (SYSLOG_CONF_LEVEL & SL_CRITICAL) == SL_CRITICAL
void syslog_critical(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_CRITICAL, mod, strFmt, argp);
    va_end(argp);
}
#endif
/*-----------------------------------------------------------------------------------*/
#if (SYSLOG_CONF_LEVEL & SL_WARN) == SL_WARN
void syslog_warn(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_WARN, mod, strFmt, argp);
    va_end(argp);
}
#endif
/*-----------------------------------------------------------------------------------*/
#if (SYSLOG_CONF_LEVEL & SL_NOTICE) == SL_NOTICE
void syslog_notice(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_NOTICE, mod, strFmt, argp);
    va_end(argp);
}
#endif
/*-----------------------------------------------------------------------------------*/
#if (SYSLOG_CONF_LEVEL & SL_INFO) == SL_INFO
void syslog_info(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_INFO, mod, strFmt, argp);
    va_end(argp);
}
#endif
/*-----------------------------------------------------------------------------------*/
#if SYSLOG_ISLEVEL(SL_DEBUG)
void syslog_debug(const char(*const mod), const char *strFmt, ...)
{
    va_list argp;

    va_start(argp, strFmt);
    VSYSLOG(SL_DEBUG, mod, strFmt, argp);
    va_end(argp);
}
#endif

