

#include "dbgpin.h"

const gpio_t dbgpin_pins[] = { DBGPIN_PINS };

unsigned dbgpin_count(void)
{
    return ARRAY_SIZE(dbgpin_pins);
}

void dbgpin_init(void)
{
    for (unsigned i = 0; i < ARRAY_SIZE(dbgpin_pins); i++) {
        gpio_init(dbgpin_pins[i], GPIO_OUT);
        gpio_clear(dbgpin_pins[i]);
    }
}
