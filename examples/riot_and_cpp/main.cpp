/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file        main.cpp
 * @brief       Demonstration of mixed c++ and c user application with pure c RIOT
 *              - mixing of c and c++ source to test name mangling
 *              - introducing a namespace to declarative block, avoiding to qualify calls, e.g. std::vector
 *              - using private and public member functions, e.g. 'cpp_obj.greet()' cannot be accessed from main.cpp
 *              - overloading of function 'cpp_obj.say_hello(...)' for 'none', 'int' or 'float'
 *              - demonstration of templated c++ container 'std::vector'
 *              - usage of iterator to access elements of the container type
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

/*
 * all included headers defining c functions, i.e. all RIOT functions, must be marked as extern "C"
 */
extern "C" {
#include "thread.h"

#include "c_functions.h"
}

#include <cstdio>
#include <vector>
//#include <iostream>
#include "cpp_class.hpp"
#include <functional>
using namespace std;

//char threadA_stack [KERNEL_CONF_STACKSIZE_MAIN];
#define TRANSCEIVER_TYPE 0 //TRANSCEIVER_AT86RF231
//#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)

//char radio_stack_buffer[RADIO_STACK_SIZE];

void print_ipv6_addr(const ipv6_addr_t *ipv6_addr)
{
    char *addr_str;
    printf("%s\n", ipv6_addr_to_str(addr_str,IPV6_MAX_ADDR_STR_LEN, ipv6_addr));
}

void printUsage()
{
    printf("Usage: init {h | r | a | e} radio_address\n");
    printf("\th\tinitialize as host\n");
    printf("\tr\tinitialize as router\n");
    printf("\ta\tinitialize as ad-hoc router\n");
    printf("\tb\tinitialize as border router\n\n");
    printf("\tradio_address must be an 8 bit integer\n");
}
//void *threadA_func(void *arg);

void bootstrapping(int argc, char **argv)
{
    sixlowpan_lowpan_bootstrapping();
}
    printf("The vector vInts has been filled with %d numbers.\n", (int)vInts.size());
void send_packet(int argc, char **argv)
{
    printf("%s\n", "sixlowpan send packet!");
    uint16_t r_addr;
    int result;
    ipv6_addr_t std_addr;
    char* msg = argv[1]; // Message
    uint16_t msg_len = (uint16_t)strlen(msg);
    r_addr = atoi(argv[2]); // Radio Dest. Address
    ipv6_addr_t ipaddr;
    printf("%s\n", "sixlowpan send packet adr init!");
    printf("%s", "sixlowpan r_addr: ");
    printf("%d\n", r_addr);
    printf("%s", "sixlowpan msg_len: ");
    printf("%d\n", msg_len);
    //fe80::ff:fe00:    
    ipv6_addr_init(&ipaddr, 0xfe80, 0, 0, 0, 0, 0x00FF, 0xFE00, r_addr);
    //ipv6_addr_init(&ipaddr, 0xABCD, 0, 0, 0, 0x1034, 0x00FF, 0xFE00, r_addr);
    //ipv6_addr_init(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x1034/*0x3612*/, 0x00ff, 0xfe00, r_addr);
    printf("%s\n", "sixlowpan send packet ipv6_sendto!");
    result = ipv6_sendto(&ipaddr, IPV6_PROTO_NUM_NONE, (uint8_t*)msg, msg_len);
    //result = sixlowpan_lowpan_sendto(TRANSCEIVER_TYPE, &r_addr, 2, (uint8_t*)msg, msg_len);
    if(result == msg_len)
    {
        printf("%s\n", "sixlowpan 'send packet' SENT!!!");
    }
    else
    {
        printf("%s\n", "sixlowpan 'send packet' NOT Delivered!!!"); 
    }
    printf("%s", "sixlowpan send result: ");
    printf("%d\n", result);
    // print_ipv6_addr(&ipaddr);
    // for (int j = 0; j < 100; j++) {
    //     test[0] = j;
    //     for (int i = 0; i < 1000; i++) {
    //         //int ipv6_sendto(const ipv6_addr_t *dest, uint8_t next_header,
    //             //const uint8_t *payload, uint16_t payload_length)
    //         ipv6_sendto(&ipaddr, IPV6_PROTO_NUM_NONE, test, 2);
    //     }
    //     //int sixlowpan_lowpan_sendto(int if_id, const void *dest, int dest_len,
    //                         //uint8_t *data, uint16_t data_len); //???

    //     //lib6lowpan_bootstrapping(&addr8);
    // }
}
void ip(int argc, char **argv)
{
    //char *addr_str;
    //printf("%s\n", ipv6_addr_to_str(addr_str,IPV6_MAX_ADDR_STR_LEN, ipv6_addr));
    printf("TODO: sixlowpan class -> Member ip!\t");
}

void context(int argc, char **argv)
{
    uint8_t i;
    lowpan_context_t *context;

    for (i = 0; i < NDP_6LOWPAN_CONTEXT_MAX; i++) {
        context = lowpan_context_num_lookup(i);

        if (context != NULL) {
            printf("%2d\tLifetime: %5u\tLength: %3d\t", context->num, context->lifetime, context->length);
            print_ipv6_addr(&(context->prefix));
        }
    }
}

void sixlowpan_init(int argc, char **argv)
    printf("%s\n", "sixlowpan initiating!");
    uint16_t r_addr;
    ipv6_addr_t std_addr;
    char* str = argv[1];
    r_addr = atoi(argv[2]);
    //size_t str_len = strlen(str);
    char *command = str; // strtok(str, " ");
    // if ((command = strtok(NULL, " ")) == NULL) {
    //     printf("%s\n", "sixlowpan command == NULL!");
    //   for(int count = 1; count < argc; count++)
    //     {
    //       printf("argv[%d] = %s\n", count, argv[count]);
    //     }
    //     printUsage();
    //     return;
    // }
    // char *p;
    // if (((p = strtok(NULL, " ")) == NULL) || ((r_addr = (uint16_t) strtol(p, NULL, 10)) == 0)) {
    //     printf("%s\n", "sixlowpan r_addr == NULL!");
    //     printUsage();
    //     return;
    // }
    printf("%s\n", "sixlowpan ipv6_addr_init initiating!");
    printf("%s", "sixlowpan r_addr: ");
    printf("%d\n", r_addr);
    ipv6_addr_init(&std_addr, 0xABCD, 0, 0, 0, 0x1034, 0x00FF, 0xFE00, r_addr);
    switch (command[0]) {
        case 'h':
            printf("INFO: Initialize as host on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            //sixlowpan_lowpan_init(); //TRANSCEIVER_TYPE, r_addr, 0);
            //net_if_set_hardware_address(TRANSCEIVER_TYPE, r_addr);
            sixlowpan_lowpan_init_interface(TRANSCEIVER_TYPE);
            break;
        case 'r':
            printf("INFO: Initialize as router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            ipv6_addr_t tmp;
            //sixlowpan_lowpan_init();//TRANSCEIVER_TYPE, r_addr, 0);
            //net_if_set_hardware_address(TRANSCEIVER_TYPE, r_addr);            
            sixlowpan_lowpan_init_interface(TRANSCEIVER_TYPE);  
            ipv6_init_as_router();          
            break;

        case 'a':
            printf("INFO: Initialize as adhoc router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            //sixlowpan_lowpan_adhoc_init(TRANSCEIVER_TYPE, &std_addr, r_addr);
            sixlowpan_lowpan_init_adhoc_interface(TRANSCEIVER_TYPE, &std_addr);
            break;

        // case 'b':
        // {
        //     printf("INFO: Initialize as border router on radio address %hu\n", r_addr);

        //     if (r_addr > 255) {
        //         printf("ERROR: radio_address not an 8 bit integer\n");
        //         return;
        //     }

        //     //int res = sixlowpan_lowpan_border_init(TRANSCEIVER_TYPE, &std_addr);
        //  int res = sixlowpan_lowpan_border_init(TRANSCEIVER_TYPE);
        //     switch (res) {
        //         case (SIXLOWERROR_SUCCESS):
        //             printf("INFO: Border router initialized.\n");
        //             break;

        //         case (SIXLOWERROR_ADDRESS):
        //             printf("ERROR: Illegal IP address: ");
        //             print_ipv6_addr(&std_addr);
        //             break;

        //         default:
        //             printf("ERROR: Unknown error (%d).\n", res);
        //             break;
        //     }

        //     break;
        //}
        default:
            printf("ERROR: Unknown command '%c'\n", command[0]);
            printUsage();
            break;
    }
}



// void init_transceiver(void)
// {
//     kernel_pid_t radio_pid = thread_create(
//                         radio_stack_buffer,
//                         sizeof(radio_stack_buffer),
//                         PRIORITY_MAIN - 2,
//                         CREATE_STACKTEST,
//                         radio,
//                         NULL,
//                         "radio");

//     uint16_t transceivers = TRANSCEIVER_DEFAULT;

//     transceiver_init(transceivers);
//     (void) transceiver_start();
//     transceiver_register(transceivers, radio_pid);
// }    

static int shell_readc(void)
{
    char c = 0;
    (void) posix_read(uart0_handler_pid, &c, 1);
    return c;
}

static void shell_putchar(int c)
{
    (void) putchar(c);
}

void PrintStackUsages(int argc, char **argv)
{
    volatile tcb_t* thread;
    for(int i = 1; i < 15; i++){
        thread = thread_get(i);
        if(thread != NULL)
        {
            printf("#%d Thread Name: %s\n", i, thread->name);
            printf("\t  Stack size: %d\n", (int)( sizeof(thread->stack_start) / sizeof(thread->stack_start[0])));
            printf("\t  Free stack: %d\n", (int)thread_measure_stack_free(thread->stack_start));
            printf("\t  Stack addr: %x\n", thread->stack_start);
        }
    }
}

shell_command_t shell_commands[] = {
    {"send", "", send_packet},
    {"init", "", sixlowpan_init},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"context", "", context},
    {"usage", "", PrintStackUsages},
    {NULL, NULL, NULL}
    };

/* main */
int main()
{
    printf("\n************ RIOT and C++ 6LoWPAN demo program ***********\n");
    printf("\n");
    printf("\t\tInitialize SixLoWPan with the Following:\n");
    printUsage();
    net_if_set_src_address_mode(0, NET_IF_TRANS_ADDR_M_SHORT);
    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, shell_readc, shell_putchar);// uart0_readc, uart0_putc);

    shell_run(&shell);



    // /* create thread A */
    // thread_create(threadA_stack, sizeof(threadA_stack), 0, CREATE_WOUT_YIELD, threadA_func, NULL, "thread A");

    // printf("******** Hello, you're in thread %s ********\n", thread_getname(thread_getpid()));
    // printf("We'll test C++ class and methods here!\n");

    // cpp_class cpp_obj;
    // printf("\n-= Test overloading functions =-\n");
    // cpp_obj.say_hello();
    // cpp_obj.say_hello(42);
    // cpp_obj.say_hello(3.141592f);

    // printf("\n-= Test namespace =-\n");
    // printf("typing std::vector is obsolete when 'using namespace std;'\n");
    // vector<int> vInts;
    // vInts.push_back(1);
    // vInts.push_back(3);
    // vInts.push_back(2);
    // printf("The vector vInts has been filled with %d numbers.\n", vInts.size());

    // printf("\n-= Test iterator =-\n");
    // printf("The content of vInts = { ");

    // for (vector<int>::iterator it = vInts.begin(); it != vInts.end(); ++it) {
    //     printf("%d ", *(it));
    // }

    // printf("}\n");

    return 0;
}

// /* thread A function implemetation */
// void *threadA_func(void *)
// {
//     int day = 13, month = 6, year = 2014;
//     int ret_day;

//     printf("\n******** Hello, now you're in %s ********\n", thread_getname(thread_getpid()));
//     printf("We'll test some C functions here!\n");

//     printf("\n-= hello function =-\n");
//     hello();

//     printf("\n-= day_of_week function =-\n");

//     printf("day %d, month %d, year %d is ", day, month, year);

//     ret_day = day_of_week(day, month, year);
//     if (ret_day >= 0){
//         char day_of_week_table[][32] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
//         printf("%s\n", day_of_week_table[ret_day]);
//     }

//     printf("\nThis demo ends here, press Ctrl-C to exit (if you're on native)!\n");

//     return NULL;
// }