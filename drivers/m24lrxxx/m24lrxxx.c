#include <stdio.h>
#include <stdint.h>

#include "xtimer.h"
#include "mutex.h"
#include "m24lrxxx.h"
#include "bitarithm.h"

int m24lrxxx_init(m24lrxxx_t *dev, i2c_t bus, i2c_speed_t speed, uint8_t address)
{
    dev->bus = bus;
    dev->speed = speed;
    dev->address = address;

    uint8_t test[10];

    int ret = i2c_read_reg(bus, address, 0, test, sizeof(test));

    printf("i2c: ret = %d [ %hhu, %hhu, %hhu, %hhu, %hhu, %hhu ]", ret,
            test[0], test[1], test[2], test[3], test[4], test[5]);

    return 0;
}

int m24lrxxx_read(const m24lrxxx_t *dev)
{
    return 0;
}
