/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @brief       6LoWPAN example application - main function
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdio.h>
#include "udp.h"
#include "kernel.h"
#include "thread.h"
#include "net_if.h"
#include "posix_io.h"
#include "shell.h"
#include "hwtimer.h" 
#include "shell_commands.h"
#include "board_uart0.h"
#include "sixlowapp.h"
#include "periph/adc.h"
#include "periph/gpio.h"  
#define ENABLE_DEBUG    (1)
#include "debug.h"

kernel_pid_t sixlowapp_udp_server_pid = KERNEL_PID_UNDEF;
void netSetup(void);
void adcSetup(void);
void activateRelayCtrl(void);
void deactivateRelayCtrl(void);
int getAdcValue(void);
char addr_str[IPV6_MAX_ADDR_STR_LEN];
char monitor_stack_buffer[KERNEL_CONF_STACKSIZE_DEFAULT - 128];//KERNEL_CONF_STACKSIZE_MAIN];
char udp_server_stack_buffer[KERNEL_CONF_STACKSIZE_DEFAULT - 512];//KERNEL_CONF_STACKSIZE_MAIN];
adc_t dev = ADC_0;
adc_precision_t pre = ADC_RES_16BIT;

const shell_command_t shell_commands[] = {
    {"ping", "Send an ICMPv6 echo request to another node", sixlowapp_send_ping},
    {"nc", "RIOT netcat - arbitrary UDP connections and listens", sixlowapp_netcat},
    {"send", "Send an 6LoWPAN packet to an IPv6 usage: send <ADR> <MSG>", send_packet},
   /* {"adc", "Send an 6LoWPAN packet to an IPv6 usage: send <ADR> <MSG>", send_packet},*/
    {NULL, NULL, NULL}
};

int main(void)
{
  printf("Node application\n");

  /*Setup net*/
  netSetup();
  printf("after netSetup\n");
  /* Setup sensors */
  adcSetup();
  printf("after adcSetup\n");
  //printf("ADC result: %d \n", getAdcValue());

    /* start thread for monitor mode --> sixlowapp_monitor */
    kernel_pid_t monitor_pid = thread_create(monitor_stack_buffer,
                                             sizeof(monitor_stack_buffer),
                                             PRIORITY_MAIN - 2,
                                             CREATE_STACKTEST,
                                             sixlowapp_monitor, NULL,
                                             "monitor");

    ipv6_register_packet_handler(monitor_pid);
    printf("after ipv6_register_packet_handler\n");
    /* Start the UDP server thread --> sixlowapp_udp_server_loop */
    sixlowapp_udp_server_pid = thread_create(udp_server_stack_buffer,
                                             sizeof(udp_server_stack_buffer),
                                             PRIORITY_MAIN, CREATE_STACKTEST,
                                             sixlowapp_udp_server_loop, NULL,
                                             "UDP receiver");

    /* Open the UART0 for the shell */
    posix_open(uart0_handler_pid, 0);
    /* initialize the shell */
    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);
    /* start the shell loop */
    shell_run(&shell);

    return 0;
}

int getAdcValue(void)
{
    int result = 0;
    activateRelayCtrl();
    result = adc_sample(dev, 0);
    deactivateRelayCtrl();
    return result;
}

void activateRelayCtrl(void)
{
    gpio_init_out(GPIO_0, GPIO_NOPULL);
    gpio_set(GPIO_0);
    hwtimer_wait(HWTIMER_TICKS_TO_US(1000000)); // need tweaking
}

void deactivateRelayCtrl(void)
{
    //gpio_init_out(GPIO_0, GPIO_NOPULL);
    gpio_clear(GPIO_0);
    hwtimer_wait(HWTIMER_TICKS_TO_US(1000000));
}

void adcSetup(void)
{
  int response = adc_init(dev, pre);
  if(response == -1)
    DEBUG("ADC init failed");
}

void netSetup(void)
{
    ipv6_addr_t tmp;
    DEBUG("NETsetup start\n");
    /* configure node to use short address mode */
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    DEBUG("NETsetup addr set src\n");
    /* configure link-local address */
    ipv6_addr_set_link_local_prefix(&tmp);
    if (!ipv6_addr_set_by_eui64(&tmp, IF_ID, &tmp)) {
        DEBUG("Can not set link-local by EUI-64 on interface %d\n", IF_ID);
        return 0;
    }
    DEBUG("NETsetup after link local\n");
    if (!ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                NDP_OPT_PI_VLIFETIME_INFINITE,
                NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        DEBUG("Can not add link-local address to interface %d\n", IF_ID);
        return 0;
    }
    DEBUG("NETsetup after add adr\n");
    /* add all nodes multicast address */
    ipv6_addr_set_all_nodes_addr(&tmp);
    DEBUG("NETsetup add all nodes addr\n");
    if (!ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
        DEBUG("Can not add all nodes address to interface %d\n", IF_ID);
        return 0;
    }
    DEBUG("NETsetup add all nodes addr after\n");
    /* add prefix to global address */
    ipv6_addr_init(&tmp, 0xabcd, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, net_if_get_hardware_address(0));
    // DEBUG("NETsetup ipv6_addr_init after\n");
    //     if (!ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
    //                           NDP_OPT_PI_VLIFETIME_INFINITE,
    //                           NDP_OPT_PI_PLIFETIME_INFINITE, 0)) {
    //     DEBUG("Can not add global address to interface %d\n", IF_ID);
    DEBUG("NETsetup end!\n");
        //return 0;
    //}
}
