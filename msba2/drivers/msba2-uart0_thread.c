#include <chardev_thread.h>
#include <ringbuffer.h>
#include <stdio.h>
#include <thread.h>

#include <board_uart0.h>

#define UART0_BUFSIZE 32

extern ringbuffer uart0_ringbuffer;
extern int uart0_handler_pid;

static char buffer[UART0_BUFSIZE];

static void uart0_loop() {
    chardev_loop(&uart0_ringbuffer);
}

void board_uart0_init() {
    ringbuffer_init(&uart0_ringbuffer, buffer, UART0_BUFSIZE);
    int pid = thread_create(KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN-1, CREATE_STACKTEST, uart0_loop, "uart0");
    uart0_handler_pid = pid;
    puts("uart0_init() [OK]");
}

