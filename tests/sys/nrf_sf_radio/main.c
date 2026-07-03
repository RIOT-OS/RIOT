#include <stdio.h>
#include "nrf_sf_radio/radio_driver.h"


int main(void)
{
    puts("nrf_sf_radio test start");

    nrf_sf_radio_start();

    if (NRF_RADIO->POWER != 1) {
        puts("ERROR: radio power");
        return 1;
    }

    if (NRF_RADIO->MODE != RADIO_MODE_MODE_Ble_1Mbit) {
        puts("ERROR: radio mode");
        return 1;
    }

    puts("SUCCESS");

    return 0;
}