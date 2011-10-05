#include <shell.h>
#include <stdlib.h>

#ifdef MODULE_PS
   extern void _ps_handler(char* unused);
#endif

const shell_command_t _shell_command_list[] = {
#ifdef MODULE_PS
    {"ps", _ps_handler},
#endif
    {NULL, NULL}
};

