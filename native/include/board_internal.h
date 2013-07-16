#ifdef MODULE_UART0
#include <sys/select.h>
void _native_handle_uart0_input(void);
void _native_init_uart0(void);
int _native_set_uart_fds(void);
#endif
