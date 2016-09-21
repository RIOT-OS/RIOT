#include "board.h"
#include "periph/gpio.h"

extern const void *interrupt_vector[];

void board_init(void)
{
    /* bootoader will initialize the cpu so we only need to initialize the Cortex-M core */
    cortexm_init();

    /* Set adress of the vector table */
    SCB->VTOR = (uint32_t) interrupt_vector;

    /* initialize the on-board LEDs */
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);
    gpio_init(LED3_PIN, GPIO_OUT);
}
