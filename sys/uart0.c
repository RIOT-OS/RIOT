#include <chardev_thread.h>
#include <ringbuffer.h>
#include <stdio.h>
#include <thread.h>
#include <msg.h>

#include <board_uart0.h>

#define UART0_BUFSIZE 		(32)
#define UART0_STACKSIZE 	(MINIMUM_STACK_SIZE + 256)

ringbuffer uart0_ringbuffer;
int uart0_handler_pid;

static char buffer[UART0_BUFSIZE];

static char uart0_thread_stack[UART0_STACKSIZE];

static void uart0_loop() {
    chardev_loop(&uart0_ringbuffer);
}

void board_uart0_init() {
    ringbuffer_init(&uart0_ringbuffer, buffer, UART0_BUFSIZE);
    int pid = thread_create(uart0_thread_stack, sizeof(uart0_thread_stack), PRIORITY_MAIN-1, CREATE_STACKTEST, uart0_loop, "uart0");
    uart0_handler_pid = pid;
    puts("uart0_init() [OK]");
}

void uart0_handle_incoming(int c) {
    rb_add_element(&uart0_ringbuffer, c);
}

void uart0_notify_thread() {
    msg m;
    m.type = 0;
    msg_send_int(&m, uart0_handler_pid);
}
