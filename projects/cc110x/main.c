#include <stdio.h>
#include <string.h>
#include <thread.h>
#include <msg.h>
#include <kernel.h>

#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"

#include "gpioint.h"
#include "hwtimer.h"
#include "swtimer.h"

/*---------------------------------------------------------------------------*/

static int message_counter = 0;
static uint32_t content;

static void protocol_handler(void* msg, int msg_size, packet_info_t* packet_info)
{
    message_counter++;
    content = *(uint32_t*) msg;
    puts(".");
}

int main(void)
{
// minimal
#ifdef MODULE_GPIOINT
    gpioint_init();
#endif

    hwtimer_init();
    printf("hwtimer....[OK]\n");

    // timers
    swtimer_init();
    printf("swtimer....[OK]\n");
    
    // Adjust hwtimer with current CPU speed
    hwtimer_wait(200000);

//    benchmark_init();
    
//    uint32_t speed = get_system_speed();
//    printf("System speed: %lu\n", speed);
    hwtimer_init_comp(F_CPU);
    printf("hwtimer fcpu....[OK]\n");


#ifdef SENDER
    printf("I'm Alice!\n");
    char *payload = "Hello!\n";
#else
    printf("I'm Bob!\n");
#endif

    // radio stack
    cc1100_init();
    cc1100_set_packet_handler(4, protocol_handler);
    cc1100_set_channel(0);
//    cc1100_set_output_power(5);
    printf("cc1100..[OK]\n");
    
    uint32_t i = 0;

#ifdef SENDER
    cc1100_set_address(2);

   
//    while(1) {
        puts(".");
        int result = cc1100_send_csmaca(1, 4, 2, i, sizeof(i));
        printf("%i\n", result);
        hwtimer_wait(1000 * 1000);
//    }
#else
    cc1100_set_address(1);
    while(1) {
        printf("Waiting...\n");
        hwtimer_wait(200000);
    }
#endif
}
