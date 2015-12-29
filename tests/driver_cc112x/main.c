/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for CC112X network device driver
 *
 * @author      Kubaszek Mateusz <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "cpu_conf.h"
#include "xtimer.h"
#include "debug.h"

#include "periph/gpio.h"
#include "t_debug.h"

#include "net/gnrc/netapi.h"

#include <time.h>
#include <sys/time.h>

#define ENABLE_DEBUG       1
#include "debug.h"
#include "t_debug.h"

netdev2_cc112x_t device;

netdev2_event_cb_t callback(netdev2_t *dev, netdev2_event_t event, void* arg)
{
    static int rec = 0;
    switch(event) {
    case NETDEV2_EVENT_ISR:

        break;
    case NETDEV2_EVENT_LINK_DOWN:
        //puts("LINK DOWN");
        break;
    case NETDEV2_EVENT_LINK_UP:
        //puts("LINK UP");
        break;
    case NETDEV2_EVENT_RX_COMPLETE:
        //printf("REC - %d\n", rec);
        ++rec;
        break;
    case NETDEV2_EVENT_RX_STARTED:
        //puts("RX STARTED");
        break;
    case NETDEV2_EVENT_TX_COMPLETE:
        break;
    case NETDEV2_EVENT_TX_STARTED:
        //puts("TX STARTED");
        break;
    default:
        break;
    }
    return NULL;
}

#define SENDER 1

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    DEBUG("CC112x device driver test\n");
    msg_t msg;
    unsigned char data[] = {53, 0, 22, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2,
                    3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3,
                    4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    debug_timeref_init();

//    device.cc112x.params = cc112x_params[0];
//    cc112x_t *cc112x = &((netdev2_cc112x_t*) (&device.netdev))->cc112x;

//    gpio_init(GPIO_T(LED_RED_PORT, LED_RED_PIN), GPIO_DIR_PUSH_PULL_DRIVE, GPIO_PULLDOWN);
//    gpio_init(GPIO_T(LED_GREEN_PORT, LED_GREEN_PIN), GPIO_DIR_PUSH_PULL_DRIVE, GPIO_PULLDOWN);
//    gpio_init(GPIO_T(LED_YELLOW_PORT, LED_YELLOW_PIN), GPIO_DIR_PUSH_PULL_DRIVE, GPIO_PULLDOWN);
//    uint16_t cnt=0;
//    while (1) {
////    	gpio_toggle(GPIO_T(LED_RED_PORT, LED_GREEN_PIN));
////    	gpio_toggle(GPIO_T(LED_RED_PORT, LED_YELLOW_PIN));
////    	gpio_toggle(GPIO_T(LED_RED_PORT, LED_RED_PIN));
//    	cnt=3;
//    	puts("\nInitialization countdown...");
//		xtimer_usleep(1*1000000);
//		--cnt;
//		printf("...%d...\n", cnt);
////		gpio_toggle(GPIO_T(LED_RED_PORT, LED_RED_PIN));
//		xtimer_usleep(1*1000000);
//		--cnt;
//		printf("...%d...\n", cnt);
////		gpio_toggle(GPIO_T(LED_RED_PORT, LED_YELLOW_PIN));
//		xtimer_usleep(1*1000000);
//		--cnt;
//		printf("...%d...\n", cnt);
////		gpio_toggle(GPIO_T(LED_RED_PORT, LED_GREEN_PIN));
//		xtimer_usleep(1*1000000);
//
//		/* Initializing device */
//		puts("Initializing CC112X spi device...");
//		uint8_t result = netdev2_cc112x_setup(&device, params);
//		if(result){
//			puts("\tInitialization ERROR...");
//		}
//	}

    DEBUG("Initializing CC112X spi device...\n");
//    uint8_t result = netdev2_cc112x_setup(&device, &device.cc112x.params);
//    if(result) {
//        DEBUG("\tInitialization ERROR...\n");
//        return 0;
//    }

    if(SENDER) {

//        struct iovec vect;
//        uint8_t data[] = {53, 0, 22, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2,
//                3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3,
//                4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//        vect.iov_base = (void*)data;
//        vect.iov_len = 10;

//        const char *txt = "Nasza codzienność zabija...";
//        device.netdev.driver = &netdev2_cc112x_driver;
//        device.netdev.event_callback = (netdev2_event_cb_t)(callback);
//        device.netdev.isr_arg = (void*)txt;
//        device.netdev.driver->init(&device.netdev);

//		puts("Waiting...");
        uint8_t channel = 25;
        while(1) {
//			printf(".");
//			fflush(stdout);
            xtimer_usleep(1500000);
//            device.netdev.driver->send(&device.netdev, &vect, 1);
            xtimer_usleep(1500000);


            /* set the outgoing message's fields */
            msg.type = GNRC_NETAPI_MSG_TYPE_SND;
            msg.content.ptr = (void *)data;
            /* send message */
            msg_send(&msg, 7);
//            cc112x_set_channel(&device.cc112x, channel);
            ++channel;
            if(channel > 30)
                channel = 25;
            DEBUG("Channel %d\n", channel);

//			printf("#");
//			fflush(stdout);
            //		puts("\nNEXT PACKET\n");
            //		xtimer_usleep(4*500000);
            //		puts("waiting...");
            //        printf("MODEM_STATUS_0 - %x\nMODEM_STATUS_1 - %x\n",
            //        		cc112x_read_reg(&device.cc112x, CC112X_MODEM_STATUS0), cc112x_read_reg(&device.cc112x, CC112X_MODEM_STATUS1));
            //        printf("MARC_STATUS_0 - %x\nMARC_STATUS_1 - %x\n MARCSTATE - %x\n",
            //				cc112x_read_reg(&device.cc112x, CC112X_MARC_STATUS0), cc112x_read_reg(&device.cc112x, CC112X_MARC_STATUS1), cc112x_read_reg(&device.cc112x, CC112X_MARCSTATE));
            //        printf("FIFO_CFG - %x\n", cc112x_read_reg(&device.cc112x, CC112X_FIFO_CFG));
            //        printf("TXBYTES - %x\nRXBYTES - %x\n", cc112x_read_reg(&device.cc112x,CC112X_NUM_TXBYTES), cc112x_read_reg(&device.cc112x, CC112X_NUM_RXBYTES));
            //		printf("TXLAST - %d\n", cc112x_read_reg(&device.cc112x, CC112X_TXLAST));
            //		printf("TXfirst - %d\n", cc112x_read_reg(&device.cc112x, CC112X_TXFIRST));
        }
    } else {
//        const char *txt = "Nasza codzienność zabija...";
//        device.netdev.driver = &netdev2_cc112x_driver;
//        device.netdev.event_callback = (netdev2_event_cb_t)(callback);
//        device.netdev.isr_arg = (void*)txt;
//        device.netdev.driver->init(&device.netdev);

        puts("Waiting...");

        while(1) {
            DEBUG(".");
            fflush(stdout);
            xtimer_usleep(100000);
        }
    }

    return 0;
}
