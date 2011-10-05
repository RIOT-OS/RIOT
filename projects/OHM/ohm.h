#ifndef OHM_H
#define OHM_H 

#include <stdint.h>

#define MAX_TICK_BUF_SIZE     (256)
#define CRIT_TICK_BUF_SIZE    (MAX_TICK_BUF_SIZE * 80/100)

#define MAX_THREAD_BUF_SIZE   (1024)
#define CRIT_THREAD_BUF_SIZE    (MAX_THREAD_BUF_SIZE * 50/100)

#define MAX_FUNC_BUF_SIZE    (4048)
#define CRIT_FUNC_BUF_SIZE    (MAX_FUNC_BUF_SIZE * 50/100)

#define OHM_LTC_TICK    (0xFFFF)

typedef enum {
    OHM_DT_LTC,
    OHM_DT_THREAD,
    OHM_DT_FUNCTION
} ohm_datatype_t;

typedef struct {
    uint32_t timestamp;
    uint32_t value;
} ohm_measurement_t;

void otto(uint32_t timestamp, uint32_t value);

#endif /* OHM_H */
