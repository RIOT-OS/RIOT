#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "thread.h"
#include "shell.h"
#include "shell_commands.h"
#include "msg.h"
#include "xtimer.h"


#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
extern int rcli_cmd(int argc, char **argv);
extern int rcli_server_start(char *port_str);
extern int ipfs_tcp_cmd(int argc, char **argv);
extern int ipfs_udp_cmd(int argc, char **argv);
extern void ipfs_udp_init_workers(void);
enum operation_t { GET, PUT, POST };
//extern int http(enum operation_t operation, char *full_url, uint16_t target_port, int donotread, char *data);
extern void rcli_send(char *addr_str, uint16_t port, char *data, size_t data_len, unsigned int num, unsigned int delay);
const shell_command_t shell_commands[] = {
    { "rcli", "listen udp port to receive commands", rcli_cmd },
    { "ipfs_tcp", "put and get objects on ipfs node (TCP version)", ipfs_tcp_cmd },
    { "ipfs_udp", "put and get objects on ipfs node (UDP version)", ipfs_udp_cmd },
    { NULL, NULL, NULL }
};

#define SERVER_PRIO (THREAD_PRIORITY_MAIN - 1)
#define G_STACKSIZE  (THREAD_STACKSIZE_MAIN)
static char g_stack[G_STACKSIZE];
static void *send_ip(void *arg)
{

    (void) arg;
    xtimer_usleep(20 * 1000000);
    char buffer[10];
    snprintf(buffer, 10, "ok");
    rcli_send("2001:470:c8cb:1:beee:7bff:fe9e:73f8", 1234, buffer, strlen(buffer), 1, 1000000LU);

/*
   char url[256];
   snprintf(url, 256, "http://2001:470:c8cb:100::10/iotlab/?o=put&h=m3-101");
   http(GET, url, 80, 1, NULL);
 */
    return NULL;
}


int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* init ipfs workers */
    ipfs_udp_init_workers();

    /* start udp server */
    rcli_server_start("1234");
    thread_create(g_stack, sizeof(g_stack), SERVER_PRIO, THREAD_CREATE_STACKTEST, send_ip, NULL, "send http ip");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
