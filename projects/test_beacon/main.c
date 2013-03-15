#include <stdio.h>
#include <stdint.h>
#include <transceiver.h>
#include <msg.h>

#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>
#include <thread.h>

#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlowerror.h"
#include "sys/net/sixlowpan/rpl/etx_beaconing.h"

ipv6_addr_t my_address = { 0 };

void init(char *str){
       transceiver_command_t tcmd;
       msg_t m;
       uint8_t chan = 10;

       uint16_t r_addr;

       int res = sscanf(str, "init %hu", &r_addr);

       if(res < 1){
           printf("Usage: init address\n");
           printf("\taddress must be an 8 bit integer\n");
           return;
       }

       printf("INFO: Initialize on address %d\n", r_addr);
       if (r_addr > 255) {
           printf("ERROR: address not an 8 bit integer\n");
           return;
       }

       sixlowpan_init(TRANSCEIVER_CC1100,r_addr,0);

       //Wir benötigen einen Link Local prefix, um unsere entsprechende Addresse im Netz abzufragen
       ipv6_addr_t ll_address;
       ipv6_set_ll_prefix(&ll_address);
       ipv6_get_saddr(&my_address, &ll_address);



       /* set channel to 10 */
       tcmd.transceivers = TRANSCEIVER_CC1100;
       tcmd.data = &chan;
       m.type = SET_CHANNEL;
       m.content.ptr = (void*) &tcmd;

       msg_send_receive(&m, &m, transceiver_pid);

       etx_init_beaconing(&my_address);
}

void table(char *str){
    thread_print_all();
}

const shell_command_t shell_commands[] = {
    {"init", "", init},
    {"table", "", table},
    {NULL, NULL, NULL}
};

int main(void) {
    printf("ETX Test Application\n");


    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}
