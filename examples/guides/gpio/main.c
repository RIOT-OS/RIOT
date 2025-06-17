#include "periph/gpio.h"
#include "board.h"
#include "ztimer.h"

// Define the LED0 pin and mode
gpio_t led0 = GPIO_PIN(1, 9);
gpio_mode_t led0_mode = GPIO_OUT;

// Define the LED0 pin and mode
gpio_t led1 = GPIO_PIN(1, 10);
gpio_mode_t led1_mode = GPIO_OUT;

// Define the button pin
gpio_t button = GPIO_PIN(1, 2);

void button_callback(void *arg)
{
    (void) arg; /* the argument is not used */
    if (!gpio_read(button)) {
        gpio_set(led1);
    }
    else {
        gpio_clear(led1);
    }
}

int main(void) {
  // Initialize the LED0 pin
  gpio_init(led0, led0_mode);
  // Turn off the LED0 pin
  gpio_clear(led0);

  // Initialize the LED1 pin
  gpio_init(led1, led1_mode);
  // Turn off the LED1 pin
  gpio_clear(led1);

  // Initialize the button pin
  gpio_init_int(button, GPIO_IN_PU, GPIO_BOTH, button_callback, NULL);

  while (1) {
    gpio_toggle(led0);
    ztimer_sleep(ZTIMER_MSEC, 500);
  }
}
