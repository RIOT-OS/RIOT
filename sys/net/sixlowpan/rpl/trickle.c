#include <stdio.h>
#include "sys/net/sixlowpan/sixlowip.h"

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
