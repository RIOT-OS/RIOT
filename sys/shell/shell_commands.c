#include <shell.h>

const shell_command_t _shell_command_list[] = {
#ifdef MODULE_PS
    {"ps", ps_handler},
#endif
    {NULL, NULL}
};

