#include <string.h>
#include <stdio.h>

#define LOG_LEVEL LOG_ALL

#include "log.h"

int log_test(int argc, char **argv);

int main(void)
{
    /* define buffer to be used by the shell */
    log_test(0, NULL);
    return 0;
}

int log_test(int argc, char **argv) {
    (void)argc;
    (void)argv;
    LOG_ERROR("Test LOG_ERROR\n");
    LOG_WARNING("Test LOG_WARNING\n");
    LOG_INFO("Test LOG_INFO\n");
    LOG_DEBUG("Test LOG_DEBUG\n");
    LOG(LOG_ERROR,"Test LOG_ERROR\n");
    LOG(LOG_WARNING,"Test LOG_WARNING\n");
    LOG(LOG_INFO,"Test LOG_INFO\n");
    LOG(LOG_DEBUG,"Test LOG_DEBUG\n");
    return 0;
}
