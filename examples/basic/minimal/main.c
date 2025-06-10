#include <stdio.h>

#include "board.h"
#include "periph_conf.h"
#include <string.h>
#include "periph/gpio.h"

#define UART_UARTFR_RXFF_BITS 1<<6
#define UART_UARTFR_TXFE_BITS 1<<7

int main(void) {
    xosc_sleep(1000); // Sleep for 3 secs to connect

    gpio_init(LED0_PIN_ID, GPIO_OUT); // Initialize the LED pin
    gpio_init(OSC_DEBUG_PIN_ID, GPIO_OUT); // Initialize the debug pin

    uint32_t i = 0;
    while (true) {
        gpio_toggle(LED0_PIN_ID); // Toggle the LED
        gpio_toggle(OSC_DEBUG_PIN_ID); // Toggle the debug pin
        printf("Hello World! %u\n", i++); // Print a message to the console
        xosc_sleep(300); // Sleep for 1 sec
    }
  return 0;
}
