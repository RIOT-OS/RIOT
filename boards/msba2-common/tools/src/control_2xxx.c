#include "control_2xxx.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "serial.h"

void hard_reset_to_bootloader(void)
{
    /*	Use this lines for flashing a node with interrupted DTR line */
    /*	printf("Press Reset - confirm with anykey\n");
    getchar();
    */
    printf("Reset CPU (into bootloader)\r\n");
    set_rts(1);		// RTS (ttl level) connects to P0.14
    /* the next two lines should be commented for the prepared node */
    set_dtr(1);		// DTR (ttl level) connects to RST
    send_break_signal();	// or break detect circuit to RST
    usleep(75000);
    /*	Use this lines for flashing a node with interrupted DTR line */
    /* printf("Release Reset - confirm with anykey\n");
    getchar();
    */
    set_dtr(0);		// allow the CPU to run:
    set_baud(baud_rate);
    set_rts(1);		// set RTS again (as it has been reset by set_baudrate)
    usleep(40000);
}

void hard_reset_to_user_code(void)
{
    printf("Reset CPU (into user code)\r\n");
    set_rts(0);		// RTS (ttl level) connects to P0.14
    set_dtr(1);		// DTR (ttl level) connects to RST
    send_break_signal();	// or break detect circuit to RST
    usleep(75000);
    set_dtr(0);		// allow the CPU to run
    usleep(40000);
}
