#ifndef WEATHER_ROUTING_H
#define WEATHER_ROUTING_H 

#include <time.h>

#define FLOODING_PROB     (100)
#define MAX_SOURCES     (20)
#define MAX_INTERVAL    (5 * 60)

typedef struct {
    uint8_t id;
    time_t timestamp;
} source_timestamp_t;

void route_packet(void* msg, int msg_size);
uint8_t update_sources(uint8_t id, time_t timestamp);

#endif /* WEATHER_ROUTING_H */
