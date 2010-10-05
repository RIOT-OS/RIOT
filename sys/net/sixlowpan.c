#include <stdio.h>
#include "drivers/cc110x/cc1100.h"

static void output(void){
	
	// TODO
	// 1. get dest addr from function param
	// 2. get pointer to packet buffer
	// 3. if dest is null send broadcast
	// 4. hc1 or hc1x header compression
	// 5. check if ip-packet is to large to fit into a single mac-packet
	// 	if no, fragment the packet
			// frag1 dispatch + header
			// fragn dispatch
			// set fragments into queue and send it 	
}
