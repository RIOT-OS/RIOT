#include "ot.h"

#include "shell.h"
#include "shell_commands.h"
#include <stdio.h>

#include <cli/cli.hpp>
#include <net/ipv6/addr.h>
#include "net/conn/udp.h"
#include "conn.h"
#include "periph_conf.h"
#include "periph/i2c.h"


int main(void)
{
#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
    openthread_uart_run();
#else
    char line_buf[3*SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

	return 0;
}
