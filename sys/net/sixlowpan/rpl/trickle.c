#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "trickle.h"
#include "sys/net/sixlowpan/rpl/rpl.h"

char timer_over_buf[TRICKLE_TIMER_STACKSIZE];
char interval_over_buf[TRICKLE_INTERVAL_STACKSIZE];
int timer_over_pid;
int interval_over_pid;

uint8_t k;
uint32_t Imin;
uint8_t Imax;
uint32_t I;
uint32_t t;
uint16_t c;
vtimer_t trickle_t_timer;
vtimer_t trickle_I_timer;
timex_t t_time;
timex_t I_time;

//struct für trickle parameter??
void reset_trickletimer(void){
	I = Imin;
	c = 0;
	//start timer
	t = (I/2) + ( rand() % ( I - (I/2) + 1 ) );
	t_time = timex_set(0,t*1000);
	I_time = timex_set(0,I*1000);
	vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid);
	vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid);

}

void init_trickle(void){
	//Create threads
	timer_over_pid = thread_create(timer_over_buf, TRICKLE_TIMER_STACKSIZE,
								   PRIORITY_MAIN-1,CREATE_SLEEPING,
								   trickle_timer_over, "trickle_timer_over");
	interval_over_pid = thread_create(interval_over_buf, TRICKLE_INTERVAL_STACKSIZE,
									  PRIORITY_MAIN-1, CREATE_SLEEPING,
									  trickle_interval_over, "trickle_interval_over");
	
}

void start_trickle(uint8_t DIOIntMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstant){
	k = DIORedundancyConstant;
	Imin = pow(2, DIOIntMin);
	Imax = DIOIntDoubl;
	//Eigentlich laut Spezifikation erste Bestimmung von I wie auskommentiert:
	//I = Imin + ( rand() % ( (Imin << Imax) - Imin + 1 ) );
	I = Imin + ( rand() % ( (4*Imin) - Imin + 1 ) );

	t = (I/2) + ( rand() % ( I - (I/2) + 1 ) );
	t_time = timex_set(0,t*1000);
	I_time = timex_set(0,I*1000);
	vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid);
	vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid);

}

void trickle_increment_counter(void){
    //call this function, when received DIO message
    c++;
}

void trickle_timer_over(void)
{
	ipv6_addr_t mcast;
	ipv6_set_all_nds_mcast_addr(&mcast);
	while(1){
		//Laut RPL Spezifikation soll k=0 wie k= Unendlich behandelt werden, also immer gesendet werden
		if( (c < k) || (k == 0)){
			send_DIO(&mcast);
		}
		thread_sleep();
	}
 
}

void trickle_interval_over(void){
	while(1){
		I = I*2;
		if(I > (Imin << Imax)){
			I=(Imin << Imax);
		}
		c=0;
		t = (I/2) + ( rand() % ( I - (I/2) + 1 ) );
		//start timer
		t_time = timex_set(0,t*1000);
		I_time = timex_set(0,I*1000);
		vtimer_set_wakeup(&trickle_t_timer, t_time, timer_over_pid);
		vtimer_set_wakeup(&trickle_I_timer, I_time, interval_over_pid);
		thread_sleep();
	}

}
