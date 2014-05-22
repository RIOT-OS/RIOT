
/**
 * @defgroup    sys_ping Ping
 * @ingroup     sys
 * @brief       Ping
 */

#include "radio/radio.h"

#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)
#define PING_PAYLOAD    (8)

typedef enum {
    L2_PING,
    L2_PONG
} l2_ping_type_t;

void ping_init(void);
void ping(radio_address_t addr);

typedef struct pong {
    int hopcount;
    int ttl;
    radio_address_t radio_address;
} ping_r;

typedef struct ping_payload {
    uint8_t type;
    char payload[PING_PAYLOAD];
} ping_payload;
