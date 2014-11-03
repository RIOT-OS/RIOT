#include <stdint.h>

#include "thread.h"
#include "LPC17xx.h"
#include "board.h"

#define WAIT_LONG   (10000000)
#define WAIT_SHORT   (3000000)

char r_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
char i_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
char o_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
char t_stack[KERNEL_CONF_STACKSIZE_DEFAULT];

void blink_R(uint8_t nr);
void blink_I(uint8_t nr);
void blink_O(uint8_t nr);
void blink_T(uint8_t nr);

void led_on(uint8_t i) {
    switch (i) {
        case 1:
            LED_ON(1);
            break;
        case 2:
            LED_ON(2);
            break;
        case 3:
            LED_ON(3);
            break;
        default:
            LED_ON(4);
            break;
    }
}

void led_off(uint8_t i) {
    switch (i) {
        case 1:
            LED_OFF(1);
            break;
        case 2:
            LED_OFF(2);
            break;
        case 3:
            LED_OFF(3);
            break;
        default:
            LED_OFF(4);
            break;
    }
}

void r_blinker(void) {
    blink_R(4);
    loop_delay(WAIT_LONG);
    thread_sleep();
}

void i_blinker(void) {
    blink_I(3);
    loop_delay(WAIT_LONG);
    thread_sleep();
}

void o_blinker(void) {
    blink_O(2);
    loop_delay(WAIT_LONG);
    thread_sleep();
}

void t_blinker(void) {
    blink_T(1);
    loop_delay(WAIT_LONG);
    thread_sleep();
}

void blink_R(uint8_t nr) {
    led_on(nr);
    loop_delay(WAIT_SHORT);
    led_off(nr);

    loop_delay(WAIT_SHORT);

    led_on(nr);
    loop_delay(WAIT_LONG);
    led_off(nr);

    loop_delay(WAIT_SHORT);

    led_on(nr);
    loop_delay(WAIT_SHORT);
    led_off(nr);
}

void blink_I(uint8_t nr) {
    led_on(nr);
    loop_delay(WAIT_SHORT);
    led_off(nr);

    loop_delay(WAIT_SHORT);

    led_on(nr);
    loop_delay(WAIT_SHORT);
    led_off(nr);
}

void blink_O(uint8_t nr) {
    led_on(nr);
    loop_delay(WAIT_LONG);
    led_off(nr);

    loop_delay(WAIT_SHORT);

    led_on(nr);
    loop_delay(WAIT_LONG);
    led_off(nr);

    loop_delay(WAIT_SHORT);

    led_on(nr);
    loop_delay(WAIT_LONG);
    led_off(nr);
}

void blink_T(uint8_t nr) {
    led_on(nr);
    loop_delay(WAIT_LONG);
    led_off(nr);
}

int main(void) {
    thread_create(r_stack, KERNEL_CONF_STACKSIZE_DEFAULT, PRIORITY_MAIN-4, CREATE_WOUT_YIELD, r_blinker, "R");
    thread_create(i_stack, KERNEL_CONF_STACKSIZE_DEFAULT, PRIORITY_MAIN-3, CREATE_WOUT_YIELD, i_blinker, "I");
    thread_create(o_stack, KERNEL_CONF_STACKSIZE_DEFAULT, PRIORITY_MAIN-2, CREATE_WOUT_YIELD, o_blinker, "O");
    thread_create(t_stack, KERNEL_CONF_STACKSIZE_DEFAULT, PRIORITY_MAIN-1, 0, t_blinker, "T");

    return 0;
}
