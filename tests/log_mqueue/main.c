#include <string.h>
#include <stdio.h>

#define LOG_LEVEL LOG_INFO
#include "shell_commands.h"
#include "shell.h"
#include "log.h"

int log_test(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    {"log_test", "starts a syslog test with predefinec values", log_test},
    {NULL, NULL, NULL}};

int main(void)
{
    /* define buffer to be used by the shell */
    log_test(0, NULL);
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    /* define own shell commands */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}

int log_test(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("Testing LOG_*(...) Macros LOG_LEVEL is %d\n",LOG_LEVEL);
    LOG_EMERG("Test LOG_EMERG(...)\n");
    LOG_ALERT("Test LOG_ALERT(...)\n");
    LOG_CRIT("Test LOG_CRIT(...)\n");
    LOG_ERROR("Test LOG_ERROR(...)\n");
    LOG_WARNING("Test LOG_WARNING(...)\n");
    LOG_NOTICE("Test LOG_NOTICE(...)\n");
    LOG_INFO("Test LOG_INFO(...)\n");
    LOG_DEBUG("Test LOG_DEBUG(...)\n");

    printf("\nTesting LOG(level,...) Macros LOG_LEVEL is %d\n",LOG_LEVEL);
    LOG(LOG_EMERG,"Test LOG(LOG_EMERG|0,...)\n");
    LOG(LOG_ALERT,"Test LOG(LOG_ALERT|0,...)\n");
    LOG(LOG_CRIT,"Test LOG(LOG_CRIT|0,...)\n");
    LOG(LOG_ERROR,"Test LOG(LOG_ERROR|0,...)\n");
    LOG(LOG_ERR,"Test LOG(LOG_ERROR|0,...)\n");
    LOG(LOG_WARNING,"Test LOG(LOG_WARNING|0,...)\n");
    LOG(LOG_NOTICE,"Test LOG(LOG_NOTICE|0,...)\n");
    LOG(LOG_INFO,"Test LOG(LOG_INFO|0,...)\n");
    LOG(LOG_DEBUG,"Test LOG(LOG_DEBUG|0,...)\n");

    printf("\nTesting LOG((level|LOG_LOCAL1),...) Macros LOG_LEVEL is %d\n",LOG_LEVEL);
    LOG((LOG_EMERG|LOG_LOCAL1),"Test LOG(LOG_EMERG|LOG_LOCAL1,...)\n");
    LOG((LOG_ALERT|LOG_LOCAL1),"Test LOG(LOG_ALERT|LOG_LOCAL1,...)\n");
    LOG((LOG_CRIT|LOG_LOCAL1),"Test LOG(LOG_CRIT|LOG_LOCAL1,...)\n");
    LOG((LOG_ERROR|LOG_LOCAL1),"Test LOG(LOG_ERROR|LOG_LOCAL1,...)\n");
    LOG((LOG_ERR|LOG_LOCAL1),"Test LOG(LOG_ERROR|LOG_LOCAL1,...)\n");
    LOG((LOG_WARNING|LOG_LOCAL1),"Test LOG(LOG_WARNING|LOG_LOCAL1,...)\n");
    LOG((LOG_NOTICE|LOG_LOCAL1),"Test LOG(LOG_NOTICE|LOG_LOCAL1,...)\n");
    LOG((LOG_INFO|LOG_LOCAL1),"Test LOG(LOG_INFO|LOG_LOCAL1,...)\n");
    LOG((LOG_DEBUG|LOG_LOCAL1),"Test LOG(LOG_DEBUG|LOG_LOCAL1,...)\n");
    return 0;
}
