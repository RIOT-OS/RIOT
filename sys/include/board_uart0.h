#ifndef __BOARD_UART0_H
#define __BOARD_UART0_H

extern int uart0_handler_pid;

void board_uart0_init(void);
void uart0_handle_incoming(int c);
void uart0_notify_thread(void);

int uart0_readc(void);
void uart0_putc(int c);

#endif /* __BOARD_UART0_H */
