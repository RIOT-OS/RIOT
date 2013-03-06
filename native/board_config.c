#include <stdio.h>
#include <stdint.h>

#include <config.h>

void config_load(void)
{
    printf("XXX: config_load(): not implemented\n");
    return;
}

uint8_t config_save(void)
{
    printf("XXX: config_save(): not implemented - your config will vanish on process termination\n");
    return 1;
}
