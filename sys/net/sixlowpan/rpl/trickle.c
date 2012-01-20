#include <stdio.h>
#include "sys/net/sixlowpan/sixlowip.h"

#define ICMP_RPL_CM	155


int test(void)
{
    ipv6_addr_t std_addr;
    struct icmpv6_hdr_t icmphdr;
    icmphdr.type = ICMP_RPL_CM;

    int pid = thread_create(KERNEL_CONF_STACKSIZE_MAIN, 0, CREATE_SLEEPING, trickle_increment_counter, "count");
   hwtimer_init();
    
}

//struct für trickle parameter??
int reset_trickletimer(void){a
 I = Imin;
 c = 0;
 //t = rand[I/2, I);
 //start timer


}

int init_trickle(int Imin, int Imax, int k){

//k = redundancy
// set 3 "constants"

}

int trickle_increment_counter(void){
    //call this function, when received DIO message
    c++;
}

int trickle_timer_over_callback(void)
{

 if(c < k){

	//send DIO message
	
 }
}

int trickle_intervall_over_callback(void){
 I = I*2;
 if(I > Imax) I=Imax;
 c=0;
 //start timer

}
