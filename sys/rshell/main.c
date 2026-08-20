/*
 * Copyright (C) 2008, 2009, 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */
const char* rshell_logo = "\
\e[31m                            ##########       \e[32m                                                          \e[0m\n\
\e[31m                         ################    \e[32m                                                          \e[0m\n\
\e[31m                       ####################  \e[32m                                                          \e[0m\n\
\e[31m                     #########      ######## \e[32m                                                          \e[0m\n\
\e[31m                    #######           #######\e[32m                                                          \e[0m\n\
\e[31m                    ######             ######\e[32m                                                          \e[0m\n\
\e[31m                   ######              ######\e[32m                                                          \e[0m\n\
\e[31m                   ######              ######\e[32m                                                          \e[0m\n\
\e[31m                   ######             #######\e[32m                                                          \e[0m\n\
\e[31m                   ######            ####### \e[32m                                                          \e[0m\n\
\e[31m                   ######          ########  \e[32m                                                          \e[0m\n\
\e[31m                   ######      ###########   \e[32m                                                          \e[0m\n\
\e[31m            ##     ######   ############     \e[32m                                                          \e[0m\n\
\e[31m       #########   ######   #########        \e[32m                                                          \e[0m\n\
\e[31m     ###########    #####     ##             \e[32m                                                          \e[0m\n\
\e[31m   ########         #####                    \e[32m                                                          \e[0m\n\
\e[31m  ######            #####                    \e[32m                                                          \e[0m\n\
\e[31m ######             #####       ####         \e[32m               ++++                           +++++  +++++\e[0m\n\
\e[31m######              ######      #####        \e[32m               ++++                           +++++  +++++\e[0m\n\
\e[31m#####               ######      #######      \e[32m  ++++++++     ++++ ++++++       +++++++      +++++  +++++\e[0m\n\
\e[31m#####               #####        #######     \e[32m++++++++++++   ++++++++++++    ++++++++++++   +++++  +++++\e[0m\n\
\e[31m#####              ######         #######    \e[32m++++++         +++++   ++++   +++++    ++++   +++++  +++++\e[0m\n\
\e[31m #####            #######          #######   \e[32m +++++++++++   ++++    ++++   ++++++++++++++  +++++  +++++\e[0m\n\
\e[31m #######        ########            #######  \e[32m     +++++++   ++++    ++++   ++++            +++++  +++++\e[0m\n\
\e[31m  ####################               ######  \e[32m++++++++++++   ++++    ++++   +++++++++++++   +++++  +++++\e[0m\n\
\e[31m    ################                   ###   \e[32m ++++++++++    ++++    ++++     ++++++++++    +++++  +++++\e[0m\n\
\e[31m       ##########                            \e[32m                                                          \e[0m\n\
\n\
welcome to Rshell, the RIOTos shell\n\
";

#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "ztimer.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "neofetch.h"
#include "auto_init_utils.h"
SHELL_COMMAND(neofetch, "gives information about the OS and (soon) the hardware", infofetch);
void rshell(void)
{

    ztimer_sleep(ZTIMER_MSEC, 5000);
    (void) puts(rshell_logo);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    while (true) {
        
    }
}
AUTO_INIT(rshell, 9999);
