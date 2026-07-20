
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
const char* riot_version = "RIOTos: \e[1m" RIOT_VERSION;
const char* rshell_version = "\e[0mRshell: \e[1m 26.001.001";
const char* cpu = "\e[0mcpu: \e[1m" RIOT_CPU;
const uint32_t ram = CPU_RAM_SIZE;
const char* board = "\e[0mboard: \e[1m" RIOT_BOARD "\e[0m";
