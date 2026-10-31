#include <stdio.h>
#include <inttypes.h>
#include <walltime.h>
extern const char* riot_logo;
extern const char* riot_version_formatted;
extern const char* rshell_version_formatted;
extern const char* cpu_formatted;
extern const uint32_t ram;
extern const char* board_formatted;
int infofetch(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    puts(riot_logo);
    puts(riot_version_formatted);
    puts(rshell_version_formatted);
    puts(cpu_formatted);
    if (ram==0) {
        printf("\e[0mram: \e[1mN/A (the variable is not defined on your device)\e[0m" "\n");
    } else {
        printf("\e[0mram: \e[1m%" PRIu32 " KiB\n", ram/1024);
    }
    
    puts(board_formatted);
    printf("\e[0muptime: \e[1m%" PRIu32 " s\e[0m\n", walltime_uptime(true));

    return 0;
}
