#include "app_error.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t *p_file_name)
{
    puts("nordic softdevice app_error_handler()");
}
