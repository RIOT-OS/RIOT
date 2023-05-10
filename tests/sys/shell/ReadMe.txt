This application shows how to use own or the system shell commands. In order to use
the system shell commands:

1. Additionally to the module: shell, shell_commands,
   the module for the corresponding system command is to include, e.g.
   module ps for the ps command (cf. the Makefile in the application root
   directory).
2. Start the shell like this:
    2.1 reserve buffer:
        char line_buf[SHELL_DEFAULT_BUFSIZE];
    2.1a run shell only with system commands:
        shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    2.1b run shell with provided commands in addition to system commands:
        shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
