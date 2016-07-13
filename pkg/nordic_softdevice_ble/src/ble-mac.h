#ifndef BLE_MAC_H
#define BLE_MAC_H

typedef enum {
    BLE_EVENT_RX_DONE = 20000,
} ble_mac_event_enum_t;

#define BLE_IFACE_ADDED             (10000)

#define BLE_SIXLOWPAN_MTU           (1280U)
#define BLE_SIXLOWPAN_L2_ADDR_LEN   (8)

#define IPV6_IID_FLIP_VALUE         (0x02)

#include "net/eui64.h"

static inline void ble_eui64_from_eui48(uint8_t eui64[8], uint8_t eui48[6], int _public)
{
    eui64[0] = eui48[5];
    eui64[1] = eui48[4];
    eui64[2] = eui48[3];
    eui64[3] = 0xFF;
    eui64[4] = 0xFE;
    eui64[5] = eui48[2];
    eui64[6] = eui48[1];
    eui64[7] = eui48[0];
    if (_public) {
        eui64[0] &= ~(IPV6_IID_FLIP_VALUE);
    }
    else {
        eui64[0] |= IPV6_IID_FLIP_VALUE;
    }
}

typedef struct {
    uint8_t src[8];
    uint8_t payload[BLE_SIXLOWPAN_MTU];
    uint16_t len;
    int8_t rssi;
} ble_mac_inbuf_t;

typedef void (*ble_mac_callback_t)(ble_mac_event_enum_t event, void*arg);
void ble_mac_init(ble_mac_callback_t callback);
int ble_mac_send(uint8_t dest[8], void *data, size_t len);

extern volatile int ble_mac_busy_tx;    /**< Flag is set to 1 when the driver
                                             is busy transmitting a packet. */
extern volatile int ble_mac_busy_rx;    /**< Flag is set to 1 when there is a
                                             received packet pending. */
#endif /* BLE_MAC_H */
