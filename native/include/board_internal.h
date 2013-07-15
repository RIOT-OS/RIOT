#ifdef MODULE_UART0
#include <sys/select.h>
extern fd_set _native_uart_rfds;
void _native_handle_uart0_input(void);
void _native_init_uart0(void);
#endif
