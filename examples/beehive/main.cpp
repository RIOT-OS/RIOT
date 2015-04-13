
extern "C" {
#include "thread.h"
#include "board_uart0.h"
#include "posix_io.h"
#include "shell.h"
#include "ipv6.h"
#include "sixlowpan.h"
#include "vtimer.h"
#include <string.h>
#include <stdlib.h>
#include "simcom.h"

}

#include "gsm_interface.hpp"
#include "border_router.hpp"
ipv6_addr_t *abr_addr;

const shell_command_t shell_commands[] = {
    //{"init", "Initialize network", rpl_udp_init},
    //{"server", "Starts a UDP server", udp_server},
    //{"send", "Send a UDP datagram", udp_send},
    {"send", "send pure message", send_to_uart1},
    {"http", "Send http request", http_post},
    {NULL, NULL, NULL}
};

int main(void)
{
    // /puts("Beehive version: "APP_VERSION);

    /* start shell */
    posix_open(uart0_handler_pid, 0);
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    
    simcom_init();
    BorderRouter<GsmInterface> router;
    router.sixlowpan_lowpan_border_init(0);
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    shell_run(&shell);
    return 0;
}
