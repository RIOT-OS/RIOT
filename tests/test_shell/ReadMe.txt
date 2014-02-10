This application shows how to use own or the system shell commands. In order to use
the system shell commands:

1. Additionally to the module: shell, shell_commands and posix,
   the module for the corresponding system command is to include, e.g.
   module ps for the ps command (cf. the Makefile in the application root
   directory).
2. The shell must be initialized as follows:
        2.1 shell_t sys_shell;
        2.2 shell_init(&shell, shell_commands, shell_bufsize, shell_readc,
                       shell_putchar);
            or shell_init(&sys_shell, NULL, shell_bufsize,
                          shell_readc, shell_putchar);
            /* to initialize without the built-in shell commands */
        2.3 shell_run(&sys_shell);
