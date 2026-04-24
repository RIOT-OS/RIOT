#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

static inline uint32_t _mode_to_dm(gpio_mode_t mode)
{
    switch (mode) {
        case GPIO_IN:
            return PSOC6_DM_HIGHZ;
        case GPIO_IN_PU:
            return PSOC6_DM_PULLUP;
        case GPIO_IN_PD:
            return PSOC6_DM_PULLDOWN;
        case GPIO_OUT:
            return PSOC6_DM_STRONG;
        case GPIO_OD:
            return PSOC6_DM_OD_SLOW;
        case GPIO_OD_PU:
            return PSOC6_DM_OD_PULLUP;
        default:
            return PSOC6_DM_HIGHZ;
    }
}

// route a pin back to GPIO mode and apply the requested drive mode
int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    uint8_t pin_num = GPIO_PIN_NUM(pin);
    GPIO_PRT_Type *port = GPIO_PORT(pin);

    psoc6_route_gpio(pin);

    switch (mode) {
        case GPIO_OUT:
        case GPIO_OD:
        case GPIO_OD_PU:
            port->OUT &= ~(1UL << pin_num);
            break;
        default:
            break;
    }

    psoc6_set_drive_mode(pin, _mode_to_dm(mode));
    return 0;
}

bool gpio_read(gpio_t pin)
{
    GPIO_PRT_Type *port = GPIO_PORT(pin);
    return ((port->IN >> GPIO_PIN_NUM(pin)) & 1U);
}

void gpio_set(gpio_t pin)
{
    GPIO_PORT(pin)->OUT_SET = (1UL << GPIO_PIN_NUM(pin));
}

void gpio_clear(gpio_t pin)
{
    GPIO_PORT(pin)->OUT_CLR = (1UL << GPIO_PIN_NUM(pin));
}

void gpio_toggle(gpio_t pin)
{
    GPIO_PORT(pin)->OUT_INV = (1UL << GPIO_PIN_NUM(pin));
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}
