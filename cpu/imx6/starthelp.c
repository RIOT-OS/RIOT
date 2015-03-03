#include "platform_init.h"

extern void board_init(void);
extern void kernel_init(void);

int starthelp(void)
{
    platform_init();

    //print_version();

    //show_freq();
    //show_ddr_config();

    board_init();
    kernel_init();

    return 0;
}
