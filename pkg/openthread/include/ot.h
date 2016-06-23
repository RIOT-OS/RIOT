#ifndef OT_H
#define OT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/ethernet.h"
#include "net/netdev2.h"

#define OPENTHREAD_XTIMER_MSG_TYPE_EVENT (0x2235)
#define OPENTHREAD_NETDEV2_MSG_TYPE_EVENT (0x2236)
#define OPENTHREAD_SERIAL_MSG_TYPE_EVENT (0x2237)

typedef struct {
	void *buf;
	size_t len;
} serial_msg_t;

void recv_pkt(netdev2_t *dev, uint8_t *buf);
void send_pkt(netdev2_t *dev);
void openthread_init(void);
void radio_init(uint8_t *tb);
void set_netdev(netdev2_t *dev);
void *ot_thread(void *arg);
#ifdef __cplusplus
}
#endif

#endif
