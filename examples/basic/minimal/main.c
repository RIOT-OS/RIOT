#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include <string.h>
#include "periph/gpio.h"

int main(void) {
    xosc_sleep(1000); // Sleep for 1s to connect

    gpio_init(LED0_PIN_ID, GPIO_OUT); // Initialize the LED pin
    gpio_init(OSC_DEBUG_PIN_ID, GPIO_OUT); // Initialize the debug pin

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    uint32_t i = 0;
    while (true) {
        gpio_toggle(LED0_PIN_ID); // Toggle the LED
        gpio_toggle(OSC_DEBUG_PIN_ID); // Toggle the debug pin
        printf("Hello World! %u\n", i++); // Print a message to the console
        xosc_sleep(1000); // Sleep for 1 sec
    }
  return 0;
}
