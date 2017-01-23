/*
 * Copyright (C) 2016 RWTH Aachen, Steffen Robertz
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
 * @brief       Hello World application
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "rgbled.h"

int main(void)
{
    puts("Pinoccio PWM LED!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

  /*  tim_t mytimer = TIMER_2;
    timer_init(mytimer,500,&pwmRed, NULL);
    while(1)
    {
    }*/


    rgbled_t my_rgbled;
    pwm_t my_pwm = PWM_0_EN;
    rgbled_init(&my_rgbled, my_pwm, RED, GREEN, BLUE);
    color_rgb_t white = {255,255,255};
    rgbled_set(&my_rgbled, &white);



    return 0;
}
