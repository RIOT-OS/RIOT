#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "dtls.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"

#define MAIN_QUEUE_SIZE         (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int dtls_client_cmd(int argc, char **argv);
extern int dtls_server_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "dtlsc", "Start a DTLS client", dtls_client_cmd },
    { "dtlss", "Start and stop a DTLS server", dtls_server_cmd },
    { NULL, NULL, NULL }
};

static void _print_ip(void)
{
    /* get interfaces and print their addresses */
    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
        int res = gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                                  sizeof(ipv6_addrs));

        if (res < 0) {
            continue;
        }
        for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv6_addr_t)); i++) {
            char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];

            ipv6_addr_to_str(ipv6_addr, &ipv6_addrs[i], IPV6_ADDR_MAX_STR_LEN);
            printf("My address is %s\n", ipv6_addr);
        }
    }

}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT (Tiny)DTLS testing implementation");
    _print_ip();

    dtls_init();

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
