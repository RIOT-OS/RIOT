#ifndef OT_H
#define OT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/ethernet.h"
#include "net/netdev2.h"
#include "thread.h"

#define OPENTHREAD_XTIMER_MSG_TYPE_EVENT (0x2235)
#define OPENTHREAD_NETDEV2_MSG_TYPE_EVENT (0x2236)
#define OPENTHREAD_SERIAL_MSG_TYPE_EVENT (0x2237)
#define OPENTHREAD_MSG_TYPE_RECV (0x2238)

typedef struct {
	void *buf;
	size_t len;
} serial_msg_t;

void recv_pkt(netdev2_t *dev);
void send_pkt(netdev2_t *dev, netdev2_event_t event);
void openthread_bootstrap(void);
void openthread_radio_init(netdev2_t *dev, uint8_t *tb, uint8_t *rb);
void *_openthread_event_loop(void *arg);
int openthread_netdev2_init(char *stack, int stacksize, char priority,
		                        const char *name);

void _event_cb(netdev2_t *dev, netdev2_event_t event);
kernel_pid_t openthread_get_pid(void);
#ifdef __cplusplus
}
#endif

#endif
