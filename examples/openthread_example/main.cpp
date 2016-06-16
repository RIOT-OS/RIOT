#include <stdio.h>
#include <openthread.h>
#include <cli/cli-serial.h>
#include "platform.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    PlatformInit();
    otInit();
    otCliSerialInit();

    while (1)
    {
        otProcessNextTasklet();
        PlatformProcessDrivers();
    }

    return 0;
}
