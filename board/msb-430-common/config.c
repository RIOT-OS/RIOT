#include <stdint.h>
#include <board-conf.h>
#include <config.h>
#include <flashrom.h>

uint8_t config_save(void) {
    configmem_t mem = { CONFIG_KEY, sysconfig  };
    return (flashrom_erase((uint8_t*) INFOMEM) && flashrom_write((uint8_t*) INFOMEM, (char*) &mem, sizeof(mem)));
}
