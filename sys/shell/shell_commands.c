#include <shell.h>
#include <stdlib.h>

#ifdef MODULE_PS
extern void _ps_handler(char* unnused);
#endif

const shell_command_t _shell_command_list[] = {
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
    {NULL, NULL, NULL}
};

