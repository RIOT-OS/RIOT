#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>

typedef enum {
    PARENT_SELECT = 0,
    PARENT_DELETE = 1,
    DIO_RCVD = 2,
    DTA_RCVD = 3,
    ALARM = 4,
    CONFIRM = 5,
    WARN = 6,
    DISARMED = 7,
    RESET = 8
} evt_t;


typedef struct {
    uint8_t dst;
    evt_t id;
    char data;
    uint8_t sequ;
} cmd_t;

#endif /* EVENTS_H */
