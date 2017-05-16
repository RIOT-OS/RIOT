#include "stdio.h"
#include "ot.h"

#include "shell.h"
#include "shell_commands.h"
#include "openthread/thread.h"
#include "openthread/ip6.h"
#include "net/ipv6/addr.h"

int main(void)
{
    openthread_uart_run();
    return 0;
}
