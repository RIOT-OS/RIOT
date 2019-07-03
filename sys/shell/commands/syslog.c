/*
 * Copyright (C) 2019 Robin Lösch <robin@chilio.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for syslog logging
 *
 * @author      Robin Lösch <robin@chilio.net>
 * @}
 */

#include "syslog/syslog.h"
#include "log.h"

int _syslog_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    openlog("test", LOG_CONS, LOG_USER);
    return 0;
}

int _syslog(int argc, char **argv)
{
    if(argc < 2) {
        return -1;
    }
    syslog(SYSLOG_EMERG, argv[1]);
    return 0;
}
