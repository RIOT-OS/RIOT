#ifndef __BOARD_UART0_H
#define __BOARD_UART0_H 

extern int uart0_handler_pid;

void board_uart0_init();
void uart0_handle_incoming(int c);
void uart0_notify_thread();

#endif /* __BOARD_UART0_H */
