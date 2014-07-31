#ifdef MODULE_UART0
#include <sys/select.h>
void _native_handle_uart0_input(void);
/**
 * @brief: initialize uart0
 *
 * @param stdiotype: "stdio", "tcp", "udp" io redirection
 * @param ioparam: a string containing a port number if stdiotype is tcp
 */
void _native_init_uart0(char *stdiotype, char *ioparam, int replay);
int _native_set_uart_fds(void);
#endif

extern int _native_null_out_file;
extern int _native_null_in_pipe[2];
void board_init(void);
