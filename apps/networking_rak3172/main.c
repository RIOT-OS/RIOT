/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "thread.h"
#include "board.h"
#include "ztimer.h"
#include "net/sock/udp.h"
#include "net/sock/tcp.h"
#include "periph/i2c.h"
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

char udp_stack[THREAD_STACKSIZE_DEFAULT];
char tcp_stack[THREAD_STACKSIZE_DEFAULT];
char i2c_stack[THREAD_STACKSIZE_DEFAULT];
kernel_pid_t main_thread, udp_thread, tcp_thread, i2c_thread;

void* udp_handler(void *arg);
void* tcp_handler(void *arg);
void* i2c_handler(void *arg);
int main(void)
{
    udp_thread = thread_create(udp_stack, sizeof(udp_stack), THREAD_PRIORITY_MAIN - 1,
                                 THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                                 udp_handler, NULL, "UDP_Custom" );
    tcp_thread = thread_create(tcp_stack, sizeof(tcp_stack), THREAD_PRIORITY_MAIN - 1,
                                 THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                                 tcp_handler, NULL, "TCP_Custom" );
    i2c_thread = thread_create(i2c_stack, sizeof(i2c_stack), THREAD_PRIORITY_MAIN - 1,
                                 THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                                 i2c_handler, NULL, "i2c_Custom" );    
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
void* udp_handler(void *arg){
    (void)arg;

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    uint8_t udp_buf[128];
    local.port = 12345;
 
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return NULL;
    }
 
    while (1) {
        sock_udp_ep_t remote;
        ssize_t res;
        if ((res = sock_udp_recv(&sock, udp_buf, sizeof(udp_buf), SOCK_NO_TIMEOUT,
                                 &remote)) >= 0) {
            printf("%s\n", udp_buf);
            if(strncmp((char*)udp_buf, "test", 5) == 0)
            {
                uint8_t i = 0;
                    while(i<50)
                        {
                            char text[127];
                            sprintf(text, "Гречиха посевная *гудок*, а я гречиха посевная = %d\n", i);
                        if (sock_udp_send(&sock, text, strlen(text), &remote) < 0) {
                            puts("Error sending reply");
                        }
                        i++;
                        ztimer_sleep(ZTIMER_MSEC, 1000);
                        }
            }

        }
    }
    return NULL;
}

#define SOCK_QUEUE_LEN  (1U)
 
sock_tcp_t sock_queue[SOCK_QUEUE_LEN];
be_uint16_t vbat;
void* tcp_handler(void *arg){
    (void)arg;
     sock_tcp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_tcp_queue_t queue;
    uint8_t tcp_buf[128];
    local.port = 12345;
 
    if (sock_tcp_listen(&queue, &local, sock_queue, SOCK_QUEUE_LEN, 0) < 0) {
        puts("Error creating listening queue");
        return NULL;
    }
    puts("Listening on port 12345");
    while (1) {
        sock_tcp_t *sock;
        char text[127];
        if (sock_tcp_accept(&queue, &sock, SOCK_NO_TIMEOUT) < 0) {
            puts("Error accepting new sock");
        }
        else {
            int read_res = 0;
 
            puts("Reading data");
            sock_tcp_write(sock, "> ", 2);
            while (read_res >= 0) {
                read_res = sock_tcp_read(sock, &tcp_buf, sizeof(tcp_buf),
                                         SOCK_NO_TIMEOUT);
                if (read_res <= 0) {
                    puts("Disconnected");
                    break;
                }
                else {
                    printf("Read: \"");
                    for (int i = 0; i < read_res; i++) {
                        printf("%c", tcp_buf[i]);
                    }
                    puts("\"");
            if(strncmp((char*)tcp_buf, "test", 4) == 0)
            {
                uint32_t i = 0;
                    while(i < 50)
                        {
                            sprintf(text, "Гречиха посевная *гудок*, а я гречиха посевная = %ld\n", i);
                        if (sock_tcp_write(sock, text, strlen(text)) < 0) {
                            puts("Error sending\n");
                        }
                        i++;
                        ztimer_sleep(ZTIMER_MSEC, 1000);
                        }
            }
            else if(strncmp((char*)tcp_buf, "led_on", 6) == 0){
                gpio_set(LED_USB_LINK);
                sprintf(text, "Светодиод включен\n");
                sock_tcp_write(sock, text, strlen(text));
            }
            else if(strncmp((char*)tcp_buf, "led_off", 7) == 0){
                gpio_clear(LED_USB_LINK);
                sprintf(text, "Светодиод выключен\n");
                sock_tcp_write(sock, text, strlen(text));
            }
            else if(strncmp((char*)tcp_buf, "vbat", 4) == 0){
                sprintf(text, "Напряжение батареи %d mv\n", vbat.u16);
                sock_tcp_write(sock, text, strlen(text));
            }
            else if(strncmp((char*)tcp_buf, "help", 4) == 0){
                sprintf(text, "Доступные команды:\n\nhelp\nled_on\nled_off\ntest\nvbat\n\n");
                sock_tcp_write(sock, text, strlen(text));
            }
            sock_tcp_write(sock, "> ", 2);
                    }
    }
            sock_tcp_disconnect(sock);
        }
    }
    sock_tcp_stop_listen(&queue);
    return NULL;
}
void* i2c_handler(void *arg){
    (void)arg;
    int dev = 0;
    i2c_init(dev);
    i2c_acquire(dev);
    while(1){
        vbat.u16 = 0;
        le_uint16_t reg_value;
        i2c_read_regs(0, 0x40, 0x02, &reg_value.u16, 2, 0);
        vbat = byteorder_ltobs(reg_value);
        vbat.u16 = (vbat.u16>>3)*4;
        i2c_release(0);
        ztimer_sleep(ZTIMER_MSEC, 10000);
    }
    return NULL;
}
