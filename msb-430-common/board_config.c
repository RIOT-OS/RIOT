#include <stdint.h>
#include <string.h>
#include <board-conf.h>
#include <config.h>
#include <flashrom.h>

void config_load(void)
{
    if (*((uint16_t *) INFOMEM) ==  CONFIG_KEY) {
        memcpy(&sysconfig, (char *)(INFOMEM + sizeof(CONFIG_KEY)), sizeof(sysconfig));
    }
    else {
        config_save();
    }
}

uint8_t config_save(void)
{
    configmem_t mem = { CONFIG_KEY, sysconfig  };
    return (flashrom_erase((uint8_t *) INFOMEM) && flashrom_write((uint8_t *) INFOMEM, (char *) &mem, sizeof(mem)));
}
