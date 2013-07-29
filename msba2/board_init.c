#include <board.h>
#include <cpu.h>

void loop_delay(void)
{
    volatile uint16_t i, j;

    for (i = 1; i < 30; i++) {
        for (j = 1; j != 0; j++) {
            asm volatile(" nop ");
        }
    }
}

void bl_blink(void)
{
    LED_RED_ON;
    LED_GREEN_ON;

    loop_delay();

    LED_RED_OFF;
    LED_GREEN_OFF;
}

void bl_init_ports(void)
{
    SCS |= BIT0;                                            // Set IO Ports to fast switching mode

    /* UART0 */
    PINSEL0 |= BIT4 + BIT6;                                 // RxD0 and TxD0
    PINSEL0 &= ~(BIT5 + BIT7);

    /* LEDS */
    FIO3DIR |= LED_RED_PIN;
    FIO3DIR |= LED_GREEN_PIN;
    LED_RED_OFF;
    LED_GREEN_OFF;

    /* short blinking of both of the LEDs on startup */
    bl_blink();
}

