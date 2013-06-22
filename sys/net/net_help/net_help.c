/*
 * common.c
 *
 *  Created on: 05.10.2011
 *      Author: Oliver
 */

#include <stdio.h>
#include <thread.h>
#include <string.h>

#include "net_help.h"

void printArrayRange(uint8_t *array, uint16_t len, char *str)
{
    int i = 0;
    printf("-------------%s-------------\n", str);

    for(i = 0; i < len; i++) {
        printf("%#x ", *(array + i));
    }

    printf("\n-----------%u-------------\n", len);
}

uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len)
{
    int count;
    uint16_t carry;

    count = len >> 1;

    if(count) {
        if(count) {
            carry = 0;

            do {
                uint16_t t = (*buf << 8) + *(buf + 1);
                count--;
                buf += 2;
                sum += carry;
                sum += t;
                carry = (t > sum);
            }
            while(count);

            sum += carry;
        }
    }

    if(len & 1) {
        uint16_t u = (*buf << 8);
        sum += (*buf << 8);

        if(sum < u) {
            sum++;
        }
    }

    return sum;
}
