#include <stdio.h>
#include <inttypes.h>
#include <walltime.h>
extern const char* riot_logo;
extern const char* riot_version;
extern const char* rshell_version;
extern const char* cpu;
extern const uint32_t ram;
extern const char* board;
int infofetch(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts(riot_logo);
    puts(riot_version);
    puts(rshell_version);
    puts(cpu);
    printf("\e[0mram: \e[1m%" PRIu32 " KiB\n", ram/1024);
    puts(board);
    printf("\e[0muptime: \e[1m%" PRIu32 " s\e[0m\n", walltime_uptime(true));

    return 0;
}
