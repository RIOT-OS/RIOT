/*
 * SPDX-FileCopyrightText: 2026 github-user: ff6f8d68  (discord-user: hexagon_inc) 
 * SPDX-License-Identifier: MIT-0
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
#include "infocommand.h"
#include "auto_init_utils.h"
SHELL_COMMAND(info, "gives information about the OS and (soon) the hardware", infofetch);
void rshell(void)
{

    ztimer_sleep(ZTIMER_MSEC, 500);
    (void) puts(rshell_logo);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    while (true) {
    }
}
AUTO_INIT(rshell, 9999);
