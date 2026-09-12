// defined variables for info command
#ifndef CPU_RAM_SIZE
#define CPU_RAM_SIZE (0000)
#endif
const char* riot_logo = "\n\
\e[31m                 ########       \e[32m                            \e[0m\n\
\e[31m               ############     \e[32m                            \e[0m\n\
\e[31m             ######     #####   \e[32m                            \e[0m\n\
\e[31m            #####        ####   \e[32m                            \e[0m\n\
\e[31m            ####         ####   \e[32m                            \e[0m\n\
\e[31m            ####        #####   \e[32m                            \e[0m\n\
\e[31m            ####      ######    \e[32m                            \e[0m\n\
\e[31m            ####  ########      \e[32m ++      ++++++    +++++++++\e[0m\n\
\e[31m    *#####  ####  ######        \e[32m ++     +++ =+++      +++   \e[0m\n\
\e[31m  ########  ####                \e[32m ++    +++    ++      +++   \e[0m\n\
\e[31m ####        ###                \e[32m ++    +++    ++      +++   \e[0m\n\
\e[31m####         ###    ####        \e[32m ++    +++    ++      +++   \e[0m\n\
\e[31m###          ###     ####       \e[32m ++    +++    ++      +++   \e[0m\n\
\e[31m####        ####      ####      \e[32m ++    +++    ++      +++   \e[0m\n\
\e[31m#####      #####       ####     \e[32m ++     +++  +++      +++   \e[0m\n\
\e[31m #############          ####    \e[32m ++      ++++++       +++   \e[0m\n\
\e[31m   #########                                               \e[0m\n\
\n\
\e[0m";
#include <stdint.h>
#include <walltime.h>
const char* riot_version_formatted = "RIOTos: \e[1m" RIOT_VERSION;
const char* rshell_version_formatted = "\e[0mRshell: \e[1m V1.0.1-260904";
const char* cpu_formatted = "\e[0mcpu: \e[1m" RIOT_CPU;
const uint32_t ram = CPU_RAM_SIZE;
const char* board_formatted = "\e[0mboard: \e[1m" RIOT_BOARD "\e[0m";
